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
#include "CPbk2ContactViewSupplier.h"

// Phonebook 2
#include "CPbk2CompressPolicyManager.h"
#include <CPbk2StoreConfiguration.h>
#include <CPbk2SortOrderManager.h>
#include <CPbk2ContactViewBuilder.h>
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2CommonUi.rsg>
#include <Pbk2DataCaging.hrh>
#include <CPbk2GroupFeaturePlugin.h>
#include <Pbk2ViewId.hrh>
#include "CPbk2NonTopContactSelector.h"
#include "Pbk2ContactView.hrh"

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactViewBase.h>
#include <CVPbkContactViewDefinition.h>
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkSortOrder.h>
#include <MVPbkFieldType.h>
#include <CVPbkSortOrderAcquirer.h>
#include <VPbkSortOrderAcquirerUid.h>
#include <MVPbkContactViewBaseChildAccessExtension.h>

// System includes
#include <barsread.h>
#include <coemain.h>

// Debugging headers
#include <Pbk2Debug.h>

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

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::CPbk2ContactViewSupplier
// --------------------------------------------------------------------------
//
inline CPbk2ContactViewSupplier::CPbk2ContactViewSupplier
        ( CVPbkContactManager& aContactManager,
          CPbk2SortOrderManager& aSortOrderManager,
          CPbk2StoreConfiguration& aStoreConfiguration,
          CPbk2StorePropertyArray& aStoreProperties ) :
            iContactManager( aContactManager ),
            iSortOrderManager( aSortOrderManager ),
            iStoreConfiguration( aStoreConfiguration ),
            iStoreProperties( aStoreProperties )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::~CPbk2ContactViewSupplier
// --------------------------------------------------------------------------
//
CPbk2ContactViewSupplier::~CPbk2ContactViewSupplier()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::~CPbk2ContactViewSupplier(0x%x)"), this);

    if ( iAllContactsView )
        {
        iAllContactsView->RemoveObserver( *this );
        }
    delete iAllContactsView;

    if ( iAllGroupsView )
        {
        iAllGroupsView->RemoveObserver( *this );
        }
    delete iAllGroupsView;

    delete iOldContactsView;
    delete iOldGroupsView;
    delete iSortOrderAcquirer;
    delete iNonTopContactSelector;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactViewSupplier::ConstructL
        ( TBool aActOnConfigurationEvents )
    {
    if ( aActOnConfigurationEvents )
        {
        // We must be the first observer for the store configuration
        // in Phonebook 2 framework. That is because all other observers
        // might ask for a new contact view after the configuration has
        // changed and since we are the first observer we get the event
        // last and can control contact views better.
        iStoreConfiguration.AddObserverL( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactViewSupplier* CPbk2ContactViewSupplier::NewL
        ( CVPbkContactManager& aContactManager,
          CPbk2SortOrderManager& aSortOrderManager,
          CPbk2StoreConfiguration& aStoreConfiguration,
          CPbk2StorePropertyArray& aStoreProperties,
          TBool aActOnConfigurationEvents )
    {
    CPbk2ContactViewSupplier* self = new (ELeave) CPbk2ContactViewSupplier
        ( aContactManager, aSortOrderManager, aStoreConfiguration,
          aStoreProperties );
    CleanupStack::PushL( self );
    self->ConstructL( aActOnConfigurationEvents );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::AllContactsViewL
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CPbk2ContactViewSupplier::AllContactsViewL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::AllContactsViewL"));

    // Creates the view lazyly when the first client needs it
    if ( !iAllContactsView )
        {
        iAllContactsView = CreateAllContactsViewL();

        // Give the view to the sort order manager
        iSortOrderManager.SetContactViewL( *iAllContactsView );
        }

    return iAllContactsView;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::TopContactsViewL
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CPbk2ContactViewSupplier::TopContactsViewL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::TopContactsViewL"));

    MVPbkContactViewBase* topContactsView = NULL;
    MVPbkContactViewBase* allContactsView = AllContactsViewL();
    TAny* extAny = allContactsView->ContactViewBaseExtension(
          TUid::Uid( KVPbkViewBaseExtChildAccess ) );
    if ( extAny )
        {
        MVPbkContactViewBaseChildAccessExtension* ext =
            static_cast<MVPbkContactViewBaseChildAccessExtension*>(extAny);
        topContactsView = ext->GetChildViewById( 
            EPbk2MergedTopContactsListPlaceHolder ); // not owned here
        }
    return topContactsView;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::AllGroupsViewL
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CPbk2ContactViewSupplier::AllGroupsViewL()
    {
    // Creates the view lazyly when the first client needs it
    if ( !iAllGroupsView )
        {
        iAllGroupsView = CreateAllGroupsViewL();
        }

    return iAllGroupsView;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::MPbk2ContactViewSupplierExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2ContactViewSupplier::MPbk2ContactViewSupplierExtension(
    TUid aExtensionUid )
    {
    if ( aExtensionUid == KMPbk2ContactViewSupplierExtension2Uid )
        {
        return static_cast<MPbk2ContactViewSupplier2*>( this );
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSupplier::ContactViewReady
        ( MVPbkContactViewBase& aView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::ContactViewReady(0x%x)"), &aView);
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSupplier::ContactViewUnavailable
        ( MVPbkContactViewBase& aView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::ContactViewUnavailable(0x%x)"), &aView);
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSupplier::ContactAddedToView
        ( MVPbkContactViewBase& aView, TInt aIndex,
          const MVPbkContactLink& aContactLink )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::ContactAddedToView(0x%x,%d,0x%x)"),
        &aView, aIndex, &aContactLink);
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSupplier::ContactRemovedFromView
        ( MVPbkContactViewBase& aView, TInt aIndex,
          const MVPbkContactLink& aContactLink )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::ContactRemovedFromView(0x%x,%d,0x%x)"),
        &aView, aIndex, &aContactLink);
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSupplier::ContactViewError
        ( MVPbkContactViewBase& aView,
          TInt aError, TBool /*aErrorNotified*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::ContactViewError(0x%x,%d)"),
        &aView, aError);
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::ConfigurationChanged
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSupplier::ConfigurationChanged()
    {
    TRAPD( err, HandleConfigurationChangedL() );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::ConfigurationChangedComplete
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSupplier::ConfigurationChangedComplete()
    {
    delete iOldContactsView;
    iOldContactsView = NULL;
    delete iOldGroupsView;
    iOldGroupsView = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::HandleConfigurationChangedL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewSupplier::HandleConfigurationChangedL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::HandleConfigurationChangedL(0x%x)"),
        this);

    MVPbkContactViewBase* allContactsView = CreateAllContactsViewL();
    CleanupDeletePushL( allContactsView );
    iSortOrderManager.SetContactViewL( *allContactsView );

    MVPbkContactViewBase* allGroupsView = NULL;
    if ( iAllGroupsView )
        {
        allGroupsView = CreateAllGroupsViewL();
        }

    CleanupStack::Pop(); // allContactsView

    // All leaving stuff done, switch the views

    if ( iAllContactsView )
        {
        iAllContactsView->RemoveObserver( *this );
        delete iOldContactsView;
        iOldContactsView = iAllContactsView;
        }
    iAllContactsView = allContactsView;

    if ( iAllGroupsView )
        {
        iAllGroupsView->RemoveObserver( *this );
        delete iOldGroupsView;
        iOldGroupsView = iAllGroupsView;
        }
    iAllGroupsView = allGroupsView;

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ( "CPbk2ContactViewSupplier::HandleConfigurationChangedL(0x%x) end" ),
            this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::CreateAllContactsViewL
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CPbk2ContactViewSupplier::CreateAllContactsViewL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::CreateAllContactsViewL(0x%x)"), this);

    CPbk2ContactViewBuilder* viewBuilder = CPbk2ContactViewBuilder::NewLC
        ( iContactManager, iStoreProperties  );

    CVPbkContactStoreUriArray* currentConfig =
        iStoreConfiguration.CurrentConfigurationL();

    CleanupStack::PushL( currentConfig );
    

    if ( !iNonTopContactSelector )
        {
        iNonTopContactSelector = CPbk2NonTopContactSelector::NewL();
        }
    MVPbkContactViewBase* result = viewBuilder->CreateTopNonTopContactViewForStoresLC
        ( *currentConfig, *this, iSortOrderManager.SortOrder(), NULL, EVPbkContactViewFlagsNone, *iNonTopContactSelector );
    CleanupStack::Pop(); // result
    CleanupStack::PopAndDestroy( 2 ); // currentConfig, viewBuilder

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactViewSupplier::CreateAllContactsViewL(0x%x) end"), this);
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewSupplier::CreateAllGroupsViewL
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CPbk2ContactViewSupplier::CreateAllGroupsViewL()
    {
    MVPbkContactViewBase* result = NULL;
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

    if ( sortOrder )
        {
        CPbk2ContactViewBuilder* viewBuilder = CPbk2ContactViewBuilder::NewLC
            ( iContactManager, iStoreProperties );

        CVPbkContactStoreUriArray* currentConfig =
            iStoreConfiguration.CurrentConfigurationL();
        CleanupStack::PushL( currentConfig );

        result = viewBuilder->CreateGroupViewForStoresLC
        	( *currentConfig, *this, *sortOrder, NULL, 
        			EVPbkContactViewFlagsNone );

        CleanupStack::Pop(); // result
        CleanupStack::PopAndDestroy( 2 ); // currentConfig, viewBuilder
        }

    return result;
    }

// End of File
