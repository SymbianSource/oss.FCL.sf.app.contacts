/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  contact presence
*
*/


// INCLUDE FILES

#include <e32std.h>

#include <CVPbkContactLinkArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactLink.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>

#include <mcontactpresenceobs.h>

#include "mpresencetrafficlights.h"
#include "mpresencetrafficlightsobs.h"
#include "contactpresence.h"
#include "contactstoreaccessor.h"
#include "presenceiconinfo.h"
#include "presenceiconinfores.h"

#ifdef _TEST_MODE
#include "testmodeutils.h"
#endif


// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CPresenceIconInfo::CPresenceIconInfo
// ----------------------------------------------------------
//
CPresenceIconInfo::CPresenceIconInfo(
    CPresenceIconInfoListener& aParent,
    MContactPresenceObs& aObs,
    MPresenceTrafficLights& aIcons,
    TBool aIsSubscription,
    TInt aOpId )
    : iParent( aParent), iObs( aObs ),
    iDestroyedPtr( NULL ),
    iIcons( aIcons ),
    iWaiting(ETrue),
    iInitialization(EFalse), iNotifyPending(EFalse), 
    iSubscription(aIsSubscription), iOpId(aOpId)
    {
    }

// ----------------------------------------------------------
// CPresenceIconInfo::~CPresenceIconInfo
// ----------------------------------------------------------
//
CPresenceIconInfo::~CPresenceIconInfo()
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::~CPresenceIconInfo this=%d" ),(TInt)this );
#endif
    ResetData();
    
    delete iContactLink;
    iContactLink = NULL;

    delete iPbkContactLink;
    iPbkContactLink = NULL;
    
    iPendings.Close();
    iPendings2.ResetAndDestroy();
    iPendings2.Close();    

    if ( iDestroyedPtr )
        {
        // We are called inside callback
        *iDestroyedPtr = ETrue;
        iDestroyedPtr = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CPresenceIconInfo::Destroy
// -----------------------------------------------------------------------------
void CPresenceIconInfo::Destroy()
    {    
    if ( iPbkContactLink && IsSubscription() )
        {         
        iIcons.UnsubscribeBrandingForContact( iPbkContactLink, this );
        }
    else
        {      
        }
    iLink.Deque();
    delete this;
    }
// ----------------------------------------------------------
// CPresenceIconInfo::NewL
// ----------------------------------------------------------
//
CPresenceIconInfo* CPresenceIconInfo::NewL(
        CPresenceIconInfoListener& aParent,
        MContactPresenceObs& aObs,
        const TDesC8& aPackedContactId,
        MPresenceTrafficLights& aIcons,
        TBool aIsSubscription,
        TInt aOpId )
    {
    CPresenceIconInfo* self = new (ELeave) CPresenceIconInfo( aParent, aObs, aIcons, aIsSubscription, aOpId );
    CleanupStack::PushL( self );
    self->ConstructL(  aPackedContactId );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresenceIconInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CPresenceIconInfo::ConstructL( const TDesC8& aPackedContactId )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::ConstructL this=%d" ),(TInt)this );
#endif
    iContactLink = aPackedContactId.AllocL();
    }

// ----------------------------------------------------------
// CPresenceIconInfo::ContactLink
// ----------------------------------------------------------
//
TPtrC8 CPresenceIconInfo::ContactLink()
    {
    return iContactLink ? iContactLink->Des() : TPtrC8();
    }

// ----------------------------------------------------------
// CPresenceIconInfo::StartGetIconInfoL
// ----------------------------------------------------------
//
void CPresenceIconInfo::StartGetIconInfoL(  )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::StartGetIconInfoL this=%d" ),(TInt)this );
#endif    
    FetchContactL();
    iWaiting = EFalse;    
    }

 // ----------------------------------------------------------
 // CPresenceIconInfo::FetchContactL
 //
 // ----------------------------------------------------------
 //
void CPresenceIconInfo::FetchContactL( )
     {
#ifdef _DEBUG
     CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::FetchContactL this=%d" ),(TInt)this );
#endif
     ResetData();
     CVPbkContactLinkArray* lArray = CVPbkContactLinkArray::NewLC(
             ContactLink() , *(iParent.ContactStoreAccessor()->AccessStoreList()) );

     iLinkArray = lArray;
     CleanupStack::Pop();  //lArray
     
     if ( !iLinkArray || !iLinkArray->Count() )
    	 {
    	 User::Leave( KErrNotFound );
    	 }
     iOperation = iParent.ContactStoreAccessor()->AccessContactManager()->RetrieveContactL( iLinkArray->At( 0 ), *this );
     }

 // ----------------------------------------------------------
 // CPresenceIconInfo::ResetData
 //
 // ----------------------------------------------------------
 //
 void CPresenceIconInfo::ResetData()
     {
     delete iLinkArray;
     iLinkArray = NULL;
     delete iOperation;
     iOperation = NULL;
     }

 // ----------------------------------------------------------
 // CPresenceIconInfo::ResetData
 //
 // ----------------------------------------------------------
 //
 void CPresenceIconInfo::ResetData( MVPbkContactOperationBase& aOperation )
     {
     delete iLinkArray;
     iLinkArray = NULL;
     if ( iOperation == &aOperation )
         {
         delete iOperation;
         iOperation = NULL;
         }
     }

 // ----------------------------------------------------------
 // CPresenceIconInfo::VPbkSingleContactOperationComplete
 //
 // ----------------------------------------------------------
 //
 void CPresenceIconInfo::VPbkSingleContactOperationComplete( 
     MVPbkContactOperationBase& aOperation,
     MVPbkStoreContact* aContact )
     {
#ifdef _DEBUG
     CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::VPbkSingleContactOperationComplete this=%d" ),(TInt)this );
#endif
     iInitialization = ETrue;     
     ResetData( aOperation );
       
     TRAPD( errx, DoVPbkSingleContactOperationCompleteL( aOperation, aContact ));
     iInitialization = EFalse;       
     if ( errx )
         {
         SendErrorCallback( errx );
         }
     else
         {
         SendPendingCallback();
         }
     return;
     }

 // ----------------------------------------------------------
 // CPresenceIconInfo::DoVPbkSingleContactOperationCompleteL
 //
 // ----------------------------------------------------------
 //
 void CPresenceIconInfo::DoVPbkSingleContactOperationCompleteL( 
     MVPbkContactOperationBase& /*aOperation*/,
     MVPbkStoreContact* aContact )
     {
#ifdef _DEBUG
     CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::DoVPbkSingleContactOperationComplete this=%d" ),(TInt)this );
#endif
          
#ifdef _TEST_MODE     
     // check if not deleted in callback
     TBool destroyed( EFalse );
     iDestroyedPtr = &destroyed; 
#endif  
     
     aContact->PushL();
           
     MVPbkContactLink* cLink = aContact->CreateLinkLC();
     iPbkContactLink = cLink;
     CleanupStack::Pop( ); // cLink
     
     // get data from db
     CDesCArrayFlat* identities = FetchContactFieldsLC( aContact );
          
     if ( iSubscription )
         {        
         // start to subscribe icon info notifications         
         iIcons.SubscribeBrandingForContactL( iPbkContactLink, identities, this, 0 );           
         }
     else
         {
         // get service icons once
         iIcons.GetAllBrandingsForContactL( iPbkContactLink, identities, this, iOpId );                  
         }
           
     CleanupStack::PopAndDestroy( identities );
     CleanupStack::PopAndDestroy( ); // aContact
                        
     // ---------------------------------------------------------             
#ifdef _TEST_MODE
    /**
     * INTERNAL TEST SUITE
     */
               
     // the Subscribe method above may call callback methods immediately and 
     // the client may have deleted us.
     if ( destroyed )
         {
         return;
         } 
     
    iDestroyedPtr = NULL;
    const TInt KTestCase = 3; 
    if ( TestModeUtils::TestCase() == KTestCase )
        {
        User::Leave( KErrGeneral );
        }
#endif
// --------------------------------------------------------- 
             
     return;
     }

 // ----------------------------------------------------------
 // CPresenceIconInfo::VPbkSingleContactOperationFailed
 //
 // ----------------------------------------------------------
 //
 void CPresenceIconInfo::VPbkSingleContactOperationFailed( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
     {
#ifdef _DEBUG
     CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::VPbkSingleContactOperationComplete this=%d" ),(TInt)this );
#endif
     ResetData();
     SendErrorCallback( aError );
     }

 // ----------------------------------------------------------
 // CPresenceIconInfo::FetchContactFieldsLC
 //
 // ----------------------------------------------------------
 //
 CDesCArrayFlat* CPresenceIconInfo::FetchContactFieldsLC( MVPbkStoreContact* aContact )
     {
#ifdef _DEBUG
     CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::FetchContactFieldsLC this=%d" ),(TInt)this );
#endif
     const TInt myCount = sizeof( KPresenceFieldGroup ) / sizeof( KPresenceFieldGroup[0]);
     CDesCArrayFlat* fieldArray = new ( ELeave ) CDesCArrayFlat( myCount );
     CleanupStack::PushL( fieldArray );
     for( TInt i = 0;i < myCount;i++ )
         {
         FetchContactFieldL( KPresenceFieldGroup[i], *fieldArray, aContact );
         }
     return fieldArray;
     }

 // ----------------------------------------------------------
 // CPresenceIconInfo::FetchContactFieldL
 //
 // ----------------------------------------------------------
 //
void CPresenceIconInfo::FetchContactFieldL( TInt aResourceId, CDesCArrayFlat& aArray, MVPbkStoreContact* aContact )
     {
#ifdef _DEBUG
     CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::FetchContactFieldL this=%d" ),(TInt)this );
#endif
     const MVPbkFieldType* type = iParent.ContactStoreAccessor()->AccessContactManager()->FieldTypes().Find( aResourceId );

     CVPbkBaseContactFieldTypeIterator* itr = CVPbkBaseContactFieldTypeIterator::NewLC( *type, aContact->Fields() );
     while ( itr->HasNext() )
         {
         const MVPbkBaseContactField* field = itr->Next();
         if ( aResourceId == R_VPBK_FIELD_TYPE_IMPP )
             {
             const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( field->FieldData() );
             aArray.AppendL( uri.Uri() );
             }
         else
             {
             const MVPbkContactFieldTextData& text = MVPbkContactFieldTextData::Cast( field->FieldData() );
             aArray.AppendL( text.Text() );
             }
         }
     CleanupStack::PopAndDestroy( itr );
     return;
     }

// ----------------------------------------------------------
//
void CPresenceIconInfo::NewIconForContact(
    MVPbkContactLink* /*aLink*/,
    const TDesC8& aBrandId,
    const TDesC8& aElementId,
    TInt /*aId*/,
    TInt /*aBrandLanguage*/ )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::NewIconForContact this=%d" ),(TInt)this );
#endif
    
    if ( !iInitialization )
        {
#ifdef _DEBUG
        CContactPresence::WriteToLog( _L8( "CPresenceIconInfo:: CALLBACK ReceiveIconInfoL this=%d" ),(TInt)this );
#endif        
        TRAP_IGNORE( iObs.ReceiveIconInfoL( ContactLink(), aBrandId, aElementId ));
        }
    else
        {
        TRAP_IGNORE( AllocatePendingNotifyL( aBrandId, aElementId ));
        }
    }

// -----------------------------------------------------------------------------    
void CPresenceIconInfo::NewIconsForContact( 
        MVPbkContactLink* aLink, 
        RPointerArray <MPresenceServiceIconInfo>& aInfoArray,
        TInt aId )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog(_L8("CPresenceIconInfo::NewIconsForContact this=%d"),(TInt)this);
#endif 
    
    TRAP_IGNORE( DoCopyInfosL( aLink, aInfoArray, aId ));
    }

// ----------------------------------------------------------
// CPresenceIconInfo::IsWaiting
//
// ----------------------------------------------------------
//
TBool CPresenceIconInfo::IsWaiting()
    {
    return iWaiting;
    }

// ----------------------------------------------------------
// CPresenceIconInfo::SendErrorCallback
//
// ----------------------------------------------------------
//
void CPresenceIconInfo::SendErrorCallback( TInt aError )
    {
    TBool destroyed( EFalse );
    iDestroyedPtr = &destroyed;
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfo:: CALLBACK PresenceSubscribeError this=%d" ),(TInt)this );
#endif
    if ( IsSubscription() )
        {
        iObs.PresenceSubscribeError( ContactLink(), aError );        
        }
    else
        {
        iObs.ErrorOccured( OpId(), aError );
        }
    
    if ( !destroyed )
        {
        Destroy();
        }
    else
        {
        }    
    }

// ----------------------------------------------------------
void CPresenceIconInfo::AllocatePendingNotifyL(
    const TDesC8& aBrandId,
    const TDesC8& aElementId )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::AllocatePendingNotifyL this=%d" ),(TInt)this );
#endif

    MContactPresenceInfo* myInfo = NULL;            
    CPresenceIconInfoRes* res = 
            CPresenceIconInfoRes::NewL( aBrandId, aElementId, KNullDesC, 0 );
    myInfo = res;
    iPendings2.AppendL( res );     
    iPendings.AppendL( myInfo );       
    
    iNotifyPending = ETrue;
    }

// ----------------------------------------------------------
void CPresenceIconInfo::SendPendingCallback()
    {
    TBool destroyed( EFalse );
    iDestroyedPtr = &destroyed;    
    TInt count = iPendings.Count();    
        
    if ( iNotifyPending && count )
        {    
        CPresenceIconInfoRes* info = iPendings2[0];        
        if ( IsSubscription() )
            {
#ifdef _DEBUG
            CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::SendPendingCallback ReceiveIconInfoL CALLBACK this=%d" ),(TInt)this );
#endif            
            TRAP_IGNORE( iObs.ReceiveIconInfoL( ContactLink(), info->BrandId(), info->ElementId() ));            
            }
        else
            {
#ifdef _DEBUG            
            CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::SendPendingCallback ReceiveIconInfosL CALLBACK this=%d" ),(TInt)this );
#endif              
            TRAP_IGNORE( iObs.ReceiveIconInfosL( ContactLink(), iPendings, info->OpId() ));  
            // This instance is not needed anymore
            Destroy();            
            }         
        }
    else if ( iNotifyPending )
        {
#ifdef _DEBUG
        CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::SendPendingCallback ReceiveIconInfosL CALLBACK this=%d" ),(TInt)this );
#endif         
        // Empty notification
        TRAP_IGNORE( iObs.ReceiveIconInfosL( ContactLink(), iPendings, iOpId ));  
        // This instance is not needed anymore
        Destroy();         
        }
    
    if ( !destroyed )
        {
        iNotifyPending = EFalse;        
        iPendings2.ResetAndDestroy();        
        iPendings.Reset();        
        iDestroyedPtr = NULL;        
        }
    else
        {        
        }
    return;
    }

// ----------------------------------------------------------
void CPresenceIconInfo::SendEmptyNotification()
    {

#ifdef _DEBUG
        CContactPresence::WriteToLog( _L8( "CPresenceIconInfo::SendEmptyNotification ReceiveIconInfoL CALLBACK this=%d" ),(TInt)this );
#endif  
    TRAP_IGNORE( iObs.ReceiveIconInfoL( ContactLink(), TPtrC8(), TPtrC8() ));
    return;
    }

// ----------------------------------------------------------

TBool CPresenceIconInfo::IsSubscription()
    {
    return iSubscription;
    }

// ----------------------------------------------------------

// ----------------------------------------------------------

TInt CPresenceIconInfo::OpId()
    {
    return iOpId;
    }



// -----------------------------------------------------------------------------    
void CPresenceIconInfo::DoCopyInfosL( 
        MVPbkContactLink* /*aLink*/, 
        RPointerArray <MPresenceServiceIconInfo>& aInfoArray,
        TInt aId )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog(_L8("CPresenceIconInfo::DoCopyInfosL this=%d"),(TInt)this);
#endif 
    
    MContactPresenceInfo* myInfo = NULL;  
        
    TInt count = aInfoArray.Count();
    
    // Empty notification is handled here, i.e. no data in presence cache curently.
    if ( count == 0 )
        {
        iOpId = aId;       
        }
    else
        {
        // Convert input to other array
        for ( TInt i = 0; i < count; i++ )
            {
            MPresenceServiceIconInfo* inf = aInfoArray[i];
            CPresenceIconInfoRes* res = 
                CPresenceIconInfoRes::NewL( inf->BrandId(), inf->ElementId(), inf->ServiceName(), aId );
            myInfo = res;
            iPendings2.AppendL( res );           
            iPendings.AppendL( myInfo );     
            }        
        }
    
    if ( !iInitialization )
        {
#ifdef _DEBUG
        CContactPresence::WriteToLog( _L8( "CPresenceIconInfo:: CALLBACK ReceiveIconInfosL this=%d" ),(TInt)this );
#endif
        TRAP_IGNORE( iObs.ReceiveIconInfosL( ContactLink(), iPendings, aId  ));
        // This instance is not needed anymore
        Destroy();
        }
    else
        {
        iNotifyPending = ETrue;        
        }    
    }

// ----------------------------------------------------------
