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
*     Interface class for an abstract process divided into steps.
*
*/


#ifndef __MPbkBackgroundProcess_H__
#define __MPbkBackgroundProcess_H__

// INCLUDES
#include <e32def.h>

// CLASS DECLARATIONS

/**
 * Abstract process divided into steps.
 */
class MPbkBackgroundProcess
    {
    public: // interface
        /**
         * Virtual destructor.
         */
        virtual ~MPbkBackgroundProcess() { }

        /**
         * Executes one cycle of the process. Called by a process driver.
         */
        virtual void StepL() =0;

        /**
         * Returns the total number of steps in this process or -1 if not 
         * known.
         */
        virtual TInt TotalNumberOfSteps() 
            { 
            return -1; 
            }

        /**
         * Returns true when this process is done.
         */
        virtual TBool IsProcessDone() const =0;

        /**
         * Called back by the process driver when this process is finished.
         */
        virtual void ProcessFinished() { }

        /**
         * Handles an error (leave) occurred in StepL. 
         * Default implementation returns aError.
         *
         * @param aError    code of the error that occurred.
         * @return  translated error code. If the return value is not KErrNone 
         *          this process will be stopped.
         * @precond aError!=KErrNone
         */
        virtual TInt HandleStepError(TInt aError)
            { 
            return aError; 
            }
    
        /**
         * Process cancelled.
         */
        virtual void ProcessCanceled() = 0;
    };


/**
 * Abstract process completion observer.
 */ 
class MPbkProcessObserver
    {
    public:
        /**
         * Called when aProcess has finished. Process is finished when it 
         * completes, is cancelled or encounters and unrecoverable error.
         */
        virtual void ProcessFinished(MPbkBackgroundProcess& aProcess) =0;
    };


#endif // __MPbkBackgroundProcess_H__


// End of File
