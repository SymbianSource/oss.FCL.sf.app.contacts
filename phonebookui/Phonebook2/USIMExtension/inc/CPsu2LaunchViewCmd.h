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
* Description:  Phonebook 2 USIM UI Extension launch view command.
*
*/


#ifndef CPSU2LAUNCHVIEWCMD_H
#define CPSU2LAUNCHVIEWCMD_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkContactViewObserver.h>
#include "Pbk2USimUI.hrh"
#include <MPbk2ProcessDecorator.h>
#include "MPsu2ViewLaunchCallback.h"

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CPsu2ViewManager;
class MVPbkContactViewBase;

// CLASS DECLARATION

/**
 * Phonebook 2 USIM UI Extension launch view command.
 */
class CPsu2LaunchViewCmd : public CActive,
                           public MPbk2Command,
                           private MVPbkContactViewObserver,
                           private MPbk2ProcessDecoratorObserver,
                           private MPsu2ViewLaunchCallback
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aViewManager  View manager.
         * @param aViewId       Id of the view to launch.
         * @return  A new instance of this class.
         */
        static CPsu2LaunchViewCmd* NewL(
                CPsu2ViewManager& aViewManager,
                TPsu2ViewId aViewId );

        /**
         * Destructor.
         */
        virtual ~CPsu2LaunchViewCmd();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );

    public: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed(
                TInt aCancelCode );

    private: // From MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        void ContactAddedToView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactViewError(
                MVPbkContactViewBase& aView,
                TInt aError,
                TBool aErrorNotified );

    private: // From MPsu2ViewLaunchCallback
        void SetContactViewL(
                MVPbkContactViewBase& aContactView );
        void HandleError(
                TInt aError );

    private: // Implementation
        CPsu2LaunchViewCmd(
                CPsu2ViewManager& aViewManager,
                TPsu2ViewId aViewId );
        void ConstructL();
        void IssueRequest();
        void OpenStoreAndViewL();

    private: // Data
        /// Ref: View manager
        CPsu2ViewManager& iViewManager;
        /// Own: Id of the view to launch
        TPsu2ViewId iViewId;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Ref: Contact view, owned until given to view manager
        MVPbkContactViewBase* iVPbkView;
        /// Own: Command flags
        TInt iCommandFlags;
        /// Own: Decorator for the wait note process
        MPbk2ProcessDecorator* iDecorator;
        /// Own: Contact view ready indicator
        TBool iContactViewReady;
    };

#endif // CPSU2LAUNCHVIEWCMD_H

// End of File
