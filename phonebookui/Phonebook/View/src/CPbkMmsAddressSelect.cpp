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
*           Phonebook generic address selection dialog class methods.
*
*/


// INCLUDE FILES

#include    "CPbkMmsAddressSelect.h"
#include    <avkon.hrh> // AVKON softkey codes
#include    <avkon.rsg> // R_AVKON_SOFTKEYS_SELECT_CANCEL;
#include    <aknnotewrappers.h>
#include    <StringLoader.h>        // StringLoader
#include    <PbkView.rsg>
#include    <CPbkFieldInfo.h>
#include    <CPbkContactItem.h>


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkMmsAddressSelect::TParams::TParams
        (const CPbkContactItem& aContact) :
    TBaseParams(aContact, aContact.DefaultMmsField())
    {
    }

EXPORT_C CPbkMmsAddressSelect::CPbkMmsAddressSelect()
    {
    }

EXPORT_C TBool CPbkMmsAddressSelect::ExecuteLD(TParams& aParams)
    {
    return CPbkAddressSelect::ExecuteLD(aParams);
    }

EXPORT_C CPbkMmsAddressSelect::~CPbkMmsAddressSelect()
	{
    delete iQueryTitle;
	}

EXPORT_C TBool CPbkMmsAddressSelect::AddressField(const TPbkContactItemField& aField) const
    {
    // Return true for non-empty phonenumber and email fields
    return (aField.FieldInfo().IsMmsField() && !aField.IsEmptyOrAllSpaces());
    }

EXPORT_C void CPbkMmsAddressSelect::NoAddressesL()
    {
    HBufC* title = ContactItem().GetContactTitleOrNullL();
    if (title)
        {
        CleanupStack::PushL(title);
        HBufC* prompt = StringLoader::LoadLC(R_QTN_PHOB_NOTE_NO_MMS_TO_NAME, *title);
        CAknInformationNote* noteDlg = new(ELeave) CAknInformationNote;
        noteDlg->ExecuteLD(*prompt);
        CleanupStack::PopAndDestroy(2); // prompt, title
        }
    else
        {
        CAknNoteWrapper * noteDlg = new(ELeave) CAknNoteWrapper;
        noteDlg->ExecuteLD(R_QTN_PHOB_NOTE_NO_MMS);
        }
    }

EXPORT_C const TDesC& CPbkMmsAddressSelect::QueryTitleL()
    {
    if (!iQueryTitle)
        {
        iQueryTitle = ContactItem().GetContactTitleL();
        }
    return *iQueryTitle;
    }

EXPORT_C TInt CPbkMmsAddressSelect::QuerySoftkeysResource() const
    {
    return R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT;
    }


//  End of File  
