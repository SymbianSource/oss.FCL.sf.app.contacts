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
*       Provides methods for phonebook video call number selection dialog.
*
*/


// INCLUDE FILES

#include    "CPbkVideoNumberSelect.h"
#include    <avkon.hrh>         // AVKON softkey codes
#include    <aknnotewrappers.h> // AVKON Notes
#include    <StringLoader.h>    // StringLoader
#include    <pbkview.rsg>               // PbkView resources
#include    <CPbkContactItem.h>


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkVideoNumberSelect::CPbkVideoNumberSelect()
    {
    // CBase::operator new(TLeave) resets member data
    }

EXPORT_C CPbkVideoNumberSelect::TParams::TParams
        (const CPbkContactItem& aContact) :
        TBaseParams(aContact, aContact.DefaultVideoNumberField())
    {
    }

EXPORT_C TBool CPbkVideoNumberSelect::ExecuteLD(TParams& aParams)
    {
    return CPbkAddressSelect::ExecuteLD(aParams);
    }

EXPORT_C CPbkVideoNumberSelect::~CPbkVideoNumberSelect()
	{
    delete iQueryTitle;
	}

/**
 * Makes video call number query return selection if Call key is pressed.
 */ 
EXPORT_C TKeyResponse CPbkVideoNumberSelect::PbkControlKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if (aType == EEventKey && aKeyEvent.iCode == EKeyPhoneSend)
        {
        // Event is Send key, tell field selection dialog to accept current selection
        AttemptExitL(ETrue);
        return EKeyWasConsumed;
        }
    return EKeyWasNotConsumed;
    }

EXPORT_C const TDesC& CPbkVideoNumberSelect::QueryTitleL()
    {
    if (!iQueryTitle)
        {
        HBufC* title = ContactItem().GetContactTitleL();
        CleanupStack::PushL(title);
        iQueryTitle = StringLoader::LoadL(R_QTN_PHOB_QTL_CALL_TO_NAME, *title);
        CleanupStack::PopAndDestroy(title);
        }
    return *iQueryTitle;
    }

EXPORT_C TInt CPbkVideoNumberSelect::QuerySoftkeysResource() const
    {
    return R_PBK_SOFTKEYS_CALL_CANCEL;
    }


//  End of File  
