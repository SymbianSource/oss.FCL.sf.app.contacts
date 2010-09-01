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
* Description: 
*     Phonebook 2 contact store configuration monitor.
*
*/


#ifndef CPBK2STORECONFIGURATIONMONITOR_H
#define CPBK2STORECONFIGURATIONMONITOR_H

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CRepository;
class TVPbkContactStoreUriPtr;
class CVPbkContactStoreUriArray;
class MPbk2StoreConfigurationObserver;

/**
 * Phonebook 2 contact store configuration monitor.
 */
NONSHARABLE_CLASS(CPbk2StoreConfigurationMonitor) : 
        public CActive
    {
	public: // Construction & destruction
		static CPbk2StoreConfigurationMonitor* NewL(
		    CRepository& aRepository,
		    MPbk2StoreConfigurationObserver& aObserver);

        ~CPbk2StoreConfigurationMonitor();

    public: // From CActive
        void RunL();
        void DoCancel();

    private: // Implementation
        CPbk2StoreConfigurationMonitor(
                CRepository& aRepository,
                MPbk2StoreConfigurationObserver& aObserver);
        void ConstructL();
        void RenewRequestL();

	private: // Data
        /// Ref: Central repository handle
        CRepository& iRepository;
        /// Ref: Observers of store configuration events
        MPbk2StoreConfigurationObserver& iObserver;
	};

#endif // CPBK2STORECONFIGURATIONMONITOR_H

// End of File
