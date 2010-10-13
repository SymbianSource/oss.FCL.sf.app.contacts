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
* Description:  Phonebook 2 Group UI Extension remove from group command.
*
*/


#ifndef CPGUREMOVEFROMGROUPCMD_H
#define CPGUREMOVEFROMGROUPCMD_H

// INCLUDES
#include <e32base.h>
#include <AknProgressDialog.h> // MProgressDialogCallback
#include <MPbk2Command.h>
#include <CPbk2FetchDlg.h>
#include <MVPbkContactObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactViewObserver.h>
#include <MVPbkContactSelector.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactGroup;
class MVPbkContactStore;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkContactViewBase;
class CEikProgressInfo;
class CAknProgressDialog;
class CAknInputBlock;

/**
 * Phonebook 2 progress dialog
 * If end call key is pressed,it will destroy itself in any case.
 */
class CPbk2AknProgressDialog : 
        public CAknProgressDialog
    {
    public: // Construction and destruction     
        /**
         * Constructor
         *   Use this if the length of the process is unknown but the progress
         *   can be calculated.
         * @param    aSelfPtr        Pointer to itself. The pointer must be
         *                           valid when the dialog is dismissed and it must
         *                           not be on the stack.
         * @param    aVisibilityDelayOff If set ETrue the dialog will be visible
         *                                   immediality. Use only when the length of
         *                                   the process is ALWAYS over 1.5 seconds.
         */
        CPbk2AknProgressDialog(CEikDialog** aSelfPtr,TBool aVisibilityDelayOff);

        /**
         * Destructor
         */
        virtual ~CPbk2AknProgressDialog();
        	
    public: // From CAknProgressDialog
        /**
         *  Handle key events (part of CONE framework)
         */
       TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
                
    protected: // From CAknProgressDialog 
        /**
         * Called by the dialog framework, returns true if the 
         * dialog can exit, false otherwise.
         *
         * @param aButtonId  Id of the softkey which was pressed
         * @return           ETrue if the dialog can exit, false otherwise.
         */
    TBool OkToExitL(TInt aButtonId);
                
    private: // Data
        /// Own: The flag indicates if the end call key pressed
        TBool iEndCallKeyPressed; 
    };

/**
 * Phonebook 2 Group UI Extension remove from group command.
 */
class CPguRemoveFromGroupCmd : 
        public CActive,
        public MPbk2Command,
        private MVPbkSingleContactOperationObserver,
        private MVPbkContactObserver,
        private MVPbkContactViewObserver,
        private MProgressDialogCallback

    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aGroup        Link to the group to remove.
         * @param aUiControl    Contact UI control.
         * @return  A new instance of this class.
         */
        static CPguRemoveFromGroupCmd* NewLC(
                MVPbkContactLink& aGroup,
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPguRemoveFromGroupCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver(
                MPbk2CommandObserver& aObserver );
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );

    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(
                TInt aError );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

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

    private: // From MProgressDialogCallback
        void DialogDismissedL(
                TInt aButtonId );

    private:  // Implementation
        CPguRemoveFromGroupCmd(
                MVPbkContactLink& aContactGroup,
                MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        TBool MoreContactsToRemove() const;
        TBool IsProcessDone() const;
        void StepL();
        void IssueRequest();
        void CompleteL();
        void CompleteWithError();
        void DeleteProgressNoteL();
        void RetrieveContactL(
                const MVPbkContactLink& aContactLink );
        void ConfirmRemovingL();
        void InitProgressDlgL();
        void HandleRemovingL();
        void ReactivateUIBlockL();

    private: // Data structures
        /// Process states
        enum TProcessState
            {
            ERetrieving,
            EConfirming,
            ERetrievingGroupContact,
            ERemoving,
            ECompleting,
            ECompletingWithError
            };

    private: // Data
        /// Ref: UI control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: Retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Ref: A group where to add members
        MVPbkContactLink& iGroupLink;
        /// Own: Contact group
        MVPbkContactGroup* iContactGroup;
        /// Own: Contacts to be removed
        MVPbkContactLinkArray* iSelectedContacts;
        /// Ref: Focused contact
        const MVPbkBaseContact* iFocusedContact;
        /// Ref: Progress dialog info
        CEikProgressInfo* iProgressDlgInfo;
        /// Own: Indicates whether this object has been destroyed
        TBool iDestroyed;
        /// Own: Progress note dialog
        CAknProgressDialog* iProgressDialog;
        /// Own: Count of contacts succesfully removed
        TInt iRemovedCount;
        /// Own: Dialog dismissed flag
        TBool iDialogDismissed;
        /// Own: Contacts to remove count
        TInt iTotalContactsToRemove;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Current state of process
        TProcessState iState;
        /// Own: Initially focused contact index
        TInt iFocusedContactIndex;
        // Own: User input blocker
        CAknInputBlock* iInputBlock;
    };

#endif // CPGUREMOVEFROMGROUPCMD_H

// End of File
