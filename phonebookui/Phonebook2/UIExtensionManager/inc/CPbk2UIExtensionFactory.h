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
* Description:  Phonebook 2 UI extension factory.
*
*/


#ifndef CPBK2UIEXTENSIONFACTORY_H
#define CPBK2UIEXTENSIONFACTORY_H

// INCLUDE FILES
#include <e32base.h>
#include <MPbk2UIExtensionFactory.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionLoader;
class CPbk2UIExtensionManager;
class CPbk2ThinUIExtensionLoader;

// CLASS DECLARATION

/**
 * Phonebook 2 UI extension factory.
 * Responsible for creating extension objects provided by extension plugins.
 */
NONSHARABLE_CLASS(CPbk2UIExtensionFactory) : public CBase,
                                             public MPbk2UIExtensionFactory
    {
    public: // Construction and destruction

        /**
         * Returns a new instance of this class.
         *
         * @param aExtensionLoader      Extension loader.
         * @param aThinExtensionLoader  Thin extension loader.
         * @param aExtensionManager     Extension manager.
         * @return  A new instance of this class.
         */
        static CPbk2UIExtensionFactory* NewL(
                CPbk2UIExtensionLoader& aExtensionLoader,
                CPbk2ThinUIExtensionLoader& aThinExtensionLoader,
                CPbk2UIExtensionManager& aExtensionManager );

        /**
         * Destructor.
         */
        ~CPbk2UIExtensionFactory();

    public: // From MPbk2UIExtensionFactory
        MPbk2ContactEditorExtension* CreatePbk2ContactEditorExtensionL(
                CVPbkContactManager& aContactManager,
                MVPbkStoreContact& aContact,
                MPbk2ContactEditorControl& aEditorControl );
        MPbk2ContactUiControlExtension* CreatePbk2UiControlExtensionL(
                CVPbkContactManager& aContactManager );
        MPbk2SettingsViewExtension* CreatePbk2SettingsViewExtensionL(
                CVPbkContactManager& aContactManager );
        MPbk2AppUiExtension* CreatePbk2AppUiExtensionL(
                CVPbkContactManager& aContactManager );
        MPbk2Command* CreatePbk2CommandForIdL(
                TInt aCommandId,
                MPbk2ContactUiControl& aUiControl ) const;
        MPbk2AiwInterestItem* CreatePbk2AiwInterestForIdL(
                TInt aInterestId,
                CAiwServiceHandler& aServiceHandler ) const;
        CAknView* CreatePhonebook2ViewL(
                TUid aId );

    private: // Implementation
        CPbk2UIExtensionFactory(
                CPbk2UIExtensionLoader& aExtensionLoader,
                CPbk2ThinUIExtensionLoader& aThinExtensionLoader,
                CPbk2UIExtensionManager& aExtensionManager );

    private: // Data
        /// Ref: UI extension loader
        CPbk2UIExtensionLoader& iExtensionLoader;
        /// Ref: Thin UI extension loader
        CPbk2ThinUIExtensionLoader& iThinExtensionLoader;
        /// Ref: UI extension manager
        CPbk2UIExtensionManager& iExtensionManager;
        /// Own: An array for plugin UIDs that overwrite Phonebook 2 commands
        mutable RArray<TUid> iPluginUidArray;
    };

#endif // CPBK2UIEXTENSIONFACTORY_H

// End of File
