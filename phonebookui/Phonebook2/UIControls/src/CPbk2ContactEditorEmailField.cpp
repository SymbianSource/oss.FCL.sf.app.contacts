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
* Description:  Phonebook 2 contact editor email field.
*
*/


#include "CPbk2ContactEditorEmailField.h"

// Phonebook 2
#include <MPbk2FieldProperty.h>
#include "MPbk2ContactEditorUiBuilder.h"
#include <CPbk2PresentationContactField.h>
#include <Pbk2PresentationUtils.h>

// Virtual Phonebook
#include <MVPbkContactFieldTextData.h>

// System includes
#include <eikcapc.h>
#include <eikedwin.h>
#include <AknUtils.h>

// --------------------------------------------------------------------------
// CPbk2ContactEditorEmailField::CPbk2ContactEditorEmailField
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorEmailField::CPbk2ContactEditorEmailField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            CPbk2ContactEditorTextField( aContactField, aUiBuilder,
                aIconInfoContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEmailField::~CPbk2ContactEditorEmailField
// --------------------------------------------------------------------------
//  
CPbk2ContactEditorEmailField::~CPbk2ContactEditorEmailField()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEmailField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorEmailField* CPbk2ContactEditorEmailField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2ContactEditorEmailField* self = 
        new ( ELeave ) CPbk2ContactEditorEmailField( aContactField,
            aUiBuilder, aIconInfoContainer );    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorEmailField::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorEmailField::ConstructL()
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
            R_AVKON_EMAIL_ADDR_SPECIAL_CHARACTER_TABLE_DIALOG );
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
    
// End of File  
