/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor text field.
*
*/


#include "CPbk2ContactEditorTextField.h"

// Phonebook 2
#include "MPbk2ContactEditorUiBuilder.h"
#include "MPbk2ContactEditorFieldVisitor.h"
#include <MPbk2FieldProperty.h>
#include <CPbk2PresentationContactField.h>
#include <Pbk2PresentationUtils.h>

// Virtual Phonebook
#include <MVPbkContactFieldTextData.h>

// System includes
#include <eikcapc.h>
#include <eikedwin.h>
#include <AknUtils.h>

// --------------------------------------------------------------------------
// CPbk2ContactEditorTextField::CPbk2ContactEditorTextField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorTextField::CPbk2ContactEditorTextField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            CPbk2ContactEditorFieldBase( aContactField, aUiBuilder,
                aIconInfoContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorTextField::~CPbk2ContactEditorTextField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorTextField::~CPbk2ContactEditorTextField()
    {    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorTextField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorTextField* CPbk2ContactEditorTextField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2ContactEditorTextField* self = 
        new ( ELeave ) CPbk2ContactEditorTextField( aContactField,
            aUiBuilder, aIconInfoContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorTextField::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorTextField::ConstructL()
    {
    // Create and insert a line in the dialog
    iControl = static_cast<CEikEdwin*>(iUiBuilder.CreateLineL(
            FieldLabel(), ControlId(), EEikCtEdwin));

    // Control is now owned by the dialog
    TInt maxFieldLength = iContactField.MaxDataLength();
    AknEditUtils::ConstructEditingL(iControl, maxFieldLength,
        maxFieldLength, EAknEditorTextCase | EAknEditorCharactersUpperCase
        | EAknEditorCharactersLowerCase, EAknEditorAlignLeft,
        ETrue, ETrue, EFalse);

    // Get text
    TPtrC dataPtr(MVPbkContactFieldTextData::Cast(
        iContactField.FieldData()).Text());
    HBufC* textBuf = HBufC::NewLC(dataPtr.Length());
    TPtr text = textBuf->Des();
    text.Zero();
    Pbk2PresentationUtils::AppendWithNewlineTranslationL(text, dataPtr);
    
    if (iContactField.FieldProperty().EditMode() == 
        EPbk2FieldEditModeLatinOnly)
        {
        iControl->SetAknEditorFlags(EAknEditorFlagLatinInputModesOnly);
        iControl->SetAknEditorSpecialCharacterTable(
            R_AVKON_SPECIAL_CHARACTER_TABLE_DIALOG );
        }
    else
        {
        // If not in 'only latin mode', convert digits to
        // foreign characters if necessary
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);
        }

    switch (iContactField.FieldProperty().DefaultCase())
        {
        case EPbk2FieldDefaultCaseLower:
            {
            iControl->SetAknEditorCase(EAknEditorLowerCase);
            break;
            }
        case EPbk2FieldDefaultCaseText:
            {
            iControl->SetAknEditorCase(EAknEditorTextCase);
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    // Set formatted text to editor control
   	iControl->SetTextL(&text);
    // SetTextL method above copied the text to the control,
    // so it is safe to destroy the buffer
    CleanupStack::PopAndDestroy(textBuf);

    iUiBuilder.LoadBitmapToFieldL
        ( iContactField.FieldProperty(), iIconInfoContainer, ControlId() );


	// Place cursor to the end of the line
	iControl->AddFlagToUserFlags(CEikEdwin::EJustAutoCurEnd);

    // CreateTextViewL() is flagged as deprecated but if it is not 
    // called here the ActivateL() below crashes sometimes.
    iControl->CreateTextViewL();
    iCaptionedCtrl = iUiBuilder.LineControl(ControlId());
    iCaptionedCtrl->SetTakesEnterKey(EFalse);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorTextField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorTextField::Control() const
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorTextField::SaveFieldL
// --------------------------------------------------------------------------
//  
void CPbk2ContactEditorTextField::SaveFieldL()
    {
    iContactDataHasChanged = EFalse;

    MVPbkContactFieldTextData& data = 
        MVPbkContactFieldTextData::Cast(iContactField.FieldData());
    TPtrC curText(data.Text());
    
    HBufC* text = iControl->GetTextInHBufL();
    if (text)
        {
        //Trim spaces at the end of formatted name
        text->Des().TrimRight();
        if (curText.Compare(*text))
            {
            CleanupStack::PushL(text);
            data.SetTextL(*text);
            CleanupStack::PopAndDestroy(text);
            iContactDataHasChanged = ETrue;
            }
        else
            {
            delete text;
            }
        }
    else if (curText.Length() > 0)
        {
        data.SetTextL(KNullDesC);
        iContactDataHasChanged = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorTextField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorTextField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorTextField::AcceptL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorTextField::AcceptL
        ( MPbk2ContactEditorFieldVisitor& aVisitor )
    {
    aVisitor.VisitL( *this );
    }

// End of File  
