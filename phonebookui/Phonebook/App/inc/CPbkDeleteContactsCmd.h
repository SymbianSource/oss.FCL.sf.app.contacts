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
*      Multiple contacts deletion command.
*
*/


#ifndef __CPbkDeleteContactsCmd_H__
#define __CPbkDeleteContactsCmd_H__

// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <MPbkCommand.h>
#include <MPbkBackgroundProcess.h>
#include <SharedDataClient.h>   // RSharedDataClient
#include <AknProgressDialog.h>


// FORWARD DECLARATIONS
class CPbkContactEngine;
class CContactIdArray;
class CPbkContactViewListControl;
class CAknProgressDialog;
class CEikProgressInfo;


// CLASS DECLARATION

/**
 * Multiple contact deletion command.
 */
class CPbkDeleteContactsCmd :
        public CActive,
        public MPbkCommand,
        private MProgressDialogCallback
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine       reference to a contact engine.
         * @param aContacts     contacts to delete.
         * @param aUiControl    contact list UI control to update.
         * @return a new delete contacts command object.
         */
        static CPbkDeleteContactsCmd* NewL
            (CPbkContactEngine& aEngine,
            const CContactIdArray& aContacts,
            CPbkContactViewListControl& aUiControl);

        /**
         * Sets aSelfPtr to NULL when this object is destroyed.
         * @precond !aSelfPtr || *aSelfPtr==this
         */
        void ResetWhenDestroyed(CPbkDeleteContactsCmd** aSelfPtr);

        /**
         * Destructor.
         */
        ~CPbkDeleteContactsCmd();

    public:  // from MPbkCommand
        void ExecuteLD();
        void AddObserver(MPbkCommandObserver& aObserver);

    private: // from CActive
    	void DoCancel();
	    void RunL();
	    TInt RunError(TInt aError);

    private: // from MProgressDialogCallback
        void DialogDismissedL(TInt aButtonId);

    private:  // Implementation
        CPbkDeleteContactsCmd
            (CPbkContactEngine& aEngine, CPbkContactViewListControl& aUiControl);
        void ConstructL(const CContactIdArray& aContacts);
        void DeleteProgressNote();
        void IssueRequest();
        void FinishProcess();
        void DestroySelf();
        TBool MoreContactsToDelete() const;
        void CompressIfRequired();
        TInt64 FreeSpaceOnDbDrive() const;

    private:  // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Ref: UI control
        CPbkContactViewListControl& iUiControl;
        /// Own: set to ETrue in destructor entry
        TBool iUnderDestruction;
        /// Own: ids of contacts to delete
        CContactIdArray* iContacts;
        /// Own: progress note dialog
        CAknProgressDialog* iProgressDialog;
        /// Own: Progress dialog info
        CEikProgressInfo* iProgressDlgInfo;
        /// Own: contact to focus after deletion
        TContactItemId iFocusId;
        /// Ref: pointer to pointer to reset in destructor
        CPbkDeleteContactsCmd** iSelfPtr;
        /// Own: count of contacts succesfully deleted
        TInt iDeletedCount;
        /// Own: shared data client
        RSharedDataClient iSharedDataClient;
        /// Own: maximum free space required to delete a contact from the DB
        TInt iFreeSpaceRequiredToDelete;
        /// Own: contact database drive
        TInt iDbDrive;
        /// Ref: command observer
        MPbkCommandObserver* iCommandObserver;
        /// Own: dialog dismissed flag
        TBool iDialogDismissed;
    };

#endif // __CPbkDeleteContactsCmd_H__

// End of File
