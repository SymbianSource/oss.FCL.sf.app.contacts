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
*     Phonebook contact editor number field.
*
*/


#ifndef __CPbkContactEditorNumberField_H__
#define __CPbkContactEditorNumberField_H__

//  INCLUDES
#include "CPbkContactEditorFieldBase.h"

// FORWARD DECLARATIONS
class CEikEdwin;
class MPbkContactEditorUiBuilder;
class CPbkContactItem;
class CEikCaptionedControl;


// CLASS DECLARATION

/**
 * Phonebook Contact editor number field. 
 */
NONSHARABLE_CLASS(CPbkContactEditorNumberField) : 
        public CPbkContactEditorFieldBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class - number editor to a field.
		 * @param aField the to be edited field
		 * @param aUiBuilder reference to UI builder
		 * @param aIconInfoContainer reference to icon info container
         */
		static CPbkContactEditorNumberField* NewL(
                TPbkContactItemField& aField,
                MPbkContactEditorUiBuilder& aUiBuilder,
				CPbkIconInfoContainer& aIconInfoContainer);
        
        /**
         * Destructor.
         */
        ~CPbkContactEditorNumberField();
		
    public:  // from MPbkContactEditorField
        void SaveFieldL();
        void AddFieldL(CPbkContactItem& aContact);
        void SetControlTextL(const TDesC& aDes);
        void ActivateL();
        void AcceptL(MPbkFieldEditorVisitor& aVisitor);
                
    protected:  // Implementation
        CPbkContactEditorNumberField(
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

#endif // __CPbkContactEditorNumberField_H__
            
// End of File
