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
*     Phonebook contact editor postal codefield.
*
*/


#ifndef __CPbkContactEditorPostalCodeField_H__
#define __CPbkContactEditorPostalCodeField_H__

//  INCLUDES
#include "CPbkContactEditorFieldBase.h"


// FORWARD DECLARATIONS
class CEikEdwin;
class MPbkContactEditorUiBuilder;
class CPbkContactItem;
class CEikCaptionedControl;


// CLASS DECLARATION

/**
 * Phonebook Contact editor reading field. 
 */
NONSHARABLE_CLASS(CPbkContactEditorPostalCodeField) : 
        public CPbkContactEditorFieldBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class - text editor to a field.
		 * @param aField the to be edited field
		 * @param aUiBuilder reference to UI builder
		 * @param aIconInfoContainer reference to icon info container
         */
		static CPbkContactEditorPostalCodeField* NewL(
                TPbkContactItemField& aField,
                MPbkContactEditorUiBuilder& aUiBuilder,
				CPbkIconInfoContainer& aIconInfoContainer);
        
        /**
         * Destructor.
         */
        ~CPbkContactEditorPostalCodeField();
		
    public:  // from MPbkContactEditorField
        void SaveFieldL();
        void AddFieldL(CPbkContactItem& aContact);
        void SetControlTextL(const TDesC& aDes);
        void ActivateL();
        void AcceptL(MPbkFieldEditorVisitor& aVisitor);


    protected: // from MPbkFieldEditorControl
        CEikEdwin* Control();
        TPbkFieldId FieldId();
                
    protected:  // Implementation
        CPbkContactEditorPostalCodeField(
				TPbkContactItemField& aField, 
				CPbkIconInfoContainer& aIconInfoContainer,
				MPbkContactEditorUiBuilder& aUiBuilder);
    
    private:    // implementation
        void ConstructL();

    protected:  // Data
        /// Ref: Fields editor
        CEikEdwin* iControl;
        /// Ref: fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
    };

#endif // __CPbkContactEditorPostalCodeField_H__
            
// End of File
