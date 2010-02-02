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
* Description:  Phonebook 2 contact editor impp field.
*
*/


#include "CPbk2ContactEditorImppField.h"

// Phonebook 2
#include <MPbk2FieldProperty.h>
#include <MPbk2FieldProperty2.h>
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

} /// namespace    


    
// --------------------------------------------------------------------------
// CPbk2ContactEditorImppField::CPbk2ContactEditorImppField
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorImppField::CPbk2ContactEditorImppField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            CPbk2ContactEditorTextField( aContactField, aUiBuilder,
                aIconInfoContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImppField::~CPbk2ContactEditorImppField
// --------------------------------------------------------------------------
//  
CPbk2ContactEditorImppField::~CPbk2ContactEditorImppField()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImppField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorImppField* CPbk2ContactEditorImppField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2ContactEditorImppField* self = 
        new ( ELeave ) CPbk2ContactEditorImppField( aContactField, aUiBuilder,
            aIconInfoContainer );    
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorImppField::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorImppField::ConstructL()
    {
    MPbk2FieldProperty2* fieldPropertyExtension =
        reinterpret_cast<MPbk2FieldProperty2*>(
            const_cast<MPbk2FieldProperty&>( iContactField.FieldProperty() ).
                FieldPropertyExtension(
                    KMPbk2FieldPropertyExtension2Uid ) );
    User::LeaveIfNull( fieldPropertyExtension ); 
    
    // There is no suport for fields with unsecified service name
    if ((fieldPropertyExtension->XSpName() == KNullDesC) &&
            (MVPbkContactFieldUriData::Cast(iContactField.FieldData()).Scheme()
            == KNullDesC))
        {
        User::Leave(KErrNotSupported);
        }
    // Create and insert a line in the dialog
    iControl = static_cast<CEikEdwin*>(iUiBuilder.CreateLineL(
            FieldLabel(), ControlId(), EEikCtEdwin));

    // Control is now owned by the dialog
    TInt maxFieldLength = iContactField.MaxDataLength();
    AknEditUtils::ConstructEditingL(iControl, maxFieldLength,
        maxFieldLength, EAknEditorTextCase | EAknEditorCharactersUpperCase
        | EAknEditorCharactersLowerCase, EAknEditorAlignLeft,
        ETrue, ETrue, EFalse);

    if (iContactField.FieldProperty().EditMode() == 
            EPbk2FieldEditModeLatinOnly)
        {
        iControl->SetAknEditorFlags(EAknEditorFlagLatinInputModesOnly);
        iControl->SetAknEditorSpecialCharacterTable(
                R_AVKON_EMAIL_ADDR_SPECIAL_CHARACTER_TABLE_DIALOG );
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
    TPtrC text = MVPbkContactFieldUriData::Cast(iContactField.FieldData()).
            Text();
            
    iControl->SetTextL(&text);    
        
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
// CPbk2ContactEditorImppField::SaveFieldL
// --------------------------------------------------------------------------
//  
void CPbk2ContactEditorImppField::SaveFieldL()
    {
    iContactDataHasChanged = EFalse;

    MVPbkContactFieldUriData& data = 
        MVPbkContactFieldUriData::Cast(iContactField.FieldData());
    TPtrC curText = data.Text();
    
    HBufC* text = iControl->GetTextInHBufL();
    if (text)
        {
        if (curText.Compare(*text))
            {
            CleanupStack::PushL(text);
            
            MPbk2FieldProperty2* fieldPropertyExtension =
                reinterpret_cast<MPbk2FieldProperty2*>(
                    const_cast<MPbk2FieldProperty&>( iContactField.FieldProperty() ).
                        FieldPropertyExtension(
                            KMPbk2FieldPropertyExtension2Uid ) );
            User::LeaveIfNull( fieldPropertyExtension ); 
                       
            TPtrC servName = fieldPropertyExtension->XSpName();
            if (servName == KNullDesC)
                {
                servName.Set(data.Scheme());
                }
            data.SetUriL(servName, *text);
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
        data.SetUriL(KNullDesC);
        iContactDataHasChanged = ETrue;
        }
    }

// End of File  
