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
* Description:  Phonebook 2 settings list state.
*
*/


// INCLUDE FILES
#include "CPbk2SettingsListState.h"

// Phonebook 2
#include <CPbk2StoreConfiguration.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <Pbk2StoreProperty.hrh>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreProperties.h>

// Ssytem includes
#include <badesca.h>
#include <aknsettingitemlist.h>
#include <akncheckboxsettingpage.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KSelectionListGranularity = 4;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicSelectionItemListNULL = 1,
    EPanicContactStoreUriArrayNULL,
    EPanicItemListStateNULL,
    EPanic_IsChanged_OOB
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2SettingsListState");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2SettingsListState::CPbk2SettingsListState
// --------------------------------------------------------------------------
//
CPbk2SettingsListState::CPbk2SettingsListState()
    {
    }

// --------------------------------------------------------------------------
// CPbk2SettingsListState::~CPbk2SettingsListState
// --------------------------------------------------------------------------
//
CPbk2SettingsListState::~CPbk2SettingsListState()
    {
    if (iSelectionItemList)
        {
        iSelectionItemList->ResetAndDestroy();
        delete iSelectionItemList;
        }

    if ( iItemListState )
        {
        iItemListState->ResetAndDestroy();
        delete iItemListState;
        }

    delete iSupportedStores;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsListState::NewL
// --------------------------------------------------------------------------
//
CPbk2SettingsListState* CPbk2SettingsListState::NewL()
    {
    CPbk2SettingsListState* self = new ( ELeave ) CPbk2SettingsListState;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsListState::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2SettingsListState::ConstructL()
    {
    iSelectionItemList =
        new ( ELeave ) CSelectionItemList( KSelectionListGranularity );
    iItemListState =
        new ( ELeave ) CSelectionItemList( KSelectionListGranularity );
    iSupportedStores =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreConfiguration().SupportedStoreConfigurationL();
    PopulateSelectionItemListL();
    }

// --------------------------------------------------------------------------
// CPbk2SettingsListState::SelectionItemList
// --------------------------------------------------------------------------
//
CSelectionItemList& CPbk2SettingsListState::SelectionItemList()
    {
    __ASSERT_DEBUG( iSelectionItemList, Panic(EPanicSelectionItemListNULL));
    return *iSelectionItemList;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsListState::SupportedStores
// --------------------------------------------------------------------------
//
CVPbkContactStoreUriArray& CPbk2SettingsListState::SupportedStores()
    {
    __ASSERT_DEBUG( iSupportedStores, Panic(EPanicContactStoreUriArrayNULL));
    return *iSupportedStores;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsListState::StoreItemStateL
// --------------------------------------------------------------------------
//
void CPbk2SettingsListState::StoreItemStateL()
    {
    __ASSERT_DEBUG( iItemListState, Panic(EPanicItemListStateNULL) );

    // Reset old list state
    iItemListState->ResetAndDestroy();

    const TInt count( iSelectionItemList->Count() );
    for ( TInt i(0); i < count; ++i )
        {
        TPtrC ptr = iSelectionItemList->At(i)->ItemText();
        CSelectableItem* item = new (ELeave ) CSelectableItem
            ( ptr, iSelectionItemList->At(i)->SelectionStatus() );
        CleanupStack::PushL(item);
        item->ConstructL();
        iItemListState->AppendL(item);
        CleanupStack::Pop(item);
        }
    }

// --------------------------------------------------------------------------
// CPbk2SettingsListState::RestoreItemStateL
// --------------------------------------------------------------------------
//
void CPbk2SettingsListState::RestoreItemStateL()
    {
    iSelectionItemList->ResetAndDestroy();
    const TInt count( iItemListState->Count() );
    for ( TInt i(0); i < count; ++i )
        {
        TPtrC ptr = iItemListState->At(i)->ItemText();
        CSelectableItem* item = new ( ELeave ) CSelectableItem
            ( ptr, iItemListState->At(i)->SelectionStatus() );
        CleanupStack::PushL(item);
        item->ConstructL();
        iSelectionItemList->AppendL(item);
        CleanupStack::Pop(item);
        }
    }

// --------------------------------------------------------------------------
// CPbk2SettingsListState::IsChanged
// --------------------------------------------------------------------------
//
TBool CPbk2SettingsListState::IsChanged()
    {
    TBool result( EFalse );
    const TInt count( iItemListState->Count() );

    __ASSERT_DEBUG( iSelectionItemList->Count() >= count,
        Panic( EPanic_IsChanged_OOB ) );

    for ( TInt i(0); i < count; ++i )
        {
        if ( iItemListState->At(i)->SelectionStatus() !=
             iSelectionItemList->At(i)->SelectionStatus() )
            {
            result = ETrue;
            break;
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsListState::PopulateSelectionItemListL
// --------------------------------------------------------------------------
//
void CPbk2SettingsListState::PopulateSelectionItemListL()
    {
    const TInt count = iSupportedStores->Count();
    for (TInt i = 0; i < count; ++i)
        {
        const CPbk2StoreProperty* storeProperty =
            Phonebook2::Pbk2AppUi()->ApplicationServices().
                StoreProperties().FindProperty
                    ( ( *iSupportedStores )[i] );
        TPtrC storeName;
        TBool append = EFalse;
        if (storeProperty)
            {
            TUint32 flags = storeProperty->Flags();
            if ( flags &= KPbk2ShownInMemorySelectionList )
                {
                append = ETrue;
                storeName.Set( storeProperty->StoreName() );
                }
            }
        else
            {
            storeName.Set( (*iSupportedStores)[i].UriDes() );
            }

        if ( append )
            {
            CSelectableItem* item = new ( ELeave ) CSelectableItem
                ( storeName, EFalse );
            CleanupStack::PushL(item);
            item->ConstructL();
            iSelectionItemList->AppendL(item);
            CleanupStack::Pop(item);
            }
        }
    }

//  End of File
