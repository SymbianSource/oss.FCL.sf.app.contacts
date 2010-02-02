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
* Description:  Phonebook 2 copy contacts starter command.
*
*/


#ifndef CPBK2COPYCONTACTSSTARTERCMD_H
#define CPBK2COPYCONTACTSSTARTERCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MPbk2CommandObserver.h>
#include <MVPbkContactStoreObserver.h>
#include <CPbk2ContactUiControlSubstitute.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class CVPbkContactStoreUriArray;
class MVPbkContactStoreList;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 copy contacts starter command.
 * Responsible for selecting target store and starting the real copy command.
 */
NONSHARABLE_CLASS(CPbk2CopyContactsStarterCmd) : public CActive,
                                                  public MPbk2Command,
                                                  public MPbk2CommandObserver,
                                                  public MVPbkContactStoreObserver
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    UI control.
         * @return  A new instance of this class.
         */
        static CPbk2CopyContactsStarterCmd* NewL(
                MPbk2ContactUiControl& aUiControl );
        
        /**
         * Destructor.
         */
        virtual ~CPbk2CopyContactsStarterCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );

    protected: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // From MPbk2CommandObserver
        void CommandFinished(
                const MPbk2Command& aCommand );

    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

    private: // Implementation
        CPbk2CopyContactsStarterCmd(
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        static TInt IdleQueryCallbackL(
                TAny* aSelf );
        void QueryL();
        void ExecuteCopyCommandL();
        void OpenStoresL();
        void InitCommandL();
        void IssueRequest();
        HBufC* GetQueryTitleLC();
        void HandleError(
                TInt aError );

    private: // Data structures
        /// Copy starter states
        enum TPbk2CopyStarterState
            {
            EPbk2CopyOpenStores,
            EPbk2CopyStarterInit,
            EPbk2CopyStarterQuery,
            EPbk2CopyStarterExecuteCopy,
            EPbk2CopyStarterComplete
            };

        /// Number of contacts
        enum TNumOfContacts
            {
            EPbk2NoContacts         = 0,
            EPbk2OneContact         = 1
            };

    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: The observer for completion of the command
        MPbk2CommandObserver* iCommandObserver;
        /// Own: An array of selected contacts
        MVPbkContactLinkArray* iSelectedContacts;
        /// Own: The actual command that does the job
        MPbk2Command* iExecutiveCommand;
        /// Own: Store control
        CPbk2ContactUiControlSubstitute* iStoreControl;
        /// Own: The URIs that are shown in the query
        CVPbkContactStoreUriArray* iTargetStoreUris;
        /// Own: The URIs of stores that failed to open
        CVPbkContactStoreUriArray* iFailedStoreUris;
        /// Own: Command state
        TInt iState;
        /// Ref: The global contact store list
        MVPbkContactStoreList* iStoreList;
        /// Own: Number of responded stores
        TInt iRespondedStores;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
        /// Own: Idle query launcher
        CIdle* iIdleQuery;
    };

#endif // CPBK2COPYCONTACTSSTARTERCMD_H
            
// End of File
