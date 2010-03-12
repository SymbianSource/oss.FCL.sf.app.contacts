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
* Description:  Phonebook 2 server application contact store manager.
*
*/


#include "CPbk2ServerAppStoreManager.h"

// Phonebook 2
#include "mpbk2serverappstartupobserver.h"
#include "CPbk2ServerAppMonitor.h"
#include "CPbk2ServerAppAppUi.h"
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreManager.h>
#include <CPbk2ContactViewBuilder.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2SortOrderManager.h>
#include <Pbk2StoreProperty.hrh>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactLinkArray.h>
#include <CVPbkSortOrderAcquirer.h>
#include <VPbkSortOrderAcquirerUid.h>
#include <MVPbkBaseContact.h>
#include <MVPbkContactSelector.h>

// Debugging headers
#include <Pbk2Debug.h>

#include <featmgr.h>


/// Unnamed namespace for local definitions
namespace {

_LIT( KGroupSortOrderDisplayName, "AllGroups" );

/**
 * Custom cleanup function.
 *
 * @param aObj  Object to clean.
 */
void CleanupResetAndDestroy( TAny* aObj )
    {
    if ( aObj )
        {
        static_cast<RImplInfoPtrArray*>( aObj )->ResetAndDestroy();
        }
    }

} /// namespace

/**
 * Helper class to filter mycard from phonebooks views
 */
class CPbk2MyCardFilter : public CBase, 
                          public MVPbkContactSelector
    {
public:
    CPbk2MyCardFilter();
    ~CPbk2MyCardFilter();
protected:  // From MVPbkContactSelector
    TBool IsContactIncluded(
                const MVPbkBaseContact& aContact );
    };

CPbk2MyCardFilter::CPbk2MyCardFilter()
    {
    }

CPbk2MyCardFilter::~CPbk2MyCardFilter()
    {
    }

TBool CPbk2MyCardFilter::IsContactIncluded(
                const MVPbkBaseContact& aContact )
    {
    TBool isContactIncluded( ETrue );
    // this is temporary solution to hide own contact from phonebook contacts list,
    // TODO remove this code when we can hide own contact with contact model

    MVPbkBaseContact& contact = const_cast<MVPbkBaseContact&>( aContact );
    TAny* extension = contact.BaseContactExtension( 
                KVPbkBaseContactExtension2Uid );

    if( extension )
        {
        MVPbkBaseContact2* baseContactExtension =
                static_cast<MVPbkBaseContact2*>( extension );
        TInt error( KErrNone );
        isContactIncluded =
                ( !baseContactExtension->IsOwnContact( error ) );
        }
    
    return isContactIncluded;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::CPbk2ServerAppStoreManager
// --------------------------------------------------------------------------
//
CPbk2ServerAppStoreManager::CPbk2ServerAppStoreManager
        ( CPbk2StoreManager& aStoreManager,
          CVPbkContactManager& aContactManager ) :
            iStoreManager( aStoreManager ),
            iContactManager( aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::~CPbk2ServerAppStoreManager
// --------------------------------------------------------------------------
//
CPbk2ServerAppStoreManager::~CPbk2ServerAppStoreManager()
    {
    iStoreManager.DeregisterStoreEvents( *this );
    iOpenedStores.Reset();
    iOpenedStores.Close();
    delete iSortOrderAcquirer;
    delete iMonitor;
    delete iFetchView;
    delete iMyCardFilter;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::ConstructL()
    {
    iStoreManager.RegisterStoreEventsL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::NewL
// --------------------------------------------------------------------------
//
CPbk2ServerAppStoreManager* CPbk2ServerAppStoreManager::NewL
        ( CPbk2StoreManager& aStoreManager,
          CVPbkContactManager& aContactManager )
    {
    CPbk2ServerAppStoreManager* self =
        new ( ELeave ) CPbk2ServerAppStoreManager
            ( aStoreManager, aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::LoadAndOpenContactStoresL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::LoadAndOpenContactStoresL
        ( CVPbkContactStoreUriArray& aStoreUris,
          MPbk2ServerAppStartupObserver& aObserver,
          TBool aLoadSavingMemoriesAlways )
    {
    if ( aLoadSavingMemoriesAlways )
        {
        TVPbkContactStoreUriPtr ptr
            ( VPbkContactStoreUris::DefaultCntDbUri() );
        if ( !aStoreUris.IsIncluded( ptr ) )
            {
            iContactManager.LoadContactStoreL( ptr );
            }
        iStoreManager.EnsureDefaultSavingStoreIncludedL();
        }

    const TInt count( aStoreUris.Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        iContactManager.LoadContactStoreL( aStoreUris[i] );
        }

    iMonitor = CPbk2ServerAppMonitor::NewL
        ( iContactManager.ContactStoresL(), aObserver );
    iMonitor->RegisterEventsL( aObserver );
    iMonitor->StartupBeginsL();

    iStoreManager.OpenStoresL();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::BuildFetchViewL
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CPbk2ServerAppStoreManager::BuildFetchViewL
        ( TVPbkContactViewType aViewType,
          const CVPbkContactStoreUriArray& aStoreUris,
          CVPbkFieldTypeSelector* aViewFilter,
          CPbk2StorePropertyArray& aStoreProperties,
          const CPbk2SortOrderManager& aSortOrderManager,
          const TUint32 aFlags )
    {
    MVPbkContactViewBase* result = NULL;
     
    CPbk2ContactViewBuilder* viewBuilder = CPbk2ContactViewBuilder::NewLC
        ( iContactManager, aStoreProperties );

    if( !iMyCardFilter )
        {
        FeatureManager::InitializeLibL();
        if( FeatureManager::FeatureSupported( KFeatureIdffContactsMycard ) )
            {
            iMyCardFilter = new (ELeave) CPbk2MyCardFilter();
            }
        FeatureManager::UnInitializeLib();    
        }

    
    if ( aViewType == EVPbkContactsView )
        {
        result = viewBuilder->CreateContactViewForStoresLC
            ( aStoreUris, *this, aSortOrderManager.SortOrder(),
              aViewFilter, aFlags, iMyCardFilter );
        CleanupStack::Pop(); // result
        }
    else if ( aViewType == EVPbkGroupsView )
        {
        const MVPbkFieldTypeList* sortOrder = NULL;

        // Acquire group sort order
        RImplInfoPtrArray implementations;
        REComSession::ListImplementationsL
            ( TUid::Uid( KVPbkSortOrderAcquirerInterfaceUID ), implementations );
        CleanupStack::PushL( TCleanupItem
                ( CleanupResetAndDestroy, &implementations ) );

        TBool found = EFalse;
        const TInt count = implementations.Count();
        for ( TInt i = count - 1; i >= 0 && !found ; --i )
            {
            CImplementationInformation* implInfo = implementations[i];

            if ( implInfo->DisplayName().
                    CompareC( KGroupSortOrderDisplayName ) == 0 )
                {
                TUid implUid = implInfo->ImplementationUid();

                CVPbkSortOrderAcquirer::TSortOrderAcquirerParam param
                    ( iContactManager.FieldTypes() );

                delete iSortOrderAcquirer;
                iSortOrderAcquirer = NULL;
                iSortOrderAcquirer = CVPbkSortOrderAcquirer::NewL
                    ( implUid, param );
                sortOrder = &iSortOrderAcquirer->SortOrder();
                found = ETrue;
                }

            }
        CleanupStack::PopAndDestroy(); // implementations
                
        result = viewBuilder->CreateGroupViewForStoresLC
            ( aStoreUris, *this, *sortOrder,
              aViewFilter, aFlags );
        
        CleanupStack::Pop(); // result
        }

    CleanupStack::PopAndDestroy(); // viewBuilder

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::RemoveReadOnlyStoreUris
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::RemoveReadOnlyStoreUris
        ( CVPbkContactStoreUriArray& aStoreUriArray,
          CPbk2StorePropertyArray& aStoreProperties ) const
    {
    // Browse the list backwards
    for ( TInt i( aStoreUriArray.Count()-1 ); i >= 0; --i )
        {
        const CPbk2StoreProperty* storeProperty =
            aStoreProperties.FindProperty( aStoreUriArray[i] );

        if ( storeProperty && !( storeProperty->Flags() & KPbk2DefaultSavingStorageItem ) )
            {
            // Remove read only URI from the array
            aStoreUriArray.Remove( aStoreUriArray[i] );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::IsContactIncludedInCurrentConfigurationL
// --------------------------------------------------------------------------
//
TBool CPbk2ServerAppStoreManager::IsContactIncludedInCurrentConfigurationL
        ( const MVPbkStoreContact& aContact,
          const CPbk2StoreConfiguration& aStoreConfiguration ) const
    {
    TBool result = EFalse;

    TVPbkContactStoreUriPtr contactUri =
        aContact.ParentStore().StoreProperties().Uri();

    CVPbkContactStoreUriArray* currentConfig =
        aStoreConfiguration.CurrentConfigurationL();
    CleanupStack::PushL( currentConfig );

    if ( currentConfig->IsIncluded( contactUri ) )
        {
        result = ETrue;
        }

    CleanupStack::PopAndDestroy( currentConfig );

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::ContactsAvailableL
// --------------------------------------------------------------------------
//
TBool CPbk2ServerAppStoreManager::ContactsAvailableL
        ( const MVPbkContactLinkArray& aContactLinks ) const
    {
    TBool ret = ETrue;

    CVPbkContactStoreUriArray* validStores = CurrentlyValidStoresL();

    for ( TInt i = 0; i < aContactLinks.Count(); ++ i )
        {
        const MVPbkContactLink& link = aContactLinks.At( i );
        if ( !validStores->IsIncluded
                ( link.ContactStore().StoreProperties().Uri() ) )
            {
            ret = EFalse;
            break;
            }
        }

    delete validStores;
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::CurrentlyValidStoresL
// --------------------------------------------------------------------------
//
CVPbkContactStoreUriArray*
        CPbk2ServerAppStoreManager::CurrentlyValidStoresL() const
    {
    CVPbkContactStoreUriArray* uris = CVPbkContactStoreUriArray::NewLC();

    const TInt count = iOpenedStores.Count();
    for ( TInt i = 0 ; i < count; ++i )
        {
        uris->AppendL( iOpenedStores[i]->StoreProperties().Uri() );
        }
    
    CleanupStack::Pop( uris );
    return uris;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::ContactViewReady
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::ContactAddedToView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::ContactRemovedFromView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::ContactViewError
        ( MVPbkContactViewBase& /*aView*/, TInt /*aError*/,
          TBool /*aErrorNotified*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::StoreReady
        ( MVPbkContactStore& aContactStore )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ServerAppStoreManager::StoreReady(0x%x)"),
        &aContactStore);

    // Add the store to the open stores list
    if ( iOpenedStores.Find( &aContactStore ) == KErrNotFound )
        {
        iOpenedStores.Append( &aContactStore );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::StoreUnavailable
        ( MVPbkContactStore& aContactStore, TInt aReason )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ServerAppStoreManager::StoreUnavailable(0x%x,%d)"),
        &aContactStore, aReason);

    // Remove the store from the open store list if it is there
    const TInt index = iOpenedStores.Find( &aContactStore );
    if ( index != KErrNotFound )
        {
        iOpenedStores.Remove( index );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppStoreManager::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppStoreManager::HandleStoreEventL
        ( MVPbkContactStore& /*aContactStore*/,
          TVPbkContactStoreEvent /*aEvent*/ )
    {
    // Do nothing
    }

// End of File
