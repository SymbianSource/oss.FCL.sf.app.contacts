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
*     Phonebook contact editor dialog's date field.
*
*/


#ifndef __CPbkContactEditorDateField_H__
#define __CPbkContactEditorDateField_H__

//  INCLUDES
#include "CPbkContactEditorFieldBase.h"
#include <eikdialg.h>

// FORWARD DECLARATIONS
class CEikDateEditor;
class MPbkContactEditorUiBuilder;
class CEikCaptionedControl;


// CLASS DECLARATION

/**
 * Phonebook Contact editor date field. 
 */
NONSHARABLE_CLASS(CPbkContactEditorDateField) : 
        public CPbkContactEditorFieldBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aField contact item field associated with this editor field
		 * @param aUiBuilder UI builder object
		 * @param aIconInfoContainer icon info container
         */
		static CPbkContactEditorDateField* NewL(
                TPbkContactItemField& aField,
                MPbkContactEditorUiBuilder& aUiBuilder,
				CPbkIconInfoContainer& aIconInfoContainer);
        
        /**
         * Destructor.
         */
        ~CPbkContactEditorDateField();
		
    public:  // from MPbkContactEditorField
        void SaveFieldL();
        void AddFieldL(CPbkContactItem& aContact);
		HBufC* ControlTextL() const;
        void ActivateL();
        void AcceptL(MPbkFieldEditorVisitor& aVisitor);
                
    private:  // Implementation
        CPbkContactEditorDateField(
				TPbkContactItemField& aField, 
				CPbkIconInfoContainer& aIconInfoContainer,
				MPbkContactEditorUiBuilder& aUiBuilder);
        void ConstructL();

    private:  // Data
        /// Ref: fields editor control
        CEikDateEditor* iControl;
        /// Ref: fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
    };

#endif // __CPbkContactEditorDateField_H__
            
// End of File
