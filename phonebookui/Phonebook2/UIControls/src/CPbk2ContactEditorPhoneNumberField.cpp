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
* Description:  Phonebook 2 contact editor phone number field.
*
*/


#include "CPbk2ContactEditorPhoneNumberField.h"
#include "Pbk2PresentationUtils.h"

// Phonebook 2
#include <MPbk2ContactEditorUiBuilder.h>
#include <MPbk2ContactEditorFieldVisitor.h>
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
#include <centralrepository.h>

// Debugging headers
#include <Pbk2Debug.h>

// The empty character.
const TText KSpace = ' ';

/// Unnamed namespace for local definitions
namespace {

/**
 * Returns the number editor max length.
 *
 * @return  Number editor max length.
 */
TInt NumberEditorMaxLengthL()
    {
    CRepository* centRep = CRepository::NewLC
        ( TUid::Uid( KCRUidPhonebookInternalConfig ) );
    TInt maxLength( KErrNotFound );
    User::LeaveIfError(
        centRep->Get( KPhonebookNumberEditorMaxLength, maxLength ) );
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("NumberEditorMaxLengthL maxLength(%d)"), maxLength );
    CleanupStack::PopAndDestroy( centRep );
    return maxLength;
    }

/**
 * Returns maximum field length.
 *
 * @param aContactField     Contact field in question.
 * @return  Maximum field length.
 */
TInt MaxFieldLengthL( CPbk2PresentationContactField& aContactField )
    {
    // Determine the length of contact field in stores and central
    // repository. The smaller will be chosen.
    TInt maxLength = KVPbkUnlimitedFieldLength;
    // Read max length from store
    TVPbkFieldStorageType dataType =
        aContactField.StoreField().FieldData().DataType();
    const MVPbkContactFieldTextData& textData =
        MVPbkContactFieldTextData::Cast
            ( aContactField.StoreField().FieldData() );
    maxLength = textData.MaxLength();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("MaxFieldLengthL maxLength(%d)"), maxLength );

    // Read max length from central repository
    TInt staticMaxLength = NumberEditorMaxLengthL();
    if (maxLength == KVPbkUnlimitedFieldLength)
        {
        maxLength = staticMaxLength;
        }
    else
        {
        maxLength = Min(staticMaxLength, maxLength);
        }
    return maxLength;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2ContactEditorPhoneNumberField::CPbk2ContactEditorPhoneNumberField
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorPhoneNumberField::CPbk2ContactEditorPhoneNumberField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            CPbk2ContactEditorFieldBase( aContactField, aUiBuilder,
                aIconInfoContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPhoneNumberField::~CPbk2ContactEditorPhoneNumberField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorPhoneNumberField::~CPbk2ContactEditorPhoneNumberField()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPhoneNumberField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorPhoneNumberField* CPbk2ContactEditorPhoneNumberField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2ContactEditorPhoneNumberField* self =
        new ( ELeave ) CPbk2ContactEditorPhoneNumberField( aContactField,
        aUiBuilder, aIconInfoContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPhoneNumberField::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorPhoneNumberField::ConstructL()
    {
    // Create and insert a line in the dialog
    iControl = static_cast<CEikEdwin*>(iUiBuilder.CreateLineL(
            FieldLabel(), ControlId(), EEikCtEdwin));

    // Control is now owned by the dialog
    TInt maxFieldLength = MaxFieldLengthL( iContactField );
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
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPhoneNumberField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorPhoneNumberField::Control() const
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPhoneNumberField::SaveFieldL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorPhoneNumberField::SaveFieldL()
    {
    iContactDataHasChanged = EFalse;

    MVPbkContactFieldTextData& data =
        MVPbkContactFieldTextData::Cast(iContactField.FieldData());
    TPtrC curText(data.Text());

    HBufC* text = iControl->GetTextInHBufL();
    if (text)
        {
        TPtr number = text->Des();
        Pbk2PresentationUtils::ReplaceNonGraphicCharacters( number, KSpace );
        AknTextUtils::ConvertDigitsTo(number, EDigitTypeWestern);
        
        TInt maxFieldLength = MaxFieldLengthL( iContactField );
        TInt dataLength = curText.Length();
                
        if ( curText.Compare(*text) || ( dataLength == maxFieldLength ) )
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
// CPbk2ContactEditorPhoneNumberField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorPhoneNumberField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorPhoneNumberField::AcceptL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorPhoneNumberField::AcceptL
        (MPbk2ContactEditorFieldVisitor& aVisitor)
    {
    aVisitor.VisitL(*this);
    }

// End of File
