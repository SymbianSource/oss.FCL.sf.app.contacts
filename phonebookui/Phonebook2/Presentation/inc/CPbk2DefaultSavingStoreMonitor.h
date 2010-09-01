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
*     Phonebook 2 default saving store monitor.
*
*/


#ifndef CPBK2DEFAULTSAVINGSTOREMONITOR_H
#define CPBK2DEFAULTSAVINGSTOREMONITOR_H

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CRepository;
class TVPbkContactStoreUriPtr;
class CVPbkContactStoreUriArray;
class MPbk2DefaultSavingStoreObserver;

/**
 * Phonebook 2 contact store configuration monitor.
 */
NONSHARABLE_CLASS(CPbk2DefaultSavingStoreMonitor) : 
        public CActive
    {
	public: // Construction & destruction
		static CPbk2DefaultSavingStoreMonitor* NewL(
		    CRepository& aRepository,
		    MPbk2DefaultSavingStoreObserver& aObserver );

        ~CPbk2DefaultSavingStoreMonitor();

    public: //New functions
        /**
         * Return default saving store's uri. 
         *
         * @return default saving store
         */
        TVPbkContactStoreUriPtr DefaultSavingStoreL() const;

    private: // From CActive
        void RunL();
        void DoCancel();
        
    private: //New functions
        /**
         * Update default saving store   
         * return default saving store, ownership transferred     
         */
        HBufC* UpdateDefaultSavingStoreL() const;

    private: // Implementation
        CPbk2DefaultSavingStoreMonitor( 
            CRepository& aRepository,
		    MPbk2DefaultSavingStoreObserver& aObserver );
        void ConstructL();
        void RenewRequestL();

	private: // Data
        CRepository& iRepository;    
        /// Own: The URI of the default store
        mutable HBufC* iDefaultStoreUri;  
        /// Ref: observer for changes
        MPbk2DefaultSavingStoreObserver& iObserver;  
	};

#endif // CPBK2DEFAULTSAVINGSTOREMONITOR_H

// End of File
