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
* Description:  Phonebook 2 Group UI extension plug-in.
*
*/


#ifndef CPGUUIEXTENSIONPLUGIN_H
#define CPGUUIEXTENSIONPLUGIN_H

// INCLUDES
#include <e32base.h>
#include <CPbk2UIExtensionPlugin.h>
#include <Pbk2PluginCommandListerner.h>

// CLASS DECLARATION

/**
 * Phonebook 2 Group UI extension plug-in.
 * Creates instances of the extension classes.
 */
class CPguUIExtensionPlugin : public CPbk2UIExtensionPlugin,
							                public MPbk2PluginCommandListerner
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPguUIExtensionPlugin* NewL();

        /**
         * Destructor.
         */
        ~CPguUIExtensionPlugin();

    public: // From MPbk2CommandListerner
        void HandlePbk2Command( TInt aCommand );
    
    private: // From CPbk2UIExtensionPlugin
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
                MPbk2ContactUiControl& aUiControl ) const;
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
        
    public :
        TBool IsEndCallKeyPressed();
        void ClearEndCallKeyPressedFlag();
		
    private: // Implementation
        CPguUIExtensionPlugin();
        void CmdOpenGroupL(
                MPbk2ContactUiControl& aUiControl ) const;

    private: // Data
        /// Own: The flag indicates if the end call key pressed
        TBool iEndCallKeyPressed;  
    };

#endif // CPGUUIEXTENSIONPLUGIN_H

// End of File
