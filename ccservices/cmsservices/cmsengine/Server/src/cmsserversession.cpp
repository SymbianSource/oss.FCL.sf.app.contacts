/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*       
*
*/


// INCLUDE FILES
#include <featmgr.h>
#include <spsettings.h>
#include <spproperty.h>
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include "cmsserver.h"
#include "cmsserverutils.h"
#include "cmsservercontact.h"
#include "cmsserversession.h"
#include "cmsphonebookproxy.h"
#include "cmsserverasynccontact.h"
#include "cmscontactfield.h"
#include "cmscontactfielditem.h"
#include "cmsdebug.h"

#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactFieldData.h>
#include <e32base.h>

namespace {

TBool IsFieldTypeVoipCapable( TInt aFieldTypeResId, TInt aVoipFlag )
    {
    TBool ret( EFalse );
    CCmsContactFieldItem::TCmsContactField cmsFieldType;
    
    cmsFieldType = CmsServerUtils::MatchFieldType( aFieldTypeResId );
     
    if ( ECmsVoIPSupportBasic & aVoipFlag && 
       ( CCmsContactFieldItem::ECmsVoipNumberGeneric == cmsFieldType 
       || CCmsContactFieldItem::ECmsVoipNumberHome == cmsFieldType
       || CCmsContactFieldItem::ECmsVoipNumberWork == cmsFieldType
       || CCmsContactFieldItem::ECmsPushToTalk == cmsFieldType
       || CCmsContactFieldItem::ECmsShareView == cmsFieldType )      
        )
        {
        ret = ETrue;
        }
    if ( ECmsVoIPSupportSip & aVoipFlag && 
          ( CCmsContactFieldItem::ECmsSipAddress == cmsFieldType )      
        )
        {
        ret = ETrue;
        }
    if ( ECmsVoIPSupportXspId & aVoipFlag && 
          ( CCmsContactFieldItem::ECmsImpp == cmsFieldType  )      
        )
        {
        ret = ETrue;
        }
    if ( ECmsVoIPSupportCallout & aVoipFlag && 
        ( CCmsContactFieldItem::ECmsMobilePhoneGeneric == cmsFieldType
        || CCmsContactFieldItem::ECmsMobilePhoneHome == cmsFieldType
        || CCmsContactFieldItem::ECmsMobilePhoneWork == cmsFieldType
        || CCmsContactFieldItem::ECmsLandPhoneGeneric == cmsFieldType
        || CCmsContactFieldItem::ECmsLandPhoneHome == cmsFieldType
        || CCmsContactFieldItem::ECmsLandPhoneWork == cmsFieldType
        || CCmsContactFieldItem::ECmsAssistantNumber == cmsFieldType
        || CCmsContactFieldItem::ECmsCarPhone == cmsFieldType )     
        )
        {
        ret = ETrue;
        }       
        
    return ret;
    }

TPtrC GetFieldData( const MVPbkContactFieldData& aFieldData )
	{
	TPtrC text ( KNullDesC() );
	
    switch ( aFieldData.DataType() )
        {
        // Text storage type handled here
        case EVPbkFieldStorageTypeText:
            {
            const MVPbkContactFieldTextData& textData = MVPbkContactFieldTextData::Cast( aFieldData );
            text.Set( textData.Text() );
            break;
            }
        // URI storage type handled here
        case EVPbkFieldStorageTypeUri:
            {
            const MVPbkContactFieldUriData& textData = MVPbkContactFieldUriData::Cast( aFieldData );
            text.Set( textData.Text() );
            break;
            }
        }
    
    return text;
	}


TBool ExistDuplicates( const MVPbkStoreContact& aContact, RArray<TInt>& aFieldIndexArray, TInt aFieldIndex )
	{
	TBool ret = EFalse;
	
	const MVPbkStoreContactFieldCollection& fields = aContact.Fields();
	const MVPbkStoreContactField& field = fields.FieldAt( aFieldIndex );
	const MVPbkContactFieldData& fieldData1 = field.FieldData();
	TPtrC fieldContent1 = GetFieldData( fieldData1 );
	
	TInt count = aFieldIndexArray.Count();
	for ( TInt i=0; i<count; i++ )
		{
		TInt index = aFieldIndexArray[i];
		const MVPbkStoreContactField& field = fields.FieldAt( index );
		const MVPbkContactFieldData& fieldData2 = field.FieldData();
		TPtrC fieldContent2 = GetFieldData( fieldData2 );
		if ( fieldContent1.CompareF( fieldContent2 ) == 0 )
			{
			// Find the duplicated field
			ret = ETrue;
			break;
			}
		}
	return ret;
	}
	
TInt GetContacActionFieldCountL(
        CCmsServerContact& aContact,
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aContactAction,
        CCmsServer* aServer )
    {
    TInt voipFlag( 0 );
    CVPbkContactManager* contactManager = aServer->PhonebookProxyHandle().
        ContactManager();
    CVPbkFieldTypeSelector* selector = VPbkFieldTypeSelectorFactory
        ::BuildContactActionTypeSelectorL(
            aContactAction, contactManager->FieldTypes());    

    const MVPbkStoreContact& storeContact = static_cast<MCmsContactInterface&>(
        aContact).Contact();
    
    const MVPbkStoreContactFieldCollection& fields = storeContact.Fields();
    TInt result = 0;
    RArray<TInt> fieldIndexArray;
	if ( aContactAction == VPbkFieldTypeSelectorFactory::EFindOnMapSelector
		|| aContactAction == VPbkFieldTypeSelectorFactory
		    ::EAssignFromMapSelector )
    	{
    	result = selector->AddressFieldsIncluded( fields );
    	}
	else if (aContactAction == VPbkFieldTypeSelectorFactory::EVOIPCallSelector)
        {
        voipFlag = aContact.ParseVoIPAvailabilityL();
        TInt count = fields.FieldCount();
		
		for (TInt i = 0; i < count; ++i)
	        {
	        const MVPbkStoreContactField& field = fields.FieldAt(i);
        	if (selector->IsFieldIncluded(field))
	            {
	            const MVPbkFieldType* type = field.BestMatchingFieldType() ;
				if (IsFieldTypeVoipCapable(type->FieldTypeResId(), voipFlag))
                    {
                    // Check if duplicate exists, and
                    // append the field index of non duplicated field to fieldIndexArray
                    if ( !ExistDuplicates( storeContact, fieldIndexArray, i ) )
                    	{
                    	fieldIndexArray.AppendL(i);                	
						}                     
                    }
                }
	    	}
		
		result = fieldIndexArray.Count();
		}	
	else
		{
		TInt count = fields.FieldCount();
		
		for (TInt i = 0; i < count; ++i)
            {
            const MVPbkStoreContactField& field = fields.FieldAt(i);
            if (selector->IsFieldIncluded(field))
                {
                if ( !ExistDuplicates( storeContact, fieldIndexArray, i ) )
                	{
                	fieldIndexArray.AppendL(i); 
                    }
                }
    	    }
		
		result = fieldIndexArray.Count();
    	}
	
	fieldIndexArray.Close();	
    delete selector;
    return result;
    }

}
 
// ----------------------------------------------------------
// CCmsServerSession::CCmsServerSession
// 
// ----------------------------------------------------------
//
CCmsServerSession::CCmsServerSession( CCmsServer* aServer ) : iCmsServer( aServer ),
                                                              iFeatureManagerInitialized( EFalse )
    {
    }

// ----------------------------------------------------------
// CCmsServerSession::NewL
// 
// ----------------------------------------------------------
//
CCmsServerSession* CCmsServerSession::NewL( CCmsServer* aServer )
    {

    PRINT( _L( "Start CCmsServerSession::NewL()" ) );
    CCmsServerSession* self = new( ELeave ) CCmsServerSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CCmsServerSession::~CCmsServerSession
// 
// ----------------------------------------------------------
//
CCmsServerSession::~CCmsServerSession()
    {
    PRINT( _L( "Start CCmsServerSession::~CCmsServerSession()" ) );
    
    if ( iFeatureManagerInitialized )
         {
         FeatureManager::UnInitializeLib();
         }
    
    delete iServerContact;
    
    PRINT( _L( "End CCmsServerSession::~CCmsServerSession()" ) );
    }

// ----------------------------------------------------------
// CCmsServerSession::StoreOpenComplete
// 
// ----------------------------------------------------------
//
void CCmsServerSession::StoreOpenComplete()
    {
    if ( iServiceMessage  )
        {
        // Some request waits when stores are opened. Handle it now.
        DoServiceL( *iServiceMessage );
        iServiceMessage = NULL;
        }
    }
 
 // ----------------------------------------------------------
// CCmsServerSession::CmsSingleContactOperationComplete
// 
// ----------------------------------------------------------
//   
 void CCmsServerSession::CmsSingleContactOperationComplete( TInt aError ) 
    {
    if ( !iCmsContactRetrieveMessage.IsNull() )
        {
    	iCmsContactRetrieveMessage.Complete( aError );
        }
    }

// ----------------------------------------------------------
// CCmsServerSession::ConstructL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::ConstructL()
    {
    PRINT( _L( "Start CCmsServerSession::ConstructL()" ) );
    FeatureManager::InitializeLibL();
    iFeatureManagerInitialized = ETrue;
    iCmsServer->PhonebookProxyHandle().UpdateXSPStoresListL();
    PRINT( _L( "End CCmsServerSession::ConstructL()" ) );
    }

// ----------------------------------------------------------
// CCmsServerSession::ServiceL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::ServiceL( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerSession::ServiceL()" ) );
    
    if ( iServiceMessage && !iCmsServer->PhonebookProxyHandle().StoreOpenStatus() )
        {
        // some message already waits for store opening completion
        aMessage.Complete( KErrNotReady );
        return;
        }
    
    if( !iCmsServer->PhonebookProxyHandle().StoreOpenStatus() )
        {
        // Stores are not ready yet, save message for later processing
        iServiceMessage = &aMessage;                                       
    	return;
        }
     
    DoServiceL( aMessage );
        
    PRINT( _L( "End CCmsServerSession::ServiceL()" ) );    
    }

// ----------------------------------------------------------
// CCmsServerSession::FindXSPContactL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::CancelXSPContactFindL( const RMessage2& /*aMessage*/ )
    {
    PRINT( _L( "Start CCmsServerSession::CancelXSPContactFindL()" ) );
    
    if( !iServerContact->IsDeleted() )
        {
        iServerContact->CancelXspContactFind();
        } 
    PRINT( _L( "End CCmsServerSession::CancelXSPContactFindL()" ) );      
    }

// ----------------------------------------------------------
// CCmsServerSession::FindXSPContactL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::FindXSPContactL( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerSession::FindXSPContactL()" ) );
    if( !iServerContact->IsDeleted() )
        {
        TRAPD( link, iServerContact->FindXSPContactL( aMessage ) );
        if( KErrNone != link )
            {
            aMessage.Complete( link );
            }
        }
    else 
        {
        aMessage.Complete( KErrNotReady );
        }
    PRINT( _L( "End CCmsServerSession::FindXSPContactL()" ) );
    }

// ----------------------------------------------------------
// CCmsServerSession::CreateContactL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::CreateContactL( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerSession::CreateContactL()" ) );
    iServerContact = CCmsServerContact::NewL( *this, *iCmsServer );
    FetchContactL( aMessage, iServerContact );
    PRINT( _L( "End CCmsServerSession::CreateContactL()" ) );
    }

// ----------------------------------------------------------
// CCmsServerSession::FindParentStoreL
// 
// ----------------------------------------------------------
//
TInt CCmsServerSession::FindParentStoreL( const RMessage2& /*aMessage*/ )
    {
    TInt store( KErrNotReady );
    if( !iServerContact->IsDeleted() )
        {
        store = iServerContact->ContactStore();
        }
    return store;
    }

// ----------------------------------------------------------
// CCmsServerSession::FetchContactL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::FetchContactL( const RMessage2& aMessage,
        MCmsContactInterface* aContactInterface )
    {
    PRINT( _L( "Start CCmsServerSession::FetchContactL()" ) );
    TInt error( KErrNotSupported );
    
    switch( aMessage.Function() )
        {
        case ECmsOpenID:
            {
            iCmsContactRetrieveMessage = aMessage;
            TInt contactId( aMessage.Int0() );
            error = iCmsServer->PhonebookProxyHandle().FetchContactL(
                contactId, aContactInterface );
            break;
            }
        case ECmsOpenLink:
            {
            iCmsContactRetrieveMessage = aMessage;
            TPtrC8 link( CmsServerUtils::ReadMsgParam8LC( 0, aMessage )->Des() );
            error = iCmsServer->PhonebookProxyHandle().FetchContactL(
                link, aContactInterface );
            CleanupStack::PopAndDestroy();  //link
            break;
            }
        case ECmsOpenPhoneNbr:
            {
            iCmsContactRetrieveMessage = aMessage;
            TPtrC phoneNbr( CmsServerUtils::ReadMsgParam16LC( 0, aMessage )->Des() );
            error = iCmsServer->PhonebookProxyHandle().FetchContactL(
                phoneNbr, aContactInterface );
            CleanupStack::PopAndDestroy();  //phoneNbr
            break;
            }
        default:
            {
            break;
            }
        }
  
    if( error != KErrNone )
        {
        aMessage.Complete( error );
        }
    PRINT( _L( "End CCmsServerSession::FetchContactL()" ) );
    }

// ----------------------------------------------------------
// CCmsServerSession::FindServiceAvailabilityL
// 
// ----------------------------------------------------------
//
TInt CCmsServerSession::FindServiceAvailabilityL( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerSession::FindServiceAvailabilityL()" ) );
    TInt availability( 1 );
    if( !iServerContact->IsDeleted() )
        {
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector service(
                ( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector )aMessage.Int0() );
        switch( service )
            {
            case VPbkFieldTypeSelectorFactory::EInstantMessagingSelector:
                {
                availability = iServerContact->AsyncContact().FindImAvailabilityL();
                PRINT1( _L( "CCmsServerSession::FindServiceAvailabilityL():IM availability: %d" ), availability );
                break;
                }
            case VPbkFieldTypeSelectorFactory::EVideoCallSelector:
                {
                availability = FeatureManager::FeatureSupported( KFeatureIdCsVideoTelephony );
                PRINT1( _L( "CCmsServerSession::FindServiceAvailabilityL():Video call availability: %d [FeatureManager]" ), availability );
                break;
                }
            case VPbkFieldTypeSelectorFactory::EVOIPCallSelector:
                {
                availability = iServerContact->ParseVoIPAvailabilityL();
                break;
                }
            default:
                {
                //In case the type is unknown, just break here and return true
                break;
                }
            }
        }
    PRINT( _L( "End CCmsServerSession::FindServiceAvailabilityL()" ) );    
    return availability;
    }

// ----------------------------------------------------------
// CCmsServerSession::FetchDataL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::FetchDataL( const RMessage2& aMessage )
    {
    PRINT( _L(" Start CCmsServerSession::FetchDataL()" ) );
    
    if( !iServerContact->IsDeleted() )
        {
        iServerContact->FetchContactData( aMessage );
        }
    else
        {
        PRINT( _L( "CCmsServerSession::FetchDataL():Contact has been deleted => complete with KErrNotFound" ) );
        aMessage.Complete( KErrNotFound );
        }
    PRINT( _L(" End CCmsServerSession::FetchDataL()" ) );    
    }

// ----------------------------------------------------------
// CCmsServerSession::OrderNotifyL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::OrderNotifyL( const RMessage2& aMessage, TBool aAskMore )
    {
    PRINT( _L( "Start CCmsServerSession::OrderNotifyL()" ) );
    
    if( !iServerContact->IsDeleted() )
        {
        iServerContact->OrderNotifyL( aMessage, aAskMore );
        }
    else
        {
        PRINT( _L( "CCmsServerSession::OrderNotifyL(): Contact has been deleted => complete with KErrNotFound" ) );
        

        iServerContact->OrderNotifyL( aMessage, ETrue );
		
		if ( !aMessage.IsNull() )
			{
			aMessage.Complete( KErrNotFound );
			}
        }
    PRINT( _L( "End CCmsServerSession::OrderNotifyL()" ) );    
    }

// ----------------------------------------------------------
// CCmsServerSession::CompleteNotify
// 
// ----------------------------------------------------------
//
void CCmsServerSession::CompleteNotify( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerSession::CompleteNotify()" ) );
    
    iServerContact->CompleteNotify( aMessage, KErrCancel );
    PRINT( _L( "End CCmsServerSession::CompleteNotify()" ) );
    }

// ----------------------------------------------------------
// CCmsServerSession::EnabledFieldsL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::EnabledFieldsL( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerSession::EnabledFieldsL()" ) );
    
	if( !iServerContact->IsDeleted() )
	    {
	    iServerContact->FetchEnabledFields( aMessage );
	    }
    else
        {
        
        PRINT( _L( "CCmsServerSession::EnabledFieldsL():  Contact has been deleted => complete with KErrNotFound" ) );
        
        aMessage.Complete( KErrNotFound );
        }	
     PRINT( _L( "End CCmsServerSession::EnabledFieldsL()" ) );    
    }

// ----------------------------------------------------------
// CCmsServerSession::FetchContactIdentifierL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::FetchContactIdentifierL( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerSession::FetchContactLinkArrayL()" ) );
    
    if( !iServerContact->IsDeleted() )
        {	
	    //Currently only contactlink array is supported.
        iServerContact->FetchContactLinkArrayL( aMessage );
        }
    else
        {
        PRINT( _L( "CCmsServerSession::FetchContactIdentifierL(): Contact has been deleted => complete with KErrNotFound" ) );
        
        aMessage.Complete( KErrNotFound );
        }
    PRINT( _L( "Start CCmsServerSession::FetchContactLinkArrayL()" ) );    
    }

// ----------------------------------------------------------
// CCmsServerSession::CancelOperation
// 
// ----------------------------------------------------------
//
void CCmsServerSession::CancelOperation( const RMessage2& aMessage )
	{
    PRINT( _L( "Start CCmsServerSession::CancelOperation()" ) );
    
    if( !iServerContact->IsDeleted() )
        {
        iServerContact->CancelOperation();
        aMessage.Complete( KErrNone );
        }
    else
        {
        PRINT( _L( "CCmsServerSession::CancelOperation():  Contact has been deleted => complete with KErrNotFound" ) );
        
        aMessage.Complete( KErrNotFound );

        }
    PRINT( _L( "End CCmsServerSession::CancelOperation()" ) );

	}


// ----------------------------------------------------------
// CCmsServerSession::PanicClient
// 
// ----------------------------------------------------------
//
void CCmsServerSession::PanicClient( TInt aPanic )
    {
    _LIT( KTxtSessionPanic,"CMS Server Session panic");
    User::Panic( KTxtSessionPanic, aPanic );
    }

// ----------------------------------------------------------
// CCmsServerSession::GetContacActionFieldCountL
// 
// ----------------------------------------------------------
//
TInt CCmsServerSession::GetContacActionFieldCountL( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerSession::GetContacActionFieldCountL()" ) );
    
    TInt result = 0;
    if( !iServerContact->IsDeleted() )
        {
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector action(
                ( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector )aMessage.Int0() );
        result = ::GetContacActionFieldCountL(*iServerContact, action, iCmsServer );
        }
    PRINT( _L( "End CCmsServerSession::GetContacActionFieldCountL()" ) );    
    return result;
    }

// ----------------------------------------------------------
// CCmsServerSession::DoServiceL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::DoServiceL( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerSession::DoServiceL()" ) );
    
    const TInt function = aMessage.Function();
    switch( function )
        {
        case ECmsFetchEnabledFields:
            {
            PRINT( _L( " CCmsServerSession::DoServiceL(): Fetch fields that have content" ) );
            
            EnabledFieldsL( aMessage );
            break;
            }
        case ECmsFetchContactField:
            {
            PRINT( _L( " CCmsServerSession::DoServiceL():Fetch contact data (single)" ) );
            
            FetchDataL( aMessage );
            break;
            }
        case ECmsCancelOperation:
            {
            PRINT( _L( " CCmsServerSession::DoServiceL():CCmsServerSession::ServiceL():Cancel an operation" ) );
            
            CancelOperation( aMessage );
            break;
            }
        case ECmsCloseSubsession:
            {
            TUint handle( aMessage.Int3() );
            
            PRINT1( _L( " CCmsServerSession::DoServiceL():Close subsession - Handle: %d" ), handle );
            
            aMessage.Complete( KErrNone ); 
            break;
            }
        case ECmsFetchContactIdentifier:
            {
            PRINT( _L( " CCmsServerSession::DoServiceL():Fetch contactlink array" ) );
            
            FetchContactIdentifierL( aMessage );
            break;
            }
        case ECmsOrderNotification:
            {
            PRINT( _L( " CCmsServerSession::DoServiceL():Order notification" ) );
            
            OrderNotifyL( aMessage, EFalse );
            break;  
            }
        case ECmsOrderNextNotification:
            {
            PRINT( _L( " CCmsServerSession::DoServiceL():Next notification" ) );
            
            OrderNotifyL( aMessage, ETrue );
            break;
            }
        case ECmsFetchServiceAvailability:
            {
            PRINT( _L( " CCmsServerSession::DoServiceL():Fetch service availability" ) );
            
            TInt result( FindServiceAvailabilityL( aMessage ) );
            aMessage.Complete( result );
            break;
            }
        case ECmsFindParentStore:
            {
            PRINT( _L( " CCmsServerSession::DoServiceL():Find parent store" ) );
            
            TInt result( FindParentStoreL( aMessage ) );
            aMessage.Complete( result );
            break;
            }
        case ECmsFindExternalContact:
            {
            PRINT( _L( " CCmsServerSession::DoServiceL():Find external contact" ) );
            
            if( iCmsServer->PhonebookProxyHandle().ExternalStoresIntalled() )
                {
                PRINT( _L( " CCmsServerSession::ServiceL(): xSP stores installed => search" ) );
                FindXSPContactL( aMessage  );
                }
            else
                {
                PRINT( _L( "  CCmsServerSession::DoServiceL():No xSP stores installed => complete with KErrNotFound" ) );
                aMessage.Complete( KErrNotFound );
                }
            break;
            }
        case ECmsCancelExternalContactFind:
            {
            CancelXSPContactFindL( aMessage );
            aMessage.Complete( KErrNone );
            break;
            }
        case ECmsCancelNotification:
            {
            PRINT( _L( " CCmsServerSession::DoServiceL():Cancel Notification" ) );
            
            CompleteNotify( aMessage );
            aMessage.Complete( KErrNone );            
            break;
            }
        case ECmsGetContactActionFieldCount:
            {
            TInt result( GetContacActionFieldCountL( aMessage ) );
            aMessage.Complete( result );
            break;
            }
        case ECmsOpenID:
        case ECmsOpenLink:
        case ECmsOpenPhoneNbr:
            {
            PRINT( _L( " CCmsServerSession::ServiceL():Create a new contact object" ) );
            CreateContactL( aMessage );
            break;
            }
        case ECmsIsTopContact:
            {
            PRINT( _L( " CCmsServerSession::ServiceL():Is Top Contact" ) );
            IsTopContactL( aMessage );
            break;
            }
		case ECmsSetVoiceCallDefault:
        	{
        	iServerContact->SetVoiceCallDefaultL();
        	aMessage.Complete( KErrNone );
        	break;
        	}
        default:
            {
            PRINT( _L( " CCmsServerSession::ServiceL(): not supported request" ) );
            aMessage.Complete( KErrNotSupported );
            break;
            }
        }
    PRINT( _L( "End CCmsServerSession::DoServiceL()" ) );  
    }


// ----------------------------------------------------------
// CCmsServerSession::IsTopContactL
// 
// ----------------------------------------------------------
//
void CCmsServerSession::IsTopContactL( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerSession::IsTopContactL()" ) );            
    if( iServerContact && !iServerContact->IsDeleted() )
        {   
        //Currently only contactlink array is supported.
        iServerContact->IsTopContactL( aMessage );
        }
    else
        {
        PRINT( _L( "CCmsServerSession::IsTopContactL'(): Contact has been deleted => complete with KErrNotFound" ) );        
        aMessage.Complete( KErrNotFound );
        }
    
    PRINT( _L( "End CCmsServerSession::IsTopContactL()" ) );            
    }


// End of file
