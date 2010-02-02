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
*      Contact database rollback event listener.
*
*/


#ifndef __CPbkDbRecovery_H__
#define __CPbkDbRecovery_H__

// INCLUDES
#include <cntdb.h>

// FORWARD DECLARATIONS
class MPbkRecoveryErrorHandler;

// CLASS DECLARATION

/**
 * Contact database rollback event listener.
 */
class CPbkDbRecovery : 
        public CBase,
        public MContactDbObserver
    {
    public: // Constructors and Destructor
        /**
         * Creates a new instance of this class.
		 * @param aDb Reference to contact database
         */
        IMPORT_C static CPbkDbRecovery* NewL(CContactDatabase& aDb);

        /**
         * Destructor.
         */
        ~CPbkDbRecovery();

    public:  // New functions
        /**
         * Sets a recovery error handler.
         */
        IMPORT_C void SetErrorHandler(MPbkRecoveryErrorHandler* aErrorHandler);

    private:  // from MContactDbObserver
	    void HandleDatabaseEventL(TContactDbObserverEvent aEvent);

    private:  // Implementation
        CPbkDbRecovery(CContactDatabase& aDb);
        void ConstructL();
        void StartRecovery();
        TBool DoRecoveryL();
        static TInt DoRecoveryL(TAny* aSelf);

    private:  // Data
		/// Ref: contact database
        CContactDatabase& iDb;
		/// Own:: contact db change notifier
        CContactChangeNotifier* iDbChangeNotifier;
		/// Ref: error handler
        MPbkRecoveryErrorHandler* iErrorHandler;
		/// Own:: starter for asynchronous callback process
        CIdle* iStarter;
    };


#endif // __CPbkDbRecovery_H__

// End of File
