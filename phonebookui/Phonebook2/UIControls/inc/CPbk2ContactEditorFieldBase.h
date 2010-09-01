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


#ifndef CPBK2CONTACTEDITORFIELDBASE_H
#define CPBK2CONTACTEDITORFIELDBASE_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ContactEditorField.h"
#include "MPbk2ContactEditorField2.h"

// FORWARD DECLARATIONS
class MVPbkStoreContactField;
class MPbk2FieldProperty;
class MPbk2ContactEditorUiBuilder;
class CPbk2IconInfoContainer;
class CPbk2PresentationContactField;
class MPbk2ContactEditorFieldVisitor;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor field base class.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorFieldBase) : public CBase,
                                                 public MPbk2ContactEditorField,
                                                 public MPbk2ContactEditorField2
    {
    protected:  // Constructors and destructor
        
        /**
         * C++ constructor.
         *
         * @param aContactField         Contact field.
         * @param aUiBuilder            UI builder for adding
         *                              the field into dialog.
         * @param aIconInfoContainer    An icon container for setting
         *                              field icon.
         */
        CPbk2ContactEditorFieldBase(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );

        /**
         * Destructor.
         */
        ~CPbk2ContactEditorFieldBase();

    public: // From MPbk2ContactEditorField
        TInt ControlId() const;
        CEikEdwin* Control() const = 0;
        void SaveFieldL() = 0;
        
        TBool FieldDataChanged() const;
        TPtrC FieldLabel() const;
        void SetFieldLabelL(
                const TDesC& aLabel );
		    HBufC* ControlTextL() const;
        void SetFocus();
        void ActivateL() = 0;
        MVPbkStoreContactField& ContactField() const;
        const MPbk2FieldProperty&  FieldProperty() const;
        void AcceptL(
                MPbk2ContactEditorFieldVisitor& aVisitor ) = 0;
        TBool ConsumesKeyEvent(
                const TKeyEvent& aKeyEvent, 
                TEventCode aType );
        TAny* ContactEditorFieldExtension(TUid aExtensionUid );
   
   public: // From MPbk2ContactEditorField2
        TBool HandleCustomFieldCommandL(TInt aCommand);        
            
        
    protected:  // Data
        /// Ref: The contact field
        CPbk2PresentationContactField& iContactField;
        /// Ref: UI builder for adding the field into dialog
        MPbk2ContactEditorUiBuilder& iUiBuilder;
        /// Ref: Icon container for setting field icon
        CPbk2IconInfoContainer& iIconInfoContainer;
        ///Own: Contact data has changed
        TBool iContactDataHasChanged;
    };

#endif  // CPBK2CONTACTEDITORFIELDBASE_H
            
// End of File
