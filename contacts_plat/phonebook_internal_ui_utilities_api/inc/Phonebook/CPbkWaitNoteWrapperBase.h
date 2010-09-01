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
*     Declares a wait note wrapper.
*
*/


#ifndef __CPbkWaitNoteWrapperBase_H__
#define __CPbkWaitNoteWrapperBase_H__

// INCLUDES
#include <e32base.h>
#include <AknProgressDialog.h>

// FORWARD DECLARATIONS
class CAknProgressDialog;
class MPbkBackgroundProcess;
class MPbkProcessObserver;
class CEikProgressInfo;

// CLASS DECLARATION

/**
 * Runs a MPbkBackgroundProcess with a wait note on display.
 */
class CPbkWaitNoteWrapperBase : 
    public CBase, 
    private MProgressDialogCallback
    {
    protected: // Constructor
        /**
         * Constructor
         */
        CPbkWaitNoteWrapperBase();
        
    public: // Interface
        /**
         * Parameter object for CPbkWaitNoteWrapperBase::ExecuteL.
         */
        struct TNoteParams
            {
            /**
             * Default constructor. Initializes all fields to default values.
             */
            IMPORT_C TNoteParams();
            
            /**
             * If set ETrue the dialog will be visible. Use only when the 
             * length of the process is ALWAYS over 1.5 seconds.
             * default = EFalse
             */
            TBool iVisibilityDelayOff;

            /**
             * The tone to play when the dialog is shown.<p>
             * default = CAknNoteDialog::ENoTone
             */
            CAknNoteDialog::TTone iTone;

            /**
             * Text for the dialog (overrides text set in resource file).<p>
             * default = NULL (use text from resources)
             */
            const TDesC* iText;

            /**
             * Sets an observer for current execution of the process.<p>
             * default = NULL
             * @see MPbkProcessObserver
             */
            MPbkProcessObserver* iObserver;

            private:  // data
				/// Own: spare data
                TInt32 iSpare1;
				/// Own: spare data
                TInt32 iSpare2;
            };

        /**
         * Displays a wait note. Cancels any currently executing process.
         *
         * @param aResId        Resource id of the wait note dialog to display.
         * @param aNoteParams   parameters for the note. Default values are 
         *                      used if this parameter is not passed.
         */
        IMPORT_C void DisplayL
            (TInt aResId, 
            const TNoteParams& aNoteParams =TNoteParams());

        /**
         * Runs a background process.
         *
         * @param aProcess      the process to run.
         * @precond DisplayL() has been called and no process is currently
         *          executing.
         */
        IMPORT_C void Execute(MPbkBackgroundProcess& aProcess);

        /**
         * Displays a wait note and runs the process. Cancels any currently
         * executing process.
         *
         * @param aProcess      the process to run.
         * @param aResId        Resource id of the wait note dialog to display.
         * @param aNoteParams   parameters for the note. Default values are 
         *                      used if this parameter is not passed.
         */
        IMPORT_C void ExecuteL
            (MPbkBackgroundProcess& aProcess, 
            TInt aResId,
            const TNoteParams& aNoteParams =TNoteParams());

        /**
         * Returns true if the wait/progress note is closed.
         */
        IMPORT_C TBool IsNoteClosed() const;

        /**
         * Sets the progress dialog callback interface to aCallback.
         */
        IMPORT_C void SetProgressDialogCallback(MProgressDialogCallback& aCallback);

        /**
         * Destructor.
         */
        virtual ~CPbkWaitNoteWrapperBase();

    private:  // Abstract interface for derived classes
        /**
         * Creates the concrete (wait or progress) dialog.
		 * @param aSelfPtr self pointer
		 * @param aVisibilityDelayOff if ETrue the dialog will be visible
         */
        virtual CAknProgressDialog* CreateDialogL
            (CEikDialog** aSelfPtr, TBool aVisibilityDelayOff) =0;

        /**
         * Returns ETrue if progress info should be shown.
         */
        virtual TBool ShowProgressInfo() const =0;

    private:  // from MProgressDialogCallback
        void DialogDismissedL(TInt aButtonId);

    private: // Implementation
        void IdleRunL();
        TInt IdleRunError(TInt aError);
        void Cancel();
        void CloseNote();
        void KillNote();
        void ProcessFinished();
        void ProcessCanceled();
        void NotifyObserver(MPbkBackgroundProcess& aProcess);
    
        /**
         * Idle callback helper class for CPbkWaitNoteWrapperBase.
         */
        class CIdleCallback : public CActive
            {
            public:
				/**
				 * Constructor.
				 * @param aWaitNoteWrapper wait note wrapper
				 */
                CIdleCallback(CPbkWaitNoteWrapperBase& aWaitNoteWrapper);

				/**
				 * Destructor.
				 */
                ~CIdleCallback();

				/**
				 * Requests service from active object.
				 */
                void IssueRequest();

            private:  // from CActive
	            void DoCancel();
	            void RunL();
	            TInt RunError(TInt aError);

            private:  // Data
				/// Ref: wait note wrapper
                CPbkWaitNoteWrapperBase& iWaitNoteWrapper;
            };
        friend class CIdleCallback;

    private: // Data
        /// Ref: the process to execute
        MPbkBackgroundProcess* iBackgroundProcess;
        /// Own: wait note dialog
        CAknProgressDialog* iWaitDialog;
        /// Ref: progress indicator
        CEikProgressInfo* iProgressInfoControl;
        /// Own: idle callback object
        CIdleCallback iIdleCallback;
        /// Ref: observer
        MPbkProcessObserver* iObserver;
        /// Ref: dialog observer
        MProgressDialogCallback* iDialogCallbackObserver;
		/// Own: note open state
		TBool iNoteOpen;
    };

#endif // __CPbkWaitNoteWrapperBase_H__

// End of File
