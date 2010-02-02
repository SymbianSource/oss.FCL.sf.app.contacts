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
*     Phonebook contact editor URL field.
*
*/


#ifndef __CPbkContactEditorUrlField_H__
#define __CPbkContactEditorUrlField_H__

//  INCLUDES
#include "CPbkContactEditorTextField.h"


// FORWARD DECLARATIONS
class CEikEdwin;
class MPbkContactEditorUiBuilder;
class CPbkContactItem;
class CEikCaptionedControl;


// CLASS DECLARATION

/**
 * Phonebook Contact editor URL field. 
 */
NONSHARABLE_CLASS(CPbkContactEditorUrlField) : 
        public CPbkContactEditorTextField
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class - URL editor to a field.
		 * @param aField the to be edited field
		 * @param aUiBuilder reference to UI builder
		 * @param aIconInfoContainer reference to icon info container
         */
		static CPbkContactEditorUrlField* NewL(
                TPbkContactItemField& aField,
                MPbkContactEditorUiBuilder& aUiBuilder,
				CPbkIconInfoContainer& aIconInfoContainer);
        
        /**
         * Destructor.
         */
        ~CPbkContactEditorUrlField();

    public:  // from MPbkContactEditorField
        void SaveFieldL();

    private:  // Implementation
        CPbkContactEditorUrlField(
				TPbkContactItemField& aField, 
				CPbkIconInfoContainer& aIconInfoContainer,
				MPbkContactEditorUiBuilder& aUiBuilder);
        void ConstructL();

    };

#endif // __CPbkContactEditorUrlField_H__
            
// End of File
