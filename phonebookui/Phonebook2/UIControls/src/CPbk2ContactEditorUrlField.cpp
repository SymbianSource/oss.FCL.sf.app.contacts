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
* Description:  Phonebook 2 contact editor URL field.
*
*/


#include "CPbk2ContactEditorUrlField.h"

// Phonebook 2
#include <MPbk2FieldProperty.h>
#include "MPbk2ContactEditorUiBuilder.h"
#include <CPbk2PresentationContactField.h>

// Virtual Phonebook
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>

// System includes
#include <eikcapc.h>
#include <eikedwin.h>
#include <AknUtils.h>
    
/// Unnamed namespace for local definitions
namespace {

// CONSTANTS
_LIT(KHttp, "http://");  

} /// namespace    


// --------------------------------------------------------------------------
// CPbk2ContactEditorUrlField::CPbk2ContactEditorUrlField
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorUrlField::CPbk2ContactEditorUrlField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            CPbk2ContactEditorTextField( aContactField, aUiBuilder,
                aIconInfoContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUrlField::~CPbk2ContactEditorUrlField
// --------------------------------------------------------------------------
//  
CPbk2ContactEditorUrlField::~CPbk2ContactEditorUrlField()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUrlField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUrlField* CPbk2ContactEditorUrlField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2ContactEditorUrlField* self = 
        new ( ELeave ) CPbk2ContactEditorUrlField( aContactField, aUiBuilder,
            aIconInfoContainer );    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUrlField::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorUrlField::ConstructL()
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

    // T9 should be deactivated in all contact editors always
    DeactivateT9(iContactField.FieldProperty().EditMode());

    if (iContactField.FieldProperty().EditMode() == 
            EPbk2FieldEditModeLatinOnly)
        {
        iControl->SetAknEditorSpecialCharacterTable(
            R_AVKON_URL_SPECIAL_CHARACTER_TABLE_DIALOG );
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

    // Get text
    TPtrC text(MVPbkContactFieldTextData::Cast(
        iContactField.FieldData()).Text());
            
    if (text.Length() == 0)
        {
        // Insert http:// prefix to URL fields
        iControl->SetTextL(&KHttp());
        }
    else
        {
    	iControl->SetTextL(&text);    
        }
        
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
