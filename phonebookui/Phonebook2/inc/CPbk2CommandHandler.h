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
* Description:  Phonebook 2 command handler.
*
*/


#ifndef CPBK2COMMANDHANDLER_H
#define CPBK2COMMANDHANDLER_H

// INCLUDES
#include <e32base.h>
#include <MPbk2CommandHandler.h>
#include <MPbk2ContactUiControl.h>
#include <Pbk2Commands.hrh>

// FORWARDS
class MPbk2CommandFactory;
class CEikMenuPane;
class CPbk2CommandStore;
class CPbk2AiwInterestArray;
class MPbk2ViewExplorer;
class MPbk2Command;
class CPbk2AppViewBase;
class CPbk2UIExtensionManager;
class CVPbkContactStoreUriArray;
class CPbk2ContactRelocator;
class CPbk2ApplicationServices;
class MVPbkContactLink;

//  CLASS DECLARATION

/**
 * Phonebook 2 command handler.
 * Responsible for handling all Phonebook 2 commands and
 * filtering of Phonebook 2 menus.
 */
class CPbk2CommandHandler : public CBase,
                            public MPbk2CommandHandler
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2CommandHandler* NewL();

        /**
         * Destructor.
         */
        ~CPbk2CommandHandler();

    public: // From MPbk2CommandHandler
        TBool HandleCommandL(
                const TInt aCommandId,
                MPbk2ContactUiControl& aControl,
                const CPbk2AppViewBase* aAppView );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                CPbk2AppViewBase& aViewBase,
                MPbk2ContactUiControl& aControl );
        void RegisterAiwInterestL(
                const TInt aInterestId,
                const TInt aMenuResourceId,
                const TInt aInterestResourceId,
                const TBool aAttachBaseService );
        TInt ServiceCmdByMenuCmd(
                TInt aMenuCmdId ) const;
        void AddAndExecuteCommandL(
                MPbk2Command* aCommand );
        void AddMenuCommandObserver(
                MPbk2MenuCommandObserver& aObserver );
        void RemoveMenuCommandObserver(
                MPbk2MenuCommandObserver& aObserver );
        void DynInitToolbarL(
                TInt aResourceId,
                CAknToolbar* aToolbar,
                const CPbk2AppViewBase& aAppView,
                MPbk2ContactUiControl& aControl );
        void OfferToolbarEventL(
                TInt aCommand,
                MPbk2ContactUiControl& aControl,
                const CPbk2AppViewBase* aAppView );

    private: // Command handlers, use Cmd prefix
         void CmdOpenSettingsViewL(
                MPbk2ContactUiControl& aControl,
                const CPbk2AppViewBase* aAppView,
                TUid aViewId );
        void CmdOpenPreviousViewL(
                MPbk2ContactUiControl& aControl,
                const CPbk2AppViewBase* aAppView );
        void CmdOpenHelpL(
                MPbk2ContactUiControl& aControl,
                const CPbk2AppViewBase* aAppView );
        TBool CmdGenericCommandL(
                TPbk2CommandId aCommandId,
                MPbk2ContactUiControl& aControl );

    private: // Implementation
        CPbk2CommandHandler();
        void ConstructL();
        void PerformStandardMenuFilteringL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                MPbk2ContactUiControl& aControl );
        void PerformStoreSpecificFilteringL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                MPbk2ContactUiControl& aControl );
        void PerformFieldTypeBasedFilteringL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                MPbk2ContactUiControl& aControl );
        TBool AreSelectedContactsFromReadOnlyStoreL(
                MPbk2ContactUiControl& aControl );
        TBool IsFromReadOnlyStore(
                const MVPbkContactLink& aContactLink ) const;
        TBool HasSpeedDialL(
                MPbk2ContactUiControl& aControl );
        TInt ToStoreFieldIndexL(
                TInt aPresIndex,
                const MVPbkStoreContact* aStoreContact );

    private: // Data
        /// Own: Command factory
        MPbk2CommandFactory* iCommandFactory;
        /// Own: Command store
        CPbk2CommandStore* iCommandStore;
        /// Own: AIW interest array
        CPbk2AiwInterestArray* iAiwInterestArray;
        /// Own: Application services
        CPbk2ApplicationServices* iAppServices;
        /// Own: Extension manager
        CPbk2UIExtensionManager* iExtensionManager;
        /// Own: Contact relocator
        CPbk2ContactRelocator* iContactRelocator;
    };

#endif // CPBK2COMMANDHANDLER_H

// End of File
