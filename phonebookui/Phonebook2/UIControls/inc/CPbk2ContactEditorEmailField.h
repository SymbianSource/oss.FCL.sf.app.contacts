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
* Description:  Phonebook 2 contact editor email field.
*
*/


#ifndef CPBK2CONTACTEDITOREMAILFIELD_H
#define CPBK2CONTACTEDITOREMAILFIELD_H

//  INCLUDES
#include "CPbk2ContactEditorTextField.h"

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor email field.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorEmailField) : 
        public CPbk2ContactEditorTextField
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContactField         Contactg field.
         * @param aUiBuilder            UI builder for adding the
         *                              field into dialog.
         * @param aIconInfoContainer    Icon container for
         *                              setting the field icon.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorEmailField* NewLC(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        
        /**
         * Destructor.
         */
        virtual ~CPbk2ContactEditorEmailField();
            
    private: // Implementation
        CPbk2ContactEditorEmailField(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        void ConstructL();
    };

#endif // CPBK2CONTACTEDITOREMAILFIELD_H
            
// End of File
