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
*     Disk space level watcher for CPbkDefaultCompressionStrategy.
*
*/


#ifndef __CDISKSPACEWATCHER_H__
#define __CDISKSPACEWATCHER_H__


// CLASS DECLARATION

/**
 * Database compression timeout timer.
 */
NONSHARABLE_CLASS(CPbkDefaultCompressionStrategy::CDiskSpaceWatcher) : 
        public CActive
    {
    public:  // Interface
		/**
         * Creates a new instance of this class.
		 * @param aCompressionStrategy compressions strategy
		 * @param aFs file server session
		 * @param aThreshold threshold
		 * @param aDrive disk drive
         */
        static CDiskSpaceWatcher* NewL
                (MPbkCompressionStrategy& aCompressionStrategy,
                RFs& aFs, TInt64 aThreshold, TInt aDrive);

		/**
         * Destructor.
         */
        ~CDiskSpaceWatcher();

    private:  // from CActive
	    void DoCancel();
        void RunL();
	    TInt RunError(TInt aError);

    private:  // Implementation
        CDiskSpaceWatcher
            (MPbkCompressionStrategy& aCompressionStrategy,
            RFs& aFs, TInt64 aThreshold, TInt aDrive);
        void RequestNotification();

    private:  // Data
		/// Ref. compression strategy
        MPbkCompressionStrategy& iCompressionStrategy;
		/// Ref: file server session
        RFs iFs;
		/// Own: threshold
        TInt64 iThreshold;
		/// Own: disk drive
        TInt iDrive;
    };

#endif // __CDISKSPACEWATCHER_H__

// End of File
