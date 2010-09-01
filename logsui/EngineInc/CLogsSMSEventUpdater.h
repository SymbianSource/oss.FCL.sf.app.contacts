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
*     Updates SMS events remote party to match one in contact model.
*
*/


#ifndef __Logs_Engine_CLogsSMSEventUpdater_H__
#define __Logs_Engine_CLogsSMSEventUpdater_H__

//  INCLUDES
#include <logviewchangeobserver.h>
#include "CLogsBaseUpdater.h"
#include <CVPbkPhoneNumberMatchStrategy.h>
// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

class CLogsSMSEventUpdater :    public CLogsBaseUpdater,
                                public MLogViewChangeObserver

    {
    
    public:     // Constructors.
        /**
         * Standard creation function. Creates and returns a new object of this
         * class.
         *
         * @param aFsSession    reference to file server session
         * @param aObserver     pointer to observer
         * @return Pointer to the new instance of this class.
         */     
        static CLogsSMSEventUpdater* NewL( RFs& aFsSession, 
                                           MLogsObserver* aObserver,
                                           CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aMatchFlags );
                                            
    private:    
        /**
         * Constructor, second phase. 
         */
        void ConstructL(CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aMatchFlags );

        /**
         * Constructor
         *
         * @param aFsSession    reference to file server session
         * @param aObserver     pointer to observer
         */
        CLogsSMSEventUpdater( RFs& aFsSession,
                              MLogsObserver* aObserver );
    
    public:     
        /**
         * Destructor.
         */
        virtual ~CLogsSMSEventUpdater();

    public: // from MLogViewChangeObserver
        void HandleLogViewChangeEventAddedL(TLogId aId,   TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);       
        void HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);
        void HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount);

    protected:    
        /**
         * This is called when a request to a log engine is completed.
         */
        void RunL();

    private:   
        void StartRunningL();   //Called from base class when processing can start
        void ContinueRunningL( TInt aFieldId );//Called from base class when processing can continue 
    };

#endif // __Logs_Engine_CLogsSMSEventUpdater_H__

// End of File
