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
* Description:  Phonebook 2 USIM UI Extension view manager.
*
*/


// INCLUDE FILES
#include "CPsu2ViewManager.h"

// Phonebook 2
#include "Pbk2USimUIError.h"
#include "CPsu2SecUi.h"
#include "MPsu2ViewLaunchCallback.h"
#include <MPbk2ViewExplorer.h>
#include <CPbk2SortOrderManager.h>
#include <Pbk2USimUIRes.rsg>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <MPbk2AppUi.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreObserver.h>
#include <MVPbkContactViewObserver.h>
#include <CVPbkContactViewDefinition.h>
#include <CVPbkContactStoreUriArray.h>
#include <VPbkSimStoreFactory.h>
#include <MVPbkSimPhone.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactStoreProperties.h>

// System includes
#include <barsread.h>
#include <coemain.h>
#include <CVPbkSortOrder.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>

// Debugging headers
#include <Pbk2Debug.h>


// --------------------------------------------------------------------------
// CPsu2ViewManager::CPsu2ViewManager
// --------------------------------------------------------------------------
//
CPsu2ViewManager::CPsu2ViewManager()
    {
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::~CPsu2ViewManager
// --------------------------------------------------------------------------
//
CPsu2ViewManager::~CPsu2ViewManager()
    {
    delete iCurrentSimView;
    delete iSecUi;
    delete iPhone;
    delete iFdnSortOrder;
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::ConstructL
// --------------------------------------------------------------------------
//
inline void CPsu2ViewManager::ConstructL()
    {
    CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceLC();

    iContactManager = &(appServices->ContactManager());

    CleanupStack::PopAndDestroy(); // appServices

    iContactStores = &iContactManager->ContactStoresL();

    iPhone = VPbkSimStoreFactory::CreatePhoneL();
    iSecUi = CPsu2SecUi::NewL(*iPhone);

    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC(
        reader, R_PSU2_FDN_NAME_DISPLAY_ORDER );
    iFdnSortOrder = CVPbkSortOrder::NewL
        (reader, iContactManager->FieldTypes() );
    CleanupStack::PopAndDestroy(); // reader
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::NewL
// --------------------------------------------------------------------------
//
CPsu2ViewManager* CPsu2ViewManager::NewL()
    {
    CPsu2ViewManager* self = new ( ELeave ) CPsu2ViewManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::ResetAndInitiateStoreAndViewL
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::ResetAndInitiateStoreAndViewL
        ( const TVPbkContactStoreUriPtr& aURI, TPsu2ViewId aViewId,
          MPsu2ViewLaunchCallback& aViewLaunchCallback )
    {
    // Reset
    iRefCount = 0;
    delete iCurrentSimView;
    iCurrentSimView = NULL;
    if ( iContactStore )
        {
        iContactStore->Close( *this );
        }

    // Initiate
    iViewId = aViewId;
    iViewLaunchCallback = &aViewLaunchCallback;

    // Retrieve store list from the contact manager
    MVPbkContactStoreList& storeList = iContactManager->ContactStoresL();
    // Check whether the plugin is loaded, does not mean that the store is open
    MVPbkContactStore* store = storeList.Find(aURI);
    if ( !store )
        {
        iContactManager->LoadContactStoreL( aURI );
        }

    // Plugin is now loaded and store exists
    iContactStore = storeList.Find( aURI );

    __ASSERT_DEBUG(iContactStore, Pbk2USimUI::Panic(
            Pbk2USimUI::EPostCond_OpenAndActivateViewL ) );

    iContactStore->OpenL( *this );
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::RemoveViewLaunchCallBack
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::RemoveViewLaunchCallBack()
    {
    iViewLaunchCallback = NULL;
    }
// --------------------------------------------------------------------------
// CPsu2ViewManager::RegisterStoreAndView
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::RegisterStoreAndView()
    {
    ++iRefCount;
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::DeregisterStoreAndView
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::DeregisterStoreAndView()
    {
    --iRefCount;

    if ( iRefCount <= 0 )
        {
        // No clients left, close view and then close store
        delete iCurrentSimView;
        iCurrentSimView = NULL;
        iContactStore->Close( *this );
        }
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::ContactStore
// --------------------------------------------------------------------------
//
MVPbkContactStore& CPsu2ViewManager::ContactStore()
    {
    return *iContactStore;
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::StoreReady
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::StoreReady( MVPbkContactStore& aContactStore )
    {
    if ( iContactStore == &aContactStore )
        {
        iStoreAvailable = ETrue;

        // Not need to create new view if we already have one.
        if ( !iCurrentSimView )
            {
            TRAPD( err, DoOpenContactViewL() );
            if ( err != KErrNone )
                {
                if ( iViewLaunchCallback )
                    {
                    iViewLaunchCallback->HandleError( err );
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::StoreUnavailable
        ( MVPbkContactStore& aContactStore, TInt aReason )
    {
    if ( iContactStore == &aContactStore )
        {
        iStoreAvailable = EFalse;
        if ( iViewLaunchCallback )
            {
            iViewLaunchCallback->HandleError( aReason );
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::HandleStoreEventL
        ( MVPbkContactStore& /*aContactStore*/,
          TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // No action
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::ContactViewReady
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::ContactViewReady( MVPbkContactViewBase& aView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2ViewManager::ContactViewReady(0x%x)"), &aView);
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::ContactViewUnavailable
        ( MVPbkContactViewBase& aView )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2ViewManager::ContactViewUnavailable(0x%x)"), &aView);
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::ContactAddedToView
        ( MVPbkContactViewBase& aView, TInt aIndex,
          const MVPbkContactLink& aContactLink )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2ViewManager::ContactAddedToView(0x%x,%d,0x%x)"),
        &aView, aIndex, &aContactLink);
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::ContactRemovedFromView
        ( MVPbkContactViewBase& aView, TInt aIndex,
         const MVPbkContactLink& aContactLink )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2ViewManager::ContactRemovedFromView(0x%x,%d,0x%x)"),
        &aView, aIndex, &aContactLink);
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::ContactViewError
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::ContactViewError
        ( MVPbkContactViewBase& aView,
          TInt aError, TBool /*aErrorNotified*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2ViewManager::ContactViewError(0x%x,%d)"),
        &aView, aError);
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::CreateViewL
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CPsu2ViewManager::CreateViewL
        ( TInt aViewDefResourceId )
    {
    MVPbkContactViewBase* view = NULL;

    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aViewDefResourceId );

    CVPbkContactViewDefinition* def =
        CVPbkContactViewDefinition::NewL( reader );
    CleanupStack::PushL( def );

    if ( aViewDefResourceId == R_PSU2_FDN_CONTACTS_VIEW )
        {
        view = iContactManager->CreateContactViewLC
            ( *this, *def, *iFdnSortOrder );
        }
    else
        {
        view = iContactManager->CreateContactViewLC
            ( *this, *def, Phonebook2::Pbk2AppUi()->ApplicationServices().
                SortOrderManager().SortOrder() );
        }

    CleanupStack::Pop(); // view
    CleanupStack::PopAndDestroy( 2 ); // def, reader
    return view;
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::OpenViewL
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CPsu2ViewManager::OpenViewL(TPsu2ViewId aViewId)
    {
    MVPbkContactViewBase* view = NULL;
    switch (aViewId)
        {
        case EPsu2OwnNumberViewId:
            {
            view = CreateViewL(R_PSU2_OWN_NUMBER_CONTACTS_VIEW );
            break;
            }
        case EPsu2ServiceDialingViewId:
            {
            view = CreateViewL(R_PSU2_SDN_CONTACTS_VIEW );
            break;
            }
        case EPsu2FixedDialingViewId:
            {
            view = CreateViewL(R_PSU2_FDN_CONTACTS_VIEW );
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    __ASSERT_DEBUG( view, Pbk2USimUI::Panic
        ( Pbk2USimUI::EPostCond_OpenAndActivateViewL ) );

    return view;
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::DoOpenContactViewL
// --------------------------------------------------------------------------
//
void CPsu2ViewManager::DoOpenContactViewL()
    {
    // Open contact view
    delete iCurrentSimView;
    iCurrentSimView = NULL;
    iCurrentSimView = OpenViewL( iViewId );

    if ( iViewLaunchCallback )
        {
        iViewLaunchCallback->SetContactViewL( *iCurrentSimView );
        }
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::StoreAvailable
// --------------------------------------------------------------------------
//
TBool CPsu2ViewManager::StoreAvailableL()
    {
    if ( !iStoreAvailable )
        {
        // Show unavailable note
        const CPbk2StoreProperty* prop =  Phonebook2::Pbk2AppUi()->
            ApplicationServices().StoreProperties().FindProperty
                ( iContactStore->StoreProperties().Uri() );
        HBufC* text = NULL;
        if ( prop )
            {
            if ( prop->StoreName().Length() > 0 )
                {
                text = StringLoader::LoadLC
                    ( R_QTN_PHOB_STORE_NOT_AVAILABLE, prop->StoreName() );
                }
            }
        if ( !text )
            {
            text = StringLoader::LoadLC
                ( R_QTN_PHOB_STORE_NOT_AVAILABLE,
                  iContactStore->StoreProperties().Uri().UriDes() );
            }
        CAknInformationNote* note =
            new ( ELeave ) CAknInformationNote( ETrue );
        // Show "not available" note
        note->ExecuteLD(*text);
        CleanupStack::PopAndDestroy(text);
        }
    return iStoreAvailable;
    }

//  End of File
