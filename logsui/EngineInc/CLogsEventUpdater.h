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
*     Updates Log events adding remote party
*
*/


#ifndef __Logs_Engine_CLogsEventUpdater_H__
#define __Logs_Engine_CLogsEventUpdater_H__

//  INCLUDES
#include "CLogsBaseUpdater.h"

// CONSTANTS
const TInt KFieldIdSize = 5; // size for field id buffer

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


class CLogsEventUpdater : public CLogsBaseUpdater
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
        static CLogsEventUpdater* NewL( RFs& aFsSession, 
                                        MLogsObserver* aObserver, 
                                        CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aMatchFlags );
    private:

        /**
         * Default constructor
         */
        CLogsEventUpdater();
                                                                       

        /**
         * Constructor, second phase. 
         */
        void ConstructL( CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aMatchFlags );

        /**
         * Constructor
         *
         * @param aFsSession        reference to file server session
         * @param aObserver         pointer to observer
         */
        CLogsEventUpdater( RFs& aFsSession, 
                           MLogsObserver* aObserver );
    
    public:     
        /**
         * Destructor.
         */
        virtual ~CLogsEventUpdater();
            
    protected:     

        void RunL();

    private: 

        void StartRunningL();   //Called from base class when processing can start
        void ContinueRunningL( TInt aFieldId );//Called from base class when processing can continue 	
		//map function from virutal pbk field type to pbk field type
        TInt NumberPbkFieldFromVPbk( TInt aNumberFieldType ) const;  
		
    protected:

        //virtual function from base class     
        void ProcessVPbkSingleContactOperationCompleteImplL(
             MVPbkContactOperationBase& aOperation,
             MVPbkStoreContact* aContact);

    public: // from MLogsReader

        void ConfigureL( const MLogsReaderConfig* aConfig ); 
          
    private: // data
        
        /// Own: number
        HBufC* iNumber; 
        
		/// Own: number field type
        TInt iNumberFieldType;
		
        /// Own: field id
        // TBuf<KFieldIdSize> iFieldId;
    };

#endif // __Logs_Engine_CLogsEventUpdater_H__

// End of File
