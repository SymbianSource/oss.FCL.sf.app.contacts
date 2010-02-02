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
* Description:  Phonebook 2 USIM UI extension plug-in.
*
*/


#ifndef CPSU2UIEXTENSIONPLUGIN_H
#define CPSU2UIEXTENSIONPLUGIN_H

// INCLUDES
#include <CPbk2UIExtensionPlugin.h>
#include <Pbk2PluginCommandListerner.h>

// FORWARD DECLARATIONS
class CPsu2ViewManager;
class MVPbkContactStore;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 USIM UI extension plug-in.
 * Creates instances of the extension classes.
 */
class CPsu2UIExtensionPlugin : public CPbk2UIExtensionPlugin,
                               public MPbk2PluginCommandListerner
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPsu2UIExtensionPlugin* NewL();

        /**
         * Destructor.
         */
        ~CPsu2UIExtensionPlugin();

    public: // From CPbk2UIExtensionPlugin
        MPbk2UIExtensionView* CreateExtensionViewL(
                TUid aViewId,
                CPbk2UIExtensionView& aView );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                MPbk2ContactUiControl& aControl );
        void UpdateStorePropertiesL(
                CPbk2StorePropertyArray& aPropertyArray );
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
                MPbk2ContactUiControl& aUiControl) const;
        MPbk2AiwInterestItem* CreatePbk2AiwInterestForIdL(
                TInt aInterestId,
                CAiwServiceHandler& aServiceHandler ) const;
        TBool GetHelpContextL(
                TCoeHelpContext& aContext,
                const CPbk2AppViewBase& aView,
                MPbk2ContactUiControl& aUiControl );
        void ApplyDynamicViewGraphChangesL(
                CPbk2ViewGraph& aViewGraph );
        void ApplyDynamicPluginInformationDataL(
                CPbk2UIExtensionInformation& aUiExtensionInformation );
        TAny* UIExtensionPluginExtension( TUid aExtensionUid );
    public: // From MPbk2PluginCommandListerner
        void HandlePbk2Command( TInt aCommand );

    public: // Implementation
        TBool IsEndKeyPressed();
        void ClearEndKeyPressedFlag();
    private: // Implementation
        CPsu2UIExtensionPlugin();
        void ConstructL();
        MVPbkContactStore* GetTargetForCopyOrNullL(
                MPbk2ContactUiControl& aUiControl ) const;
        TBool OnlyStoreShownL(
                const TDesC& aStoreUri );

    private: // Data
        /// Own: Manages SIM views
        CPsu2ViewManager* iViewManager;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
        /// Own: The flag of the End key (termination call key) pressed or not
        TBool iEndKeyPressed;
    };

#endif // CPSU2UIEXTENSIONPLUGIN_H

// End of File
