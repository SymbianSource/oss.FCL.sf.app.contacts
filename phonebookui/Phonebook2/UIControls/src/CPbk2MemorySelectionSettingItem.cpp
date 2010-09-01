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
* Description:  Phonebook 2 memory selection setting item.
*
*/


#include "CPbk2MemorySelectionSettingItem.h"

// Phonebook 2
#include "CPbk2MemorySelectionSettingPage.h"
#include <CPbk2StoreConfiguration.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <Pbk2UIControls.rsg>

// Virtual Phonebook
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>

// System includes
#include <badesca.h>
#include <StringLoader.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KBufferSize( 100 );

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicStoreNotSupported = 1,
    EPanicIndexOOB
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2MemorySelectionSettingItem");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

}  /// namespace

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::CPbk2MemorySelectionSettingItem
// --------------------------------------------------------------------------
//
CPbk2MemorySelectionSettingItem::CPbk2MemorySelectionSettingItem
        ( TInt aSettingId, CVPbkContactStoreUriArray& aSupportedStores,
        CSelectionItemList& aSelectionItemList ) :
            CAknSettingItem( aSettingId ),
            iSupportedStores( aSupportedStores ),
            iSelectionItemList( aSelectionItemList )
    {
    }

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::~CPbk2MemorySelectionSettingItem
// --------------------------------------------------------------------------
//
CPbk2MemorySelectionSettingItem::~CPbk2MemorySelectionSettingItem()
    {
    delete iSeveralMemories;
    delete iSettingText;
    }

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::EditItemL
// --------------------------------------------------------------------------
//
TBool CPbk2MemorySelectionSettingItem::EditItemL()
    {
    CPbk2MemorySelectionSettingPage* dlg =
        new ( ELeave ) CPbk2MemorySelectionSettingPage
            ( SettingPageResourceId(), &iSelectionItemList );
    dlg->SetSettingPageObserver( this );
    return dlg->ExecuteLD();
    }

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::RestoreStateL
// --------------------------------------------------------------------------
//
void CPbk2MemorySelectionSettingItem::RestoreStateL()
    {
    UpdateSettingTextL();
    UpdateListBoxTextL();
    }

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::CompleteConstructionL
// --------------------------------------------------------------------------
//
void CPbk2MemorySelectionSettingItem::CompleteConstructionL()
    {
    iSettingText = HBufC::NewL( KBufferSize );
    iSeveralMemories = StringLoader::LoadL
        ( R_PHONEBOOK2_SETTINGS_SEVERAL_MEMORIES );
    }

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::EditItemL
// --------------------------------------------------------------------------
//
void CPbk2MemorySelectionSettingItem::EditItemL( TBool /*aCalledFromMenu*/ )
    {
    __ASSERT_DEBUG( ETrue, Panic(EPanicStoreNotSupported) );
    }

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::HandleSettingPageEventL
// --------------------------------------------------------------------------
//
void CPbk2MemorySelectionSettingItem::HandleSettingPageEventL
        ( CAknSettingPage* aSettingPage, TAknSettingPageEvent aEventType )
    {
    switch ( aEventType )
        {
        case EEventSettingChanged:
        case EEventSettingCancelled:
            {
            break;
            }
        case EEventSettingOked:
            {
            UpdateSettingTextL();
            break;
            }
        }
    CAknSettingItem::HandleSettingPageEventL( aSettingPage, aEventType );
    }

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::StoreL
// --------------------------------------------------------------------------
//
void CPbk2MemorySelectionSettingItem::StoreL()
    {
    CVPbkContactStoreUriArray* currentConfig =
         Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreConfiguration().CurrentConfigurationL();
    CleanupStack::PushL( currentConfig );

    const TInt count = iSelectionItemList.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        __ASSERT_DEBUG( iSupportedStores.Count() > i, Panic( EPanicIndexOOB ) );
        if ( iSelectionItemList.At( i )->SelectionStatus() &&
             !currentConfig->IsIncluded( ( iSupportedStores)[i] ) )
            {
            Phonebook2::Pbk2AppUi()->ApplicationServices().
                StoreConfiguration().
                    AddContactStoreURIL( ( iSupportedStores )[i] );
            }
        else if( !iSelectionItemList.At( i )->SelectionStatus() &&
                currentConfig->IsIncluded( ( iSupportedStores )[i] ) )
            {
            Phonebook2::Pbk2AppUi()->ApplicationServices().
                StoreConfiguration().
                    RemoveContactStoreURIL( ( iSupportedStores )[i] );
            }
        }

    CleanupStack::PopAndDestroy( currentConfig );
    }

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::LoadL
// --------------------------------------------------------------------------
//
void CPbk2MemorySelectionSettingItem::LoadL()
    {
    CVPbkContactStoreUriArray* currentConfig =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            StoreConfiguration().CurrentConfigurationL();

    const TInt count = iSelectionItemList.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        __ASSERT_DEBUG( iSupportedStores.Count() > i, Panic ( EPanicIndexOOB ) );
        TBool isSelected = currentConfig->IsIncluded
            ( ( iSupportedStores )[i] );
        iSelectionItemList.At(i)->SetSelectionStatus( isSelected );
        }
    delete currentConfig;
    UpdateSettingTextL();
    }

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::SettingTextL
// --------------------------------------------------------------------------
//
const TDesC& CPbk2MemorySelectionSettingItem::SettingTextL()
    {
    return *iSettingText;
    }

// --------------------------------------------------------------------------
// CPbk2MemorySelectionSettingItem::UpdateSettingTextL
// --------------------------------------------------------------------------
//
void CPbk2MemorySelectionSettingItem::UpdateSettingTextL()
    {
    TPtr ptr = iSettingText->Des();
    ptr.Zero();
    const TInt count( iSelectionItemList.Count() );
    TBool onlyOneSelected( ETrue );

    //loop selection item list and check selected ones
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( iSelectionItemList.At( i )->SelectionStatus() )
            {
            if ( onlyOneSelected )
                {
                ptr.Append( iSelectionItemList.At( i )->ItemText() );
                onlyOneSelected = EFalse;
                }
            else
                {
                //More than one selected
                ptr.Zero();
                ptr.Append( *iSeveralMemories );
                break;
                }
            }
        }
    }

// End of File
