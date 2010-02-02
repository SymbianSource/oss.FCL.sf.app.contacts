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
* Description:  Phonebook 2 contact editor impp field.
*
*/


#ifndef CPBK2CONTACTEDITORIMPPFIELD_H
#define CPBK2CONTACTEDITORIMPPFIELD_H

// INCLUDES
#include "CPbk2ContactEditorTextField.h"

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor impp field.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorimppField) :
        public CPbk2ContactEditorTextField
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContactField         Contact field.
         * @param aUiBuilder            UI builder for adding the
         *                              field into dialog.
         * @param aIconInfoContainer    Icon container for
         *                              setting the icon.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorimppField* NewLC(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        
        /**
         * Destructor.
         */
        virtual ~CPbk2ContactEditorimppField();
            
    private: // Implementation
        CPbk2ContactEditorimppField(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        void ConstructL();
    };

/**
 * Phonebook 2 contact editor IMPP field.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorImppField) :
        public CPbk2ContactEditorTextField
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContactField         Contact field.
         * @param aUiBuilder            UI builder for adding the
         *                              field into dialog.
         * @param aIconInfoContainer    Icon container for
         *                              setting the icon.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorImppField* NewLC(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        
        /**
         * Destructor.
         */
        virtual ~CPbk2ContactEditorImppField();
            
    public: // From MPbk2ContactEditorField
        void SaveFieldL();

    private: // Implementation
        CPbk2ContactEditorImppField(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        void ConstructL();
    };

#endif // CPBK2CONTACTEDITORIMPPFIELD_H
            
// End of File
