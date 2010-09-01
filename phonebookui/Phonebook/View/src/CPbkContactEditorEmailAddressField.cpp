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
*           Methods for phonebook Contact editor URL field.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorEmailAddressField.h"
#include "MPbkContactEditorUiBuilder.h"

#include <eikdialg.h>
#include <eikedwin.h>
#include <AknUtils.h>
#include <eikcapc.h>
#include <aknedsts.h>

#include <TPbkContactItemField.h>
#include <CPbkContactItem.h>
#include <CPbkFieldInfo.h>
#include <CPbkConstants.h>


/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
/// Panic codes for CPbkContactEditorEmailAddressField
enum TPanicCode
    {
    EPanicPreCond_ConstructL,
    };
#endif  // _DEBUG

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactEditorEmailAddressField");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace

// ================= MEMBER FUNCTIONS =======================

inline CPbkContactEditorEmailAddressField::CPbkContactEditorEmailAddressField
        (TPbkContactItemField& aField,
        CPbkIconInfoContainer& aIconInfoContainer,
        MPbkContactEditorUiBuilder& aUiBuilder) :
    CPbkContactEditorTextField(aField, aIconInfoContainer, aUiBuilder)
    {
    }

inline void CPbkContactEditorEmailAddressField::ConstructL()
    {
    __ASSERT_DEBUG(ContactItemField().StorageType() == KStorageTypeText,
        Panic(EPanicPreCond_ConstructL));

    // Create and insert a line in the dialog
    iControl = static_cast<CEikEdwin*>(iUiBuilder.CreateLineL(
            FieldLabel(), ControlId(), EEikCtEdwin));

    // ctrl is now owned by the dialog
    AknEditUtils::ConstructEditingL(iControl, ContactItemField().FieldInfo().MaxLength(),
            CPbkConstants::FieldEditorLength(),
            EAknEditorTextCase | EAknEditorCharactersUpperCase | EAknEditorCharactersLowerCase, EAknEditorAlignLeft,
            ETrue, ETrue, EFalse);

    iControl->SetAknEditorSpecialCharacterTable(
        R_AVKON_EMAIL_ADDR_SPECIAL_CHARACTER_TABLE_DIALOG);

    if (ContactItemField().FieldInfo().EditMode() == EPbkFieldEditModeLatinOnly)
        {
        iControl->SetAknEditorFlags(EAknEditorFlagLatinInputModesOnly);
        }

    switch (ContactItemField().FieldInfo().DefaultCase())
        {
        case EPbkFieldDefaultCaseLower:
            {
            iControl->SetAknEditorCase(EAknEditorLowerCase);
            break;
            }
        case EPbkFieldDefaultCaseText:
            {
            iControl->SetAknEditorCase(EAknEditorTextCase);
            break;
            }
        default:
            break;
        }

    TPtrC text = ContactItemField().Text();
    iControl->SetTextL(&text);

    LoadBitmapToFieldL(iUiBuilder);

    // Place cursor to the end of the line
    iControl->AddFlagToUserFlags(CEikEdwin::EJustAutoCurEnd);

    // CreateTextViewL() is flagged as deprecated but if it is not
    // called here the ActivateL() below crashes sometimes.
    iControl->CreateTextViewL();
    iCaptionedCtrl = iUiBuilder.LineControl(ControlId());
    iCaptionedCtrl->SetTakesEnterKey(ETrue);
    }

CPbkContactEditorEmailAddressField* CPbkContactEditorEmailAddressField::NewL
        (TPbkContactItemField& aField,
        MPbkContactEditorUiBuilder& aUiBuilder,
        CPbkIconInfoContainer& aIconInfoContainer)
    {
    CPbkContactEditorEmailAddressField* self = new(ELeave) CPbkContactEditorEmailAddressField(
            aField, aIconInfoContainer, aUiBuilder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// End of File
