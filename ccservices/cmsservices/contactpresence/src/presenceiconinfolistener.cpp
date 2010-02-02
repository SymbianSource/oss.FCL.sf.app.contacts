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
* Description: 
*
*/


#include <mcontactpresenceobs.h>
#include <MVPbkContactLink.h>

#include "contactpresence.h"
#include "presenceiconinfo.h"
#include "mpresencetrafficlights.h"
#include "contactstoreaccessor.h"
#include "presenceiconinfolistener.h"

#ifdef _TEST_MODE
#include "testmodeutils.h"
#endif

// ----------------------------------------------------------
//
CPresenceIconInfoListener::CPresenceIconInfoListener( MContactPresenceObs& aObs )
: CActive(0), iObs( aObs ),
iInfoReqList(CPresenceIconInfo::LinkOffset()),
iStoreOpen(EFalse), iDestroyedPtr( NULL )
    {
    // No implementation required
    }

// ----------------------------------------------------------
//
CPresenceIconInfoListener::~CPresenceIconInfoListener()
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog(_L8("CPresenceIconInfoListener::destructor this=%d" ),(TInt)this);
#endif

    DeleteAllRequests();

    delete iBrandedIcons;
    iBrandedIcons = NULL;

    Cancel();

    delete iStoreAccess;
    iStoreAccess = NULL;

    if ( iDestroyedPtr )
        {
        // We are called inside callback
        *iDestroyedPtr = ETrue;
        iDestroyedPtr = NULL;
        }
    }

// ----------------------------------------------------------
//
CPresenceIconInfoListener* CPresenceIconInfoListener::NewL(
    CVPbkContactManager* aManager,
    MContactPresenceObs& aObs )
    {
    CPresenceIconInfoListener* self = new (ELeave)CPresenceIconInfoListener( aObs );
    CleanupStack::PushL(self);
    self->ConstructL( aManager );
    CleanupStack::Pop(); // self;
    return self;
    }

// ----------------------------------------------------------
//
void CPresenceIconInfoListener::ConstructL( CVPbkContactManager* aManager )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog(_L8("CPresenceIconInfoListener::ConstructL this=%d" ),(TInt)this);
#endif
    iBrandedIcons = MPresenceTrafficLights::NewL( );
    iStoreAccess =  CContactStoreAccessor::NewL( aManager, *this );

    CActiveScheduler::Add(this);
    }

// ----------------------------------------------------------
// CPresenceIconInfoListener::SubscribePresenceInfoL
// ----------------------------------------------------------
//
void CPresenceIconInfoListener::SubscribePresenceInfoL(
        const MDesC8Array& aLinkArray  )
    {
    TInt count = aLinkArray.MdcaCount();
#ifdef _DEBUG
    CContactPresence::WriteToLog(
    _L8( "CPresenceIconInfoListener::SubscribePresenceInfoL array count=%d this=%d" ),count,(TInt)this);
#endif    
    for ( TInt i=0; i < count; i++ )
        {
        CPresenceIconInfo* infoH = GetSinglePresenceIconInfoL( aLinkArray.MdcaPoint(i), ETrue, 0 );
        if ( iStoreOpen )
            {
            TRAPD( err, infoH->StartGetIconInfoL( ));
            if ( err )
                {
                infoH->Destroy();
                User::Leave( err );
                }
            }
        else
            {
            OpenStoreL( aLinkArray.MdcaPoint(i) );
            }
        }
    }

// ----------------------------------------------------------
//
void CPresenceIconInfoListener::GetPresenceInfoL(
    const TDesC8& aPackedLink, TInt aOpId )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog(
    _L8( "CPresenceIconInfoListener::GetPresenceInfoL this=%d" ), (TInt)this);
#endif    

    CPresenceIconInfo* infoH = GetSinglePresenceIconInfoL( aPackedLink, EFalse, aOpId );
    if ( iStoreOpen )
        {
        TRAPD( err, infoH->StartGetIconInfoL( ));
        if ( err )
            {
            infoH->Destroy();
            User::Leave( err );
            }
        }
    else
        {
        OpenStoreL();
        }

    }

// ----------------------------------------------------------
//
void CPresenceIconInfoListener::ResubscribePresenceInfoL( MVPbkContactLink* aLink )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfoListener::ResubscribePresenceInfoL this=%d" ), (TInt)this );
#endif    
    DoResubscribePresenceInfoL( aLink, ETrue );    
    }

// ----------------------------------------------------------
//
CPresenceIconInfo* CPresenceIconInfoListener::GetSinglePresenceIconInfoL(
    const TDesC8& aContactLink, TBool aIsSubscription, TInt aOpId )
    {
    CPresenceIconInfo* infoH = NULL;
    if ( aIsSubscription )
        {
        infoH = SearchIconInfo( aContactLink );        
        }
    else
        {
        // get method always creates own instance
        }
    if ( !infoH  )
        {
        infoH = CreateIconInfoL( aContactLink, aIsSubscription, aOpId );
        }
    return infoH;
    }

// ----------------------------------------------------------
//
CPresenceIconInfo* CPresenceIconInfoListener::SearchIconInfo( const TDesC8& aContactLink )
    {
    TDblQueIter<CPresenceIconInfo> rIter( iInfoReqList );
    rIter.SetToFirst();

    while ( rIter )
        {
        CPresenceIconInfo* req = rIter;
        rIter++;
        if ( !req->ContactLink().Compare( aContactLink ))
            {
            return req;
            }
        }
    return (CPresenceIconInfo*)NULL;
    }

// ----------------------------------------------------------
//
CPresenceIconInfo* CPresenceIconInfoListener::CreateIconInfoL(
    const TDesC8& aContactLink, TBool aIsSubscription, TInt aOpId )
    {
    CPresenceIconInfo* info = CPresenceIconInfo::NewL( *this, iObs, aContactLink, *iBrandedIcons, aIsSubscription, aOpId );
    iInfoReqList.AddLast( *info );
    return info;
    }

// ----------------------------------------------------------
// CPresenceIconInfoListener::OpenStoreL
// ----------------------------------------------------------
//
void CPresenceIconInfoListener::OpenStoreL()
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog(_L8("CPresenceIconInfoListener::OpenStoreL 1 this=%d" ),(TInt)this);
#endif
   
    if ( !IsActive() )
        {
        iStoreAccess->OpenStoresL( iStatus );
        SetActive();
        }
    }


// ----------------------------------------------------------
// CPresenceIconInfoListener::OpenStoreL
// ----------------------------------------------------------
//
void CPresenceIconInfoListener::OpenStoreL( const TDesC8& aPackedLink )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog(_L8("CPresenceIconInfoListener::OpenStoreL 2 this=%d" ),(TInt)this);
#endif
   
    if ( !IsActive() )
        {
        iStoreAccess->OpenStoresL( iStatus, aPackedLink  );
        SetActive();
        }
    }

// ----------------------------------------------------------
//
void CPresenceIconInfoListener::RunL()
    {
    TInt myStatus = iStatus.Int();
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfoListener::RunL starts status=%d" ), myStatus );
#endif

    if ( !myStatus )
        {
        iStoreOpen = ETrue;
        TRAPD( errx, StartPendingsL() );
        if ( errx )
            {
            SendErrorCallbacks( errx );
            return;
            }
        }
    else
        {
        iStoreOpen = EFalse;
        SendErrorCallbacks( myStatus );
        }  
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfoListener::RunL ends" ) );
#endif    
    return;
    }

// ----------------------------------------------------------
//
void CPresenceIconInfoListener::DoCancel()
    {
    delete iStoreAccess;
    iStoreAccess = NULL;
    }

// ----------------------------------------------------------
//
void CPresenceIconInfoListener::StartPendingsL( )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfoListener::StartPendingsL" ) );
#endif

#ifdef _TEST_MODE
    const TInt myTestCase = 2; 
    // TestModeUtils::SetMemError( myTestCase );  
    if ( TestModeUtils::TestCase() == myTestCase )
        {
        User::Leave( KErrGeneral );
        }
#endif
    	
    TDblQueIter<CPresenceIconInfo> rIter( iInfoReqList );
    rIter.SetToFirst();  
    while ( rIter )
        {
        CPresenceIconInfo* req = rIter;
        rIter++;
        req->StartGetIconInfoL( );
        // Clean failed and pending requests in RunError/SendErrorCallbacks.
        }     
    }

// ----------------------------------------------------------
//
CContactStoreAccessor* CPresenceIconInfoListener::ContactStoreAccessor()
    {
    return iStoreAccess;
    }

// ----------------------------------------------------------
// CPresenceIconInfoListener::CancelSubscribePresenceInfo
// ----------------------------------------------------------
//
void CPresenceIconInfoListener::CancelSubscribePresenceInfo( const MDesC8Array& aLinkArray )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfoListener::CancelSubscribePresenceInfo MDesC8Array this=%d" ), (TInt)this );
#endif

    TInt count = aLinkArray.MdcaCount();
    for ( TInt i=0; i < count; i++ )
        {
        CPresenceIconInfo* infoH = SearchIconInfo( aLinkArray.MdcaPoint(i) );
        if ( infoH )
            {
            infoH->Destroy();
            }
        else
            {
            // not found
            }
        }
    }

// ----------------------------------------------------------
//
void CPresenceIconInfoListener::CancelSubscribePresenceInfo( MVPbkContactLink* aLink )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfoListener::CancelSubscribePresenceInfo MVPbkContactLink this=%d" ), (TInt)this );
#endif  
    TRAP_IGNORE( DoResubscribePresenceInfoL( aLink, EFalse )); 
    }

// ----------------------------------------------------------
// CPresenceIconInfoListener::CancelGet
// ----------------------------------------------------------
//
void CPresenceIconInfoListener::CancelGet( TInt aOpId )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfoListener::CancelGet this=%d" ), (TInt)this );
#endif

    TDblQueIter<CPresenceIconInfo> rIter( iInfoReqList );
    rIter.SetToFirst();  
    while ( rIter )
        {
        CPresenceIconInfo* req = rIter;
        rIter++;
        if ( req->OpId() == aOpId )
            {
            req->Destroy();
            break;
            }
        }
    }

// ----------------------------------------------------------
//
void CPresenceIconInfoListener::DeleteAllRequests()
    {
    TDblQueIter<CPresenceIconInfo> rIter( iInfoReqList );
    rIter.SetToFirst();

    while ( rIter )
        {
        CPresenceIconInfo* req = rIter;
        rIter++;
        req->Destroy();
        }
    }

// ----------------------------------------------------------
//
void CPresenceIconInfoListener::SendErrorCallbacks( TInt aMyStatus )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CPresenceIconInfoListener::SendErrorCallbacks" ) );
#endif
    TBool destroyed( EFalse );
    iDestroyedPtr = &destroyed;

    TDblQueIter<CPresenceIconInfo> rIter( iInfoReqList );
    rIter.SetToFirst();
    // get all the pending request and callback error method
    while ( rIter )
        {
        CPresenceIconInfo* req = rIter;
        rIter++;
        if ( req->IsWaiting() )
            {
#ifdef _DEBUG
            CContactPresence::WriteToLog( _L8( "CPresenceIconInfoListener CALLBACK PresenceSubscribeError" ) );
#endif
            // The following deletes the request itself
            req->SendErrorCallback( aMyStatus );
            if (destroyed)
                {
                // client application may have deleted entire ContactPresence in callback
                return;
                }
            else
                {              
                // continue sending errors
                }
            }
        else
            {
            }
        }
    iDestroyedPtr = NULL;     
    return;
    }

// ----------------------------------------------------------
//
void CPresenceIconInfoListener::DoResubscribePresenceInfoL( MVPbkContactLink* aLink, TBool aResubscribe )
    {
    if ( !aLink )
        {
        User::Leave( KErrArgument );
        }
    HBufC8* packed = aLink->PackLC();    
    CPresenceIconInfo* infoH = SearchIconInfo( packed ? packed->Des() : TPtrC8() );
    if ( infoH )
        {
        infoH->SendEmptyNotification();
        infoH->Destroy();
        infoH = NULL;
        if ( aResubscribe )
            {
            infoH = CreateIconInfoL( packed->Des(), ETrue, 0 );
            TRAPD( err, infoH->StartGetIconInfoL( ));  
            if ( err )
                {
                infoH->Destroy();
                User::Leave( err );
                } 
            else
                {                
                }
            } 
        else
            {
            // This is cancel use case
            }
        }
    else
        {
        // no match
        }
        
    CleanupStack::PopAndDestroy( packed );    
    }

