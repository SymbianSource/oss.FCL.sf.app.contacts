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
*      Executes a MPbkBackgroundProcess in an idle loop.
*
*/


#ifndef __CPBKIDLEPROCESSRUNNER_H__
#define __CPBKIDLEPROCESSRUNNER_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPbkBackgroundProcess;
class MPbkProcessObserver;

// CLASS DECLARATION

/**
 * Executes a MPbkBackgroundProcess in an idle loop.
 */
NONSHARABLE_CLASS(CPbkIdleProcessRunner) :
        public CBase
    {
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aPriority desired priority of the active object
         */
        static CPbkIdleProcessRunner* NewL(TInt aPriority);

        /**
         * Destructor.
         */
        ~CPbkIdleProcessRunner();

    public: // Interface
        /**
         * Executes aProcess asynchronously in an idle loop.
         *
         * @param aProcess  process to execute.
         * @param aObserver process observer.
         */
        void Execute
            (MPbkBackgroundProcess& aProcess, MPbkProcessObserver* aObserver);

        /**
         * Executes aProcess in an idle loop. This function returns when the 
         * process completes or is interrupted.
         *
         * @param aProcess  process to execute.
         * @exception any error when executing the process.
         */
        void SynchronousExecuteL
            (MPbkBackgroundProcess& aProcess);

        /**
         * Cancels current execution.
         */
        void Cancel();

    private:  // Implementation
        CPbkIdleProcessRunner(TInt aPriority);

        class CRunner : public CActive
            {
            public:  // Interface
                CRunner(TInt aPriority);
                ~CRunner();
                void Start
                    (MPbkBackgroundProcess& aProcess, 
                    MPbkProcessObserver* aObserver);
                void RunSyncL(MPbkBackgroundProcess& aProcess);
                void Stop();

            private:  // from CActive
                void DoCancel();
                void RunL();
                TInt RunError(TInt aError);

            private:  // Implementation
                void ProcessFinished();
                void ProcessCanceled();
                void Finalize(MPbkBackgroundProcess* aProcess);
                void IssueRequest();

            private:  // Data
                MPbkBackgroundProcess* iProcess;
                MPbkProcessObserver* iObserver;
                TInt* iProcessError;
                CActiveSchedulerWait iActiveWait;
            };

    private:  // Data
		/// Own: the active object
        CRunner iRunner;
    };


#endif // __CPBKIDLEPROCESSRUNNER_H__

// End of File
