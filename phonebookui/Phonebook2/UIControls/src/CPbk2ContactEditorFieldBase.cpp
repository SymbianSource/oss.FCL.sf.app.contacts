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
* Description:  Phonebook 2 contact editor field base class.
*
*/


#include "CPbk2ContactEditorFieldBase.h"

// Phonebook 2
#include "MPbk2ContactEditorUiBuilder.h"
#include <MPbk2FieldProperty.h>
#include <CPbk2PresentationContactField.h>

// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>

// System includes
#include <eikedwin.h>

// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::CPbk2ContactEditorFieldBase
// -----------------------------------------------------------------------------
//
CPbk2ContactEditorFieldBase::CPbk2ContactEditorFieldBase
        ( CPbk2PresentationContactField& aContactField,
          MPbk2ContactEditorUiBuilder& aUiBuilder,
          CPbk2IconInfoContainer& aIconInfoContainer ) :
            iContactField( aContactField ),
            iUiBuilder( aUiBuilder ),
            iIconInfoContainer( aIconInfoContainer ),
            iContactDataHasChanged( EFalse )
    {
    }
    
// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::~CPbk2ContactEditorFieldBase
// -----------------------------------------------------------------------------
//
CPbk2ContactEditorFieldBase::~CPbk2ContactEditorFieldBase()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::ControlId
// -----------------------------------------------------------------------------
//
TInt CPbk2ContactEditorFieldBase::ControlId() const
    {
    return ( TInt ) this;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::ContactEditorFieldExtension
// -----------------------------------------------------------------------------
//    
TAny* CPbk2ContactEditorFieldBase::ContactEditorFieldExtension(TUid aExtensionUid )
 {
     if( aExtensionUid == KMPbk2ContactEditorFieldExtension2Uid )
         {
         return static_cast<MPbk2ContactEditorField2*>( this );
         }    
     
    return NULL;
 }   
// CPbk2ContactEditorFieldBase::HandleCustomFieldCommandL
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorFieldBase::HandleCustomFieldCommandL(TInt /*aCommand*/ )
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::FieldDataChanged
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorFieldBase::FieldDataChanged() const
    {
    return iContactDataHasChanged;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::FieldLabel
// -----------------------------------------------------------------------------
//
TPtrC CPbk2ContactEditorFieldBase::FieldLabel() const
    {
    return iContactField.FieldLabel();
    }
  
// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::SetFieldLabelL
// -----------------------------------------------------------------------------
//  
void CPbk2ContactEditorFieldBase::SetFieldLabelL( const TDesC& aLabel )
    {
    iUiBuilder.SetCurrentLineCaptionL( aLabel );
    iContactField.SetFieldLabelL( aLabel );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::ControlTextL
// -----------------------------------------------------------------------------
//  
HBufC* CPbk2ContactEditorFieldBase::ControlTextL() const
    {
    HBufC* textBuf = NULL;

    CCoeControl* ctrl = iUiBuilder.Control( ControlId() );
    TVPbkFieldStorageType fType = iContactField.FieldData().DataType();
    if ( ctrl && ( ( fType == EVPbkFieldStorageTypeText ) ||
            (fType == EVPbkFieldStorageTypeUri) ) )
        {
        textBuf = static_cast<CEikEdwin*>( ctrl )->GetTextInHBufL();
        }

    return textBuf;
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::SetFocus
// -----------------------------------------------------------------------------
//
void CPbk2ContactEditorFieldBase::SetFocus()
    {
    // It's not fatal if focusing fails
    TRAP_IGNORE( iUiBuilder.TryChangeFocusL( ControlId() ) );
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::ContactField
// -----------------------------------------------------------------------------
//    
MVPbkStoreContactField& CPbk2ContactEditorFieldBase::ContactField() const
    {
    return iContactField.StoreField();
    }

// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::FieldProperty
// -----------------------------------------------------------------------------
//    
const MPbk2FieldProperty& CPbk2ContactEditorFieldBase::FieldProperty() const
    {
    return iContactField.FieldProperty();
    }
    
// -----------------------------------------------------------------------------
// CPbk2ContactEditorFieldBase::ConsumesKeyEvent
// -----------------------------------------------------------------------------
//
TBool CPbk2ContactEditorFieldBase::ConsumesKeyEvent
        ( const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
    {
    return EFalse;
    }   

// End of File  
