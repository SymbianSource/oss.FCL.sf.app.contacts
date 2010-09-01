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
* Description:  Phone number matching strategy for parallel matching.
*
*/


#ifndef CVPBKPHONENUMBERPARALLELMATCHSTRATEGY_H
#define CVPBKPHONENUMBERPARALLELMATCHSTRATEGY_H

// INCLUDE FILES
#include <CVPbkPhoneNumberMatchStrategy.h>

/**
 * Phone number matching strategy for parallel matching.
 */
NONSHARABLE_CLASS(CVPbkPhoneNumberParallelMatchStrategy)
		: public CVPbkPhoneNumberMatchStrategy
    {
    public: // Construction & destruction
        /**
         * Creates a new instance of this class.
         *
         * @param aConfig Configuration data for phone number matching.
         * @param aContactManager Contact manager to be used in matching.
         * @param aObserver Observer for the matching operation.
         * @return Newly created instance of this class.
         */
        static CVPbkPhoneNumberParallelMatchStrategy* NewL(
                const TConfig& aConfig,
                CVPbkContactManager& aContactManager,
                MVPbkContactFindObserver& aObserver);

        ~CVPbkPhoneNumberParallelMatchStrategy();

    private: // From CVPbkPhoneNumberMatchStrategy
        MVPbkContactOperation* CreateFindOperationLC(const TDesC& aPhoneNumber);
        void InitMatchingL();

    private: // Implementation
        /**
         * Constructor.
         */
        CVPbkPhoneNumberParallelMatchStrategy();

        /**
         * Initializes the class.
         *
         * @param aConfig Configuration data for phone number matching.
         * @param aContactManager Contact manager to be used in matching.
         * @param aObserver Observer for the matching operation.
         */
        void ConstructL(
                const TConfig& aConfig,
                CVPbkContactManager& aContactManager,
                MVPbkContactFindObserver& aObserver);

    private: // Data
        /// Flag to indicate if the matching has been started.
        TBool iMatchingStarted;
	};

#endif // CVPBKPHONENUMBERPARALLELMATCHSTRATEGY_H
//End of file

