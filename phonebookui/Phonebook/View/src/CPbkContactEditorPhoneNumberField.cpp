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
*           Methods for Phonebook Contact editor phone number field.
*
*/


// INCLUDE FILES
#include "CPbkContactEditorPhoneNumberField.h"
#include "MPbkContactEditorUiBuilder.h"
#include "MPbkFieldEditorVisitor.h"

#include <uikon.hrh>
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
/// Panic codes for CPbkContactEditorPhoneNumberField
enum TPanicCode
    {
	EPanicPreCond_ConstructL,
	EPanicPreCond_SaveControlL
    };
#endif  // _DEBUG

// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactEditorPhoneNumberField");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactEditorPhoneNumberField::CPbkContactEditorPhoneNumberField
        (TPbkContactItemField& aField,
		CPbkIconInfoContainer& aIconInfoContainer,
		MPbkContactEditorUiBuilder& aUiBuilder) :
    CPbkContactEditorFieldBase(aField, aIconInfoContainer, aUiBuilder)
    {
    }

inline void CPbkContactEditorPhoneNumberField::ConstructL()
    {
    __ASSERT_DEBUG(Field().StorageType() == KStorageTypeText, 
        Panic(EPanicPreCond_ConstructL));

    // Create and insert a line in the dialog
    iControl = static_cast<CEikEdwin*>(iUiBuilder.CreateLineL(
            ContactItemField().Label(), ControlId(), EEikCtEdwin));

    // ctrl is now owned by the dialog
    AknEditUtils::ConstructEditingL(iControl, ContactItemField().
        FieldInfo().MaxLength(),
            CPbkConstants::FieldEditorLength(), 
            EAknEditorTextCase | EAknEditorCharactersUpperCase
            | EAknEditorCharactersLowerCase, EAknEditorAlignLeft, 
            ETrue, ETrue, EFalse);

    // Set input capabilities and character modes
    if (ContactItemField().FieldInfo().EditMode() == EPbkFieldEditModeNumeric)
        {
        iControl->SetAknEditorNumericKeymap(EAknEditorStandardNumberModeKeymap);
        iControl->SetAknEditorInputMode(EAknEditorNumericInputMode);
        iControl->SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
        iControl->SetAknEditorSpecialCharacterTable(0);
        }
    
    // Set text to editor control
    HBufC* textBuf = HBufC::NewLC(Field().FieldInfo().MaxLength());
    TPtr text= textBuf->Des();
    text = Field().Text().Left(Field().FieldInfo().MaxLength());
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);
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

CPbkContactEditorPhoneNumberField* CPbkContactEditorPhoneNumberField::NewL
        (TPbkContactItemField& aField,
        MPbkContactEditorUiBuilder& aUiBuilder,
		CPbkIconInfoContainer& aIconInfoContainer)
    {
    CPbkContactEditorPhoneNumberField* self =
        new(ELeave) CPbkContactEditorPhoneNumberField(aField,
            aIconInfoContainer, aUiBuilder);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkContactEditorPhoneNumberField::~CPbkContactEditorPhoneNumberField()
    {
    }

void CPbkContactEditorPhoneNumberField::SaveFieldL()
    {
    __ASSERT_DEBUG(iControl && Field().StorageType() == KStorageTypeText, 
            Panic(EPanicPreCond_SaveControlL));

    iContactDataHasChanged = EFalse;
    
    HBufC* text = iControl->GetTextInHBufL();
    if (text)
        {
        if (Field().TextStorage()->Text() != *text)
            {
            // store the text in the contact item
            TPtr number = text->Des();
            AknTextUtils::ConvertDigitsTo(number, EDigitTypeWestern);
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

void CPbkContactEditorPhoneNumberField::AddFieldL(CPbkContactItem& aContact)
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

void CPbkContactEditorPhoneNumberField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

void CPbkContactEditorPhoneNumberField::AcceptL
        (MPbkFieldEditorVisitor& aVisitor)
    {
    aVisitor.VisitL(*this);
    }

// End of File
