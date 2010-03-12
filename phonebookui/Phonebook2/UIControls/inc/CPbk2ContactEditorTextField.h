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
* Description:  Phonebook 2 contact editor text field.
*
*/


#ifndef CPBK2CONTACTEDITORTEXTFIELD_H
#define CPBK2CONTACTEDITORTEXTFIELD_H

// INCLUDES
#include "CPbk2ContactEditorFieldBase.h"

// FORWARD DECLARATIONS
class CEikEdwin;
class CEikCaptionedControl;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor text field.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorTextField) :
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
         *                              setting the field icon.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorTextField* NewLC(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        
        /**
         * Destructor.
         */
        virtual ~CPbk2ContactEditorTextField();

    public: // From MPbk2ContactEditorField
        CEikEdwin* Control() const;
        void SaveFieldL();
        void ActivateL();
        void AcceptL(
                MPbk2ContactEditorFieldVisitor& aVisitor );
        
    protected: // Implementation
        CPbk2ContactEditorTextField(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        void ConstructL();

        // Deactivate T9 in editor
        void DeactivateT9(TInt8 aEditMode);

    protected: // Data
        /// Ref: Editor control
        CEikEdwin* iControl;
        /// Ref: Fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
    };

#endif // CPBK2CONTACTEDITORTEXTFIELD_H
            
// End of File
