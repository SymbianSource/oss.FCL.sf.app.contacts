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
* Description:  Phonebook 2 contact editor date field.
*
*/


#ifndef CPBK2CONTACTEDITORDATEFIELD_H
#define CPBK2CONTACTEDITORDATEFIELD_H

// INCLUDES
#include "CPbk2ContactEditorFieldBase.h"

// FORWARD DECLARATIONS
class CEikDateEditor;
class CEikCaptionedControl;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor date field.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorDateField) : 
        public CPbk2ContactEditorFieldBase
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContactField         Contact field.
         * @param aUiBuilder            UI builder for adding the
         *                              field into dialog.
         * @param aIconInfoContainer    Icon container for
         *                              the setting field icon.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorDateField* NewLC(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        
        /**
         * Destructor.
         */
        virtual ~CPbk2ContactEditorDateField();

    public: // From MPbk2ContactEditorField
        CEikEdwin* Control() const;
        void SaveFieldL();
        void ActivateL();
        void AcceptL(
                MPbk2ContactEditorFieldVisitor& aVisitor );
    
    protected: // Implementation
        CPbk2ContactEditorDateField(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        void ConstructL();

    protected: // Data
        /// Ref: Editor control
        CEikDateEditor* iControl;
        /// Ref: Fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
    };

#endif  // CPBK2CONTACTEDITORDATEFIELD_H
            
// End of File
