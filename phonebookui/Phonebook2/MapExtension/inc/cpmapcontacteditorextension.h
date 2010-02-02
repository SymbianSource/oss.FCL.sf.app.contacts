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
* Description:  Phonebook 2 contact editor dialog extension.
*
*/

#ifndef CPMAPCONTACTEDITOREXTENSION_H
#define CPMAPCONTACTEDITOREXTENSION_H

// INCLUDE FILES
#include <e32base.h>
#include <MPbk2ContactEditorExtension.h>
#include <MPbk2ContactEditorExtensionExtension.h>
#include <Pbk2FieldProperty.hrh>

// FORWARD DECLARATIONS
class CEikMenuPane;
class CVPbkContactManager;
class MPbk2ContactEditorControl;
class MPbk2ContactEditorUIField;
class MPbk2UIField;

// CLASS DECLARATION
class CPmapContactEditorExtension : public CBase,
								    public MPbk2ContactEditorExtension,
								    public MPbk2ContactEditorExtensionExtension
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
        static CPmapContactEditorExtension* NewL(
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                MPbk2ContactEditorControl& aEditorControl );

        /**
         * Destructor.
         */
        ~CPmapContactEditorExtension();

    public: // From MPbk2ContactEditorExtension
        MPbk2ContactEditorField* CreateEditorFieldL(
                MVPbkStoreContactField& aField,
                const MPbk2FieldProperty& aFieldProperty,
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                CPbk2IconInfoContainer& aIconInfoContainer );
        
        MPbk2ContactEditorUIField* CreateFieldLC( 
                		MPbk2UIField& aField,
                		TInt aCustomPosition, 
                        MPbk2ContactEditorUiBuilder& aUiBuilder,
                        const TDesC& aCustomText, CPbk2IconInfoContainer& aIconInfoContainer );
        
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
        TAny* ContactEditorExtensionExtension(
                        TUid /*aExtensionUid*/ );

    private: // Implementation
        CPmapContactEditorExtension(
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                MPbk2ContactEditorControl& aEditorControl );
        void ConstructL();

    private: // Data types

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: The edited contact
        MVPbkStoreContact& iContact;
        /// Ref: Contact editor control
        MPbk2ContactEditorControl& iEditorControl;

	};

#endif // CPMAPCONTACTEDITOREXTENSION_H

// End of File
