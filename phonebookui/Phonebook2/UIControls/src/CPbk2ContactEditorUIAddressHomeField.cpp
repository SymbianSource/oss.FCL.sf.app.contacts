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
* Description:  Phonebook 2 contact editor dialog UI address home field.
*
*/

#include "CPbk2ContactEditorUIAddressHomeField.h"

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
// CPbk2ContactEditorUIAddressHomeField::CPbk2ContactEditorUIAddressHomeField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAddressHomeField::CPbk2ContactEditorUIAddressHomeField
        ( MPbk2UIField* aField,
	      MPbk2ContactEditorUiBuilder& aUiBuilder,
	      CPbk2IconInfoContainer& aIconInfoContainer,
	      TInt aCustomPosition ) :
            CPbk2ContactEditorUIFieldBase( aField, aUiBuilder,
                aIconInfoContainer, aCustomPosition )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressHomeField::~CPbk2ContactEditorUIAddressHomeField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAddressHomeField::~CPbk2ContactEditorUIAddressHomeField()
    { 
    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressHomeField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAddressHomeField* CPbk2ContactEditorUIAddressHomeField::NewL
        ( MPbk2UIField* aField,
      	  MPbk2ContactEditorUiBuilder& aUiBuilder,
      	  CPbk2IconInfoContainer& aIconInfoContainer,
	      TInt aCustomPosition,
          const TDesC& aCustomText )
    {
    CPbk2ContactEditorUIAddressHomeField* self = 
        new ( ELeave ) CPbk2ContactEditorUIAddressHomeField( aField,
            aUiBuilder, aIconInfoContainer, aCustomPosition );
    CleanupStack::PushL( self );
    self->ConstructL(aCustomText);
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressHomeField::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAddressHomeField::ConstructL(const TDesC& aCustomText)
    {
    User::LeaveIfNull(iField);
    MPbk2ContactEditorUiBuilderExtension* ext = 
	    static_cast<MPbk2ContactEditorUiBuilderExtension*>(
	        iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));
	
	iControl = static_cast<CPbk2ContactEditorReadonlyField*>(
	ext->AddCustomFieldToFormL(iCustomPosition, R_PBK2_ADDRESS_HOME_LINE));     
	iControl->InitializeL(this);  
	SetControlTextL(aCustomText);
	iControl->CreateTextViewL();
	iCaptionedCtrl = iUiBuilder.LineControl(ControlId()); 
	iUiBuilder.TryChangeFocusL(ControlId());
	LoadBitmapToFieldL(iField->IconId());
	SetFieldLabelL(iField->DefaultLabel());
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressHomeField::ControlId
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorUIAddressHomeField::ControlId()
    {
     return EPbk2EditorLineAddressHome; 
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressHomeField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorUIAddressHomeField::Control()
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressHomeField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAddressHomeField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressHomeField::HandleCustomFieldCommandL
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorUIAddressHomeField::HandleCustomFieldCommandL(TInt aCommand )
    {
    TBool ret(EFalse);
    MPbk2ContactEditorUiBuilderExtension* ext = 
         static_cast<MPbk2ContactEditorUiBuilderExtension*>(
             iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));        
    
    if(aCommand == EPbk2CmdEditorHandleCustomSelect)
        {
        ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorSelectAddressHome);
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressHomeField::SetControlTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAddressHomeField::SetControlTextL(const TDesC& aText)
    {
    iControl->SetTextL(&aText);
    }


// End of File  
