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
* Description:  Phonebook2 contact editor date field.
*
*/


#include "CPbk2ContactEditorDateField.h"

// Phonebook 2
#include "MPbk2ContactEditorUiBuilder.h"
#include "MPbk2ContactEditorFieldVisitor.h"
#include <MPbk2FieldProperty.h>
#include <CPbk2PresentationContactField.h>
#include <pbk2uicontrols.rsg>

// Virtual Phonebook
#include <MVPbkContactFieldDateTimeData.h>

// System includes
#include <eikcapc.h>
#include <eikmfne.h>
#include <AknUtils.h>
#include <barsread.h>

// --------------------------------------------------------------------------
// CPbk2ContactEditorDateField::CPbk2ContactEditorDateField
// --------------------------------------------------------------------------
//
CPbk2ContactEditorDateField::CPbk2ContactEditorDateField
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            CPbk2ContactEditorFieldBase( aContactField, aUiBuilder,
                aIconInfoContainer )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDateField::~CPbk2ContactEditorDateField
// --------------------------------------------------------------------------
//  
CPbk2ContactEditorDateField::~CPbk2ContactEditorDateField()
    {    
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDateField::NewLC
// --------------------------------------------------------------------------
//
CPbk2ContactEditorDateField* CPbk2ContactEditorDateField::NewLC
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer )
    {
    CPbk2ContactEditorDateField* self = 
        new ( ELeave ) CPbk2ContactEditorDateField( aContactField,
        aUiBuilder, aIconInfoContainer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDateField::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDateField::ConstructL()
    {
    // Create and insert a line in the dialog
    iControl = static_cast<CEikDateEditor*>(iUiBuilder.CreateLineL(
            FieldLabel(), ControlId(), EEikCtDateEditor));

    // Control is now owned by the dialog
    TResourceReader reader;
	CCoeEnv::Static()->CreateResourceReaderLC(reader, R_PBK2_EDITOR_DATE_SETTINGS);
    TTime minimumDate = iControl->ReadDate(reader);
    TTime maximumDate = iControl->ReadDate(reader);
    CleanupStack::PopAndDestroy(); // R_PBK_EDITOR_DATE_SETTINGS

	TTime time = MVPbkContactFieldDateTimeData::Cast(
	    iContactField.FieldData()).DateTime();
	    
	if (time == Time::NullTTime())
		{
        time.HomeTime();
        }
    iControl->ConstructL( minimumDate, maximumDate, time, EFalse );

    // Enable partial screen input
    iControl->SetFeature(CEikMfne::EPartialScreenInput, ETrue);
    iUiBuilder.LoadBitmapToFieldL
        ( iContactField.FieldProperty(), iIconInfoContainer, ControlId() );

    iCaptionedCtrl = iUiBuilder.LineControl(ControlId());
    iUiBuilder.SetEditableL(ETrue);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDateField::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorDateField::Control() const
    {
    // Can not return CEikEdwin object from here, but
    // for this class - it does not matter
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDateField::SaveFieldL
// --------------------------------------------------------------------------
//  
void CPbk2ContactEditorDateField::SaveFieldL()
    {
    iContactDataHasChanged = EFalse;
    
    const TTime dateTime = iControl->Date();
    MVPbkContactFieldDateTimeData& data =
        MVPbkContactFieldDateTimeData::Cast(iContactField.FieldData());
    if (data.DateTime() != dateTime)
        {
        data.SetDateTime(dateTime);
        iContactDataHasChanged = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDateField::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDateField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDateField::AcceptL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorDateField::AcceptL
        (MPbk2ContactEditorFieldVisitor& aVisitor)
    {
    aVisitor.VisitL(*this);
    }

// End of File  
