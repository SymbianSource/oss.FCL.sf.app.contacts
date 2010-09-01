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
* Description:  Phonebook 2 contact editor synchronisation field.
*
*/


#ifndef CPBK2CONTACTEDITORSYNCFIELD_H
#define CPBK2CONTACTEDITORSYNCFIELD_H

// INCLUDES
#include "CPbk2ContactEditorFieldBase.h"
#include <badesca.h>

// FORWARD DECLARATIONS
class CEikCaptionedControl;
class CAknPopupField;
class CAknQueryValueTextArray;
class CAknQueryValueText;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor synchronisation field.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorSyncField) :
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
        static CPbk2ContactEditorSyncField* NewLC(
            CPbk2PresentationContactField& aContactField,
            MPbk2ContactEditorUiBuilder& aUiBuilder,
            CPbk2IconInfoContainer& aIconInfoContainer );
        
        /**
         * Destructor.
         */
        virtual ~CPbk2ContactEditorSyncField();

    public: // From MPbk2ContactEditorField
        CEikEdwin* Control() const;
        void SaveFieldL();
        TBool FieldDataChanged() const;
		HBufC* ControlTextL() const;
        void ActivateL();
        void AcceptL(
                MPbk2ContactEditorFieldVisitor& aVisitor );
        TBool ConsumesKeyEvent(
                const TKeyEvent& aKeyEvent, 
                TEventCode aType );
        TBool HandleCustomFieldCommandL(TInt aCommand );
        
    protected: // Implementation
        CPbk2ContactEditorSyncField(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        void ConstructL();
        void DoSetFieldValueAsCurrentSelection();

    protected: // Data
        /// Ref: Fields editor
        CAknPopupField* iControl;
        /// Ref: Fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
        /// Own: Text array of sync items 
        CDesCArrayFlat* iSyncArrayText;
        /// Own: Synchronisation query text values
        CAknQueryValueTextArray* iSyncArray;
        /// Own: Synchronisation text values
        CAknQueryValueText* iSyncTextValues;
        /// Own: Indicates is using default sync value
        TBool iIsUsingDefaultSyncValue;
        /// Own: Initial selection index
        TInt iInitialSyncConfSelection;
    };

#endif // CPBK2CONTACTEDITORSYNCFIELD_H
            
// End of File
