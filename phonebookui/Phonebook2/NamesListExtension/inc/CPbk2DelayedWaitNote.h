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


#ifndef CPBK2DELAYEDWAITNOTE_H
#define CPBK2DELAYEDWAITNOTE_H

//  INCLUDES
#include <e32base.h>

#include <MPbk2ProcessDecorator.h>

/**
 * Utility class that implements the wait logic and
 * process decoration if needed.
 */
class CPbk2DelayedWaitNote : public CTimer,
                             private MPbk2ProcessDecoratorObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2DelayedWaitNote* NewL( MPbk2ProcessDecoratorObserver& aObserver, TInt aResourceId );

        /**
         * Destructor.
         */
        ~CPbk2DelayedWaitNote();

    public: // Interface
        
        /**
         * Starts the timer and after it expires, will display the 
         * wait note decorator.
         */
        void Start();
        /**
         * Stops displaying the wait note decorator or just stops the timer
         * if the wait note is not yet shown.
         * Call to Stop will always result to ProcessDismissed call of the observer.
         */
        void Stop();
        
    private: // From CTimer
        void RunL();
        TInt RunError( TInt aError );

    private: // From MPbk2ProcessDecoratorObserver 
        void ProcessDismissed( TInt aCancelCode );

    private: // Implementation
        CPbk2DelayedWaitNote( MPbk2ProcessDecoratorObserver& aObserver, TInt aResourceId );
        void ConstructL();
        void DismissWaitNote();
        
    private: // Data
        /// Own: Decorator for the wait note process
        MPbk2ProcessDecorator* iDecorator;
        // Notifiy if wait note is on
        TBool iStarted;
        // Ref:    
        MPbk2ProcessDecoratorObserver& iObserver;
        TInt iResourceId; 
    };

#endif // CPBK2DELAYEDWAITNOTE_H
            
// End of File
