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
* Description:  Phonebook 2 MMC UI extension plug-in.
*
*/


#ifndef CPMUUIEXTENSIONPLUGIN_H
#define CPMUUIEXTENSIONPLUGIN_H

// INCLUDES
#include <e32base.h>
#include <CPbk2UIExtensionPlugin.h>

// FORWARD DECLARATIONS
class CPbkAppViewBase;

// CLASS DECLARATION

/**
 * Phonebook 2 MMC UI extension plug-in.
 * Creates instances of the extension classes.
 */
class CPmuUIExtensionPlugin : public CPbk2UIExtensionPlugin
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPmuUIExtensionPlugin* NewL();

        /**
         * Destructor.
         */
        ~CPmuUIExtensionPlugin();

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

    private: // Implementation
        CPmuUIExtensionPlugin();
    };

#endif // CPMUUIEXTENSIONPLUGIN_H

// End of File
