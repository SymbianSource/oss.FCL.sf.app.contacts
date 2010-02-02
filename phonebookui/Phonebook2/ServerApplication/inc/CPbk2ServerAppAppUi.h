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
* Description:  Phonebook 2 application server application UI.
*
*/


#ifndef CPBK2SERVERAPPAPPUI_H
#define CPBK2SERVERAPPAPPUI_H

// INCLUDES
#include <CPbk2AppUiBase.h>
#include <MPbk2KeyEventHandler.h>
#include <RPbk2LocalizedResourceFile.h>

// FORWARD DECLARATIONS
class CPbk2ServerAppDocument;
class CPbk2StoreConfiguration;
class CPbk2StoreManager;
class MPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 application server application UI.
 */
class CPbk2ServerAppAppUi : public CPbk2AppUiBase<CAknAppUi>,
                            public MPbk2KeyEventHandler
    {
    public: // Construction and destruction

        /**
         * Constructor.
         */
        CPbk2ServerAppAppUi();

        /**
         * Destructor.
         */
        ~CPbk2ServerAppAppUi();

    public: // Types

        /**
         * Server app status pane layouts.
         */
        enum TPbk2ServerAppStatusPaneLayout
            {
            EStatusPaneLayoutEmpty = 0,
            EStatusPaneLayoutUsual
            };

    public: // Interface

        /**
         * Returns the concrete store manager.
         *
         * @return  Store manager.
         */
        CPbk2StoreManager& StoreManager() const;

        /**
         * Changes the statuspane layout
         *
         * @param   The layout to change.
         */
        void ChangeStatuspaneLayoutL(
                TPbk2ServerAppStatusPaneLayout aLayout );

    public: // From MPbk2AppUi
        CPbk2AppViewBase* ActiveView() const;
        MPbk2ViewExplorer* Pbk2ViewExplorer() const;
        MPbk2StartupMonitor* Pbk2StartupMonitor() const;
        CPbk2TabGroupContainer* TabGroups() const;
        MPbk2KeyEventHandler& KeyEventHandler();
        MPbk2ApplicationServices& ApplicationServices() const;
        void HandleCommandL(
                TInt aCommand );
        void ExitApplication();

    private: // From MPbk2KeyEventHandler
        TBool Pbk2ProcessKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // Implementation
        void ConstructL();
        TBool LocalVariationFeatureEnabled(
                TInt aFeatureFlag );
	
	public:	// CAknAppUi
		void HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination);
		
    private: // Data
        /// Own: Phonebook 2 UI controls resource file
        RPbk2LocalizedResourceFile iUiControlsResourceFile;
        /// Own: Phonebook 2 commands resource file
        RPbk2LocalizedResourceFile iCommandsResourceFile;
        /// Own: Phonebook 2 common UI resource file
        RPbk2LocalizedResourceFile iCommonUiResourceFile;
        /// Ref: Application services
        MPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2SERVERAPPAPPUI_H

// End of File
