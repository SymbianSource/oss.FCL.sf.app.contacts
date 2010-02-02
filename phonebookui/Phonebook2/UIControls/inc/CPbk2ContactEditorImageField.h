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
* Description:  Phonebook 2 contact editor image field.
*
*/


#ifndef CPBK2CONTACTEDITORIMAGEFIELD_H
#define CPBK2CONTACTEDITORIMAGEFIELD_H

// INCLUDES
#include "CPbk2ContactEditorFieldBase.h"
#include "CPbk2ContactEditorReadonlyField.h"

// FORWARD DECLARATIONS
class CEikEdwin;
class CEikCaptionedControl;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor ringtone field.
 */
NONSHARABLE_CLASS(CPbk2ContactEditorImageField) :
        public CPbk2ContactEditorFieldBase
    {
    public:
        enum TTextState
            {
            ENoData,  //AddImage
            EFilename,            
            EThumbnail
            };
        
        enum TClipFileExtension
            {
            EClipFileExtension,
            EDontClipFileExtension
            };
    
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContactField         Contact field.
         * @param aUiBuilder            UI builder for adding the
         *                              field into dialog.
         * @param aIconInfoContainer    Icon container for
         *                              setting the field icon.
         * @param aCustomPosition       Position where this custom controls should be placed.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorImageField* NewLC(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer,
                TInt aCustomPosition);
        
        /**
         * Destructor.
         */
        virtual ~CPbk2ContactEditorImageField();

        /**
         * Returns control id of this custom line.
         *
         * @return  Control id.
         */
        TInt ControlId() const; 

    public:
        void SetTextL();
        TInt TextState();        
        
    private: // From MPbk2ContactEditorField        
        CEikEdwin* Control() const;
        void SaveFieldL();
        void ActivateL();
        void AcceptL(
                MPbk2ContactEditorFieldVisitor& aVisitor );
        TBool ConsumesKeyEvent
        	( const TKeyEvent& aKeyEvent, TEventCode aType );
        TAny* ContactEditorFieldExtension(
                TUid aExtensionUid );
        HBufC* ControlTextL() const;      
        TBool HandleCustomFieldCommandL(TInt aCommand );
        
    private: // Implementation
        CPbk2ContactEditorImageField(
                CPbk2PresentationContactField& aContactField,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        void ConstructL(TInt aCustomPosition);
        void FormatTextL(TPtrC aDataPtr, TClipFileExtension aClipFileExtension);       
        
    private: // Data
        /// Ref: Editor control
        CPbk2ContactEditorReadonlyField* iControl;
        /// Ref: Fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
        /// Own: Text content when editor was opened
        HBufC* iInitialText;
        TInt iTextState;
    };

#endif // CPBK2CONTACTEDITORIMAGEFIELD_H
            
// End of File
