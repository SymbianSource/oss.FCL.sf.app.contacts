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
* Description:  Phonebook 2 contact editor postal code field.
*
*/


#include "CPbk2ContactEditorPostalCodeField.h"

// Phonebook 2
#include "MPbk2ContactEditorUiBuilder.h"
#include "MPbk2ContactEditorFieldVisitor.h"
#include <MPbk2FieldProperty.h>
#include <CPbk2PresentationContactField.h>
#include <Phonebook2PrivateCRKeys.h>

// Virtual Phonebook
#include <MVPbkContactFieldTextData.h>
#include <VPbkVariant.hrh>

// System includes
#include <eikcapc.h>
#include <eikedwin.h>
#include <AknUtils.h>
#include <centralrepository.h>

// --------------------------------------------------------------------------
// CPbk2ContactEditorPostalCodeField::CPbk2ContactEditorPostalCodeField
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorPostalCodeField::CPbk2ContactEditorPostalCodeField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) : 
            CPbk2ContactEditorFieldBase( aContactField, aUiBuilder,
                aIconInfoContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPostalCodeField::~CPbk2ContactEditorPostalCodeField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorPostalCodeField::~CPbk2ContactEditorPostalCodeField()
    {    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPostalCodeField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorPostalCodeField* CPbk2ContactEditorPostalCodeField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2ContactEditorPostalCodeField* self = 
        new ( ELeave ) CPbk2ContactEditorPostalCodeField( aContactField, 
            aUiBuilder, aIconInfoContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPostalCodeField::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorPostalCodeField::ConstructL()
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
    TPtr text= textBuf->Des();
    text = dataPtr;
    
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(text);

    // Set default editor mode (defaults to numeric unless variated)
    TInt lvFlags;
    CRepository* key = CRepository::NewLC(TUid::Uid(KCRUidPhonebook));
    TInt err = key->Get(KPhonebookLocalVariationFlags, lvFlags);
    CleanupStack::PopAndDestroy(key);
    if ((err == KErrNone) && (lvFlags & EVPbkLVPostalFieldAlphaDefault))
        {
        iControl->SetAknEditorInputMode(EAknEditorTextInputMode);
        }
    else
        {
        iControl->SetAknEditorInputMode(EAknEditorNumericInputMode);
        }
    // No japanese input modes tolerated, set the allowed ones manually
    iControl->SetAknEditorAllowedInputModes (EAknEditorTextInputMode |
        EAknEditorNumericInputMode);
    
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
    iCaptionedCtrl->SetTakesEnterKey(ETrue);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPostalCodeField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorPostalCodeField::Control() const
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPostalCodeField::SaveFieldL
// --------------------------------------------------------------------------
//  
void CPbk2ContactEditorPostalCodeField::SaveFieldL()
    {
    iContactDataHasChanged = EFalse;

    MVPbkContactFieldTextData& data = 
        MVPbkContactFieldTextData::Cast(iContactField.FieldData());
    TPtrC curText(data.Text());
    
    HBufC* text = iControl->GetTextInHBufL();
    if (text)
        {
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
// CPbk2ContactEditorPostalCodeField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorPostalCodeField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPostalCodeField::AcceptL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorPostalCodeField::AcceptL
        (MPbk2ContactEditorFieldVisitor& aVisitor)
    {
    aVisitor.VisitL(*this);
    }

// End of File  
