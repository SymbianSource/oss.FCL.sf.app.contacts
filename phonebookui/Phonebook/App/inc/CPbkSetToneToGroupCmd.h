/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Assign ringing tone to group members command.
*
*/


#ifndef __CPbkSetToneToGroupCmd_H__
#define __CPbkSetToneToGroupCmd_H__

// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <AknProgressDialog.h>
#include <MPbkCommand.h>
#include <MPbkBackgroundProcess.h>


// FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkSetToneToGroup;

// CLASS DECLARATION

/**
 * Assign ringing tone to group members command.
 */
class CPbkSetToneToGroupCmd :
        public CActive,
        public MPbkCommand,
        private MPbkProcessObserver,
        private MProgressDialogCallback
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine reference to a contact engine
         * @param aGroupId the id of the group to operate with
		 * @param aRingingToneName ringing tone file name
         * @return a new delete contacts command object
         */
        static CPbkSetToneToGroupCmd* NewL
            (CPbkContactEngine& aEngine,
            TContactItemId aGroupId,
            const TDesC& aRingingToneName);

        /**
         * Sets aSelfPtr to NULL when this object is destroyed.
         * @precond !aSelfPtr || *aSelfPtr==this
         */
        void ResetWhenDestroyed(CPbkSetToneToGroupCmd** aSelfPtr);

        /**
         * Destructor.
         */
        ~CPbkSetToneToGroupCmd();

    public:  // from MPbkCommand
        void ExecuteLD();
        void AddObserver(MPbkCommandObserver& aObserver);

    private: // from CActive
    	void DoCancel();
	    void RunL();
	    TInt RunError(TInt aError);
        
    public:  // from MPbkProcessObserver
        void ProcessFinished(MPbkBackgroundProcess& aProcess);

    private: // from MProgressDialogCallback
        void DialogDismissedL(TInt aButtonId);

    private:  // Implementation
        CPbkSetToneToGroupCmd(CPbkContactEngine& aEngine);
        void ConstructL(TContactItemId aGroupId, const TDesC& aRingingToneName);
        void DeleteProgressNote();
        void IssueRequest();

    private:  // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: set to ETrue in destructor entry
        TBool iDestroyed;
        /// Own: progress note dialog
        CAknProgressDialog* iProgressDialog;
        /// Own: Progress dialog info
        CEikProgressInfo* iProgressDlgInfo;
        /// Own: contacts ringing tone setting process
        CPbkSetToneToGroup* iSetToneToGroupProcess;
        /// Ref: pointer to pointer to reset in destructor
        CPbkSetToneToGroupCmd** iSelfPtr;
        /// Own: count of contacts succesfully set tone
        TInt iSetCount;
        /// Ref: command observer
        MPbkCommandObserver* iCommandObserver;
        /// Own: dialog dismissed flag
        TBool iDialogDismissed;
    };

#endif // __CPbkSetToneToGroupCmd_H__

// End of File
