/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Check if a Contact Link is valid
*
*/

#ifndef __Logs_Engine_CLogsCntLinkChecker_H__
#define __Logs_Engine_CLogsCntLinkChecker_H__

//  INCLUDES
#include "CLogsBaseUpdater.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


class CLogsCntLinkChecker : public CLogsBaseUpdater
    {
    
    public:     // Constructors.

        /**
         * Standard creation function.
         *
         * @param aFsSession        reference to file server session
         * @param aObserver         pointer to observer
         *
         * @return Pointer to the new instance of this class.
         */     
        static CLogsCntLinkChecker* NewL( RFs& aFsSession, 
                                          MLogsObserver* aObserver );
    private:

        /**
         * Default constructor
         */
        CLogsCntLinkChecker();
                                                                       

        /**
         * Constructor, second phase. 
         */
        void ConstructL( );

        /**
         * Constructor
         *
         * @param aFsSession        reference to file server session
         * @param aObserver         pointer to observer
         */
        CLogsCntLinkChecker( RFs& aFsSession, 
                           MLogsObserver* aObserver );
    
    public:     
        /**
         * Destructor.
         */
        virtual ~CLogsCntLinkChecker();
            
    protected:     

        void RunL();

    private: 

        void StartRunningL();   //Called from base class when processing can start
        void ContinueRunningL( TInt aFieldId );//Called from base class when processing can continue
        void CntLinkDeletedL( );//Called from base class when processing can continue
    protected:

        //virtual function from base class     
        void ProcessVPbkSingleContactOperationCompleteImplL(
             MVPbkContactOperationBase& aOperation,
             MVPbkStoreContact* aContact);
			 
    public: // from MLogsReader

        void ConfigureL( const MLogsReaderConfig* aConfig ); 
		
        IMPORT_C TBool IsSameContactLink( const TDesC8& aContactLink ) const;
        IMPORT_C TBool IsCntLinkValidSync ( const TDesC8& aContactLink );
     
        void Stop();
   
        void HandleStoreEventL(
                        MVPbkContactStore& aContactStore, 
                        TVPbkContactStoreEvent aStoreEvent); 
    private: // data
        
        /// Own: contact link
        HBufC8* iContactLink; 
        
        TBool iContactLinkValid;
        
        CActiveSchedulerWait *iWait;
        
    };

#endif // __Logs_Engine_CLogsCntLinkChecker_H__

// End of File
