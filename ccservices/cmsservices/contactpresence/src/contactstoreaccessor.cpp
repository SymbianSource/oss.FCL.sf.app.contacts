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


#include <CVPbkSortOrder.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreObserver.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactViewBase.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactIdConverter.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CPbk2StoreConfiguration.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactViewDefinition.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreProperties.h>

/* this set compiles but leaks memory!
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactStoreList.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CPbk2StoreConfiguration.h>
#include <MVPbkContactOperationBase.h>
*/

#ifdef _TEST_MODE
#include "testmodeutils.h"
#endif

#include "contactpresence.h"
#include "contactstoreaccessor.h"
#include "presenceiconinfolistener.h"

// ----------------------------------------------------------
//
CContactStoreAccessor::CContactStoreAccessor( CPresenceIconInfoListener& aListener )
: iStoreReady(EFalse), iOpening(EFalse), iGivenManager(EFalse), iDestroyedPtr( NULL ), iListener(aListener)
    {
    // No implementation required
    }

// ----------------------------------------------------------
//
CContactStoreAccessor::~CContactStoreAccessor()
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactStoreAccessor::~CContactStoreAccessor this=%d" ),(TInt)this );
#endif
    
    if ( iOpening )
        {
        iOpening = EFalse;
        iStoreReady = EFalse;
        if ( iClientStatus )
            {
            User::RequestComplete( iClientStatus, KErrCancel );
            }
        else
            {
            }
        }
               
    if ( !iGivenManager )
        {         
        delete iContactManager;
        iContactManager = NULL;
        }
    else 
        {
        DoCloseStore();
        }

    delete iStoreConfiguration;
    iStoreConfiguration = NULL;


    }

// ----------------------------------------------------------
//
void CContactStoreAccessor::DoCloseStore()
    {
    if ( iContactStore )
        {
        iContactStore->Close( *this );
        }
    }

// ----------------------------------------------------------
//
CContactStoreAccessor* CContactStoreAccessor::NewL( CVPbkContactManager* aManager, CPresenceIconInfoListener& aListener )
    {
    CContactStoreAccessor* self = new (ELeave)CContactStoreAccessor(aListener);
    CleanupStack::PushL(self);
    self->ConstructL( aManager );
    CleanupStack::Pop(); // self;
    return self;
    }

// ----------------------------------------------------------
//
void CContactStoreAccessor::ConstructL( CVPbkContactManager* aManager )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactStoreAccessor::ConstructL begins aManager=%d this=%d" ),(TInt)aManager, (TInt)this );
#endif
        
    iStoreConfiguration = CPbk2StoreConfiguration::NewL();  
    
    if ( aManager )
        {
        iContactManager = aManager;
        iStoreList = &iContactManager->ContactStoresL();
        iGivenManager = ETrue;
        // 
        }
    else
        {
        iGivenManager = EFalse;        
        CVPbkContactStoreUriArray* uriList = iStoreConfiguration->CurrentConfigurationL();
        CleanupStack::PushL( uriList );

        iContactManager = CVPbkContactManager::NewL( *uriList, NULL );
        CleanupStack::PopAndDestroy( uriList );
        iStoreList = &iContactManager->ContactStoresL();
        }
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactStoreAccessor::ConstructL ends this=%d" ),(TInt)this );
#endif

    }

// ----------------------------------------------------------
//
void CContactStoreAccessor::OpenStoresL( TRequestStatus& aStatus )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactStoreAccessor::OpenStoresL this=%d" ),(TInt)this );
#endif
    iClientStatus = &aStatus;
    *iClientStatus = KRequestPending;
    
    if ( iStoreReady )
        {        
        User::RequestComplete( iClientStatus, KErrNone );
        }
    else if ( !iOpening  )
        {
        iStoreList->OpenAllL( *this );
        iOpening = ETrue;
        }
    else
        {
        // wait
        }             
    }

// ----------------------------------------------------------
//
void CContactStoreAccessor::OpenStoresL( TRequestStatus& aStatus, const TDesC8& aPackedLink )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactStoreAccessor::OpenStoresL this=%d" ),(TInt)this );
#endif
    
    if ( !iGivenManager )
        {
        OpenStoresL( aStatus );
        return;
        }
    iClientStatus = &aStatus;
    *iClientStatus = KRequestPending;

    if ( iStoreReady )
        {
        User::RequestComplete( iClientStatus, KErrNone );        
        }
    else if ( !iOpening  )
        {
        CVPbkContactLinkArray* lArray =
            CVPbkContactLinkArray::NewLC( aPackedLink, *AccessStoreList() );
        
        const MVPbkContactLink& cLink = lArray->At(0);
        TPtrC uriPtr( cLink.ContactStore().StoreProperties().Uri().UriDes() );
        
        iContactStore = NULL;    
        iContactStore = iContactManager->ContactStoresL().Find( uriPtr );
        if ( iContactStore )
            {
            iContactStore->OpenL( *this );
            iOpening = ETrue;            
            }    
        else
            {
            User::RequestComplete( iClientStatus, KErrNotFound );            
            }        
        CleanupStack::PopAndDestroy(); // lArray
        }
    else
        {
        // wait
        } 
    return;
    }

// ----------------------------------------------------------
// CCmsPhonebookProxy::OpenComplete
//
// ----------------------------------------------------------
//
void CContactStoreAccessor::OpenComplete()
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactStoreAccessor::OpenComplete this=%d" ),(TInt)this );
#endif

    TInt ret(KErrNone);
    
// ---------------------------------------------------------             
#ifdef _TEST_MODE
    /**
     * INTERNAL TEST SUITE
     */
    const TInt KTestCase = 1; 
    if ( TestModeUtils::TestCase( ) == KTestCase )
        {
        ret = KErrGeneral;
        }  	
#endif
// ---------------------------------------------------------      

    if ( iOpening )
        {
        iOpening = EFalse;
        iStoreReady = ETrue;
        User::RequestComplete( iClientStatus, ret );
        }
    else
        {
        }
    return;
    }

// ----------------------------------------------------------
// CContactStoreAccessor::StoreReady
//
// ----------------------------------------------------------
//
void CContactStoreAccessor::StoreReady( MVPbkContactStore& aContactStore )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactStoreAccessor::StoreReady this=%d" ),(TInt)this );
#endif
    iContactStore = &aContactStore;
    
    if ( iGivenManager )
        {
        OpenComplete();
        }
    else
        {
        // waiting for OpenComplete()
        }
    }

// ----------------------------------------------------------
// CContactStoreAccessor::StoreUnavailable
//
// ----------------------------------------------------------
//
void CContactStoreAccessor::StoreUnavailable( MVPbkContactStore& /*aContactStore*/, TInt aReason )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactStoreAccessor::StoreUnavailable this=%d" ),(TInt)this );
#endif
    
    iStoreReady = EFalse;    
    if ( iOpening )
        {
        iOpening = EFalse;
        User::RequestComplete( iClientStatus, aReason  );        
        }
    }

// ----------------------------------------------------------
// CContactStoreAccessor::HandleStoreEventL
//
// ----------------------------------------------------------
//
void CContactStoreAccessor::HandleStoreEventL( MVPbkContactStore& /*aContactStore*/,
                                            TVPbkContactStoreEvent aStoreEvent )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactStoreAccessor::HandleStoreEventL this=%d" ),(TInt)this );;
#endif  
    MVPbkContactLink* linkki = aStoreEvent.iContactLink;    
    TVPbkContactStoreEvent::TVPbkContactStoreEventType event = aStoreEvent.iEventType;
    if ( event == TVPbkContactStoreEvent::EContactDeleted )
        {
        iListener.CancelSubscribePresenceInfo( linkki );           
        }
    else if ( event == TVPbkContactStoreEvent::EContactChanged )
        {
        iListener.ResubscribePresenceInfoL( linkki );        
        }
    else
        {
        // no action
        }
    }


// ----------------------------------------------------------
//
MVPbkContactStoreList* CContactStoreAccessor::AccessStoreList()
    {
    return iStoreList;
    }

// ----------------------------------------------------------
//
CVPbkContactManager* CContactStoreAccessor::AccessContactManager()
    {
    return iContactManager;
    }


