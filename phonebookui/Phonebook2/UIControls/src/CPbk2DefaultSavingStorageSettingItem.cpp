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
* Description:  Phonebook 2 default saving storage setting item.
*
*/


// INCLUDE FILES
#include "CPbk2DefaultSavingStorageSettingItem.h"

// Phonebook 2
#include "CPbk2DefaultSavingStorageQueryListBoxModel.h"
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <Pbk2UIControls.rsg>
#include <Pbk2StoreProperty.hrh>
#include <Phonebook2InternalCRKeys.h>

// Virtual Phonebook
#include <MVPbkContactStoreList.h>
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>

// System includes
#include <badesca.h>
#include <aknsettingitemlist.h>
#include <akncheckboxsettingpage.h>
#include <aknselectionlist.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <aknlists.h>
#include <aknPopup.h>
#include <centralrepository.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KStoreUriArrayGranularity = 3;

#ifdef _DEBUG

enum TPanicCode
    {
    EPanic_UpdateL_OOB = 1,
    EPanic_StoreL_OOB,
    EPanic_CreatePopupItemListL_OOB
    };

void Panic( TPanicCode aPanic )
    {
    _LIT( KPanicCat, "CPbk2DefaultSavingStorageSettingItem" );
    User::Panic( KPanicCat, aPanic );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::CPbk2DefaultSavingStorageSettingItem
// --------------------------------------------------------------------------
//
CPbk2DefaultSavingStorageSettingItem::CPbk2DefaultSavingStorageSettingItem
        ( TInt aResourceId,  TInt& aValue,
        CVPbkContactStoreUriArray& aSupportedStores,
        CSelectionItemList& aSelectionItemList ) :
            CAknEnumeratedTextPopupSettingItem( aResourceId, aValue ),
            iSupportedStores( aSupportedStores ),
            iSelectionItemList( aSelectionItemList )
    {
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::~CPbk2DefaultSavingStorageSettingItem
// --------------------------------------------------------------------------
//
CPbk2DefaultSavingStorageSettingItem::~CPbk2DefaultSavingStorageSettingItem()
    {
    delete iRepository;
    if ( iStoreUriArray )
        {
        iStoreUriArray->ResetAndDestroy();
        }
    delete iStoreUriArray;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::Value
// --------------------------------------------------------------------------
//
TInt CPbk2DefaultSavingStorageSettingItem::Value()
    {
    return InternalValue();
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::UpdateL
// --------------------------------------------------------------------------
//
TBool CPbk2DefaultSavingStorageSettingItem::UpdateL()
    {
    TInt value( Value() );
    __ASSERT_DEBUG( iStoreUriArray->Count() > value,
        Panic ( EPanic_UpdateL_OOB ) );
    // Save old item text
    HBufC* oldItem = iStoreUriArray->At( value )->AllocLC();
    // Create new item list
    CreatePopupItemListL();
    
    TBool result = EFalse;

    if ( CheckSelectedItems() )
        {
        // Check is the selected default saving storage in the selected list
        if ( IsDefaultSavingStorageL( *oldItem ) )
            {
            // Set new index
            SetSelectedIndex( *oldItem );            
            // Update list box and view
            UpdateListBoxTextL();
            result = ETrue;
            }
        else
            {            
            // Check store count, must be at least one
            TInt storeCount( iStoreUriArray->Count() );
            if ( storeCount == 1 )
                {
                SetSelectedIndex( 0 );
                UpdateListBoxTextL();
                ShowInformationNoteL();
                result = ETrue;
                }
            else
                {
                result = LaunchDefaultSavingMemoryQueryL();
                }
            }
        }
    else
        {        
        // Memory selection list includes only read only items
        result = LaunchDefaultSavingMemoryQueryL();
        }
    
    CleanupStack::PopAndDestroy( oldItem );        
    return result;        
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::RestoreStateL
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStorageSettingItem::RestoreStateL()
    {
    CreatePopupItemListL();
    UpdateListBoxTextL();
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::CompleteConstructionL
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStorageSettingItem::CompleteConstructionL()
    {
    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();

    iRepository = CRepository::NewL(
        TUid::Uid( KCRUidPhonebookStoreConfiguration ) );

    iEnumeratedTextArray = EnumeratedTextArray();
    iPopupTextArray = PoppedUpTextArray();

    iStoreUriArray = new ( ELeave ) CArrayPtrFlat<HBufC>
        ( KStoreUriArrayGranularity );

    CreatePopupItemListL();
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::StoreL
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStorageSettingItem::StoreL()
    {
    TInt value( Value() );
    __ASSERT_DEBUG( iStoreUriArray->Count() > value,
        Panic ( EPanic_StoreL_OOB ) );

    HBufC* defaultSavingStore = iStoreUriArray->At( value );
    User::LeaveIfError( iRepository->Set( KPhonebookDefaultSavingStoreUri,
        *defaultSavingStore ) );
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::LoadL
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStorageSettingItem::LoadL()
    {
    TVPbkContactStoreUriPtr defaultSavingStore =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreConfiguration().DefaultSavingStoreL();

    const TInt count( iStoreUriArray->Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( iStoreUriArray->At( i )->Compare
                ( defaultSavingStore.UriDes() ) == 0 )
            {
            SetSelectedIndex( i );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::CreatePopupItemListL
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStorageSettingItem::CreatePopupItemListL()
    {
    iPopupTextArray->ResetAndDestroy();
    iStoreUriArray->ResetAndDestroy();

    const TInt count( iSelectionItemList.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( iSelectionItemList.At( i )->SelectionStatus() )
            {
            __ASSERT_DEBUG( iSupportedStores.Count() > i,
                Panic( EPanic_CreatePopupItemListL_OOB ) );
            const CPbk2StoreProperty* storeProperty =
                Phonebook2::Pbk2AppUi()->ApplicationServices().
                    StoreProperties().FindProperty
                        ( iSupportedStores[i] );

            if ( storeProperty->Flags() & KPbk2DefaultSavingStorageItem )
                {
                HBufC* item = iSelectionItemList.At(i)->ItemText().AllocLC();
                iPopupTextArray->AppendL( item ); // take ownership
                CleanupStack::Pop(); // item

                // Store URI array
                item = iSupportedStores[i].UriDes().AllocLC();
                iStoreUriArray->AppendL( item );
                CleanupStack::Pop( item );
                }
            }
        }

    if ( iPopupTextArray->Count() == 0 )
        {
        // All the stores are read only, select all the writable stores
        for ( TInt i( 0 ); i < count; ++i )
            {
            // All the selected stores are read only
            if ( !iSelectionItemList.At( i )->SelectionStatus() )
                {
                __ASSERT_DEBUG( iSupportedStores.Count() > i,
                    Panic( EPanic_CreatePopupItemListL_OOB ) );
                const CPbk2StoreProperty* storeProperty =
                    Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties().
                        FindProperty( iSupportedStores[i] );
                if ( storeProperty->Flags() & KPbk2DefaultSavingStorageItem )
                    {
                    HBufC* item = iSelectionItemList.At( i )->
                        ItemText().AllocLC();
                    iPopupTextArray->AppendL( item ); // takes ownership
                    CleanupStack::Pop(); // item

                    // Store URI array
                    item = iSupportedStores[i].UriDes().AllocLC();
                    iStoreUriArray->AppendL( item );
                    CleanupStack::Pop( item );
                    }
                }
            }
        }

    CreateEnumeratedItemListL();
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::CreateEnumeratedItemListL
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStorageSettingItem::CreateEnumeratedItemListL()
    {
    iEnumeratedTextArray->ResetAndDestroy();
    const TInt count( iPopupTextArray->Count() );
    for ( TInt i( 0 ); i< count; ++i )
        {
        // Enumerated test array
        HBufC* buf = iPopupTextArray->At( i )->AllocLC();
        CAknEnumeratedText* textItem =
            new ( ELeave ) CAknEnumeratedText( i, buf );
        CleanupStack::Pop( buf );
        CleanupStack::PushL( textItem );
        iEnumeratedTextArray->AppendL( textItem ); // takes ownership
        CleanupStack::Pop( textItem );
        }
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::ShowInformationNoteL
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStorageSettingItem::ShowInformationNoteL()
    {
    CEikonEnv* env = CEikonEnv::Static();
    HBufC* noteText = StringLoader::LoadLC
        ( R_PHONEBOOK2_SETTINGS_INFORMATION_NOTE, env );

    CAknInformationNote* dialog =
        new ( ELeave ) CAknInformationNote( R_AKN_INFORMATION_NOTE_WAIT );
    dialog->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy( noteText ); // noteText
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::LaunchDefaultSavingMemoryQueryL
// --------------------------------------------------------------------------
//
TBool CPbk2DefaultSavingStorageSettingItem::LaunchDefaultSavingMemoryQueryL()
    {
    // Create list box
    CAknSinglePopupMenuStyleListBox* listBox =
        new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( listBox );

    // Create popup list
    CAknPopupList* popupList = CAknPopupList::NewL( listBox,
        R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
        AknPopupLayouts::EMenuWindow );
    CleanupStack::PushL( popupList );
    HBufC* title = StringLoader::LoadLC
        ( R_PHONEBOOK2_SELECT_SAV_MEM_POPUP_TITLE );
    popupList->SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );

    // Init list box
    listBox->ConstructL( popupList, EAknListBoxMenuList );
    listBox->CreateScrollBarFrameL( ETrue );
    listBox->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );

    // Create list box model
    MDesCArray* model = CPbk2DefaultSavingStorageQueryListBoxModel::NewLC
        ( *iPopupTextArray );
    // Listbox takes the ownership
    listBox->Model()->SetItemTextArray( model );
    listBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop( 2 ); // model, popupList

    TBool result = popupList->ExecuteLD();

    if ( result )
        {
        // Select is pressed
        SetSelectedIndex( listBox->CurrentItemIndex() );
        UpdateListBoxTextL();
        }
    CleanupStack::PopAndDestroy(); //listBox
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::IsDefaultSavingStorageL
// --------------------------------------------------------------------------
//
TBool CPbk2DefaultSavingStorageSettingItem::IsDefaultSavingStorageL
        ( HBufC& aStoreName )
    {
    TBool result( EFalse );

    const TInt count( iStoreUriArray->Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        // This compare is enough, because aOldItem is taken from the
        // same list and that's why it does not matter if iStoreUriArray
        // includes UriNames or Uris
        HBufC* store = iStoreUriArray->At( i );
        if ( store->Compare( aStoreName ) == 0 )
            {
            result = ETrue;
            break;
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::SetSelectedIndex
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStorageSettingItem::SetSelectedIndex
        ( TInt aNewIndex )
    {
    SetInternalValue( aNewIndex );
    CAknEnumeratedTextPopupSettingItem::SetSelectedIndex( aNewIndex );
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::SetSelectedIndex
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStorageSettingItem::SetSelectedIndex
        ( const TDesC& aStoreUri )
    {
    const TInt count( iStoreUriArray->Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( iStoreUriArray->At( i )->Compare( aStoreUri ) == 0 )
            {
            SetSelectedIndex( i );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStorageSettingItem::CheckSelectedItems
// --------------------------------------------------------------------------
//
TBool CPbk2DefaultSavingStorageSettingItem::CheckSelectedItems()
    {
    TBool result( EFalse );

    const TInt count( iSelectionItemList.Count() );
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( iSelectionItemList.At( i )->SelectionStatus() )
            {
            const CPbk2StoreProperty* storeProperty =
                Phonebook2::Pbk2AppUi()->ApplicationServices().
                    StoreProperties().FindProperty
                        ( iSupportedStores[i] );

            if ( storeProperty->Flags() & KPbk2DefaultSavingStorageItem )
                {
                result = ETrue;
                break;
                }
            }
        }

    return result;
    }

// End of File
