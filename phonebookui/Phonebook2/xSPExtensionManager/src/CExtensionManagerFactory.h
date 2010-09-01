/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       ExtensionManager factory.
*
*/


#ifndef __CEXTENSIONMANAGERFACTORY_H__
#define __CEXTENSIONMANAGERFACTORY_H__

// INCLUDES
#include <e32base.h>
#include <CPbk2UIExtensionPlugin.h>

#include "CxSPScanner.h"
#include "CxSPViewActivator.h"

class MxSPFactory;
class CxSPViewIdChanger;
class CxSPContactManager;
class CxSPViewManager;
class CxSPAppUiManager;

// CLASS DECLARATION
/**
 * Implementation of Extension Manager UI extension plug-in.
 * Responsible for:
 * - implementing UI extension plug-in interface
 */
class CExtensionManagerFactory : public CPbk2UIExtensionPlugin
	{
	public: // Construction and destruction
        /**
         * Static factory function that performs the 2-phased construction.
         *
         * @return Newly created CExtensionManagerFactory.
         */
		static CExtensionManagerFactory* NewL();

        /**
         * Standard C++ destructor.
         */
        ~CExtensionManagerFactory();

	public: // Implementation
        /**
         * Appends an extension factory to this multifactory.
         * Ownership is not transferred.
         *
         * @param aFactory Factory to append.
         */
        void AppendL(MxSPFactory* aFactory);


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
                CPbk2ViewGraph& aViewGraph);
		void ApplyDynamicPluginInformationDataL(
                CPbk2UIExtensionInformation& iUiExtensionInformation );

    private: // Implementation
        /**
         * Standard C++ constructor.
         */
		CExtensionManagerFactory();

        /**
         * Performs the 2nd phase of the construction.
         */
		void ConstructL();

		/**
		 * Scans for xSP ECOM plugin extensions
		 */
		void ScanExtensionsL();

		/**
		 * Creates xSPs' view extension objects
		 */
		void CreatePbkViewExtensionL();

    private: // Data

         // Extension factories.
        CArrayPtrFlat<MxSPFactory> iFactories;

        // Own: scanner
        CxSPScanner* iScanner;

        /// Array of extensions
        CxSPArray iExtensions;

        /// Own: view id changer
        CxSPViewIdChanger* iViewIdChanger;

        /// Own: contact manager
        CxSPContactManager* iContactManager;

        /// Flag to check whether sort view exists
        TBool iSortView;

        /// Own: view manager
        CxSPViewManager* iViewManager;

        /// Not owned: app ui manager
        CxSPAppUiManager* iAppUiManager;

        // Own: View activator
        CxSPViewActivator* iViewActivator;
	};

#endif // __CEXTENSIONMANAGERFACTORY_H__

// End of File
