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
*      Interface for Phonebook database compression strategy.
*
*/


#ifndef __CPBKDEFAULTCOMPRESSIONSTRATEGY_H__
#define __CPBKDEFAULTCOMPRESSIONSTRATEGY_H__

// INCLUDES
#include <cntdb.h>      // for MContactUiCompactObserver
#include "MPbkCompressionStrategy.h"

// CLASS DECLARATION

/**
 * Default Phonebook database compression strategy.
 */
NONSHARABLE_CLASS(CPbkDefaultCompressionStrategy) :
        public CBase,
        public MPbkCompressionStrategy,
        private MContactDbObserver,
        private MContactUiCompactObserver
    {
    public:
        /**
         * Creates a new instance of this class.
         *
         * @param aDb       contact database whose compression to manage.
         * @param aDbDrive  the drive aDb is on. This parameter is needed 
         *                  because the drive cannot be retrieved from 
         *                  CContactDatabase once it is opened.
         * @param aFs       Open file system handle. 
         */
        static CPbkDefaultCompressionStrategy* NewL
            (CContactDatabase& aDb, TDriveNumber aDbDrive, RFs& aFs);

        /**
         * Destructor.
         */
        ~CPbkDefaultCompressionStrategy();

    public: // from MPbkCompressionStrategy
        void SetCompressUi(MPbkCompressUi* aCompressUi);
        TBool CheckCompress();
        TBool IsCompressionEnabled();
        void CompressL();
        void CancelCompress();
        void Release();

    private:  // from MContactDbObserver
    	void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

    private:  // from MContactUiCompactObserver
	    void Step(TInt aStep);
	    void HandleError(TInt aError);

    private:  // Implementation
        CPbkDefaultCompressionStrategy
            (CContactDatabase& aDb, TDriveNumber aDbDrive, RFs& aFs);
        void ConstructL();
        void DoCompressL();
        TBool FileSpaceLowOnDbDrive() const;
        class CCompressionTimer;
        friend class CCompressionTimer;
        class CDiskSpaceWatcher;

    private:  // Data
        /// Ref: contact database whose compression to manage
        CContactDatabase& iDb;
        /// Own: drive the contact database is on
        TDriveNumber iDbDrive;
        /// Own: File server connection
        mutable RFs iFs;
        /// Own: contact database change notifier
        CContactChangeNotifier* iDbChangeNotifier;
        // Own: compression timer
        CCompressionTimer* iCompressionTimer;
        /// Own: disk space watcher
        CDiskSpaceWatcher* iDiskSpaceWatcher;
        /// Own: executing database compression object
        CContactActiveCompress* iCompressor;
        /// Ref: compress UI
        MPbkCompressUi* iCompressUi;
    };


#endif // __CPBKDEFAULTCOMPRESSIONSTRATEGY_H__


// End of File
