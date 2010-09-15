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
* Description:  Phonebook 2 contact editor DTMF field.
*
*/


#include "CPbk2ContactEditorDTMFField.h"

// Phonebook 2
#include "MPbk2ContactEditorUiBuilder.h"
#include "MPbk2ContactEditorFieldVisitor.h"
#include <MPbk2FieldProperty.h>
#include <CPbk2PresentationContactField.h>
#include <MPbk2ContactEditorExtension.h>
#include <Phonebook2InternalCRKeys.h>

// Virtual Phonebook
#include <MVPbkContactFieldTextData.h>

// System includes
#include <eikcapc.h>
#include <eikedwin.h>
#include <AknUtils.h>

// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2ContactEditorDTMFField::CPbk2ContactEditorDTMFField
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorDTMFField::CPbk2ContactEditorDTMFField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            CPbk2ContactEditorFieldBase( aContactField, aUiBuilder,
                aIconInfoContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDTMFField::~CPbk2ContactEditorDTMFField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorDTMFField::~CPbk2ContactEditorDTMFField()
    {    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDTMFField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorDTMFField* CPbk2ContactEditorDTMFField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2ContactEditorDTMFField* self = 
        new ( ELeave ) CPbk2ContactEditorDTMFField( aContactField, 
        aUiBuilder, aIconInfoContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDTMFField::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorDTMFField::ConstructL()
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
    text = dataPtr.Left(maxFieldLength);
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);
    
    // Set input capabilities and character modes
    if (iContactField.FieldProperty().EditMode() == EPbk2FieldEditModeNumeric)
        {
        iControl->SetAknEditorNumericKeymap(EAknEditorStandardNumberModeKeymap);
        iControl->SetAknEditorInputMode(EAknEditorNumericInputMode);
        iControl->SetAknEditorAllowedInputModes(EAknEditorNumericInputMode);
        iControl->SetAknEditorSpecialCharacterTable(0);
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

    iControl->CreateTextViewL();
    iCaptionedCtrl = iUiBuilder.LineControl(ControlId());
    iCaptionedCtrl->SetTakesEnterKey(ETrue);
    
    // Enable partial screen input
    TInt flags = iControl->AknEditorFlags();
    iControl->SetAknEditorFlags(flags | EAknEditorFlagEnablePartialScreen);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDTMFField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorDTMFField::Control() const
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDTMFField::SaveFieldL
// --------------------------------------------------------------------------
//  
void CPbk2ContactEditorDTMFField::SaveFieldL()
    {
    iContactDataHasChanged = EFalse;

    MVPbkContactFieldTextData& data = 
        MVPbkContactFieldTextData::Cast(iContactField.FieldData());
    TPtrC curText(data.Text());
    
    HBufC* text = iControl->GetTextInHBufL();
    if (text)
        {
        TPtr number = text->Des();
        AknTextUtils::ConvertDigitsTo(number, EDigitTypeWestern);
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
// CPbk2ContactEditorDTMFField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDTMFField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDTMFField::AcceptL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDTMFField::AcceptL
        (MPbk2ContactEditorFieldVisitor& aVisitor)
    {
    aVisitor.VisitL(*this);
    }

// End of File  
