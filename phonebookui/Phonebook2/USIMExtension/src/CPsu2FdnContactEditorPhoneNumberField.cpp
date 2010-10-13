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
* Description:  Phonebook 2 FDN contact editor phone number field.
*
*/


#include "CPsu2FdnContactEditorPhoneNumberField.h"

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
        ("FDN NumberEditorMaxLengthL maxLength(%d)"), maxLength );
    CleanupStack::PopAndDestroy( centRep );
    return maxLength;
    }

/**
 * Returns maximum field length.
 *
 * @param aContactField     Contact field in question.
 * @return  Maximum field length.
 */
TInt MaxFieldLengthL( MVPbkStoreContactField& aContactField )
    {
    // Determine the length of contact field in stores and central
    // repository. The smaller will be chosen.
    TInt maxLength = KVPbkUnlimitedFieldLength;
    // Read max length from store
    TVPbkFieldStorageType dataType =
        aContactField.FieldData().DataType();
    const MVPbkContactFieldTextData& textData =
        MVPbkContactFieldTextData::Cast
            ( aContactField.FieldData() );
    maxLength = textData.MaxLength();
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("FDN MaxFieldLengthL maxLength(%d)"), maxLength );

    // Read max length from central repository
    TInt staticMaxLength = NumberEditorMaxLengthL();
    if ( maxLength == KVPbkUnlimitedFieldLength )
        {
        maxLength = staticMaxLength;
        }
    else
        {
        maxLength = Min( staticMaxLength, maxLength );
        } 
    return maxLength;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::CPsu2FdnContactEditorPhoneNumberFi
// --------------------------------------------------------------------------
//
inline CPsu2FdnContactEditorPhoneNumberField::
    CPsu2FdnContactEditorPhoneNumberField
        ( MVPbkStoreContactField& aContactField,
          const MPbk2FieldProperty& aFieldProperty,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          const CPbk2IconInfoContainer& aIconInfoContainer ) :
            iContactField( aContactField ),
            iFieldProperty( aFieldProperty ),
            iUiBuilder( aUiBuilder ),
            iIconInfoContainer( aIconInfoContainer ),
            iContactDataHasChanged( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::~CPsu2FdnContactEditorPhoneNumberF
// --------------------------------------------------------------------------
//
CPsu2FdnContactEditorPhoneNumberField::
        ~CPsu2FdnContactEditorPhoneNumberField()
    {
    }

// --------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::NewL
// --------------------------------------------------------------------------
//
CPsu2FdnContactEditorPhoneNumberField*
    CPsu2FdnContactEditorPhoneNumberField::NewL
        ( MVPbkStoreContactField& aContactField,
          const MPbk2FieldProperty& aFieldProperty,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPsu2FdnContactEditorPhoneNumberField* self =
        new ( ELeave ) CPsu2FdnContactEditorPhoneNumberField
            ( aContactField, aFieldProperty, aUiBuilder,
              aIconInfoContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::ConstructL
// --------------------------------------------------------------------------
//
inline void CPsu2FdnContactEditorPhoneNumberField::ConstructL()
    {
    const TInt KPlusSignLength = 1;
    
    // Create and insert a line in the dialog
    iControl = static_cast<CEikEdwin*>
        ( iUiBuilder.CreateLineL( FieldLabel(), ControlId(), EEikCtEdwin ) );

    // Get max field length from store contact and central repository
    TInt maxFieldLength = MaxFieldLengthL( iContactField ); 
    
    // Get max presentation field length
    TInt maxUiFieldLength = iFieldProperty.MaxLength(); 
    
    // Get the minimum of all of these
    maxFieldLength = Min( maxFieldLength, maxUiFieldLength );
    
    // Control is now owned by the dialog
    // Increase maxFieldLength by one (space for '+' sign)    
    AknEditUtils::ConstructEditingL( iControl, 
        maxFieldLength + KPlusSignLength, maxUiFieldLength + KPlusSignLength,
        EAknEditorTextCase | EAknEditorCharactersUpperCase
        | EAknEditorCharactersLowerCase, EAknEditorAlignLeft,
        ETrue, ETrue, EFalse );

    // Get text
    TPtrC dataPtr( MVPbkContactFieldTextData::Cast
        ( iContactField.FieldData() ).Text() ) ;
    HBufC* textBuf = HBufC::NewLC( dataPtr.Length() );
    TPtr text = textBuf->Des();
    // Increase maxFieldLength by one (space for '+' sign)
    text = dataPtr.Left( maxFieldLength + KPlusSignLength ); 
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( text );

    // Set input capabilities and character modes
    if ( iFieldProperty.EditMode() == EPbk2FieldEditModeNumeric )
        {
        iControl->SetAknEditorNumericKeymap
            ( EAknEditorFixedDiallingNumberModeKeymap );
        iControl->SetAknEditorInputMode( EAknEditorNumericInputMode );
        iControl->SetAknEditorAllowedInputModes
            ( EAknEditorNumericInputMode );
        iControl->SetAknEditorSpecialCharacterTable( 0 );
        }

    // Set formatted text to editor control
   	iControl->SetTextL( &text );
    // SetTextL method above copied the text to the control,
    // so it is safe to destroy the buffer
    CleanupStack::PopAndDestroy( textBuf );

    iUiBuilder.LoadBitmapToFieldL
        ( iFieldProperty, iIconInfoContainer, ControlId() );

	// Place cursor to the end of the line
	iControl->AddFlagToUserFlags( CEikEdwin::EJustAutoCurEnd );

    iControl->CreateTextViewL();
    iCaptionedCtrl = iUiBuilder.LineControl( ControlId() );
    iCaptionedCtrl->SetTakesEnterKey( ETrue );


    }

// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::ControlId
// -----------------------------------------------------------------------------
//
TInt CPsu2FdnContactEditorPhoneNumberField::ControlId() const
    {
    return (TInt) this;
    }

// --------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPsu2FdnContactEditorPhoneNumberField::Control() const
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::SaveFieldL
// --------------------------------------------------------------------------
//
void CPsu2FdnContactEditorPhoneNumberField::SaveFieldL()
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

// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::HandleCustomFieldCommandL
// -----------------------------------------------------------------------------
//
TBool CPsu2FdnContactEditorPhoneNumberField::HandleCustomFieldCommandL(TInt /*aCommand*/ )
    {
    return EFalse;
    }


// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::ContactUiControlExtension
// -----------------------------------------------------------------------------
//
TAny* CPsu2FdnContactEditorPhoneNumberField::ContactEditorFieldExtension(TUid aExtensionUid )
	{
     if( aExtensionUid == KMPbk2ContactEditorFieldExtension2Uid )
        { 
		return static_cast<MPbk2ContactEditorField2*>( this );
		}
		
    return NULL;
    }



// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::FieldDataChanged
// -----------------------------------------------------------------------------
//
TBool CPsu2FdnContactEditorPhoneNumberField::FieldDataChanged() const
    {
    return iContactDataHasChanged;
    }

// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::FieldLabel
// -----------------------------------------------------------------------------
//
TPtrC CPsu2FdnContactEditorPhoneNumberField::FieldLabel() const
    {
    return iFieldProperty.DefaultLabel();
    }

// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::SetFieldLabelL
// -----------------------------------------------------------------------------
//  
void CPsu2FdnContactEditorPhoneNumberField::SetFieldLabelL
        ( const TDesC& /*aLabel*/ )
    {
    // Do nothing.
    // FDN contact's label can not be changed.
    }

// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::ControlTextL
// -----------------------------------------------------------------------------
//  
HBufC* CPsu2FdnContactEditorPhoneNumberField::ControlTextL() const
    {
    HBufC* textBuf = NULL;
    CCoeControl* ctrl = iUiBuilder.Control(ControlId());
    if (ctrl && 
        iContactField.FieldData().DataType() == EVPbkFieldStorageTypeText)
        {
        textBuf = static_cast<CEikEdwin*>(ctrl)->GetTextInHBufL();
        }
    return textBuf;
    }

// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::SetFocus
// -----------------------------------------------------------------------------
//
void CPsu2FdnContactEditorPhoneNumberField::SetFocus()
    {
    // It's not fatal if focusing fails
    TRAP_IGNORE(iUiBuilder.TryChangeFocusL(ControlId()));
    }

// --------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::ActivateL
// --------------------------------------------------------------------------
//
void CPsu2FdnContactEditorPhoneNumberField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::ContactField
// -----------------------------------------------------------------------------
//    
MVPbkStoreContactField&
        CPsu2FdnContactEditorPhoneNumberField::ContactField() const
    {
    return iContactField;
    }

// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::FieldProperty
// -----------------------------------------------------------------------------
//    
const MPbk2FieldProperty&
        CPsu2FdnContactEditorPhoneNumberField::FieldProperty() const
    {
    return iFieldProperty;
    }

// --------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::AcceptL
// --------------------------------------------------------------------------
//
void CPsu2FdnContactEditorPhoneNumberField::AcceptL
        ( MPbk2ContactEditorFieldVisitor& aVisitor )
    {
    aVisitor.VisitL( *this );
    }

// -----------------------------------------------------------------------------
// CPsu2FdnContactEditorPhoneNumberField::ConsumesKeyEvent
// -----------------------------------------------------------------------------
//
TBool CPsu2FdnContactEditorPhoneNumberField::ConsumesKeyEvent
        ( const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
    {
    return EFalse;
    }   

// End of File
