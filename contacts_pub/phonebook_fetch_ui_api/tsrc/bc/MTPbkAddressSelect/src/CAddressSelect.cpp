/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*           Methods for Email address selection dialog.
*
*/








// INCLUDE FILES

#include    "CAddressSelect.h"
#include    <CPbkContactItem.h>
#include    <avkon.rsg> // R_AVKON_SOFTKEYS_SELECT_CANCEL;
#include    <avkon.hrh> // AVKON softkey codes
#include    <AknNoteWrappers.h>
#include    <StringLoader.h>        // StringLoader
#include    <CPbkFieldInfo.h>
#include    <CPbkContactEngine.h>
#include    "MTPbkAddressSelect.h"
#include    <PbkAddressSelect.rsg>

// ================= MEMBER FUNCTIONS =======================

CAddressSelect::CAddressSelect()
    {
	iIsSame = EFalse;
    }

CAddressSelect::TParams::TParams
        (const CPbkContactItem& aContact) :
    TBaseParams(aContact, aContact.DefaultEmailField())
    {
    }

TBool CAddressSelect::ExecuteLD(TParams& aParams, CMTPbkAddressSelect& aAddressSelect)
    {
    TBool   ret     = EFalse,
            isSame  = EFalse;
    
	iContact = &aParams.ContactItem();
	iAddressSelect = &aAddressSelect;
    ret = CPbkAddressSelect::ExecuteLD(aParams);

    if ( aParams.FocusedField() == aParams.SelectedField() )
        {
        isSame = ETrue;
        aAddressSelect.SetSelectedAndFocusedField(isSame);
        }
        
	// CPbkAddressSelect::ExecuteLD takes care of LD function semantics
        
    return ret;
    }

CAddressSelect::~CAddressSelect()
	{
	delete iQueryTitle;
	}

EXPORT_C TBool CAddressSelect::AddressField(const TPbkContactItemField& aField) const
    {
    const CPbkContactItem* contact(&ContactItem());

    TBool isSame = EFalse;

    if ( iContact==contact)
        {
        isSame = ETrue;
        iAddressSelect->SetContactReturnValue(isSame);
        }
    
    // Return true for non-empty email fields
    return ((aField.FieldInfo().FieldId()) == EPbkFieldIdEmailAddress &&
            !aField.IsEmptyOrAllSpaces());
    }

EXPORT_C void CAddressSelect::NoAddressesL()
    {
    // if contact is empty and it hasn't got email addresses it's ok if selected
    // field and focused field are different
    TBool isSame = ETrue;
    iAddressSelect->SetSelectedAndFocusedField(isSame);
    }

EXPORT_C const TDesC& CAddressSelect::QueryTitleL()
    {
    iQueryTitle = ContactItem().GetContactTitleL();
    return *iQueryTitle;
    }

EXPORT_C TInt CAddressSelect::QuerySoftkeysResource() const
    {
    return R_CBA;
    }

//  End of File
