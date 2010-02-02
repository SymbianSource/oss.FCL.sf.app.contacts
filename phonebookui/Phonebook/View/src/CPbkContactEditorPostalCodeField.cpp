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
*           Methods for phonebook Contact editor postal code field.
*           Needed to filter out japanese input modes out.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorPostalCodeField.h"
#include "MPbkContactEditorUiBuilder.h"
#include "MPbkFieldEditorVisitor.h"

#include <eikdialg.h>
#include <eikedwin.h>
#include <AknUtils.h>
#include <eikcapc.h>
#include <featmgr.h>

#include <TPbkContactItemField.h>
#include <CPbkContactItem.h>
#include <CPbkFieldInfo.h>
#include <CPbkConstants.h>


/// Unnamed namespace for local definitons
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
/// Panic codes for CPbkContactEditorPostalCodeField
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
    _LIT(KPanicText, "CPbkContactEditorPostalCodeField");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

CPbkContactEditorPostalCodeField::CPbkContactEditorPostalCodeField
        (TPbkContactItemField& aField,
		CPbkIconInfoContainer& aIconInfoContainer,
		MPbkContactEditorUiBuilder& aUiBuilder) :
    CPbkContactEditorFieldBase(aField, aIconInfoContainer, aUiBuilder)
    {
    }

inline void CPbkContactEditorPostalCodeField::ConstructL()
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

    // No Japanese input modes tolerated in Japanese variant,
    // set the allowed onces manually
    if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
        {
        iControl->SetAknEditorFlags(EAknEditorFlagLatinInputModesOnly);
        }
    else
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);
        }
    iControl->SetAknEditorInputMode(EAknEditorTextInputMode);
    iControl->SetAknEditorAllowedInputModes (EAknEditorTextInputMode |
        EAknEditorNumericInputMode);

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

CPbkContactEditorPostalCodeField* CPbkContactEditorPostalCodeField::NewL
        (TPbkContactItemField& aField,
        MPbkContactEditorUiBuilder& aUiBuilder,
		CPbkIconInfoContainer& aIconInfoContainer)
    {
    CPbkContactEditorPostalCodeField* self =
        new(ELeave) CPbkContactEditorPostalCodeField(aField,
        aIconInfoContainer, aUiBuilder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactEditorPostalCodeField::~CPbkContactEditorPostalCodeField()
    {
    }

void CPbkContactEditorPostalCodeField::SaveFieldL()
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

void CPbkContactEditorPostalCodeField::AddFieldL(CPbkContactItem& aContact)
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

void CPbkContactEditorPostalCodeField::SetControlTextL(const TDesC& aDes)
    {
    iControl->SetTextL(&aDes);
    }

void CPbkContactEditorPostalCodeField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }


CEikEdwin* CPbkContactEditorPostalCodeField::Control()
    {
    return iControl;
    }

TPbkFieldId CPbkContactEditorPostalCodeField::FieldId()
    {
    return ContactItemField().FieldInfo().FieldId();
    }

void CPbkContactEditorPostalCodeField::AcceptL
        (MPbkFieldEditorVisitor& aVisitor)
    {
    aVisitor.VisitL(*this);
    }

// End of File

