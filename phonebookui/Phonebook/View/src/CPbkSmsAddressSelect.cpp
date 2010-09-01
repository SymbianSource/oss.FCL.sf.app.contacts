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
*       Provides methods for generic phonebook address selection dialog.
*
*/


// INCLUDE FILES

#include    "CPbkSmsAddressSelect.h"
#include    <avkon.rsg> // AVKON softkey resources
#include    <StringLoader.h>        // StringLoader
#include    <CPbkContactItem.h>


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkSmsAddressSelect::CPbkSmsAddressSelect()
    {
    }

EXPORT_C CPbkSmsAddressSelect::TParams::TParams
        (const CPbkContactItem& aContact) :
    TBaseParams(aContact, aContact.DefaultSmsField())
    {
    }

EXPORT_C CPbkSmsAddressSelect::~CPbkSmsAddressSelect()
	{
    delete iQueryTitle;
	}

EXPORT_C const TDesC& CPbkSmsAddressSelect::QueryTitleL()
    {
    if (!iQueryTitle)
        {
        iQueryTitle = ContactItem().GetContactTitleL();
        }
    return *iQueryTitle;
    }

EXPORT_C TInt CPbkSmsAddressSelect::QuerySoftkeysResource() const
    {
    return R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT;
    }


//  End of File  
