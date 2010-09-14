/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 server application start-up monitor.
*
*/


// INCLUDE FILES
#include "CPbk2ServerAppMonitor.h"

// Phonebook2
#include "mpbk2serverappstartupobserver.h"
#include "CPbk2ServerAppAppUi.h"
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <MPbk2StoreObservationRegister.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2StartupObserver.h>
#include <MPbk2ApplicationServices.h>
#include <pbk2commonui.rsg>
#include <pbk2uicontrols.rsg>

// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStoreList.h>

// System includes
#include <StringLoader.h>
#include <aknnotewrappers.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KGranularity( 4 );

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::CPbk2ServerAppMonitor
// --------------------------------------------------------------------------
//
CPbk2ServerAppMonitor::CPbk2ServerAppMonitor
        ( const MVPbkContactStoreList& aStoreList,
          MPbk2ServerAppStartupObserver& aObserver ) :
            iContactStoreList( &aStoreList ),
            iServerAppObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::~CPbk2ServerAppMonitor
// --------------------------------------------------------------------------
//
CPbk2ServerAppMonitor::~CPbk2ServerAppMonitor()
    {
    delete iUnavailableStoreNames;
    iObservers.Close();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::NewL
// --------------------------------------------------------------------------
//
CPbk2ServerAppMonitor* CPbk2ServerAppMonitor::NewL(
        const MVPbkContactStoreList& aStoreList,
        MPbk2ServerAppStartupObserver& aObserver )
    {
    CPbk2ServerAppMonitor* self =
        new ( ELeave ) CPbk2ServerAppMonitor( aStoreList, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::ConstructL()
    {
    iEikEnv = CEikonEnv::Static();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::StartupBeginsL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::StartupBeginsL()
    {
    Phonebook2::Pbk2AppUi()->ApplicationServices().
        StoreObservationRegister().RegisterStoreEventsL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::HandleStartupComplete
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::HandleStartupComplete()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::HandleStartupFailed
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::HandleStartupFailed( TInt aError )
    {
    HandleError( aError );
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::RegisterEventsL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::RegisterEventsL
        ( MPbk2StartupObserver& aObserver )
    {
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::DeregisterEvents
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::DeregisterEvents
        ( MPbk2StartupObserver& aObserver )
    {
    TInt pos = iObservers.Find( &aObserver );
    if ( pos != KErrNotFound )
        {
        iObservers.Remove( pos );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::NotifyViewActivationL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::NotifyViewActivationL( TUid /*aViewId*/ )
    {
    // UI services are based on dialogs not application views
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::NotifyViewActivationL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::NotifyViewActivationL(
        TUid /*aViewId*/, MVPbkContactViewBase& /*aContactView*/ )
    {
    // UI services are based on dialogs not application views
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::StoreReady( MVPbkContactStore& aContactStore )
    {
    HandleStoreNotification( aContactStore );
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::StoreUnavailable
        ( MVPbkContactStore& aContactStore, TInt /*aReason*/ )
    {
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );

    // Get the property of the failed store
    const CPbk2StoreProperty* prop = appUi.ApplicationServices().
        StoreProperties().FindProperty
            ( aContactStore.StoreProperties().Uri() );

    if ( prop && prop->StoreName().Length() > 0 )
        {
        TRAPD(res, AddUnavailableStoreNameL( prop->StoreName() ) );
        HandleError( res );
        }
    else
        {
        TRAPD(res, AddUnavailableStoreNameL(
                    aContactStore.StoreProperties().Uri().UriDes() ) );
        HandleError( res );
        }

    HandleStoreNotification( aContactStore );
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::HandleStoreEventL
        ( MVPbkContactStore& /*aContactStore*/,
          TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::AddUnavailableStoreNameL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::AddUnavailableStoreNameL( const TDesC& aName )
    {
    if (!iUnavailableStoreNames)
        {
        iUnavailableStoreNames = new(ELeave) CDesCArrayFlat( KGranularity );
        }

    TInt dummy = 0;
    if ( iUnavailableStoreNames->Find( aName, dummy ) != 0 )
        {
        iUnavailableStoreNames->AppendL( aName );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::ShowUnavailableStoresL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::ShowUnavailableStoresL()
    {
    if ( iUnavailableStoreNames )
        {
        const TInt count = iUnavailableStoreNames->MdcaCount();
        for ( TInt i = 0; i < count ;++i )
            {
            // Get the store name
            HBufC* text = StringLoader::LoadLC(
                R_QTN_PHOB_STORE_NOT_AVAILABLE,
                iUnavailableStoreNames->MdcaPoint( i ) );
            CAknInformationNote* note =
                new ( ELeave ) CAknInformationNote( ETrue );
            // Show "not available" note
            note->ExecuteLD( *text );
            CleanupStack::PopAndDestroy( text );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::HandleStoreNotification
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::HandleStoreNotification
        ( MVPbkContactStore& /*aContactStore*/ )
    {
    ++iHandledStores;
    if ( iContactStoreList->Count() == iHandledStores )
        {
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreObservationRegister().DeregisterStoreEvents( *this );
        // Show unavailable stores if exists
        TRAP_IGNORE( ShowUnavailableStoresL() );
        SendMessageToObservers();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::SendMessageToObservers
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::SendMessageToObservers()
    {
    TInt err( KErrNone );
    const TInt count = iObservers.Count();
        {
        for ( TInt i = 0; i < count && err == KErrNone; ++i )
            {
            TRAP( err, iObservers[i]->ContactUiReadyL( *this ) );
            HandleError( err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppMonitor::HandleError
// --------------------------------------------------------------------------
//
void CPbk2ServerAppMonitor::HandleError( TInt aResult )
    {
    if ( aResult != KErrNone )
        {
        iServerAppObserver.StartupCanceled( aResult );
        }
    }

//  End of File
