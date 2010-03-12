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


#include <spsettings.h>
#include <MVPbkFieldType.h>
#include "cmscontactfield.h"
#include <MVPbkContactLink.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkTopContactManager.h>

#include <featmgr.h>
#include <crcseprofileregistry.h>

#include "bpas.h"
#include "bpasinfo.h"
#include "cmsserver.h"
#include "bpasiconinfo.h"
#include "cmsnotifyevent.h"
#include "cmsserverutils.h"
#include "bpasserviceinfo.h"
#include "cmsservercontact.h"
#include "cmsphonebookproxy.h"
#include "cmsserverasynccontact.h"
#include "cmsserverxspcontacthandler.h"
#include "mpresencetrafficlights.h"
#include "cmsserverdefines.h"
#include "cmsdebug.h"

// ----------------------------------------------------
// CCmsServerContact::CCmsServerContact
// 
// ----------------------------------------------------
// 
CCmsServerContact::CCmsServerContact( CCmsServerSession& aSession,
                                      CCmsServer& aCmsServer ) :
                                      iContactDeleted( EFalse ),
                                      iPresenceNotifySubscribed( EFalse ),                                       
                                      iSession( aSession ),
                                      iCmsServer( aCmsServer )
    {
    }

// ----------------------------------------------------
// CCmsServerContact::NewL
// 
// ----------------------------------------------------
// 
CCmsServerContact* CCmsServerContact::NewL( CCmsServerSession& aSession,
                                            CCmsServer& aCmsServer )
    {
    CCmsServerContact* self = new( ELeave ) CCmsServerContact( aSession, aCmsServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CCmsServerContact::ConstructL
// 
// ----------------------------------------------------
// 
void CCmsServerContact::ConstructL()
    {
    PRINT( _L( "Start CCmsServerContact::ConstructL()" ) );
    
    iContactManager = iCmsServer.PhonebookProxyHandle().ContactManager();
    iAsyncContact = CCmsServerAsyncContact::NewL( *this, iCmsServer );
    iBrandedPresence = CBPAS::NewL();
    iBrandedPresence->SetObserver( this );
    PRINT( _L( "End CCmsServerContact::ConstructL()" ) );
    }

// ----------------------------------------------------
// CCmsServerContact::CCmsServerContact
// 
// ----------------------------------------------------
// 
CCmsServerContact::~CCmsServerContact()
    {
    PRINT( _L( "Start CCmsServerContact::~CCmsServerContact()" ) );
    
    iCmsServer.PhonebookProxyHandle().RemoveObserver( this );
        
    if ( iPresenceNotifySubscribed && iContactLink ) 
        {
        if ( iVoipHandler )
            {
            iVoipHandler->UnsubscribeBrandingForContact( iContactLink, this );            
            }
        if ( iChatHandler )
            {
            iChatHandler->UnsubscribeBrandingForContact( iContactLink, this );            
            }        
        }
    delete iStoreUri;
    delete iChatHandler;
    delete iVoipHandler;
    delete iXSPContactHandler;
    delete iContactLink;
    delete iContact;
    delete iCachedField8;
    delete iAsyncContact;
    delete iCachedField16;
    delete iBrandedPresence;
    delete iPackedContactLinkArray;
    delete iOperation;
    iPresenceEvents.ResetAndDestroy();
    iPhonebookEvents.ResetAndDestroy();
    iPresenceEvents.Close();
    iPhonebookEvents.Close();
    iXSPFindOperationsArray.ResetAndDestroy();
    iXSPFindOperationsArray.Close();
    delete iIdArray;

    PRINT( _L( "End CCmsServerContact::~CCmsServerContact()" ) );
    }

// ----------------------------------------------------
// CCmsServerContact::FetchEnabledFields
// 
// ----------------------------------------------------
// 
void CCmsServerContact::FetchEnabledFields( const RMessage2& aMessage )
    {
    PRINT( _L( "Start CCmsServerContact::FetchEnabledFields()" ) );
    
    iMessage = aMessage;
    if( iCachedField16 )
        {
        PRINT( _L("CCmsServerContact::FetchEnabledFields():Handle cached data send" ) );
        CachedDataSend16( KFirstParam );
        }
    else
        {
        PRINT( _L("CCmsServerContact::FetchEnabledFields():Initiate new fetch" ) );
        iAsyncContact->FetchEnabledFields(); 
        }

    PRINT( _L( "End CCmsServerContact::FetchEnabledFields()" ) );
    }

// ----------------------------------------------------
// CCmsServerContact::FetchContactData
// 
// ----------------------------------------------------
//
void CCmsServerContact::FetchContactData( const RMessage2& aMessage )
    {
    PRINT( _L(" Start CCmsServerContact::FetchContactData()" ) );
    
    iMessage = aMessage;
    if( !CachedData() )
        {
        PRINT( _L( "CCmsServerContact::FetchContactData():Initiate new fetch" ) );
        
        CCmsContactFieldItem::TCmsContactField fieldType = 
            ( CCmsContactFieldItem::TCmsContactField )aMessage.Int0();
        iAsyncContact->FetchContactData( fieldType ); 
        }
    PRINT( _L(" End CCmsServerContact::FetchContactData()" ) );    
    }

// ----------------------------------------------------
// CCmsServerContact::AsyncContact
// 
// ----------------------------------------------------
//
CCmsServerAsyncContact& CCmsServerContact::AsyncContact()
    {
    return *iAsyncContact;
    }

// ----------------------------------------------------
// CCmsServerContact::OfferContactEventL
// 
// ----------------------------------------------------
//
void CCmsServerContact::OfferContactEventL( TCmsPhonebookEvent aEventType,
                                             const MVPbkContactLink* aContactLink )
    {
    PRINT( _L(" Start CCmsServerContact::OfferContactEventL()" ) );
    
    TBuf<5> buffer( _L( "" ) );

    if ( !iContact )
    	{
		buffer.AppendNum( aEventType );
        CCmsNotifyEvent* event = CCmsNotifyEvent::NewL( buffer, *this );
        CleanupStack::PushL( event );
        User::LeaveIfError( iPhonebookEvents.Append( event ) );
        CleanupStack::Pop();  //event
        return;
    	}

    MVPbkContactLink* self = Contact().CreateLinkLC();
    if( aContactLink && aContactLink->IsSame( *self ) )
        {
        buffer.AppendNum( aEventType );
        
        if( ECmsContactDeleted == aEventType )
            {
            PRINT( _L( "CCmsServerContact::OfferContactEventL():  Contact deleted, invalidate this subsession" ) );
            delete iContact;
            iContact = NULL;
            iContactDeleted = ETrue;
            }
        else
            {
            HBufC8* link = self->PackLC();
            TInt error( iCmsServer.PhonebookProxyHandle().FetchContactL( *link, this ) );
            CleanupStack::PopAndDestroy();  //link
            PRINT1( _L( "CCmsServerContact::OfferContactEventL(): Contact handle reloaded - Error: %d" ), error );
            
            //Suppress UREL warning
            error = KErrNone;
            }
            
        if( !iPbkNotifyMessage.IsNull() )
            {
            TInt error( iPbkNotifyMessage.Write( KSecondParam, buffer ) );
            PRINT1( _L( "CCmsServerContact::OfferContactEventL():iPbkmsg.Complete() in OfferContactEventL this=%d" ), (TInt)this );
                        
            iPbkNotifyMessage.Complete( error );           
            }
        else  //Add to queue
            {
            PRINT( _L( "CCmsServerContact::OfferContactEventL():Message == NULL => Add to queue" ) );
            
            CCmsNotifyEvent* event = CCmsNotifyEvent::NewL( buffer, *this );
            CleanupStack::PushL( event );
            User::LeaveIfError( iPhonebookEvents.Append( event ) );
            CleanupStack::Pop();  //event
            }
        }
    CleanupStack::PopAndDestroy();  //self
    PRINT( _L(" End CCmsServerContact::OfferContactEventL()" ) );
    }

// ----------------------------------------------------
// CCmsServerContact::IsDeleted
// 
// ----------------------------------------------------
// 
TBool CCmsServerContact::IsDeleted() const
    {
    return iContactDeleted;
    }

// ----------------------------------------------------
// CCmsServerContact::OrderNotifyL
// 
// ----------------------------------------------------
// 
void CCmsServerContact::OrderNotifyL( const RMessage2& aMessage, TBool aAskMore )
    {
    PRINT( _L(" Start CCmsServerContact::OrderNotifyL()" ) );
    
    TBool foundType( EFalse); 
    if ( CCmsContactFieldItem::ECmsPresenceAllNotification & aMessage.Int0() )    
        {
        foundType = ETrue;
        StartPresenceNotificationL( aMessage, aAskMore );
        }
    if ( CCmsContactFieldItem::ECmsPhonebookNotification & aMessage.Int0() )
        {
        foundType = ETrue;    
        PRINT1( _L( "CCmsServerContact::OrderNotifyL():iPbkMsg initiated in CCmsServerContact::OrderNotifyL() this=%d" ), (TInt)this );
                
        if( !aAskMore )
            {
            iCmsServer.PhonebookProxyHandle().AddObserverL( this );
            PRINT( _L( "CCmsServerContact::OrderNotifyL():Phonebook observer added" ) );
            }
        else
            {
            if( iPhonebookEvents.Count() > 0 )
                {
                PRINT( _L( "CCmsServerContact::OrderNotifyL():  Complete queued message" ) );
                
                CCmsNotifyEvent* event( iPhonebookEvents[0] );
                event->Complete( KSecondParam, aMessage );
                iPhonebookEvents.Remove( 0 );
                delete event;
                event = NULL;
                }
            }
        iPbkNotifyMessage = aMessage;
        }
    if ( !foundType )
        {
        User::Leave( KErrNotSupported );
        }
    PRINT( _L(" End CCmsServerContact::OrderNotifyL()" ) );    
    }

// ----------------------------------------------------------
// CCmsServerContact::StartPresenceNotificationL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::StartPresenceNotificationL( const RMessage2& aMessage, TBool aAskMore )
    {
    PRINT( _L(" Start CCmsServerContact::StartPresenceNotificationL()" ) );
          
    if ( !aAskMore )
        {
        PRINT( _L( "CCmsServerContact::StartPresenceNotificationL(): First notification request => send presence info to client" ) );
                    
        if ( iPresenceNotifySubscribed )
            {
            User::Leave( KErrInUse );
            }
        iPresenceNotifyMessage = aMessage;        
        if ( !iContactLink )
            {
            iContactLink = iContact->CreateLinkLC();
            CleanupStack::Pop(); // iContactLink
            } 
        if (  CCmsContactFieldItem::ECmsPresenceVoIPNotification & aMessage.Int0() )
            {
            StartPresenceVoipNotificationL();            
            }
        if (  CCmsContactFieldItem::ECmsPresenceChatNotification & aMessage.Int0() )
            {
            StartPresenceChatNotificationL();             
            }                         
        iPresenceNotifySubscribed = ETrue;                
        }
    else
        {
        PRINT( _L( "CCmsServerContact::StartPresenceNotificationL():Ask more == ETrue => Save the kernel message & check message queue" ) );
        
        iPresenceNotifyMessage = aMessage;
        if( iPresenceEvents.Count() > 0 )
            {
            PRINT( _L( "CCmsServerContact::StartPresenceNotificationL():Complete queued message" ) );
            
            CCmsNotifyEvent* event( iPresenceEvents[0] );
            event->Complete( KSecondParam, iPresenceNotifyMessage );
            iPresenceEvents.Remove( 0 );
            delete event;
            event = NULL;
            }
        }
    PRINT( _L(" End CCmsServerContact::StartPresenceNotificationL()" ) );    
    }

// ----------------------------------------------------------
// CCmsServerContact::StartPresenceVoipNotificationL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::StartPresenceVoipNotificationL( )
    {  
PRINT( _L(" Start CCmsServerContact::StartPresenceVoipNotificationL" ) );
    
    const TInt count = sizeof( KPresenceFieldGroup ) / sizeof( KPresenceFieldGroup[0] );    
        
    CDesCArrayFlat* fieldArray = new ( ELeave ) CDesCArrayFlat( count );
    CleanupStack::PushL( fieldArray );
    for ( TInt i=0; i < count; i++ )
        {
        AddVoipFieldL( *fieldArray, KPresenceFieldGroup[i] );
        }
    
    if ( !iVoipHandler )
        {
        iVoipHandler = MPresenceTrafficLights::NewL();
        }    
    iVoipHandler->SubscribeBrandingForContactL( iContactLink, fieldArray, this, CCmsContactFieldItem::ECmsPresenceVoIPNotification );
         
    CleanupStack::PopAndDestroy( fieldArray );
    return;
    }
    
// ----------------------------------------------------------
// CCmsServerContact::AddVoipFieldL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::AddVoipFieldL( CDesCArrayFlat& aArray, TInt aResourceId )
    {
    const MVPbkFieldType* type = iContactManager->FieldTypes().Find( aResourceId );
    CVPbkBaseContactFieldTypeIterator* itr = CVPbkBaseContactFieldTypeIterator::NewLC( 
            *type, iContact->Fields() );
    while( itr->HasNext() )
        {
        const MVPbkBaseContactField* field = itr->Next();
        if ( aResourceId == R_VPBK_FIELD_TYPE_IMPP )
            {
            const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( field->FieldData() );
            if ( iAsyncContact->IsVoipCapableL( uri.Scheme() ) )
                {
                aArray.AppendL( uri.Uri() );
                }            
            }
        else
            {
            const MVPbkContactFieldTextData& text = MVPbkContactFieldTextData::Cast( field->FieldData() );
            aArray.AppendL( text.Text() );               
            }
        }
    CleanupStack::PopAndDestroy( itr );  //itr
    }

// ----------------------------------------------------------
// CCmsServerContact::StartPresenceChatNotificationL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::StartPresenceChatNotificationL( )
    {
    PRINT( _L(" Start CCmsServerContact::StartPresenceChatNotificationL" ) );
     
    const TInt KMyServiceEstimate = 5;      
        
    CDesCArrayFlat* fieldArray = new ( ELeave ) CDesCArrayFlat( KMyServiceEstimate );
    CleanupStack::PushL( fieldArray );
    
    const MVPbkFieldType* type = iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
    CVPbkBaseContactFieldTypeIterator* itr = CVPbkBaseContactFieldTypeIterator::NewLC( 
            *type, iContact->Fields() );
    while( itr->HasNext() )
        {
        const MVPbkBaseContactField* field = itr->Next();
        const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( field->FieldData() );
        if ( CmsServerUtils::IsFeatureSupportedL( ESubPropertyIMLaunchUid, uri.Scheme() ) )//iAsyncContact->IsImCapableL( uri.Scheme() ) )
            {
            fieldArray->AppendL( uri.Uri() );
            }
        }
    CleanupStack::PopAndDestroy( itr );  //itr
        
    if ( !iChatHandler )
        {
        iChatHandler = MPresenceTrafficLights::NewL();
        }    
    iChatHandler->SubscribeBrandingForContactL( iContactLink, fieldArray, this, CCmsContactFieldItem::ECmsPresenceChatNotification );
        
    CleanupStack::PopAndDestroy( fieldArray );
    PRINT( _L(" End CCmsServerContact::StartPresenceChatNotificationL" ) );
    return;
    }

// ----------------------------------------------------------
// CCmsServerContact::CompleteNotify
// 
// ----------------------------------------------------------
//
void CCmsServerContact::CompleteNotify( const RMessage2& aMessage, TInt aStatus )
    {
    PRINT( _L(" Start CCmsServerContact::CompleteNotify()" ) );
    PRINT1( _L( "CCmsServerContact::CompleteNotify(): Status:   %d" ), aStatus );
    
    if ( CCmsContactFieldItem::ECmsPresenceAllNotification & aMessage.Int0() )
        {
        // stop subscriptions
        if ( iPresenceNotifySubscribed && iContactLink ) 
            {
            if ( iVoipHandler )
                {
                iVoipHandler->UnsubscribeBrandingForContact( iContactLink, this );
                delete iVoipHandler;
                iVoipHandler = NULL;
                }
            if ( iChatHandler )
                {
                iChatHandler->UnsubscribeBrandingForContact( iContactLink, this ); 
                delete iChatHandler;
                iChatHandler = NULL;                
                }
            iPresenceNotifySubscribed = EFalse;            
            }
        
        EmptyPresenceQueue();
        if( !iPresenceNotifyMessage.IsNull())
            {
            iPresenceNotifyMessage.Complete( aStatus );
            }
        }
    if( CCmsContactFieldItem::ECmsPhonebookNotification & aMessage.Int0() )
        {
        EmptyPhonebookQueue();
        iCmsServer.PhonebookProxyHandle().RemoveObserver( this );
        if( !iPbkNotifyMessage.IsNull() )
            {
            PRINT1( _L( "CCmsServerContact::CompleteNotify():  iPbkMsg.Complete() in CompleteNotify this=%d"),(TInt)this );
             
            iPbkNotifyMessage.Complete( aStatus );
            }
        }
    }

// ----------------------------------------------------
// CCmsServerContact::EmptyPresenceQueue
// 
// ----------------------------------------------------
// 
void CCmsServerContact::EmptyPresenceQueue()
    {
    const TInt count = iPresenceEvents.Count();
    for( TInt i = 0;i < count;i++ )
        {
        CCmsNotifyEvent* event( iPresenceEvents[0] );
        iPresenceEvents.Remove( 0 );
        delete event;
        event = NULL;
        }
    }

// ----------------------------------------------------
// CCmsServerContact::EmptyPhonebookQueue
// 
// ----------------------------------------------------
// 
void CCmsServerContact::EmptyPhonebookQueue()
    {
    const TInt count = iPhonebookEvents.Count();
    for( TInt i = 0;i < count;i++ )
        {
        CCmsNotifyEvent* event( iPhonebookEvents[0] );
        iPhonebookEvents.Remove( 0 );
        delete event;
        event = NULL;
        }
    }

// ----------------------------------------------------
// CCmsServerContact::CachedData
// 
// ----------------------------------------------------
// 
TBool CCmsServerContact::CachedData()
    {
    PRINT( _L(" Start CCmsServerContact::CachedData()" ) );
    
    TBool retVal = EFalse;
    if( iCachedField16 )
        {
        PRINT( _L( "CCmsServerContact::CachedData():Handle cached 16-bit data send" ) );
        
        retVal = ETrue;
        CachedDataSend16( KSecondParam );
        }
    else if( iCachedField8 )
        {
        PRINT( _L( "CCmsServerContact::CachedData(): Handle cached 8-bit data send" ) );
        
        retVal = ETrue;
        CachedDataSend8( KSecondParam );
        }
    PRINT( _L(" End CCmsServerContact::CachedData()" ) );    
    return retVal;
    }

// ----------------------------------------------------
// CCmsServerContact::CachedDataSend
// 
// ----------------------------------------------------
// 
void CCmsServerContact::CachedDataSend16( TInt aMessageParam )
    {
    PRINT( _L(" Start CCmsServerContact::CachedDataSend()" ) );
    
    TPtrC streamDesc( iCachedField16->Des() );
    TInt descLength = 0;
    TRAP_IGNORE( descLength = iMessage.GetDesMaxLengthL( aMessageParam ));
    __ASSERT_ALWAYS( descLength >= streamDesc.Length(),
                    CCmsServer::PanicServer( EBufferOverflow ) );
    TInt error = iMessage.Write( aMessageParam, streamDesc );
    iMessage.Complete( error );
    PRINT1( _L( "CCmsServerContact::CachedDataSend16(): Data written to client - Error: %d" ), error );
    
    delete iCachedField16;
    iCachedField16 = NULL;
    PRINT( _L(" End CCmsServerContact::CachedDataSend()" ) );
    }

// ----------------------------------------------------
// CCmsServerContact::CachedDataSend
// 
// ----------------------------------------------------
// 
void CCmsServerContact::CachedDataSend8( TInt aMessageParam )
    {
    PRINT( _L(" Start CCmsServerContact::CachedDataSend()" ) );
    
    TPtrC8 streamDesc( iCachedField8->Des() );
    TInt descLength = 0;    
    TRAP_IGNORE( descLength = iMessage.GetDesMaxLengthL( aMessageParam ));
    __ASSERT_ALWAYS( descLength >= streamDesc.Length(),
                    CCmsServer::PanicServer( EBufferOverflow ) );
    TInt error = iMessage.Write( aMessageParam, streamDesc );
    iMessage.Complete( error );
    PRINT1( _L( "CCmsServerContact::CachedDataSend8(): Data written to client - Error: %d" ), error );
   
    delete iCachedField8;
    iCachedField8 = NULL;
    PRINT( _L(" End CCmsServerContact::CachedDataSend()" ) );
    }

// ----------------------------------------------------
// CCmsServerContact::ContactReadyL
// 
// ----------------------------------------------------
// 
void CCmsServerContact::ContactReadyL( TInt aError, MVPbkStoreContact* aContact )
    {
    TPtrC uri( _L( "" ) );
    if( KErrNone == aError && aContact )
        {
        delete iContact;
        iContact = aContact;
        //Create contactlink array of this contact. This is needed for UI operations. 
        //Only 1 contact link appended to array.
        //Possible performance issue. Should this be created only when array is requested from client side?
        CreateContactLinkArrayL(); 
        uri.Set( aContact->ParentStore().StoreProperties().Uri().UriDes() );
        ParseContactStore( uri );
        }
    }

// ----------------------------------------------------------
// CCmsServerContact::ParseContactStore
//
// ----------------------------------------------------------
//
void CCmsServerContact::ParseContactStore( const TDesC& aStoreUri )
    {
    using namespace VPbkContactStoreUris;
    if( aStoreUri.CompareF( SimGlobalAdnUri() ) == 0 ||
        aStoreUri.CompareF( SimGlobalFdnUri() ) == 0 )
        {
        iContactStore = ECmsContactStoreSim;
        }
    else if( aStoreUri.CompareF( SimGlobalSdnUri() ) == 0 )
        {
        iContactStore = ECmsContactStoreSdn;
        }
    else if( aStoreUri.CompareF( DefaultCntDbUri() ) == 0 )
        {
        iContactStore = ECmsContactStorePbk;
        }
    else
        {
        iContactStore = ECmsContactStoreXsp;
        }
    delete iStoreUri;
    iStoreUri = NULL;
    iStoreUri = aStoreUri.Alloc();
    PRINT1( _L( "CCmsServerContact::ParseContactStore() - Store: %d" ), iContactStore );
    }

// ----------------------------------------------------------
// CCmsServerContact::FindXSPContactL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::FindXSPContactL( const RMessage2& aMessage )
    {
    delete iXSPContactHandler;
    iXSPContactHandler = NULL;
    iXSPContactHandler = CCmsServerXSPContactHandler::NewL( *this, iCmsServer );
    iXSPContactHandler->SaveMessage( aMessage );
    
    if ( iIdArray != NULL )
    	{
    	delete iIdArray;
    	iIdArray = NULL;
    	}
    	
    iIdArray = new ( ELeave ) CDesCArrayFlat( 5 );
    const MVPbkFieldType* type = iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
    CVPbkBaseContactFieldTypeIterator* itr = CVPbkBaseContactFieldTypeIterator::NewLC( *type, iContact->Fields() );
    if( itr->HasNext() )
        {
        while( itr->HasNext() )
            {
            const MVPbkBaseContactField* field = itr->Next();
            const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( field->FieldData() );
            HBufC* data = uri.Uri().AllocL();
            CleanupStack::PushL( data );
            iIdArray->AppendL( *data );
            CleanupStack::PopAndDestroy();  //data
            }
        
        TInt count = iIdArray->Count();
        // Notify XSPContactHandler about number of find operations
        iXSPContactHandler->NumberOfFinds( count );
        // Start find operation for the first search string in the array
        MVPbkContactOperationBase* XSPFindOperation = 
            iCmsServer.PhonebookProxyHandle().FindXSPContactL
                (iIdArray->MdcaPoint(0), *iXSPContactHandler );
        
        CleanupStack::PushL( XSPFindOperation );
        iXSPFindOperationsArray.AppendL( XSPFindOperation );
        CleanupStack::Pop( XSPFindOperation );
        
        // Notify XSPContactHandler about number of stores used in find operation
        TInt storesNumber = iCmsServer.PhonebookProxyHandle().NumberOfStoresInFind( *iStoreUri );
        iXSPContactHandler->NumberOfStoresInFind( storesNumber );
        }
    else 
    	{
	    User::Leave( KErrNotFound );
    	}
    	
    CleanupStack::PopAndDestroy( itr );  //itr
    }

// ----------------------------------------------------------
// CCmsServerContact::ContactStore
// 
// ----------------------------------------------------------
//
TInt CCmsServerContact::ContactStore() const
    {
    return ( TInt )iContactStore;
    }   

// ----------------------------------------------------------
// CCmsServerContact::CancelXspContactFind
// 
// ----------------------------------------------------------
//
void CCmsServerContact::CancelXspContactFind() const
    {
    if( iXSPContactHandler )
        {
        iXSPContactHandler->Cancel();
        }
    }

// ----------------------------------------------------------
// CCmsServerContact::StoreUri
// 
// ----------------------------------------------------------
//
TPtrC CCmsServerContact::StoreUri()
    {   
    return iStoreUri ? iStoreUri->Des() : TPtrC();
    }

// ----------------------------------------------------------
// CCmsServerContact::Contact
// 
// ----------------------------------------------------------
//
const MVPbkStoreContact& CCmsServerContact::Contact() const
    {
    __ASSERT_DEBUG( iContact, CCmsServer::PanicServer( ENullContactHandle ) );
    return *iContact;
    }

// ----------------------------------------------------------
// CCmsServerContact::HandleField
// 
// ----------------------------------------------------------
//
TBool CCmsServerContact::HandleField( HBufC* aFieldData )
    {
    PRINT( _L(" Start CCmsServerContact::HandleField( 16-bit )" ) );
    
    RMessage2* message = NULL;
    TBool clearToDelete = ETrue;
    TPtr desc( aFieldData->Des() );
    const TInt clientDescLength( ClientDesLength( KSecondParam, message ) );
    if( clientDescLength >= desc.Length() )
        {
        
        TInt error( message->Write( KSecondParam, desc ) );
        message->Complete( error );
        }
    else if( clientDescLength < KErrNone )
        {
        message->Complete( clientDescLength );
        }
    else
        {
        clearToDelete = EFalse;
        iCachedField16 = aFieldData;
        message->Complete( desc.Length() );
        }
    PRINT( _L(" End CCmsServerContact::HandleField( 16-bit )" ) );
        
    return clearToDelete;
    }

// ----------------------------------------------------------
// CCmsServerContact::ClientDesLength
// 
// ----------------------------------------------------------
//
TInt CCmsServerContact::ClientDesLength( TInt aMessageSlot, RMessage2*& aMessage )
    {
    TInt clientDesLength = KErrGeneral;
    //Both of the messages must not be NULL; if they are, the code panics
    //and, once the smoke has cleared, we need to find out what is wrong
    if( iPresenceNotifyMessage.IsNull() )
        {
        aMessage = &iMessage;
        clientDesLength = iMessage.GetDesMaxLength( aMessageSlot );
        }
    else
        {
        aMessage = &iPresenceNotifyMessage;
        clientDesLength = iPresenceNotifyMessage.GetDesMaxLength( aMessageSlot );
        }
    return clientDesLength;                        
    }

// ----------------------------------------------------------
// CCmsServerContact::HandleField
// 
// ----------------------------------------------------------
//
TBool CCmsServerContact::HandleField( HBufC8* aFieldData )
    {
    PRINT( _L(" Start CCmsServerContact::HandleField()" ) );
    
    RMessage2* message = NULL;
    TBool clearToDelete = ETrue;
    TPtrC8 desc( aFieldData ? aFieldData->Des() : TPtrC8() );
    const TInt clientDescLength( ClientDesLength( KSecondParam, message ) );
    if( clientDescLength >= desc.Length() )
        {
        TInt error( message->Write( KSecondParam, desc ) );
        message->Complete( error );
        }
    else if( clientDescLength < KErrNone )
        {
        message->Complete( clientDescLength );
        }
    else
        {
        clearToDelete = EFalse;
        iCachedField8 = aFieldData;
        message->Complete( desc.Length() );
        }
    PRINT( _L(" End CCmsServerContact::HandleField()" ) );    
    return clearToDelete;
    }

// ----------------------------------------------------------
// CCmsServerContact::HandleEnabledFields
// 
// ----------------------------------------------------------
//
TBool CCmsServerContact::HandleEnabledFields( HBufC* aEnabledFields )
    {
    PRINT( _L(" Start CCmsServerContact::HandleEnabledFields()" ) );
    
    RMessage2* message = NULL;
    TBool clearToDelete = ETrue;
    const TPtrC fields( aEnabledFields->Des() );
    const TInt clientDescLength( ClientDesLength( 0, message ) );
    if( clientDescLength >= fields.Length() )
        {
        TInt error( message->Write( KFirstParam, fields ) );
        message->Complete( error );
        }
    else
        {
        clearToDelete = EFalse;
        iCachedField16 = aEnabledFields;
        message->Complete( fields.Length() );
        }
    PRINT( _L(" End CCmsServerContact::HandleEnabledFields()" ) );    
    return clearToDelete;
    }

// ----------------------------------------------------------
// CCmsServerContact::HandleError
// 
// ----------------------------------------------------------
//
void CCmsServerContact::HandleError( TInt aError )
    {
    PRINT( _L( "Start CCmsServerContact::HandleError()" ) );
    PRINT1( _L( "CCmsServerContact::HandleError(): Error:   %d" ), aError );
        
    iMessage.Complete( aError );
    PRINT( _L( "End CCmsServerContact::HandleError()" ) );    
    }

// ----------------------------------------------------------
// CCmsServerContact::HandleInfosL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::HandleInfosL( TInt /*aErrorCode*/, RPointerArray<CBPASInfo>& aInfos )
    {
    PRINT( _L(" Start CCmsServerContact::HandleInfosL()" ) );
    PRINT1( _L( "CCmsServerContact::HandleInfosL():Info count: %d" ), aInfos.Count() );
    
    if( 0 < aInfos.Count() )
        {
        TRAPD( error, DeliverPresenceDataL( aInfos ) );
        if( error != KErrNone )
            {
            iMessage.Complete( KErrNotFound );
            }
        } 
    else
        {
        iMessage.Complete( KErrNotFound );
        }
    PRINT( _L(" End CCmsServerContact::HandleInfosL()" ) );
    }

// ----------------------------------------------------------
// CCmsServerContact::HandleSubscribedInfoL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::HandleSubscribedInfoL( CBPASInfo* aInfo )
    {
    PRINT( _L(" Start CCmsServerContact::HandleSubscribedInfoL()" ) );
    PRINT1( _L( "CCmsServerContact::HandleSubscribedInfoL(): URI: %S" ), (&aInfo->Identity()) );
        
    if( aInfo )
        {
        HBufC8* data = NULL;
        if( aInfo->HasPresence() )
            {
            CBPASIconInfo* iconInfo = aInfo->IconInfo();
            CBPASServiceInfo* serviceInfo = aInfo->ServiceInfo();
            TPtrC8 brandId( serviceInfo ? serviceInfo->BrandId() : TPtrC8() );
            TPtrC8 elementId( iconInfo ? iconInfo->ImageElementId() : TPtrC8() );
            data = CompileBrandInfoDataLC( brandId, elementId );
            }
        else
            {
            data = CompileBrandInfoDataLC( TPtrC8(), TPtrC8() );
            }
        SendOrQueueBrandedDataL( data->Des() );
        CleanupStack::PopAndDestroy();  //data
        }
    else 
        {
        CCmsServer::PanicServer( ENullBPASHandle );
        }
    
    PRINT( _L(" End CCmsServerContact::HandleSubscribedInfoL()" ) );
    }

// ----------------------------------------------------------
// CCmsServerContact::SendOrQueueBrandDataL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::SendOrQueueBrandedDataL( const TDesC8& aData )
    {
    PRINT( _L(" Start CCmsServerContact::SendOrQueueBrandedDataL()" ) );
    
    if( !iPresenceNotifyMessage.IsNull() )
        {
        TInt error( iPresenceNotifyMessage.Write( KSecondParam, aData ) );
        iPresenceNotifyMessage.Complete( error ); 
        }
    else  //Add to queue
        {
        PRINT( _L( "CCmsServerContact::SendOrQueueBrandedDataL( ):Message == NULL => Add to queue" ) );
        
        CCmsNotifyEvent* event = CCmsNotifyEvent::NewL( aData, *this );
        CleanupStack::PushL( event );
        User::LeaveIfError( iPresenceEvents.Append( event ) );
        CleanupStack::Pop();  //event
        }
    PRINT( _L(" End CCmsServerContact::SendOrQueueBrandedDataL()" ) );    
    }

// ----------------------------------------------------------
// CCmsServerContact::CompileBrandInfoDataLC
// 
// ----------------------------------------------------------
//
HBufC8* CCmsServerContact::CompileBrandInfoDataLC( const TDesC8& aBrandId, const TDesC8& aElementId )
    {
    PRINT( _L(" Start CCmsServerContact::CompileBrandInfoDataL()" ) );
    PRINT1( _L( "CCmsServerContact::CompileBrandInfoDataLC(): Brand ID:   %S" ), &aBrandId );
    PRINT1( _L( "CCmsServerContact::CompileBrandInfoDataLC(): Element ID: %S" ), &aElementId );
    
    HBufC8* packetBuf = HBufC8::NewLC( aBrandId.Length() + aElementId.Length() +
                                     ( KDelimiterNewLine().Length() * 2 ) );
    TPtr8 packetDesc( packetBuf->Des() );
    packetDesc.Copy( aBrandId );
    packetDesc.Append( KDelimiterNewLine );
    packetDesc.Append( aElementId );
    packetDesc.Append( KDelimiterNewLine );
    PRINT( _L(" End CCmsServerContact::CompileBrandInfoDataL()" ) );
    return packetBuf;
    }

// ----------------------------------------------------------
// CCmsServerContact::ConstructIconInfoL
// 
// ----------------------------------------------------------
//
HBufC8* CCmsServerContact::ConstructIconInfoL( const TDesC8& aBrandId, const TDesC8& aTextId,
                                               const TDesC8& aImageId, TInt aLangId, 
                                               CCmsContactFieldItem::TCmsContactNotification aServiceTypeId )
    {
    PRINT( _L("Start CCmsServerContact::ConstructIconInfoL()" ) );
   
    TBuf8<32> temp( _L8( "" ) );
    CBufFlat* buffer = CBufFlat::NewL( KDefaultBinaryBufferSize );
    CleanupStack::PushL( buffer );
    temp.AppendNum( KPresenceDataBrandId );
    temp.Append( KDelimiterSpaceLit );
    buffer->InsertL( buffer->Size(), temp );
    buffer->InsertL( buffer->Size(), KDelimiterQuoteLit );
    buffer->InsertL( buffer->Size(), aBrandId );
    buffer->InsertL( buffer->Size(), KDelimiterQuoteLit );
    buffer->InsertL( buffer->Size(), KDelimiterSpaceLit );
    temp.Zero();
    temp.AppendNum( KPresenceDataElementTextId );
    temp.Append( KDelimiterSpaceLit );
    buffer->InsertL( buffer->Size(), temp );
    buffer->InsertL( buffer->Size(), KDelimiterQuoteLit );
    buffer->InsertL( buffer->Size(), aTextId ); 
    buffer->InsertL( buffer->Size(), KDelimiterQuoteLit );
    buffer->InsertL( buffer->Size(), KDelimiterSpaceLit );
    temp.Zero();
    temp.AppendNum( KPresenceDataElementImageId );
    temp.Append( KDelimiterSpaceLit );
    buffer->InsertL( buffer->Size(), temp );
    buffer->InsertL( buffer->Size(), KDelimiterQuoteLit );
    buffer->InsertL( buffer->Size(), aImageId );
    buffer->InsertL( buffer->Size(), KDelimiterQuoteLit );
    buffer->InsertL( buffer->Size(), KDelimiterSpaceLit );
    
    temp.Zero();
    temp.AppendNum( KPresenceDataLanguageId );
    temp.Append( KDelimiterSpaceLit );
    temp.Append( KDelimiterQuoteLit );
    temp.AppendNum( aLangId );
    temp.Append( KDelimiterQuoteLit );
    buffer->InsertL( buffer->Size(), temp );
    buffer->InsertL( buffer->Size(), KDelimiterSpaceLit );
    
    temp.Zero();
    temp.AppendNum( KPresenceDataServiceType );
    temp.Append( KDelimiterSpaceLit );
    temp.Append( KDelimiterQuoteLit );
    temp.AppendNum( aServiceTypeId );
    temp.Append( KDelimiterQuoteLit );
    buffer->InsertL( buffer->Size(), temp );
    buffer->InsertL( buffer->Size(), KDelimiterSpaceLit );    
    
    buffer->Compress();
    HBufC8* data = HBufC8::NewL( buffer->Size() );
    TPtr8 desc( data->Des() );
    buffer->Read( 0, desc, buffer->Size() );
    buffer->Reset();
    CleanupStack::PopAndDestroy();  //buffer
    PRINT( _L(" End CCmsServerContact::ConstructIconInfoL()" ) );
    
    return data;
    }

// ----------------------------------------------------------
// CCmsServerContact::DeliverPresenceDataL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::DeliverPresenceDataL( RPointerArray<CBPASInfo>& aInfos )
    {
    PRINT( _L(" Start CCmsServerContact::DeliverPresenceDataL()" ) );
    
    const TInt count = aInfos.Count();
    CBufSeg* buffer = CBufSeg::NewL( KDefaultBinaryBufferSize );
    CleanupStack::PushL( buffer );
    for( TInt i = 0;i < count;i++ )
        {
        CBPASIconInfo* iconInfo = aInfos[i]->IconInfo();
        CBPASServiceInfo* serviceInfo = aInfos[i]->ServiceInfo();
        const CBPASPresenceInfo* presInfo = aInfos[i]->PresenceInfo();
        if( iconInfo && serviceInfo )
            {
            HBufC8* iconData = ConstructIconInfoL( serviceInfo->BrandId(), iconInfo->TextElementId(),
                                                   iconInfo->ImageElementId(), serviceInfo->LanguageId(), 
                                                   CCmsContactFieldItem::ECmsUnknownNotification );
            CleanupStack::PushL( iconData );
            buffer->InsertL( buffer->Size(), iconData->Des() );
            CleanupStack::PopAndDestroy();
            if( presInfo )
                {
                PRINT( _L( "CCmsServerContact::DeliverPresenceDataL():Presence info is available" ) );
                
                TPtrC identity( aInfos[i]->Identity() );
                HBufC8* idBuffer8 = HBufC8::NewLC( identity.Length() );
                TPtr8 idDesc8( idBuffer8->Des() );
                idDesc8.Copy( identity );
                PRINT1( _L( "CCmsServerContact::DeliverPresenceDataL():Identity: %S" ), &idDesc8 );
                
                TBuf8<5> temp( _L8( "" ) );
                temp.AppendNum( KPresenceDataUserIdentity );
                temp.Append( KDelimiterSpaceLit );
                buffer->InsertL( buffer->Size(), temp );
                buffer->InsertL( buffer->Size(), KDelimiterQuoteLit );
                buffer->InsertL( buffer->Size(), idDesc8 );
                buffer->InsertL( buffer->Size(), KDelimiterQuoteLit );
                CleanupStack::PopAndDestroy();  //idBuffer8
                }
            buffer->InsertL( buffer->Size(), KDelimiterNewLine );
            }
        else
            {
            PRINT( _L( "CCmsServerContact::DeliverPresenceDataL():* No icon information available" ) );
                
            //Append a newline to indicate that 
            //icon information could not be found
            buffer->InsertL( buffer->Size(), KDelimiterNewLine );
            }
        }
    buffer->Compress();
    HBufC8* data = HBufC8::NewLC( buffer->Size() );
    TPtr8 desc( data->Des() );
    buffer->Read( 0, desc, buffer->Size() );
    buffer->Reset();
    HandleField( data ) ? CleanupStack::PopAndDestroy() : CleanupStack::Pop();  //data
    CleanupStack::PopAndDestroy();  //buffer
    PRINT( _L(" End CCmsServerContact::DeliverPresenceDataL()" ) );
    }

// ----------------------------------------------------------
// CCmsServerContact::CreateContactLinkArrayL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::CreateContactLinkArrayL( )
    {
    PRINT( _L(" Start CCmsServerContact::CreateContactLinkArrayL()" ) );
    
    
    delete iPackedContactLinkArray;
    iPackedContactLinkArray = NULL;
        
    CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC();     

    MVPbkContactLink* lnk = Contact().CreateLinkLC();
    //Check if lnk is created ok. 
    //If lnk is null then it's not in cleanupstack.
    if(NULL != lnk)
        {
        linkArray->AppendL( lnk );
        CleanupStack::Pop(); // lnk        
        } 
    else
        {
        PRINT( _L( "CCmsServerContact::CreateContactLinkArrayL(): CreateLinkLC FAILED!" ) );
                
        }               
    iPackedContactLinkArray = linkArray->PackLC(); 
    CleanupStack::Pop(); // iPackedContactLinkArray

    CleanupStack::PopAndDestroy(); // linkArray       
    PRINT( _L(" End CCmsServerContact::CreateContactLinkArrayL()" ) );
    }


// ----------------------------------------------------------
// CCmsServerContact::FetchContactLinkArrayL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::FetchContactLinkArrayL( const RMessage2& aMessage )
    {
    PRINT( _L(" Start CCmsServerContact::FetchContactLinkArrayL()" ) );
    
    
    __ASSERT_DEBUG( iContact, CCmsServer::PanicServer( ENullContactHandle ) );    
        
    TPtr8 desc( iPackedContactLinkArray->Des() );
    const TInt clientDescLength = aMessage.GetDesMaxLength( KFirstParam );
    if( clientDescLength >= desc.Length() )
        {
        
        TInt error( aMessage.Write( KFirstParam, desc ) );
        aMessage.Complete( error );
        }
    else
        {
        //IF client hasn't reserve enough memory, leave with KErrArgument
        
        aMessage.Complete( KErrArgument );
        }   
    PRINT( _L(" End CCmsServerContact::FetchContactLinkArrayL()" ) );                 
    }


// ----------------------------------------------------
// CCmsServerContact::BPASHandle
// 
// ----------------------------------------------------
// 
CBPAS& CCmsServerContact::BPASHandle()
    {
    return *iBrandedPresence;
    }

// ----------------------------------------------------
// CCmsServerContact::XSPHandler
// 
// ----------------------------------------------------
//
CCmsServerXSPContactHandler* CCmsServerContact::XSPHandler() const
    {
    return iXSPContactHandler;
    }

// ----------------------------------------------------
// CCmsServerContact::BPASHandle
// 
// ----------------------------------------------------
//
void CCmsServerContact::CacheData( HBufC* a16BitData )
    {
    iCachedField16 = a16BitData;
    }

// ----------------------------------------------------
// CCmsServerContact::BPASHandle
// 
// ----------------------------------------------------
//
void CCmsServerContact::CacheData( HBufC8* a8BitData )
    {
    iCachedField8 = a8BitData;
    }
        
// ----------------------------------------------------------
// CCmsServerContact::CancelOperation
// 
// ----------------------------------------------------------
//
void CCmsServerContact::CancelOperation()
    {
    PRINT( _L(" Start CCmsServerContact::CancelOperation()" ) );
    
    TBool active = EFalse;
    //Check if anything's active and cancel it
    if( iAsyncContact->IsActive() )
        {
        active = ETrue;
        iAsyncContact->Cancel();
        
        }
    if( active )
        {
        iMessage.Complete( KErrCancel );
        
        }
    //Clear all cached data
    delete iCachedField16;
    iCachedField16 = NULL;
    delete iCachedField8;
    iCachedField8 = NULL;
    PRINT( _L(" End CCmsServerContact::CancelOperation()" ) );
    }

void CCmsServerContact::CleanupResetAndDestroy(TAny*  aItem)
    {
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
        {
        RPointerArray<CRCSEProfileEntry> *pMyArray = (RPointerArray<CRCSEProfileEntry>*) aItem;
        pMyArray->ResetAndDestroy();
        pMyArray->Close();
        }
    }	

	
// ----------------------------------------------------
// CCmsServerContact::ParseVoIPAvailabilityL
//
// ----------------------------------------------------
//
TInt CCmsServerContact::ParseVoIPAvailabilityL()
    {
    PRINT( _L( "Start CCmsServerContact::ParseVoIPAvailabilityL()" ) );
    
    TInt bitter( 0 );
    TBool found( EFalse );

    if( CmsServerUtils::IsVoIPSupportedL() )
        {
        bitter |= ECmsVoIPSupportBasic;
        CheckServiceProviderSupportL( bitter );

        CSPSettings* settings = CSPSettings::NewLC();

        if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
            {
            RIdArray idArray;
            CleanupClosePushL(idArray);
            User::LeaveIfError( settings->FindServiceIdsL(idArray) );
		
            CRCSEProfileRegistry* profileRegistry = CRCSEProfileRegistry::NewLC();
            RPointerArray<CRCSEProfileEntry> entries;
            CleanupStack::PushL( TCleanupItem(CleanupResetAndDestroy, &entries) );

            // Check if we have atleast one SPSetting entry
            // Any entry in this array uses SIP protocol for VoIP
            for (TInt i = 0; !found && i < idArray.Count(); ++i)
                {
                profileRegistry->FindByServiceIdL( idArray[i], entries );
                if (entries.Count() > 0)
                    {
                    bitter |= ECmsVoIPSupportSip;
                    found = ETrue;
                    }
                }
		
            CleanupStack::PopAndDestroy( 3 ); //entries, profileRegistry, idArray,
            }
        
        if( settings->IsFeatureSupported( ESupportCallOutFeature ) )
            {
            bitter |= ECmsVoIPSupportCallout;
            }

        CleanupStack::PopAndDestroy(); // settings

        
        }
    PRINT( _L( "End CCmsServerContact::ParseVoIPAvailabilityL()" ) );    
    return bitter;
    }

// ----------------------------------------------------
// CCmsServerContact::StartNextFindOperationL
//
// ----------------------------------------------------
//
void CCmsServerContact::StartNextFindOperationL( TInt aNextFindStringPos )
    {
    MVPbkContactOperationBase*  XSPFindOperation = 
        iCmsServer.PhonebookProxyHandle().FindXSPContactL
            (iIdArray->MdcaPoint(aNextFindStringPos), *iXSPContactHandler );
    CleanupStack::PushL( XSPFindOperation );
    iXSPFindOperationsArray.AppendL( XSPFindOperation );
    CleanupStack::Pop( XSPFindOperation );
    }

// ----------------------------------------------------
// CCmsServerContact::CheckServiceProviderSupportL
// 
// ----------------------------------------------------
//
void CCmsServerContact::CheckServiceProviderSupportL( TInt& aBitFlag )
    {
    TBool found( EFalse );
    const MVPbkStoreContact& contact = Contact();
    const MVPbkFieldType* type = iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
    CVPbkBaseContactFieldTypeIterator* itr = CVPbkBaseContactFieldTypeIterator::NewLC( *type, contact.Fields() );
    while( itr->HasNext() && !found )
        {
        const MVPbkBaseContactField* field = itr->Next();
        const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( field->FieldData() );
        if( iAsyncContact->IsVoipCapableL( uri.Scheme() ) )
            {
            aBitFlag |= ECmsVoIPSupportXspId;
            found = ETrue;
            }
        }
    CleanupStack::PopAndDestroy();  //itr
    }

// ----------------------------------------------------
// CCmsServerContact::NewIconForContact
// 
// ----------------------------------------------------
//
void CCmsServerContact::NewIconForContact(
        MVPbkContactLink* /*aLink*/, 
        const TDesC8& aBrandId, 
        const TDesC8& aElementId,
        TInt aId,
        TInt aBrandLanguage )
	{
    PRINT1( _L( "CCmsServerContact::NewIconForContact(): aId=%d" ), aId );
    TRAP_IGNORE( DoNewIconForContactL( aBrandId, aElementId, aId, aBrandLanguage ));
    return;   
	}

// ----------------------------------------------------
// CCmsServerContact::NewIconsForContact
// 
// ----------------------------------------------------
//
void CCmsServerContact::NewIconsForContact( 
        MVPbkContactLink* /*aLink*/, 
        RPointerArray <MPresenceServiceIconInfo>& /*aInfoArray*/,
        TInt /*aId*/ )
    {
    // No real implementation needed.
    }

// ----------------------------------------------------
// CCmsServerContact::DoNewIconForContactL
// 
// ----------------------------------------------------
//
void CCmsServerContact::DoNewIconForContactL( 
        const TDesC8& aBrandId, 
        const TDesC8& aElementId,
        TInt aId,
        TInt aBrandLanguage )
	{ 	  
    HBufC8* data = ConstructIconInfoL( aBrandId, KNullDesC8, aElementId, aBrandLanguage, 
                   CCmsContactFieldItem::TCmsContactNotification(aId) );
    CleanupStack::PushL( data );    
    TInt const newSize = data->Length() + sizeof( KDelimiterNewLine );        
    CBufFlat* buffer = CBufFlat::NewL( newSize );
    CleanupStack::PushL( buffer );    
    buffer->InsertL( buffer->Size(), data->Des() );
    buffer->InsertL( buffer->Size(), KDelimiterNewLine );     
    SendOrQueueBrandedDataL( buffer->Ptr(0) );    
    CleanupStack::PopAndDestroy( buffer );    
    CleanupStack::PopAndDestroy( data );    
	}

// ----------------------------------------------------------
// CCmsServerContact::IsTopContactL
// 
// ----------------------------------------------------------
//
void CCmsServerContact::IsTopContactL( const RMessage2& aMessage )
    {
    PRINT( _L(" Start CCmsServerContact::IsTopContactL()" ) );    
    
    __ASSERT_DEBUG( iContact, CCmsServer::PanicServer( ENullContactHandle ) );    
    
    TBool isFav( EFalse );
    isFav = CVPbkTopContactManager::IsTopContact( *iContact );
    
    TPckgC<TInt> isTopContact( isFav );        
    aMessage.WriteL( KFirstParam, isTopContact );       
    
    aMessage.Complete( KErrNone );
    
    PRINT( _L(" End CCmsServerContact::IsTopContactL()" ) );                 
    }
// ----------------------------------------------------
// CCmsServerContact::VPbkSingleContactOperationComplete
// 
// ----------------------------------------------------
//
void CCmsServerContact::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    TRAPD( res, ContactReadyL( KErrNone, aContact ) );
    iCmsServer.CmsSingleContactOperationComplete( res );
    }

// ----------------------------------------------------
// CCmsServerContact::VPbkSingleContactOperationFailed
// 
// ----------------------------------------------------
//
void CCmsServerContact::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aError )
    {
    TRAP_IGNORE ( ContactReadyL( aError, NULL ) );
    iCmsServer.CmsSingleContactOperationComplete( aError );
    }

// ----------------------------------------------------
// CCmsServerContact::FetchContactL
// 
// ----------------------------------------------------
//
void CCmsServerContact::FetchContactL( MVPbkContactLink* aContactLinkToFetch )
    {
    if( iOperation )
        {
        delete iOperation;
        iOperation = NULL;
        }
    iOperation = iContactManager->RetrieveContactL( *aContactLinkToFetch, *this );
    }
// End of File
