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
*     Phonebook contact editor syncronization field.
*
*/


#ifndef __CPbkContactEditorSyncField_H__
#define __CPbkContactEditorSyncField_H__

//  INCLUDES
#include "CPbkContactEditorFieldBase.h"


// FORWARD DECLARATIONS
class CEikEdwin;
class MPbkContactEditorUiBuilder;
class CPbkContactItem;
class CEikCaptionedControl;
class CAknPopupField;
class CAknQueryValueTextArray;
class CAknQueryValueText;


// CLASS DECLARATION

/**
 * Phonebook Contact editor syncronization field. 
 */
NONSHARABLE_CLASS(CPbkContactEditorSyncField) : 
        public CPbkContactEditorFieldBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class - text editor to a field.
		 * @param aField the to be edited field
		 * @param aUiBuilder reference to UI builder
		 * @param aIconInfoContainer reference to icon info container
         */
		static CPbkContactEditorSyncField* NewL(
                TPbkContactItemField& aField,
                MPbkContactEditorUiBuilder& aUiBuilder,
				CPbkIconInfoContainer& aIconInfoContainer);
        
        /**
         * Destructor.
         */
        ~CPbkContactEditorSyncField();
		
    public:  // from MPbkContactEditorField
        void SaveFieldL();
        TBool FieldDataChanged() const;
        void AddFieldL(CPbkContactItem& aContact);
        void ActivateL();
        void AcceptL(MPbkFieldEditorVisitor& aVisitor);
        TBool ConsumesKeyEvent(const TKeyEvent& aKeyEvent, 
                TEventCode aType);

    protected: // from MPbkFieldEditorControl
        CEikEdwin* Control();
        TPbkFieldId FieldId();
                
    protected:  // Implementation
        CPbkContactEditorSyncField(
				TPbkContactItemField& aField, 
				CPbkIconInfoContainer& aIconInfoContainer,
				MPbkContactEditorUiBuilder& aUiBuilder);
    
    private:    // implementation
        void ConstructL();
        void DoSetFieldValueAsCurrentSelection();

    protected:  // Data
        /// Ref: Fields editor
        CAknPopupField* iControl;
        /// Ref: fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
        ///Own: text array of sync items 
        CDesCArrayFlat* iSyncArrayText;
        ///Own: Syncronization query text values
        CAknQueryValueTextArray* iSyncArray;
        ///Own: Syncronization text values
        CAknQueryValueText* iSyncTextValues;
        ///Own: Is using default sync value
        TBool iIsUsingDefaultSyncValue;
        ///Own: Initial selection index
        TInt iInitialSyncConfSelection;
         
    };

#endif // __CPbkContactEditorSyncField_H__
            
// End of File
