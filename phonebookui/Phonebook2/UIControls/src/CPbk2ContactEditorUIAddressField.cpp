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
* Description:  Phonebook 2 contact editor dialog UI address field.
*
*/

#include "CPbk2ContactEditorUIAddressField.h"

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
// CPbk2ContactEditorUIAddressField::CPbk2ContactEditorUIAddressField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAddressField::CPbk2ContactEditorUIAddressField
        ( MPbk2UIField* aField,
	      MPbk2ContactEditorUiBuilder& aUiBuilder,
	      CPbk2IconInfoContainer& aIconInfoContainer,
	      TInt aCustomPosition ) :
            CPbk2ContactEditorUIFieldBase( aField, aUiBuilder,
                aIconInfoContainer, aCustomPosition )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressField::~CPbk2ContactEditorUIAddressField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAddressField::~CPbk2ContactEditorUIAddressField()
    { 
    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorUIAddressField* CPbk2ContactEditorUIAddressField::NewL
        ( MPbk2UIField* aField,
      	  MPbk2ContactEditorUiBuilder& aUiBuilder,
      	  CPbk2IconInfoContainer& aIconInfoContainer,
	      TInt aCustomPosition,
          const TDesC& aCustomText )
    {
    CPbk2ContactEditorUIAddressField* self = 
        new ( ELeave ) CPbk2ContactEditorUIAddressField( aField,
            aUiBuilder, aIconInfoContainer, aCustomPosition );
    CleanupStack::PushL( self );
    self->ConstructL(aCustomText);
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressField::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAddressField::ConstructL(const TDesC& aCustomText)
    {
    User::LeaveIfNull(iField);
    MPbk2ContactEditorUiBuilderExtension* ext = 
	    static_cast<MPbk2ContactEditorUiBuilderExtension*>(
	        iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));
	
	iControl = static_cast<CPbk2ContactEditorReadonlyField*>(
	ext->AddCustomFieldToFormL(iCustomPosition, R_PBK2_ADDRESS_LINE));     
	iControl->InitializeL(this); 
	SetControlTextL(aCustomText);
	iControl->CreateTextViewL();
	iCaptionedCtrl = iUiBuilder.LineControl(ControlId()); 
	iUiBuilder.TryChangeFocusL(ControlId());
	LoadBitmapToFieldL(iField->IconId());
	SetFieldLabelL(iField->DefaultLabel());
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressField::ControlId
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorUIAddressField::ControlId()
    {
     return EPbk2EditorLineAddress; 
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorUIAddressField::Control()
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAddressField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressField::HandleCustomFieldCommandL
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorUIAddressField::HandleCustomFieldCommandL(TInt aCommand )
    {
    TBool ret(EFalse);
    MPbk2ContactEditorUiBuilderExtension* ext = 
         static_cast<MPbk2ContactEditorUiBuilderExtension*>(
             iUiBuilder.ContactEditorUiBuilderExtension(KNullUid));        
    
    if(aCommand == EPbk2CmdEditorHandleCustomSelect)
        {
        ret = ext->HandleCustomFieldCommandL(EPbk2CmdEditorSelectAddress);
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressField::SetControlTextL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorUIAddressField::SetControlTextL(const TDesC& aText)
    {
    iControl->SetTextL(&aText);
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorUIAddressField::ConsumesKeyEvent
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorUIAddressField::ConsumesKeyEvent
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse keyResponse = iControl->OfferKeyEventL( aKeyEvent, aType );

    if ( EKeyWasConsumed == keyResponse )
        {
        return ETrue;
        }
            
    return EFalse;
    }   


// End of File  
