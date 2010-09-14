/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 store manager.
*
*/


// INCLUDE FILES
#include <CPbk2StoreManager.h>

// Phonebook 2
#include "CPbk2CompressPolicyManager.h"
#include <CPbk2StoreConfiguration.h>
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>
#include <pbk2commonui.rsg>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>

// System includes
#include <barsread.h>
#include <coemain.h>
#include <aknnotewrappers.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Checks is the error received from the database at the open
 * fatal or not.
 *
 * @param aError    Error code.
 * @return  ETrue if the error is a fatal one.
 */
TBool IsFatalDbOpenError( TInt aError )
    {
    TBool ret = ETrue;

    switch ( aError )
        {
        case KErrNone:              // FALLTHROUGH
        case KErrCancel:            // FALLTHROUGH
        case KErrNoMemory:          // FALLTHROUGH
        case KErrNotSupported:      // FALLTHROUGH
        case KErrArgument:          // FALLTHROUGH
        case KErrDied:              // FALLTHROUGH
        case KErrInUse:             // FALLTHROUGH
        case KErrServerTerminated:  // FALLTHROUGH
        case KErrServerBusy:        // FALLTHROUGH
        case KErrNotReady:          // FALLTHROUGH
        case KErrAccessDenied:      // FALLTHROUGH
        case KErrLocked:            // FALLTHROUGH
        case KErrWrite:             // FALLTHROUGH
        case KErrDisMounted:        // FALLTHROUGH
        case KErrDiskFull:          // FALLTHROUGH
        case KErrAbort:             // FALLTHROUGH
        case KErrBadPower:          // FALLTHROUGH
        case KErrDirFull:           // FALLTHROUGH
        case KErrHardwareNotAvailable:
            {
            ret = EFalse;
            break;
            }

        default:
            {
            // All error codes not handled above end up here,
            // especially KErrCorrupt
            break;
            }
        }

    return ret;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2StoreManager::CPbk2StoreManager
// --------------------------------------------------------------------------
//
inline CPbk2StoreManager::CPbk2StoreManager
        ( CVPbkContactManager& aContactManager,
          CPbk2StoreConfiguration& aStoreConfiguration,
          TBool aActOnConfigurationEvents ) :
            iContactManager( aContactManager ),
            iStoreConfiguration( aStoreConfiguration ),
            iActOnConfigurationEvents( aActOnConfigurationEvents )
    {
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::~CPbk2StoreManager
// --------------------------------------------------------------------------
//
CPbk2StoreManager::~CPbk2StoreManager()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreManager::~CPbk2StoreManager(0x%x)"), this);

    if ( iContactStores )
        {
        iContactStores->CloseAll(*this);
        }

    iFailedStores.Reset();
    iFailedStores.Close();

    iOpenedStores.Reset();
    iOpenedStores.Close();

    iStoreObservers.Reset();
    iStoreObservers.Close();

    delete iCompressPolicy;
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2StoreManager::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreManager::ConstructL(0x%x)"), this);

    iStoreConfiguration.AddObserverL( *this );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreManager::ConstructL(0x%x) Store configuration created"),
        this);

    iCompressPolicy =
        CPbk2CompressPolicyManager::NewL( *this, iContactManager );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreManager::ConstructL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreManager* CPbk2StoreManager::NewL
        ( CVPbkContactManager& aContactManager,
          CPbk2StoreConfiguration& aStoreConfiguration,
          TBool aActOnConfigurationEvents )
    {
    CPbk2StoreManager* self = new (ELeave) CPbk2StoreManager
        ( aContactManager, aStoreConfiguration, aActOnConfigurationEvents );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::OpenStoresL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreManager::OpenStoresL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreManager::OpenStoresL"));

    if (iContactStores)
        {
        iContactStores->CloseAll(*this);
        }

    iContactStores = &iContactManager.ContactStoresL();

    if (iContactStores)
        {
        iContactStores->OpenAllL(*this);
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::EnableCompression
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreManager::EnableCompression(TBool aEnable)
    {
    if (aEnable)
        {
        iCompressPolicy->StartAllPolicies();
        }
    else
        {
        iCompressPolicy->StopAllPolicies();
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::EnsureDefaultSavingStoreIncludedL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreManager::EnsureDefaultSavingStoreIncludedL()
    {
    iStoreConfiguration.AddDefaultSavingStoreObserverL( *this );
    iContactManager.LoadContactStoreL
        ( iStoreConfiguration.DefaultSavingStoreL() );
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::EnsureDefaultSavingStoreIncludedL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreManager::EnsureCurrentConfigurationStoresIncludedL()
    {
    CVPbkContactStoreUriArray* uriArray =
        iStoreConfiguration.CurrentConfigurationL();
    CleanupStack::PushL( uriArray );
    TInt count( uriArray->Count() );
    for ( TInt i(0); i < count; ++i )
        {
        iContactManager.LoadContactStoreL( (*uriArray)[i] );
        }
    CleanupStack::PopAndDestroy( uriArray );
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::RegisterStoreEventsL
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::RegisterStoreEventsL
        (const MVPbkContactStoreObserver& aObserver)
    {
    User::LeaveIfError( iStoreObservers.Append( &aObserver ) );
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::DeregisterStoreEvents
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::DeregisterStoreEvents
        (const MVPbkContactStoreObserver& aObserver)
    {
    const TInt index = iStoreObservers.Find( &aObserver );
    if ( index > KErrNotFound )
        {
        iStoreObservers.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::CurrentlyValidStoresL
// --------------------------------------------------------------------------
//
CVPbkContactStoreUriArray* CPbk2StoreManager::CurrentlyValidStoresL() const
    {
    CVPbkContactStoreUriArray* uris = CurrentlyValidShownStoresL();
    CleanupStack::PushL( uris );

    // If default saving store is not part of current config,
    // check if it still valid store
    TVPbkContactStoreUriPtr defaultSavingStoreUri =
        iStoreConfiguration.DefaultSavingStoreL();
    if ( !uris->IsIncluded( defaultSavingStoreUri ))
        {
        // Default store is not in the uris array
        MVPbkContactStore* defaultStore =
            iContactStores->Find( defaultSavingStoreUri );
        // Add the default saving store to the list
        // if it is loaded, opened and not failed
        if ( defaultStore && IsStoreOpen(*defaultStore)
            && iFailedStores.Find( defaultStore ) == KErrNotFound )
            {
            // Default saving store is valid store and successfully opened
            uris->AppendL( defaultSavingStoreUri );
            }
        }
    CleanupStack::Pop( uris );

    return uris;
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::CurrentlyValidShownStoresL
// --------------------------------------------------------------------------
//
CVPbkContactStoreUriArray*
        CPbk2StoreManager::CurrentlyValidShownStoresL() const
    {
    // Gets the list of URI's that compose the current configuration
    CVPbkContactStoreUriArray* uris =
        iStoreConfiguration.CurrentConfigurationL();

    // Remove stores that are not open or are unavailable from the list
    const TInt count = uris->Count();
    for (TInt i = count - 1; i >= 0; --i)
        {
        TVPbkContactStoreUriPtr uri = (*uris)[i];
        MVPbkContactStore* store = iContactStores->Find(uri);
        if ( !(store && IsStoreOpen( *store ) &&
             ( iFailedStores.Find( store ) == KErrNotFound ) ) )
            {
            // When the store is loaded, opened and not failed
            // it is not removed from the URI list
            uris->Remove(uri);
            }
        }

    return uris;
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::OpenComplete
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::OpenComplete()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreManager::OpenComplete"));
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::StoreReady(
        MVPbkContactStore& aContactStore)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreManager::StoreReady(0x%x)"),
        &aContactStore);

    // If the store was on the failed list, remove it
    // from there
    const TInt index = iFailedStores.Find(&aContactStore);
    if (index != KErrNotFound)
        {
        iFailedStores.Remove(index);
        }

    // Add it to the open stores list
    if (iOpenedStores.Find(&aContactStore) == KErrNotFound)
        {
        iOpenedStores.Append(&aContactStore);
        }

    // Notify observers
    for (TInt i=0; i < iStoreObservers.Count(); ++i)
        {
        iStoreObservers[i]->StoreReady(aContactStore);
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::StoreUnavailable
        ( MVPbkContactStore& aContactStore, TInt aReason )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreManager::StoreUnavailable(0x%x,%d)"),
        &aContactStore, aReason);

    // If the store is not found from the failed list,
    // add it there and try to replace it.
    // If the replace fails, the store will be found from the
    // failed list, and the replace is not called again.

    TBool informObservers = ETrue;
    if ( iFailedStores.Find( &aContactStore ) == KErrNotFound )
        {
        iFailedStores.Append( &aContactStore );
        if ( IsFatalDbOpenError( aReason ) )
            {
            informObservers = EFalse; // inform only if the replace fails too
            TRAPD( err, ReplaceStoreL( aContactStore ) );
            if ( err != KErrNone )
                {
                informObservers = ETrue;
                CCoeEnv::Static()->HandleError( err );
                }
            }
        }

    // Remove the store from the open store list if it is there
    const TInt index = iOpenedStores.Find( &aContactStore );
    if ( index != KErrNotFound )
        {
        iOpenedStores.Remove( index );
        }

    // Notify observers
    if ( informObservers )
        {
        for ( TInt i=0; i < iStoreObservers.Count(); ++i )
            {
            iStoreObservers[i]->StoreUnavailable( aContactStore, aReason );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::HandleStoreEventL
        ( MVPbkContactStore& aContactStore,
          TVPbkContactStoreEvent aEvent )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreManager::HandleStoreEventL(0x%x,%d)"),
        &aContactStore, aEvent.iEventType);

    // Notify observers
    for (TInt i=0; i < iStoreObservers.Count(); ++i)
        {
        iStoreObservers[i]->HandleStoreEventL(aContactStore, aEvent);
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::ConfigurationChanged
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::ConfigurationChanged()
    {
    if ( iActOnConfigurationEvents )
        {
        TRAPD( err, HandleConfigurationChangedL() );
        if ( err != KErrNone )
            {
            CCoeEnv::Static()->HandleError( err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::ConfigurationChangedComplete
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::ConfigurationChangedComplete()
    {
    if ( iActOnConfigurationEvents )
        {
        TRAPD( err,CloseUnnecessaryStoresL() );
        if ( err != KErrNone )
            {
            CCoeEnv::Static()->HandleError( err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::SavingStoreChanged
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::SavingStoreChanged()
    {
    TRAPD(err,HandleSavingStoreChangedL());
    if (err != KErrNone)
        {
        CCoeEnv::Static()->HandleError(err);
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::HandleConfigurationChangedL
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::HandleConfigurationChangedL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreManager::HandleConfigurationChangedL(0x%x)"),
        this);

    EnsureCurrentConfigurationLoadedAndOpenL();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ( "CPbk2StoreManager::HandleConfigurationChangedL(0x%x) end" ),
            this );
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::EnsureCurrentConfigurationLoadedAndOpenL
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::EnsureCurrentConfigurationLoadedAndOpenL()
    {
    CVPbkContactStoreUriArray* supportedConfiguration =
            iStoreConfiguration.SupportedStoreConfigurationL();
    CleanupStack::PushL(supportedConfiguration);

    CVPbkContactStoreUriArray* currentConfiguration =
            iStoreConfiguration.CurrentConfigurationL();
    CleanupStack::PushL(currentConfiguration);

    MVPbkContactStoreList& storeList = iContactManager.ContactStoresL();
    const TInt supportedCount = supportedConfiguration->Count();
    for (TInt i = 0; i < supportedCount; ++i )
        {
        // loop through the supported configurations and load and open
        // all stores that are not part of the current configuration
        TVPbkContactStoreUriPtr supportedStore = (*supportedConfiguration)[i];
        if (currentConfiguration->IsIncluded(supportedStore))
            {
            // if the supported store is not loaded, then load it
            if ( !storeList.Find(supportedStore) )
                {
                iContactManager.LoadContactStoreL(supportedStore);
                }
            // if the supported store is not open, then open it
            MVPbkContactStore* store = storeList.Find(supportedStore);
            if (store && iOpenedStores.Find(store) == KErrNotFound)
                {
                store->OpenL(*this);
                }
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // currentConfiguration,
    }                                 // supportedConfiguration

// --------------------------------------------------------------------------
// CPbk2StoreManager::CloseUnnecessaryStoresL
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::CloseUnnecessaryStoresL()
    {
    CVPbkContactStoreUriArray* supportedConfiguration =
        iStoreConfiguration.SupportedStoreConfigurationL();
    CleanupStack::PushL( supportedConfiguration );

    CVPbkContactStoreUriArray* currentConfiguration =
        iStoreConfiguration.CurrentConfigurationL();
    CleanupStack::PushL( currentConfiguration );

    MVPbkContactStoreList& storeList = iContactManager.ContactStoresL();
    TVPbkContactStoreUriPtr defaultStore =
            iStoreConfiguration.DefaultSavingStoreL();
    const TInt supportedCount = supportedConfiguration->Count();
    for (TInt i = 0; i < supportedCount; ++i )
        {
        TVPbkContactStoreUriPtr supportedStore = (*supportedConfiguration)[i];
        TBool isDefault(
                    defaultStore.Compare(
                                supportedStore,
                                TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0 );

        if ( !currentConfiguration->IsIncluded( supportedStore ) && !isDefault )
            {
            // close stores that are not part of the current configuration
            // check whether the supported store is loaded
            MVPbkContactStore* store = storeList.Find(supportedStore);
            if (store)
                {
                // Close all which are not needed anymore
                store->Close( *this );

                // remove the store from the open store list if it is there
                const TInt index = iOpenedStores.Find(store);
                if (index != KErrNotFound)
                    {
                    iOpenedStores.Remove(index);
                    }
                }
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // currentConfiguration,
                                      // supportedConfiguration
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::HandleSavingStoreChangedL
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::HandleSavingStoreChangedL()
    {
    CVPbkContactStoreUriArray* supportedConfiguration =
            iStoreConfiguration.SupportedStoreConfigurationL();
    CleanupStack::PushL(supportedConfiguration);

    MVPbkContactStoreList& storeList = iContactManager.ContactStoresL();
    const TInt supportedCount = supportedConfiguration->Count();
    for ( TInt i = 0; i < supportedCount; ++i )
        {
        TVPbkContactStoreUriPtr supportedStore = (*supportedConfiguration)[i];
        if ( supportedStore.Compare(
                iStoreConfiguration.DefaultSavingStoreL(),
                    TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
            {
            // load the plugin if it isnt loaded
            if ( !storeList.Find(supportedStore))
                {
                iContactManager.LoadContactStoreL(supportedStore);
                }
            // if the saving store is not open, then open it
            MVPbkContactStore* store = storeList.Find(supportedStore);
            if (store && iOpenedStores.Find(store) == KErrNotFound)
                {
                store->OpenL(*this);
                break; // the default has been loaded so break the loop
                }
            }
        }

    CleanupStack::PopAndDestroy(); // supportedConfiguration
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::IsStoreOpen
// --------------------------------------------------------------------------
//
TBool CPbk2StoreManager::IsStoreOpen
        ( MVPbkContactStore& aContactStore ) const
    {
    TBool ret = EFalse;
    if ( iOpenedStores.Find( &aContactStore ) >= 0 )
        {
        ret = ETrue;
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2StoreManager::ReplaceStoreL
// --------------------------------------------------------------------------
//
void CPbk2StoreManager::ReplaceStoreL
        ( MVPbkContactStore& aContactStore )
    {
    // Display corrupted note
    CAknNoteWrapper* note = new ( ELeave ) CAknNoteWrapper;
    note->ExecuteLD( R_PBK2_NOTE_DATABASE_CORRUPTED );

    // Replace database
    aContactStore.ReplaceL( *this );
    }

// End of File
