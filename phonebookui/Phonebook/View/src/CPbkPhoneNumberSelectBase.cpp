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
*       Provides methods for the base class for phonebook phone number selection queries.
*
*/


// INCLUDE FILES

#include    "CPbkPhoneNumberSelectBase.h"
#include    <aknnotewrappers.h>
#include    <StringLoader.h>        // StringLoader
#include    <pbkview.rsg>
#include    <CPbkFieldInfo.h>
#include    <CPbkContactItem.h>


// ================= MEMBER FUNCTIONS =======================

EXPORT_C TBool CPbkPhoneNumberSelectBase::AddressField(const TPbkContactItemField& aField) const
    {
    // Return true for non-empty phonenumber fields
    return (aField.FieldInfo().IsPhoneNumberField() && !aField.IsEmptyOrAllSpaces());
    }

EXPORT_C void CPbkPhoneNumberSelectBase::NoAddressesL()
    {
    HBufC* title = ContactItem().GetContactTitleOrNullL();
    if (title)
        {
        CleanupStack::PushL(title);
        HBufC* prompt = StringLoader::LoadLC(R_QTN_PHOB_NOTE_NO_NUMBER_TO_NAME, *title);
        CAknInformationNote* noteDlg = new(ELeave) CAknInformationNote;
        noteDlg->ExecuteLD(*prompt);
        CleanupStack::PopAndDestroy(2); // prompt, title
        }
    else
        {
        CAknNoteWrapper* noteDlg = new(ELeave) CAknNoteWrapper;
        noteDlg->ExecuteLD(R_QTN_PHOB_NOTE_NO_NUMBER);
        }
    }

//  End of File  
