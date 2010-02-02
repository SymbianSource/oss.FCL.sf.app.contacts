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
* Description:  A command for launching phonebook info dialog
*
*/



#ifndef CPBK2INFODIALOGCMD_H
#define CPBK2INFODIALOGCMD_H

//  INCLUDES
#include <MPbk2Command.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class CVPbkContactStoreUriArray;
class MPbk2CommandObserver;

// CLASS DECLARATION

/**
 *  A command for launching phonebook info dialog
 */
NONSHARABLE_CLASS(CPbk2InfoDialogCmd) : 
        public CActive,
        public MPbk2Command
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         * @param aUiControl ui control
         * @return a new instance of this class
         */
        static CPbk2InfoDialogCmd* NewL(
            MPbk2ContactUiControl& aUiControl);
        
        /**
         * Destructor.
         */
        virtual ~CPbk2InfoDialogCmd();

    public: // From MPbk2Command.
        void ExecuteLD();
        void AddObserver(MPbk2CommandObserver& aObserver);
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);
        
    private: // implementation
        CPbk2InfoDialogCmd(MPbk2ContactUiControl& aUiControl);
        void ConstructL();
        void IssueRequest();
        void QueryL();
        void LaunchDialogL();
        void HandleError(TInt aError);
                    
    private:    // Data
        /// Own: Currently valid stores
        CVPbkContactStoreUriArray* iValidStores;
        /// Ref: Observer for the command
        MPbk2CommandObserver* iCommandObserver;
        /// Own: State of the active object
        TInt iState;
    };

#endif      // CPBK2INFODIALOGCMD_H
            
// End of File
