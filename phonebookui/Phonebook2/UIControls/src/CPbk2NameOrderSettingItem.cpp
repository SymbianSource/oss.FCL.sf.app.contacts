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
* Description: 
*       Provides methods for Name Order settings item.
*
*/


// INCLUDE FILES
#include "CPbk2NameOrderSettingItem.h"   // This class

#include <Pbk2UIControls.rsg>
#include <CPbk2SortOrderManager.h>
#include <StringLoader.h>

/// Unnamed namespace for local definitions
namespace {

/// Append enumerated text items to array
void AppendEnumeratedItemL( TInt aResourceId, 
    CPbk2SortOrderManager::TPbk2NameDisplayOrder aEnumeration,
    CArrayPtr<CAknEnumeratedText>& aEnumeratedArray )
    {
    HBufC* lastSpaceFirst = StringLoader::LoadLC( aResourceId );
    CAknEnumeratedText* enumeratedText = new( ELeave ) CAknEnumeratedText( 
        aEnumeration, 
        lastSpaceFirst );
    CleanupStack::Pop( lastSpaceFirst );
    CleanupStack::PushL( enumeratedText );
    aEnumeratedArray.AppendL( enumeratedText );
    CleanupStack::Pop( enumeratedText );
    }
}
// ================= MEMBER FUNCTIONS =======================

// CPbk2NameOrderSettingItem

CPbk2NameOrderSettingItem::CPbk2NameOrderSettingItem(
        CPbk2SortOrderManager& aSortOrderManager, 
        TInt aResourceId, 
        TInt& aValue) : 
    CAknEnumeratedTextPopupSettingItem(aResourceId, aValue),
    iSortOrderManager(aSortOrderManager)
    {
    }

TInt CPbk2NameOrderSettingItem::Value()
    {
	return ExternalValue();
    }

// ---------------------------------------------------------------------------
// From class CAknEnumeratedTextPopupSettingItem
// CPbk2NameOrderSettingItem::CompleteConstructionL
// ---------------------------------------------------------------------------
//
void CPbk2NameOrderSettingItem::CompleteConstructionL()
    {
    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
    
    // get reference from the base class to texts that are showed in
    // settings item list
    CArrayPtr<CAknEnumeratedText>* enumeratedTexts = EnumeratedTextArray();
    // get reference from the base class to texts that are showed in
    // the pop up. In this case these are same as enumerated texts
    CArrayPtr<HBufC>* poppedTexts = PoppedUpTextArray();
    
    // Append items to enumerated array in the same order 
    // as enumerations in TPbk2NameDisplayOrder
    AppendEnumeratedItemL( 
        R_QTN_PHOB_SET_NAMES_LAST_SPACE_FIRST, 
        CPbk2SortOrderManager::EPbk2NameDisplayOrderLastNameFirstName,
        *enumeratedTexts );
    
    HBufC* lastSeparatorFirst = StringLoader::LoadLC( 
        R_QTN_PHOB_SET_NAMES_LAST_SEPARATOR_FIRST, 
        Separator() );
    CAknEnumeratedText* enumeratedText = new( ELeave ) CAknEnumeratedText( 
        CPbk2SortOrderManager::EPbk2NameDisplayOrderLastNameSeparatorFirstName, 
        lastSeparatorFirst );
    CleanupStack::Pop( lastSeparatorFirst );
    CleanupStack::PushL( enumeratedText );
    enumeratedTexts->AppendL( enumeratedText );
    CleanupStack::Pop( enumeratedText ); 
    
    AppendEnumeratedItemL( 
        R_QTN_PHOB_SET_NAMES_FIRST_SPACE_LAST, 
        CPbk2SortOrderManager::EPbk2NameDisplayOrderFirstNameLastName,
        *enumeratedTexts );
    
    // Append items to the poppedup array in the same order as in enumerated
    // array
    const TInt count = enumeratedTexts->Count();
    for ( TInt i = 0; i < count; ++i )
        {
        HBufC* text = enumeratedTexts->At( i )->Value()->AllocLC();
        poppedTexts->AppendL( text );
        CleanupStack::Pop( text );
        }
    }

// ---------------------------------------------------------------------------
// From class CAknEnumeratedTextPopupSettingItem
// CPbk2NameOrderSettingItem::StoreL
// ---------------------------------------------------------------------------
//    
void CPbk2NameOrderSettingItem::StoreL()
    {
    CAknEnumeratedTextPopupSettingItem::StoreL();
    
    CPbk2SortOrderManager::TPbk2NameDisplayOrder selectedOrder( 
        static_cast<CPbk2SortOrderManager::TPbk2NameDisplayOrder>( 
            Value() ) );
    
    if ( selectedOrder == 
         CPbk2SortOrderManager::EPbk2NameDisplayOrderLastNameSeparatorFirstName )
        {                           
        iSortOrderManager.SetNameDisplayOrderL( selectedOrder, Separator() );
        }
    else
        {
        iSortOrderManager.SetNameDisplayOrderL( selectedOrder );
        }
    }

// ---------------------------------------------------------------------------
// From class CAknEnumeratedTextPopupSettingItem
// CPbk2NameOrderSettingItem::IsSeparatorSpace
// ---------------------------------------------------------------------------
//    
TPtrC CPbk2NameOrderSettingItem::Separator()
    {
    // Retrieve current separator. If it's space then use the language
    // specific default separator.
    TPtrC separator( iSortOrderManager.CurrentSeparator() );
    if ( separator.Length() > 0 && TChar(separator[0]).IsSpace() )
        {
        separator.Set( iSortOrderManager.DefaultSeparator() );
        }
    return separator;
    }
//  End of File
