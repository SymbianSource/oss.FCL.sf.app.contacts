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
*     Sub view to a CContactViewBase.
*
*/


#ifndef __CCOMPRESSIONTIMER_H__
#define __CCOMPRESSIONTIMER_H__

#include "CPbkDefaultCompressionStrategy.h"

// CLASS DECLARATION

/**
 * Database compression timeout timer for CPbkDefaultCompressionStrategy.
 */
NONSHARABLE_CLASS(CPbkDefaultCompressionStrategy::CCompressionTimer) :
        public CTimer
    {
    public:  // Interface
        /**
         * Creates a new instance of this class.
         * @param aCompressionStrategy the compression strategy
         * @param aTimeout compression time interval
         */
        static CCompressionTimer* NewL
                (CPbkDefaultCompressionStrategy& aCompressionStrategy,
                TTimeIntervalSeconds aTimeout);

        /**
         * Starts the timer.
         */
        void Start();

    private:  // from CTimer
        void RunL();
        TInt RunError(TInt aError);

    private:  // Implementation
        CCompressionTimer
            (CPbkDefaultCompressionStrategy& aCompressionStrategy,
            TTimeIntervalSeconds aTimeout);
        void ConstructL();

    private:  // Data
        /// Ref: the compression strategy
        CPbkDefaultCompressionStrategy& iCompressionStrategy;
        /// Own: timer
        TTimeIntervalMicroSeconds32 iTimeout;
    };


#endif // __CCOMPRESSIONTIMER_H__

// End of File
