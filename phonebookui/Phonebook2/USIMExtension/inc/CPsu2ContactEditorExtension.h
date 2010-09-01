/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 USIM UI contact editor extension.
*
*/


#ifndef CPSU2CONTACTEDITOREXTENSION_H
#define CPSU2CONTACTEDITOREXTENSION_H

// INCLUDE FILES
#include <e32base.h>
#include <MPbk2ContactEditorExtension.h>
#include <Pbk2FieldProperty.hrh>

// FORWARD DECLARATIONS
class CEikMenuPane;
class CVPbkContactManager;
class MPbk2ContactEditorControl;

// CLASS DECLARATION
class CPsu2ContactEditorExtension : public CBase,
								    public MPbk2ContactEditorExtension
    {
    public:
        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @param aContact          The contact being edited.
         * @param aEditorControl    Contact editor control.
         * @return  A new instance of this class.
         */
        static CPsu2ContactEditorExtension* NewL(
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                MPbk2ContactEditorControl& aEditorControl );

        /**
         * Destructor.
         */
        ~CPsu2ContactEditorExtension();

    public: // From MPbk2ContactEditorExtension
        MPbk2ContactEditorField* CreateEditorFieldL(
                MVPbkStoreContactField& aField,
                const MPbk2FieldProperty& aFieldProperty,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        TBool ProcessCommandL(
                TInt aCommandId );
        TBool OkToDeleteContactL(
                MPbk2ContactEditorEventObserver::TParams& aParams );
        TBool OkToSaveContactL(
                MPbk2ContactEditorEventObserver::TParams& aParams );
        void ModifyButtonGroupContainerL(
        		CEikButtonGroupContainer& aButtonGroupContainer );
        void DoRelease();
        void ContactEditorOperationCompleted(
                MVPbkContactObserver::TContactOpResult aResult,
                TParams aParams );
        void ContactEditorOperationFailed(
                MVPbkContactObserver::TContactOp aOpCode,
                TInt aErrorCode,
                TParams aParams,
                TFailParams& aFailParams );

    private: // Implementation
        CPsu2ContactEditorExtension(
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                MPbk2ContactEditorControl& aEditorControl );
        void ConstructL();
        TInt ContactOrigin() const;
        void ShowInformationNote(
                TInt aResourceId,
                TBool aShowStoreName ) const;
        void ShowInformationNoteL(
                TInt aResourceId,
                TBool aShowStoreName ) const;
        void FocusControlOfType(
                TPbk2FieldCtrlType aType );
        void AddLastNameFieldL();
        void HandleTextTooLongL(
                TFailParams& aFailParams ) const;
        void HandleContactEditorOperationFailedL(
                TInt aErrorCode,
                TFailParams& aFailParams );

    private: // Data types
        enum TPsu2ContactOrigin
            {
            /// Contact is not a (U)SIM contact
            EPsu2NotSimContact,
            /// Contact is a ADN contact
            EPsu2AdnContact,
            /// Contact is a SDN contact
            EPsu2SdnContact,
            /// Contact is a FDN contact
            EPsu2FdnContact
            };

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: The edited contact
        MVPbkStoreContact& iContact;
        /// Ref: Contact editor control
        MPbk2ContactEditorControl& iEditorControl;
        /// Own: Indicates the contact origin
        TInt iContactOrigin;
        /// Own: 'end-key' was pressed
        TBool iEndKeyWasPressed;
	};

#endif // CPSU2CONTACTEDITOREXTENSION_H

// End of File
