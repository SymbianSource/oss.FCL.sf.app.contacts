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
*           Methods for phonebook Contact editor reading text field (Japanese).
*
*/


// INCLUDE FILES
#include "CPbkContactEditorReadingField.h"
#include "MPbkContactEditorUiBuilder.h"
#include "MPbkFieldEditorVisitor.h"

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
/// Panic codes for CPbkContactEditorReadingField
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
    _LIT(KPanicText, "CPbkContactEditorReadingField");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

CPbkContactEditorReadingField::CPbkContactEditorReadingField
        (TPbkContactItemField& aField,
		CPbkIconInfoContainer& aIconInfoContainer,
		MPbkContactEditorUiBuilder& aUiBuilder) :
    CPbkContactEditorFieldBase(aField, aIconInfoContainer, aUiBuilder)
    {
    }

inline void CPbkContactEditorReadingField::ConstructL()
    {
    __ASSERT_DEBUG(ContactItemField().StorageType() == KStorageTypeText, 
        Panic(EPanicPreCond_ConstructL));

    // Create and insert a line in the dialog
    iControl = static_cast<CEikEdwin*>(iUiBuilder.CreateLineL(
            FieldLabel(), ControlId(), EEikCtEdwin));

    // ctrl is now owned by the dialog
    AknEditUtils::ConstructEditingL(iControl, ContactItemField().
        FieldInfo().MaxLength(), 
            CPbkConstants::FieldEditorLength(), 
            EAknEditorTextCase | EAknEditorCharactersUpperCase
            | EAknEditorCharactersLowerCase, EAknEditorAlignLeft,
            ETrue, ETrue, EFalse);

    // Get text
    HBufC* textBuf = HBufC::NewLC(ContactItemField().Text().Length());
    TPtr text= textBuf->Des();
    text = ContactItemField().Text();

    iControl->SetAknEditorInputMode(EAknEditorKatakanaInputMode);
    iControl->SetAknEditorAllowedInputModes (EAknEditorKatakanaInputMode |
        EAknEditorHalfWidthTextInputMode | EAknEditorNumericInputMode);

    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);

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
            {
            break;
            }
        }

    // Set formatted text to editor control
   	iControl->SetTextL(&text);
    // SetTextL method above copied the text to the control,
    // so it is safe to destroy the buffer
    CleanupStack::PopAndDestroy(textBuf);

    LoadBitmapToFieldL(iUiBuilder);

	// Place cursor to the end of the line
	iControl->AddFlagToUserFlags(CEikEdwin::EJustAutoCurEnd);

    // CreateTextViewL() is flagged as deprecated but if it is not 
    // called here the ActivateL() below crashes sometimes.
    iControl->CreateTextViewL();
    iCaptionedCtrl = iUiBuilder.LineControl(ControlId());
    iCaptionedCtrl->SetTakesEnterKey(ETrue);
    }

CPbkContactEditorReadingField* CPbkContactEditorReadingField::NewL
        (TPbkContactItemField& aField,
        MPbkContactEditorUiBuilder& aUiBuilder,
		CPbkIconInfoContainer& aIconInfoContainer)
    {
    CPbkContactEditorReadingField* self =
        new(ELeave) CPbkContactEditorReadingField(aField,
        aIconInfoContainer, aUiBuilder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactEditorReadingField::~CPbkContactEditorReadingField()
    {
    }

void CPbkContactEditorReadingField::SaveFieldL()
    {
    __ASSERT_DEBUG(iControl && Field().StorageType() == KStorageTypeText, 
            Panic(EPanicPreCond_SaveFieldL));

    iContactDataHasChanged = EFalse;
    
    HBufC* text = iControl->GetTextInHBufL();
    if (text)
        {
        if (Field().TextStorage()->Text() != *text)
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

void CPbkContactEditorReadingField::AddFieldL(CPbkContactItem& aContact)
    {
    HBufC* text = iControl->GetTextInHBufL();    
    if (text)
        {
        CleanupStack::PushL(text);
        TPbkContactItemField* field = aContact.AddOrReturnUnusedFieldL
            (Field().FieldInfo());
        CleanupStack::Pop(text);
        if (field)
            {
            field->TextStorage()->SetText(text);
            }
        }
    }

void CPbkContactEditorReadingField::SetControlTextL(const TDesC& aDes)
    {
    iControl->SetTextL(&aDes);
    }

void CPbkContactEditorReadingField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }


CEikEdwin* CPbkContactEditorReadingField::Control()
    {
    return iControl;
    }

TPbkFieldId CPbkContactEditorReadingField::FieldId()
    {
    return ContactItemField().FieldInfo().FieldId();
    }

void CPbkContactEditorReadingField::AcceptL
        (MPbkFieldEditorVisitor& aVisitor)
    {
    aVisitor.VisitL(*this);
    }

// End of File

