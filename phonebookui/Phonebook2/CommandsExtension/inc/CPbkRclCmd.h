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



#ifndef CPbkRclCmd_H
#define CPbkRclCmd_H

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
NONSHARABLE_CLASS(CPbkRclCmd) : 
        public CActive,
        public MPbk2Command
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         * @param aUiControl ui control
         * @return a new instance of this class
         */
        static CPbkRclCmd* NewL(
            MPbk2ContactUiControl& aUiControl);
        
        /**
         * Destructor.
         */
        virtual ~CPbkRclCmd();

    public: // From MPbk2Command.
        void ExecuteLD();
        void AddObserver(MPbk2CommandObserver& aObserver);
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);
        
    private: // implementation
        CPbkRclCmd(MPbk2ContactUiControl& aUiControl);
        void ConstructL();
        void IssueRequest();
        void LaunchDialogL();
        void HandleError(TInt aError);
                    
    private:    // Data
        /// Ref: Observer for the command
        MPbk2CommandObserver* iCommandObserver;
        /// Own: State of the active object
        TInt iState;
        /// Ref: ui control - used to fetch the findtext
        MPbk2ContactUiControl& iUiControl;
    };

#endif      // CPbkRclCmd_H
            
// End of File
