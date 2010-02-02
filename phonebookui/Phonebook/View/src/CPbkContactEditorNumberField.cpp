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
*           Methods for phonebook contact editor number field.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorNumberField.h"
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
/// Panic codes for CPbkContactEditorNumberField
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
    _LIT(KPanicText, "CPbkContactEditorNumberField");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

CPbkContactEditorNumberField::CPbkContactEditorNumberField
        (TPbkContactItemField& aField,
		CPbkIconInfoContainer& aIconInfoContainer,
		MPbkContactEditorUiBuilder& aUiBuilder) :
    CPbkContactEditorFieldBase(aField, aIconInfoContainer, aUiBuilder)
    {
    }

inline void CPbkContactEditorNumberField::ConstructL()
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

    if (ContactItemField().FieldInfo().EditMode()
            == EPbkFieldEditModeLatinOnly)
        {
        iControl->SetAknEditorFlags(EAknEditorFlagLatinInputModesOnly);
        }
    else
        {
        // If not in 'only latin mode', convert digits to
        // foreign characters if necessary
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);
        }
    
	if (ContactItemField().FieldInfo().EditMode() == EPbkFieldEditModeNumeric)
        {
        iControl->SetAknEditorNumericKeymap(EAknEditorPlainNumberModeKeymap);
        iControl->SetAknEditorInputMode(EAknEditorNumericInputMode);
        iControl->SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
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

CPbkContactEditorNumberField* CPbkContactEditorNumberField::NewL
        (TPbkContactItemField& aField,
        MPbkContactEditorUiBuilder& aUiBuilder,
		CPbkIconInfoContainer& aIconInfoContainer)
    {
    CPbkContactEditorNumberField* self =
        new(ELeave) CPbkContactEditorNumberField(aField,
        aIconInfoContainer, aUiBuilder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactEditorNumberField::~CPbkContactEditorNumberField()
    {
    }

void CPbkContactEditorNumberField::SaveFieldL()
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

void CPbkContactEditorNumberField::AddFieldL(CPbkContactItem& aContact)
    {
    HBufC* text = iControl->GetTextInHBufL();    
    if (text)
        {
        CleanupStack::PushL(text);
        TPbkContactItemField* field = aContact.AddOrReturnUnusedFieldL
            ( Field().FieldInfo() );
        CleanupStack::Pop(text);
        if (field)
            {
            field->TextStorage()->SetText(text);
            }
        }
    }

void CPbkContactEditorNumberField::SetControlTextL(const TDesC& aDes)
    {
    iControl->SetTextL(&aDes);
    }

void CPbkContactEditorNumberField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

void CPbkContactEditorNumberField::AcceptL(MPbkFieldEditorVisitor& aVisitor)
    {
    aVisitor.VisitL(*this);
    }

// End of File
