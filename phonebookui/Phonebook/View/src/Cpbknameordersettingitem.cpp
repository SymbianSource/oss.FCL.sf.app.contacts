/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
#include "Cpbknameordersettingitem.h"   // This class
#include <CPbkContactEngine.h>
#include <StringLoader.h>
#include <PbkView.rsg>

#include <PbkView.hrh>
#include "PbkViewUtils.h"

/// Unnamed namespace for local definitions
namespace {

void AppendEnumeratedItemL( TInt aResourceId, 
    TInt aEnumeration,
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
    
void AppendEnumeratedStringItemL( TInt aResourceId, 
    TInt aEnumeration,
    const TDesC& aString,
    CArrayPtr<CAknEnumeratedText>& aEnumeratedArray )
    {
    HBufC* lastSpaceFirst = StringLoader::LoadLC( aResourceId, aString );
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

// CPbkNameOrderSettingItem

CPbkNameOrderSettingItem::CPbkNameOrderSettingItem
		(TInt aResourceId, TInt& aValue, CPbkContactEngine& aEngine)
		: CAknEnumeratedTextPopupSettingItem(aResourceId, aValue),
		 iEngine(aEngine)
    {    
    }

TInt CPbkNameOrderSettingItem::Value()
    {
	return ExternalValue();
    }

void CPbkNameOrderSettingItem::ChangeValueL()
	{
	// First get the current value
	LoadL();
	TInt value = InternalValue();
	
	// Then change it...
	if (value == CPbkContactEngine::EPbkNameOrderLastNameFirstName)
		{
		value = CPbkContactEngine::EPbkNameOrderFirstNameLastName;
		}
	else
		{
		value = CPbkContactEngine::EPbkNameOrderLastNameFirstName;
		}

	// ...and set it
	SetInternalValue(value);
	StoreL();
	}

void CPbkNameOrderSettingItem::CompleteConstructionL()
    {
    CAknEnumeratedTextPopupSettingItem::CompleteConstructionL();
    
    // get reference from the base class to texts that are showed in
    // settings item list
    CArrayPtr<CAknEnumeratedText>* enumeratedTexts = EnumeratedTextArray();
    // get reference from the base class to texts that are showed in
    // the pop up. In this case these are same as enumerated texts
    CArrayPtr<HBufC>* poppedTexts = PoppedUpTextArray();
    
    // Append items to enumerated array
    
    // Last name First name item
    AppendEnumeratedItemL( 
        R_QTN_PHOB_SET_NAMES_LAST_SPACE_FIRST, 
        EPbkSettingNameOrderLastSpaceFirst,
        *enumeratedTexts );

    TBuf<1> sepChar;
    sepChar.Append(PbkViewUtils::ResolveNameOrderSeparatorL(iEngine));
    AppendEnumeratedStringItemL(
        R_QTN_PHOB_SET_NAMES_LAST_SEPARATOR_FIRST,
        EPbkSettingNameOrderLastSeparatorFirst,
        sepChar,
        *enumeratedTexts );

    AppendEnumeratedItemL( 
        R_QTN_PHOB_SET_NAMES_FIRST_SPACE_LAST, 
        EPbkSettingNameOrderFirstSpaceLast,
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

//  End of File
