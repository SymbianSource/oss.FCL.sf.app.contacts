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

#include    "CPbkURLAddressSelect.h"
#include    <avkon.hrh> // AVKON softkey codes
#include    <avkon.rsg> // R_AVKON_SOFTKEYS_SELECT_CANCEL;
#include    <aknnotewrappers.h>
#include    <StringLoader.h>        // StringLoader
#include    <PbkView.rsg>
#include    <CPbkFieldInfo.h>
#include    <CPbkContactItem.h>


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkURLAddressSelect::TParams::TParams
        (const CPbkContactItem& aContact) :
    TBaseParams(aContact, aContact.DefaultMmsField())
    {
    }

EXPORT_C CPbkURLAddressSelect::CPbkURLAddressSelect()
    {
    }

EXPORT_C TPtrC CPbkURLAddressSelect::ExecuteLD(TParams& aParams)
    {
    TPtrC result;
	if (CPbkAddressSelect::ExecuteLD(aParams))
        {
        result.Set(aParams.SelectedField()->Text());
        }
    return result;
    }

EXPORT_C CPbkURLAddressSelect::~CPbkURLAddressSelect()
	{
    delete iQueryTitle;
	}

EXPORT_C TBool CPbkURLAddressSelect::AddressField
	(const TPbkContactItemField& aField) const
    {
    // Return true for non-empty URL fields
    return ((aField.FieldInfo().FieldId() == EPbkFieldIdURL) &&
		!aField.IsEmptyOrAllSpaces());
    }

EXPORT_C void CPbkURLAddressSelect::NoAddressesL()
    {
	// This never gets used, since when focus is on contact
	// without URL the go to URL menu item is not shown
    }

EXPORT_C const TDesC& CPbkURLAddressSelect::QueryTitleL()
    {
    if (!iQueryTitle)
        {
        iQueryTitle = ContactItem().GetContactTitleL();
        }
    return *iQueryTitle;
    }

EXPORT_C TInt CPbkURLAddressSelect::QuerySoftkeysResource() const
    {
    return R_PBK_SOFTKEYS_GOTO_CANCEL;
    }


//  End of File  
