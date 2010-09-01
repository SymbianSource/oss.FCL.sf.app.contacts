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
*      Multiple contacts assignation to group command.
*
*/


#ifndef __CPbkAssignToGroupCmd_H__
#define __CPbkAssignToGroupCmd_H__

// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <AknProgressDialog.h>
#include <MPbkCommand.h>
#include <MPbkBackgroundProcess.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CContactIdArray;
class CPbkAssignToGroup;
class CPbkContactViewListControl;

// CLASS DECLARATION

/**
 * Assign multiple contact to a group command.
 */
class CPbkAssignToGroupCmd :
        public CActive,
        public MPbkCommand,
        private MPbkProcessObserver,
        private MProgressDialogCallback
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine reference to a contact engine.
         * @param aContacts contacts to delete.
		 * @param aUiControl the ui control
		 * @param aGroupId id of the group to assign to
         * @return a new delete contacts command object.
         */
        static CPbkAssignToGroupCmd* NewL
            (CPbkContactEngine& aEngine,
            const CContactIdArray& aContacts,
            CPbkContactViewListControl& aUiControl,
            TContactItemId aGroupId);

        /**
         * Sets aSelfPtr to NULL when this object is destroyed.
         * @precond !aSelfPtr || *aSelfPtr==this
         */
        void ResetWhenDestroyed(CPbkAssignToGroupCmd** aSelfPtr);

        /**
         * Destructor.
         */
        ~CPbkAssignToGroupCmd();

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
        CPbkAssignToGroupCmd(CPbkContactEngine& aEngine, CPbkContactViewListControl& aUiControl);
        void ConstructL(const CContactIdArray& aContacts, TContactItemId aGroupId);
        void DeleteProgressNote();
        void IssueRequest();

    private:  // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Ref: UI control
        CPbkContactViewListControl& iUiControl;
        /// Own: set to ETrue in destructor entry
        TBool iDestroyed;
        /// Own: progress note dialog
        CAknProgressDialog* iProgressDialog;
        /// Own: Progress dialog info
        CEikProgressInfo* iProgressDlgInfo;
        /// Own: contacts deletion process
        CPbkAssignToGroup* iAssignToGroupProcess;
        /// Own: contact to focus after deletion
        TContactItemId iFocusId;
        /// Ref: pointer to pointer to reset in destructor
        CPbkAssignToGroupCmd** iSelfPtr;
        /// Own: count of contacts succesfully assigned
        TInt iAssignedCount;
        /// Ref: command observer
        MPbkCommandObserver* iCommandObserver;
        /// Own: dialog dismissed flag
        TBool iDialogDismissed;
        /// Own: dialog dismissed with red 'end' button
        TBool iRedButtonPressed;
    };


#endif // __CPbkAssignToGroupCmd_H__

// End of File
