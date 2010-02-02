/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Phonebook Contact editor field base class.
*
*/


#ifndef __CPbkContactEditorFieldBase_H__
#define __CPbkContactEditorFieldBase_H__

//  INCLUDES
#include <MPbkContactEditorField.h>
#include "MPbkFieldEditorControl.h"
#include "TPbkContactItemField.h"

// FORWARD DECLARATIONS
class CPbkIconInfoContainer;
class CPbkContactItem;
class MPbkContactEditorUiBuilder;


// CLASS DECLARATION

/**
 * Phonebook Contact editor field base class. 
 */
NONSHARABLE_CLASS(CPbkContactEditorFieldBase) : 
        public CBase,
        public MPbkContactEditorField,
        public MPbkFieldEditorControl
    {
    protected:  // Construction
        /**
         * Constructor.
		 * @param aField reference to the field to edit
		 * @param aIconInfoContainer reference to icon info container
		 * @param aUiBuilder reference to UI builder
         */
        CPbkContactEditorFieldBase(
				const TPbkContactItemField& aField, 
				CPbkIconInfoContainer& aIconInfoContainer,
				MPbkContactEditorUiBuilder& aUiBuilder);

        /**
         * Destructor.
         */
		~CPbkContactEditorFieldBase();

        /**
         * Base constructor.
         */
        void BaseConstructL();

    public:  // from MPbkContactEditorField
        TInt ControlId() const;
        TPbkFieldId FieldId() const;
        TPtrC FieldLabel() const;
        void SetFieldLabelL(HBufC* aLabel);
        const TPbkContactItemField& ContactItemField() const;
		HBufC* ControlTextL() const;
        void ActivateL() = 0;
        void AcceptL(MPbkFieldEditorVisitor& aVisitor) = 0;
        TBool ConsumesKeyEvent(const TKeyEvent& aKeyEvent, TEventCode aType);
        TBool FieldDataChanged() const;


    protected: // from MPbkFieldEditorControl
        CEikEdwin* Control() { return NULL; }
        TPbkFieldId FieldId() { return EPbkFieldIdNone; }
        void LoadBitmapToFieldL(MPbkContactEditorUiBuilder& aUiBuilder);
    
    protected: // utility functions
        /**
         * Returns reference to iContactItemField.
         */
        TPbkContactItemField& Field();
		        
    protected:  // Data
        /// Own: Contact item field associated with this editor field
        TPbkContactItemField iContactItemField;
		/// Ref: Icon info container
		CPbkIconInfoContainer& iIconInfoContainer;
		/// Ref: UI builder
		MPbkContactEditorUiBuilder& iUiBuilder;
        ///Own: Contact data has changed
        TBool iContactDataHasChanged;
    };

#endif // __CPbkContactEditorFieldBase_H__
            
// End of File
