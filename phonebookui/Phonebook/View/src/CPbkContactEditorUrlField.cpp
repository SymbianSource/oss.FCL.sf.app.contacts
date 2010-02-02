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
#include "CPbkContactEditorUrlField.h"
#include "MPbkContactEditorUiBuilder.h"

#include <eikdialg.h>
#include <eikedwin.h>
#include <AknUtils.h>
#include <eikcapc.h>

#include <TPbkContactItemField.h>
#include <CPbkContactItem.h>
#include <CPbkFieldInfo.h>
#include <CPbkConstants.h>


/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
/// Panic codes for CPbkContactEditorUrlField
enum TPanicCode
    {
    EPanicPreCond_ConstructL,
    EPanicPreCond_SaveFieldL
    };
#endif  // _DEBUG

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactEditorUrlField");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace

_LIT(KHttp, "http://");

// ================= MEMBER FUNCTIONS =======================

inline CPbkContactEditorUrlField::CPbkContactEditorUrlField
        (TPbkContactItemField& aField,
        CPbkIconInfoContainer& aIconInfoContainer,
        MPbkContactEditorUiBuilder& aUiBuilder) :
    CPbkContactEditorTextField(aField, aIconInfoContainer, aUiBuilder)
    {
    }

inline void CPbkContactEditorUrlField::ConstructL()
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
        R_AVKON_URL_SPECIAL_CHARACTER_TABLE_DIALOG);

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

    // Set formatted text to editor control
    TPtrC text = ContactItemField().Text();
    if (text.Length() == 0)
        {
        // Insert http:// prefix to URL fields
        iControl->SetTextL(&KHttp());
        }
    else
        {
        iControl->SetTextL(&text);
        }

    LoadBitmapToFieldL(iUiBuilder);

    // Place cursor to the end of the line
    iControl->AddFlagToUserFlags(CEikEdwin::EJustAutoCurEnd);

    // CreateTextViewL() is flagged as deprecated but if it is not
    // called here the ActivateL() below crashes sometimes.
    iControl->CreateTextViewL();
    iCaptionedCtrl = iUiBuilder.LineControl(ControlId());
    iCaptionedCtrl->SetTakesEnterKey(ETrue);
    }

CPbkContactEditorUrlField* CPbkContactEditorUrlField::NewL
        (TPbkContactItemField& aField,
        MPbkContactEditorUiBuilder& aUiBuilder,
        CPbkIconInfoContainer& aIconInfoContainer)
    {
    CPbkContactEditorUrlField* self = new(ELeave) CPbkContactEditorUrlField(
            aField, aIconInfoContainer, aUiBuilder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactEditorUrlField::~CPbkContactEditorUrlField()
    {
    }

void CPbkContactEditorUrlField::SaveFieldL()
    {
    __ASSERT_DEBUG(iControl && Field().StorageType() == KStorageTypeText,
            Panic(EPanicPreCond_SaveFieldL));

    iContactDataHasChanged = EFalse;

    HBufC* text = iControl->GetTextInHBufL();
    if (text)
        {
        if (!text->Compare(KHttp))
            {
            // field contains only the http:// prefix
            // do not save
            Field().TextStorage()->SetTextL(KNullDesC);
            iContactDataHasChanged = ETrue;
            }
        else if (Field().TextStorage()->Text() != *text)
            {
            // store the text in the contact item
            Field().TextStorage()->SetText(text);
            text = NULL;
            iContactDataHasChanged = ETrue;
            }
        delete text;
        }
    else
        {
        // Ensure field text is empty
        if (Field().TextStorage()->Text().Length() > 0)
            {
            Field().TextStorage()->SetTextL(KNullDesC);
            iContactDataHasChanged = ETrue;
            }
        }
    }

// End of File
