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
* Description:  Phonebook 2 contact editor dialog UI assign from maps field.
*
*/

#include "CPbk2ContactEditorUIAFMField.h"

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
#include <pbk2mapuires.rsg>
// System includes
#include <eikcapc.h>
#include <eikedwin.h>
#include <AknUtils.h>
#include <StringLoader.h>

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAFMField::CPbk2ContactEditorUIAFMField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAFMField::CPbk2ContactEditorUIAFMField
        ( MPbk2UIField* aField,
	      MPbk2ContactEditorUiBuilder& aUiBuilder,
	      CPbk2IconInfoContainer& aIconInfoContainer,
	      TInt aCustomPosition ) :
            CPbk2ContactEditorUIFieldBase( aField, aUiBuilder,
                aIconInfoContainer, aCustomPosition )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAFMField::~CPbk2ContactEditorUIAFMField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAFMField::~CPbk2ContactEditorUIAFMField()
    { 
    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAFMField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAFMField* CPbk2ContactEditorUIAFMField::NewL
        ( MPbk2UIField* aField,
      	  MPbk2ContactEditorUiBuilder& aUiBuilder,
      	  CPbk2IconInfoContainer& aIconInfoContainer,
	      TInt aCustomPosition )
    {
    CPbk2ContactEditorUIAFMField* self = 
        new ( ELeave ) CPbk2ContactEditorUIAFMField( aField,
            aUiBuilder, aIconInfoContainer, aCustomPosition );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAFMField::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAFMField::ConstructL()
    {
    User::LeaveIfNull(iField);
    MPbk2ContactEditorUiBuilderExtension* ext = 
	    static_cast<MPbk2ContactEditorUiBuilderExtension*>(
	        iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));
	
	iControl = static_cast<CPbk2ContactEditorReadonlyField*>(
	ext->AddCustomFieldToFormL(iCustomPosition, R_PBK2_ASSIGN_FROM_MAPS_LINE));     
	iControl->InitializeL(this); 
	HBufC* text = StringLoader::LoadL(R_QTN_PHOB_ADDRESS_EDITOR_ASSIGN_FROM_MAP);
	CleanupStack::PushL(text);
	SetControlTextL(text->Des());
	CleanupStack::PopAndDestroy(text);
	iControl->CreateTextViewL(); 
	iCaptionedCtrl = iUiBuilder.LineControl(ControlId()); 
	iUiBuilder.TryChangeFocusL(ControlId());
	LoadBitmapToFieldL(iField->IconId());
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAFMField::ControlId
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorUIAFMField::ControlId()
    {
     return EPbk2EditorLineAssignFromMaps; 
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAFMField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorUIAFMField::Control()
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAFMField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAFMField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIAFMField::HandleCustomFieldCommandL
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorUIAFMField::HandleCustomFieldCommandL(TInt aCommand )
    {
    TBool ret(EFalse);
    MPbk2ContactEditorUiBuilderExtension* ext = 
         static_cast<MPbk2ContactEditorUiBuilderExtension*>(
             iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));        
    
    if(aCommand == EPbk2CmdEditorHandleCustomSelect)
        {
        ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorSelectAssignFromMaps);
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAFMField::SetControlTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAFMField::SetControlTextL(const TDesC& aText)
    {
    iControl->SetTextL(&aText);
    }


// End of File  
