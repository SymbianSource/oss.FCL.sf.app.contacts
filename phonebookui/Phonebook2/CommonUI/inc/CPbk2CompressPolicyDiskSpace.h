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
* Description:  Phonebook 2 compress policy disk space utility.
*
*/


#ifndef CPBK2COMPRESSPOLICYDISKSPACE_H
#define CPBK2COMPRESSPOLICYDISKSPACE_H

// INCLUDES
#include <f32file.h>
#include "MPbk2CompressPolicy.h"

// FORWARD DECLARATIONS
class MPbk2CompressPolicyManager;

// CLASS DECLARATION

/**
 * Phonebook 2 compress policy disk space utility.
 * Responsible for quering FileServer for notification when
 * free disk space drops below given threshold.
 */
NONSHARABLE_CLASS(CPbk2CompressPolicyDiskSpace) : public CActive,
                                                  public MPbk2CompressPolicy
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer.
         * @param aFs           File server session.
         * @param aThreshold    Threshold.
         * @param aDrive        Drive.
         * @return  A new instance of this class.
         */
        static CPbk2CompressPolicyDiskSpace* NewL(
                MPbk2CompressPolicyManager& aObserver,
                RFs& aFs,
                TInt64 aThreshold,
                TInt aDrive = KDefaultDrive );

        /**
         * Destructor.
         */
        ~CPbk2CompressPolicyDiskSpace();

    public: // From MPbk2CompressPolicy
        void Start();
        void Stop();

    private:  // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(
                TInt aError );

    private:  // Implementation
        CPbk2CompressPolicyDiskSpace(
                MPbk2CompressPolicyManager& aObserver,
                RFs& aFs,
                TInt64 aThreshold,
                TInt aDrive );
        void RequestNotification();

    private: // Data
        /// Ref: Compression policy manager
        MPbk2CompressPolicyManager& iManager;
        /// Ref: File server session
        RFs iFs;
        /// Own: Threshold
        TInt64 iThreshold;
        /// Own: Disk drive
        TInt iDrive;
    };

#endif // CPBK2COMPRESSPOLICYDISKSPACE_H

// End of File
