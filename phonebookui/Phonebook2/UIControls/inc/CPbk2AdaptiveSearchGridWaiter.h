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
* Description:  Phonebook2 Adaptive Search Grid
*               helper class
*/


#ifndef CPBK2ADAPTIVESEARCHGRIDWAITER_H
#define CPBK2ADAPTIVESEARCHGRIDWAITER_H

//  INCLUDES
#include <e32base.h>
#include <AknProgressDialog.h>

class CAknWaitDialog;

/**
 * @internal Only Phonebook 2 internal use supported!
 */
class MPbk2SearchGridWaiterObserver
    {
    public: // Interface
        /**
         * Grid delay complete
         */ 
        virtual void GridDelayCompleteL() = 0;
        
        virtual void WaitNoteDismissed() = 0;
    };

/**
 * Utility class that implements the wait logic and
 * process decoration if needed.
 */
NONSHARABLE_CLASS( CPbk2AdaptiveSearchGridWaiter ) :
        public CTimer,
        private MProgressDialogCallback

    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2AdaptiveSearchGridWaiter* NewL( MPbk2SearchGridWaiterObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2AdaptiveSearchGridWaiter();

    public: // Interface

        void Start();
        void Stop();
        
    private: // From CTimer
        void RunL();
        TInt RunError( TInt aError );

    private: // From MProgressDialogCallback
        void DialogDismissedL( TInt aButtonId );

    private: // Implementation
        CPbk2AdaptiveSearchGridWaiter( MPbk2SearchGridWaiterObserver& aObserver );
        void ConstructL();
        void DismissWaitNote();
        
    private: // Data
        /// Own: Decorator for the process
        CAknWaitDialog*  iWaitNote;
        // Notifiy if wait note is on
        TBool iStarted;
        // Ref:
        MPbk2SearchGridWaiterObserver& iObserver;
    };

#endif // CPBK2ADAPTIVESEARCHGRIDWAITER_H
            
// End of File
