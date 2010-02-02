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
* Description:  Phonebook 2 FDN contact editor phone number field.
*
*/


#ifndef CPSU2FDNCONTACTEDITORPHONENUMBERFIELD_H
#define CPSU2FDNCONTACTEDITORPHONENUMBERFIELD_H

// INCLUDES
#include <MPbk2ContactEditorField.h>
#include <MPbk2ContactEditorField2.h>

// FORWARD DECLARATIONS
class CEikEdwin;
class CEikCaptionedControl;
class MVPbkStoreContactField;
class MPbk2ContactEditorUiBuilder;
class MPbk2FieldProperty;
class CPbk2IconInfoContainer;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor FDN phone number field.
 */
NONSHARABLE_CLASS(CPsu2FdnContactEditorPhoneNumberField) : 
        public CBase,
        public MPbk2ContactEditorField,
        public MPbk2ContactEditorField2
    {
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         *
         * @param aContactField         The field to create editor field for.
         * @param aFieldProperty        Field property.
         * @param aUiBuilder            Builder for adding the field into
         *                               dialog.
         * @param aIconInfoContainer    Icon container for setting the icon.
         * @return  A new instance of this class.
         */
        static CPsu2FdnContactEditorPhoneNumberField* NewL(
                MVPbkStoreContactField& aContactField,
                const MPbk2FieldProperty& aFieldProperty,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        
        /**
         * Destructor.
         */
        ~CPsu2FdnContactEditorPhoneNumberField();

    public: // From MPbk2ContactEditorField
        TInt ControlId() const;
        CEikEdwin* Control() const;
        void SaveFieldL();
              
        TBool FieldDataChanged() const;
        TPtrC FieldLabel() const;
        void SetFieldLabelL(
                const TDesC& aLabel );
        HBufC* ControlTextL() const;
        void SetFocus();
        void ActivateL();
        MVPbkStoreContactField& ContactField() const;
        const MPbk2FieldProperty&  FieldProperty() const;
        void AcceptL(
                MPbk2ContactEditorFieldVisitor& aVisitor );
        TBool ConsumesKeyEvent(
                const TKeyEvent& aKeyEvent, 
                TEventCode aType );
        TAny* ContactEditorFieldExtension(TUid aExtensionUid );
    
     public: // From MPbk2ContactEditorField2            
        TBool HandleCustomFieldCommandL(TInt aCommand );  
        
    private: // Implementation
        CPsu2FdnContactEditorPhoneNumberField(
                MVPbkStoreContactField& aContactField,
                const MPbk2FieldProperty& aFieldProperty,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                const CPbk2IconInfoContainer& aIconInfoContainer );
        void ConstructL();

    private:  // Data
        /// Ref: Store contact field
        MVPbkStoreContactField& iContactField;
        /// Ref: Field property
        const MPbk2FieldProperty& iFieldProperty;
        /// Ref: UI builder
        MPbk2ContactEditorUiBuilder& iUiBuilder;
        /// Ref: Icon info container
        const CPbk2IconInfoContainer& iIconInfoContainer;
        /// Ref: Field editor
        CEikEdwin* iControl;
        /// Ref: Field captioned control
        CEikCaptionedControl* iCaptionedCtrl;
        /// Own: Indicates has contact data changed
        TBool iContactDataHasChanged;
    };

#endif // CPSU2FDNCONTACTEDITORPHONENUMBERFIELD_H
            
// End of File
