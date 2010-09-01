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
*       Provides methods for phonebook call number selection dialog.
*
*/


// INCLUDE FILES

#include "CPbkSendKeyAddressSelect.h"
#include <avkon.hrh>         // AVKON softkey codes
#include <aknnotewrappers.h> // AVKON Notes
#include <StringLoader.h>    // StringLoader
#include <PbkView.rsg>
#include <CPbkContactItem.h>
#include <CPbkContactEngine.h>
#include <CPbkConstants.h>
#include <CPbkFieldInfo.h>
#include <featmgr.h>

// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkSendKeyAddressSelect::CPbkSendKeyAddressSelect
        (CPbkContactEngine& aContactEngine) :
        iEngine(aContactEngine)
    {
    // CBase::operator new(TLeave) resets member data
    }

EXPORT_C CPbkSendKeyAddressSelect::TParams::TParams
        (const CPbkContactItem& aContact,
        const TPbkContactItemField* aDefaultField) :
        TBaseParams(aContact, aDefaultField)
    {
    }

EXPORT_C TBool CPbkSendKeyAddressSelect::ExecuteLD(TParams& aParams)
    {
    return CPbkAddressSelect::ExecuteLD(aParams);
    }

EXPORT_C CPbkSendKeyAddressSelect::~CPbkSendKeyAddressSelect()
	{
    delete iQueryTitle;
	}

/**
 * Makes phone number query return selection if Call key is pressed.
 */ 
EXPORT_C TKeyResponse CPbkSendKeyAddressSelect::PbkControlKeyEventL
        (const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if (aType == EEventKey && aKeyEvent.iCode == EKeyPhoneSend)
        {
        // Event is Send key, tell field selection dialog to accept current selection
        AttemptExitL(ETrue);
        return EKeyWasConsumed;
        }
    return EKeyWasNotConsumed;
    }

EXPORT_C const TDesC& CPbkSendKeyAddressSelect::QueryTitleL()
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

EXPORT_C TInt CPbkSendKeyAddressSelect::QuerySoftkeysResource() const
    {
    return R_PBK_SOFTKEYS_CALL_CANCEL;
    }

EXPORT_C TBool CPbkSendKeyAddressSelect::AddressField
        (const TPbkContactItemField& aField) const
    {
    // Return true for non-empty voip fields    
    TBool ret = ETrue;
    
    // Show VoIP fields only if the feature is on        
    if (FeatureManager::FeatureSupported(KFeatureIdCommonVoip))
        {
        ret = aField.FieldInfo().IsVoipField() &&
            !aField.IsEmptyOrAllSpaces();
        }
    else
        {
        ret = aField.FieldInfo().IsPhoneNumberField() &&
            !aField.IsEmptyOrAllSpaces();
        }

    if (aField.FieldInfo().FieldId() == EPbkFieldIdPushToTalk)
        {
        // If the field is a PTT field, we must query for POC support
        // and show it only in case the support exists
        ret = iEngine.Constants()->LocallyVariatedFeatureEnabled(EPbkLVPOC);        
        // If some product does not want to enable the POC feature, 
        // it can be variated with Phonebook's central repository flags.
        }
    
    return ret;
    }
    

EXPORT_C void CPbkSendKeyAddressSelect::NoAddressesL()
    {
    HBufC* title = ContactItem().GetContactTitleOrNullL();
    if (title)
        {
        CleanupStack::PushL(title);
        HBufC* prompt = StringLoader::LoadLC
            (R_QTN_PHOB_NOTE_NO_NUMBER_TO_NAME, *title);
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
