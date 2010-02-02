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
* Description:  Phonebook 2 contact editor composite UI extension.
*
*/


#ifndef CPBK2CONTACTEDITOREXTENSION_H
#define CPBK2CONTACTEDITOREXTENSION_H

// INCLUDE FILES
#include <e32base.h>
#include <MPbk2ContactEditorExtension.h>

// FORWARD DECLARATIONS
class CEikMenuPane;
class CPbk2UIExtensionLoader;
class CVPbkContactManager;
class MVPbkStoreContact;
class MPbk2ContactEditorControl;

// CLASS DECLARATION
NONSHARABLE_CLASS(CPbk2ContactEditorExtension) : 
        public CBase,
		public MPbk2ContactEditorExtension
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aExtensionLoader  UI extension loader.
         * @param aContactManager   Virtual Phonebook contact manager.
         * @param aContact          The contact being edited.
         * @param aEditorControl    Contact editor control.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEditorExtension* NewL(
                CPbk2UIExtensionLoader& aExtensionLoader,
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                MPbk2ContactEditorControl& aEditorControl );

        /**
         * Destructor.
         */
        ~CPbk2ContactEditorExtension();

    public: // From MPbk2ContactEditorExtension
        void DoRelease();
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
        CPbk2ContactEditorExtension(
                CPbk2UIExtensionLoader& aExtensionLoader );
        void ConstructL(
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                MPbk2ContactEditorControl& aEditorControl );
        void LoadEditorExtensionsL(
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                MPbk2ContactEditorControl& aEditorControl );

    private: // Data
        /// Ref: Extension loader
        CPbk2UIExtensionLoader& iExtensionLoader;
        /// Own: Array of contact editor extensions
        CArrayPtrFlat<MPbk2ContactEditorExtension> iContactEditors;
	};

#endif // CPBK2CONTACTEDITOREXTENSION_H

// End of File
