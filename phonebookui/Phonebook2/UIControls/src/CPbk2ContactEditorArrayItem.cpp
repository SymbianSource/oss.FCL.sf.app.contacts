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
* Description:  Phonebook 2 contact editor dialog array item.
*
*/

#include "CPbk2ContactEditorArrayItem.h"

// Phonebook 2

// Virtual Phonebook

// System includes

// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::CPbk2ContactEditorArrayItem
// --------------------------------------------------------------------------
//
CPbk2ContactEditorArrayItem::CPbk2ContactEditorArrayItem
        ( MPbk2ContactEditorField* aContactEdytorField ) :
         iContactEdytorField(aContactEdytorField), iContactEdytorUIField(NULL)
    {
    }
        
// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::CPbk2ContactEditorArrayItem
// --------------------------------------------------------------------------
//
CPbk2ContactEditorArrayItem::CPbk2ContactEditorArrayItem
        ( MPbk2ContactEditorUIField* aContactEdytorUIField ) :
         iContactEdytorField(NULL), iContactEdytorUIField(aContactEdytorUIField)
    {
    }
     
// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::~CPbk2ContactEditorArrayItem
// --------------------------------------------------------------------------
//
CPbk2ContactEditorArrayItem::~CPbk2ContactEditorArrayItem()
    {
    delete iContactEdytorField;
    delete iContactEdytorUIField;
    }
        
// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEditorArrayItem* CPbk2ContactEditorArrayItem::NewL
        ( MPbk2ContactEditorField* aContactEdytorField ) 
	{
	User::LeaveIfNull(aContactEdytorField);
	CPbk2ContactEditorArrayItem* self = 
        new ( ELeave ) CPbk2ContactEditorArrayItem( aContactEdytorField );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
        
// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEditorArrayItem* CPbk2ContactEditorArrayItem::NewL
        ( MPbk2ContactEditorUIField* aContactEdytorUIField ) 
	{
	User::LeaveIfNull(aContactEdytorUIField);
	CPbk2ContactEditorArrayItem* self = 
        new ( ELeave ) CPbk2ContactEditorArrayItem( aContactEdytorUIField );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::ContactEditorUIField
// --------------------------------------------------------------------------
//
MPbk2ContactEditorUIField* CPbk2ContactEditorArrayItem::ContactEditorUIField() 
	{
	return iContactEdytorUIField;
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::ContactEditorField
// --------------------------------------------------------------------------
//
MPbk2ContactEditorField* CPbk2ContactEditorArrayItem::ContactEditorField() 
	{
	return iContactEdytorField;
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorArrayItem::ConstructL() 
	{

	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::ControlId
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorArrayItem::ControlId() 
	{
	if( iContactEdytorUIField )
		{
		return iContactEdytorUIField->ControlId();
		}
	return iContactEdytorField->ControlId();
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::SetFocus
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorArrayItem::SetFocus() 
	{
	if( iContactEdytorUIField )
		{
		iContactEdytorUIField->SetFocus();
		}
	else
		{
		iContactEdytorField->SetFocus();
		}
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::ActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorArrayItem::ActivateL() 
	{
	if( iContactEdytorUIField )
		{
		iContactEdytorUIField->ActivateL();
		}
	else
		{
		iContactEdytorField->ActivateL();
		}
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::ConsumesKeyEvent
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorArrayItem::ConsumesKeyEvent(
		const TKeyEvent& aKeyEvent, 
        TEventCode aType ) 
	{
	if( iContactEdytorUIField )
		{
		return iContactEdytorUIField->ConsumesKeyEvent( aKeyEvent, aType );
		}
	return iContactEdytorField->ConsumesKeyEvent( aKeyEvent, aType );
	}

// --------------------------------------------------------------------------
// CPbk2ContactEditorArrayItem::Control
// --------------------------------------------------------------------------
//
CEikEdwin* CPbk2ContactEditorArrayItem::Control() 
	{
	if( iContactEdytorUIField )
		{
		return iContactEdytorUIField->Control();
		}
	return iContactEdytorField->Control();
	}
