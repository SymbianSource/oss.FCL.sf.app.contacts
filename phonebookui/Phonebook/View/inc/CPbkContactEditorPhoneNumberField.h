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
*     Phonebook contact editor phone number field.
*
*/


#ifndef __CPbkContactEditorPhoneNumberField_H__
#define __CPbkContactEditorPhoneNumberField_H__

//  INCLUDES
#include "CPbkContactEditorFieldBase.h"

// FORWARD DECLARATIONS
class MPbkContactEditorUiBuilder;
class CEikEdwin;
class CPbkContactItem;
class CEikCaptionedControl;

// CLASS DECLARATION

/**
 * Phonebook Contact editor text field. 
 */
NONSHARABLE_CLASS(CPbkContactEditorPhoneNumberField) : 
        public CPbkContactEditorFieldBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class - phone number editor to a field.
		 * @param aField the to be edited field
		 * @param aUiBuilder reference to UI builder
		 * @param aIconInfoContainer reference to icon info container
         */
		static CPbkContactEditorPhoneNumberField* NewL(
                TPbkContactItemField& aField,
                MPbkContactEditorUiBuilder& aUiBuilder,
				CPbkIconInfoContainer& aIconInfoContainer);
        
        /**
         * Destructor.
         */
        ~CPbkContactEditorPhoneNumberField();
		
    public:  // from MPbkContactEditorField
        void SaveFieldL();
        void AddFieldL(CPbkContactItem& aContact);
        void ActivateL();
        void AcceptL(MPbkFieldEditorVisitor& aVisitor);
                        
    private:  // Implementation
        CPbkContactEditorPhoneNumberField(
				TPbkContactItemField& aField, 
				CPbkIconInfoContainer& aIconInfoContainer,
				MPbkContactEditorUiBuilder& aUiBuilder);
        void ConstructL();

    private:  // Data
        /// Ref: Fields editor
        CEikEdwin* iControl;
        /// Ref: fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
    };

#endif // __CPbkContactEditorPhoneNumberField_H__
            
// End of File
