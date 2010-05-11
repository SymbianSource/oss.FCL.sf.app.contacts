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
* Description:  Phonebook 2 application UI.
*
*/


#ifndef CPBK2APPUI_H
#define CPBK2APPUI_H

//  INCLUDES
#include <CPbk2AppUiBase.h>
#include <coemain.h>
#include <MPbk2AppUiExtension.h>
#include <MPbk2KeyEventHandler.h>
#include <RPbk2LocalizedResourceFile.h>

//  FORWARD DECLARATIONS
class CPbk2Document;
class CPbk2Application;
class MPbk2AppUiExtension;
class CPbk2ViewExplorer;
class CPbk2StartupMonitor;
class CPbk2ViewState;
class CPbk2StoreManager;
class CPbk2TabGroupContainer;
class MPbk2ApplicationServices;
class MPbk2ApplicationServices2;

//  CLASS DECLARATION

/**
 * Phonebook2 application UI.
 * An object of this class is created by the Symbian OS
 * framework by a call to CPbk2Document::CreateAppUiL().
 * The application UI object creates and owns the application's
 * views and handles system commands selected from the menu.
 */
class CPbk2AppUi : public CPbk2AppUiBase<CAknViewAppUi>,
                   public MPbk2KeyEventHandler,
                   private MCoeForegroundObserver
    {
    public:  // Constructors and destructor

        /**
         * Constructor.
         */
        CPbk2AppUi();

        /**
         * Destructor.
         */
        ~CPbk2AppUi();

    public: // Interface

        /**
         * Returns the Phonebook 2 document object.
         *
         * @return  Phonebook document.
         */
        CPbk2Document* PhonebookDocument() const;

        /**
         * Returns the concrete store manager.
         *
         * @return  Store manager.
         */
        CPbk2StoreManager& StoreManager() const;

        /**
         * Returns the Phonebook 2 application object.
         *
         * @return  Phonebook application.
         */
        CPbk2Application* Pbk2Application() const;

        /**
         * Exits Phonebook 2 in a correct way.
         */
        void ExitL();

        /**
         * Activates Phonebook view.
         *
         * @param aViewId       Id of the view to activate.
         * @param aViewState    View activation parameters.
         */
        void ActivatePhonebookViewL(
                TUid aViewId,
                const CPbk2ViewState* aViewState );

    public: // From CPbk2AppUiBase
        CPbk2AppViewBase* ActiveView() const;
        MPbk2ViewExplorer* Pbk2ViewExplorer() const;
        MPbk2StartupMonitor* Pbk2StartupMonitor() const;
        CPbk2TabGroupContainer* TabGroups() const;
        MPbk2KeyEventHandler& KeyEventHandler();
        MPbk2ApplicationServices& ApplicationServices() const;
        void HandleCommandL(
                TInt aCommand );
        void ExitApplication();

    private: // From MCoeForegroundObserver
        void HandleGainingForeground();
        void HandleLosingForeground();

    private: // From CAknViewAppUi
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        TKeyResponse HandleKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void HandleResourceChangeL(
                TInt aType );
        void HandleForegroundEventL(
                TBool aForeground );

    private: // From MPbk2KeyEventHandler
        TBool Pbk2ProcessKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // Implementation
        void ConstructL();
        void CmdExit();
        TBool LocalVariationFeatureEnabled(
                TInt aFeatureFlag );
        void LeavePbkInMemExitL();

    private: // Data
        /// Own: Phonebook 2 UI controls dll resource file
        RPbk2LocalizedResourceFile iUiControlsResourceFile;
        /// Own: Phonebook 2 commands dll resource file
        RPbk2LocalizedResourceFile iCommandsResourceFile;
        /// Own: Phonebook 2 common UI dll resource file
        RPbk2LocalizedResourceFile iCommonUiResourceFile;
        /// Own: View explorer
        CPbk2ViewExplorer* iViewExplorer;
        /// Own: Tab group
        CPbk2TabGroupContainer* iTabGroups;
        /// Own: AppUI extension
        MPbk2AppUiExtension* iAppUiExtension;
        /// Own: Start-up monitor
        CPbk2StartupMonitor* iStartupMonitor;
        /// Own: Indicates is this app in foreground
        TBool iIsRunningForeground;
        /// Ref: Application services
        MPbk2ApplicationServices* iAppServices;
        /// Ref: Application services extension
        MPbk2ApplicationServices2* iAppServicesExtension;
    };

#endif  // CPBK2APPUI_H

// End of File
