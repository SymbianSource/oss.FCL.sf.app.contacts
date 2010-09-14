/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor dialog UI address office field.
*
*/

#include "CPbk2ContactEditorUIAddressOfficeField.h"

// Phonebook 2
#include <MPbk2ContactEditorUiBuilder.h>
#include "MPbk2ContactEditorFieldVisitor.h"
#include "MPbk2ContactEditorUiBuilderExtension.h"
#include "CPbk2IconInfoContainer.h"
#include <pbk2commonui.rsg>
#include "MPbk2UIField.h"
#include "Pbk2EditorLineIds.hrh" 
#include <Pbk2UIControls.hrh>
#include "CPbk2ContactEditorReadonlyField.h"
#include <pbk2uicontrols.rsg>
// System includes
#include <eikcapc.h>
#include <eikedwin.h>
#include <AknUtils.h>

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressOfficeField::CPbk2ContactEditorUIAddressOfficeField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAddressOfficeField::CPbk2ContactEditorUIAddressOfficeField
        ( MPbk2UIField* aField,
	      MPbk2ContactEditorUiBuilder& aUiBuilder,
	      CPbk2IconInfoContainer& aIconInfoContainer,
	      TInt aCustomPosition ) :
            CPbk2ContactEditorUIFieldBase( aField, aUiBuilder,
                aIconInfoContainer, aCustomPosition )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressOfficeField::~CPbk2ContactEditorUIAddressOfficeField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAddressOfficeField::~CPbk2ContactEditorUIAddressOfficeField()
    { 
    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressOfficeField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAddressOfficeField* CPbk2ContactEditorUIAddressOfficeField::NewL
        ( MPbk2UIField* aField,
      	  MPbk2ContactEditorUiBuilder& aUiBuilder,
      	  CPbk2IconInfoContainer& aIconInfoContainer,
	      TInt aCustomPosition,
          const TDesC& aCustomText )
    {
    CPbk2ContactEditorUIAddressOfficeField* self = 
        new ( ELeave ) CPbk2ContactEditorUIAddressOfficeField( aField,
            aUiBuilder, aIconInfoContainer, aCustomPosition );
    CleanupStack::PushL( self );
    self->ConstructL(aCustomText);
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressOfficeField::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAddressOfficeField::ConstructL(const TDesC& aCustomText)
    {
    User::LeaveIfNull(iField);
    MPbk2ContactEditorUiBuilderExtension* ext = 
	    static_cast<MPbk2ContactEditorUiBuilderExtension*>(
	        iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));
	
	iControl = static_cast<CPbk2ContactEditorReadonlyField*>(
	ext->AddCustomFieldToFormL(iCustomPosition, R_PBK2_ADDRESS_OFFICE_LINE));     
	iControl->InitializeL(this); 
	SetControlTextL(aCustomText);
	iControl->CreateTextViewL();
	iCaptionedCtrl = iUiBuilder.LineControl(ControlId()); 
	iUiBuilder.TryChangeFocusL(ControlId());
	LoadBitmapToFieldL(iField->IconId());
	SetFieldLabelL(iField->DefaultLabel());
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressOfficeField::ControlId
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorUIAddressOfficeField::ControlId()
    {
     return EPbk2EditorLineAddressOffice; 
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressOfficeField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorUIAddressOfficeField::Control()
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressOfficeField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAddressOfficeField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressOfficeField::HandleCustomFieldCommandL
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorUIAddressOfficeField::HandleCustomFieldCommandL(TInt aCommand )
    {
    TBool ret(EFalse);
    MPbk2ContactEditorUiBuilderExtension* ext = 
         static_cast<MPbk2ContactEditorUiBuilderExtension*>(
             iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));        
    
    if(aCommand == EPbk2CmdEditorHandleCustomSelect)
        {
        ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorSelectAddressOffice);
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressOfficeField::SetControlTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAddressOfficeField::SetControlTextL(const TDesC& aText)
    {
    iControl->SetTextL(&aText);
    }


// End of File  
