/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef __ClientSrvTester_H__
#define __ClientSrvTester_H__

//  INCLUDES
#include <fbs.h>
#include <badesca.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <presencecachereadhandler2.h>
#include <presencecachewritehandler2.h>
#include <mpresencebuddyinfo2.h>



_LIT( KCmsTesterLogPath,              "\\logs\\testframework\\Presence\\" ); 
_LIT( KCmsTesterLogFile,              "presencecachetester.txt" );

//Forward declarations

class CStifItemParser;
class MPresenceCacheWriter2;
class MPresenceCacheReader2;


NONSHARABLE_CLASS( CClientSrvTester ) : public CScriptBase, 
    public MPresenceCacheReadHandler2, public MPresenceCacheWriteHandler2
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CClientSrvTester* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Two-phased constructor.
        */
        void RequestComplete( TInt aStatus );
        
        /**
        * Destructor.
        */
        virtual ~CClientSrvTester();
        
               
    // _______________________ Methods for tester observer __________________________  
    public:
   

    public: // Functions from base classes
        
        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );        
        
   // from  MPresenceCacheReadHandler2
   

       void HandlePresenceReadL(TInt aErrorCode,
                 RPointerArray<MPresenceBuddyInfo2>& aPresenceBuddyInfoList);


       void HandlePresenceNotificationL(TInt aErrorCode,
                MPresenceBuddyInfo2* aPresenceBuddyInfo);
       
  // from PresenceCacheWriteHandler2
           
       void HandlePresenceWriteL(TInt aErrorCode);     
  
       
    private:

        CClientSrvTester( CTestModuleIf& aTestModuleIf );


        void ConstructL();


        void DeleteMe();
        
        TInt TesterAtoi( const TDesC& aDescInteger );
                
    // _______________________ Test Methods __________________________
        
        TInt SaveBuddyL( CStifItemParser& aItem );
        
        TInt FetchBuddyL( CStifItemParser& aItem );
        
        TInt FetchAllBuddiesL( CStifItemParser& aItem );        
        
        TInt SubscribeBuddyL( CStifItemParser& aItem );
        
        TInt UnsubscribeBuddyL( CStifItemParser& aItem ); 

        TInt DeleteBuddyL( CStifItemParser& aItem );        
        
        TInt SetNextDataL( CStifItemParser& aItem );  
        
        TInt CheckCountersL( CStifItemParser& aItem );  
        
        TInt DeleteServiceL( CStifItemParser& aItem );  
        
        TInt WriteMultiplePresenceL( CStifItemParser& aItem );         

        TInt VerifyThatBuddyNotFoundL( CStifItemParser& aItem );
    private:    //Data
        
        RFs                                         iFs;
        TInt                                        iExpectedResult;
        
        MPresenceCacheWriter2* iWriter;
        MPresenceCacheReader2* iReader;
        
        // Expected values to be get
        HBufC* iBuddyId;
        HBufC* iAText;        
        MPresenceBuddyInfo2::TAvailabilityValues iAvailability;
        HBufC* iStatusText;        
        HBufC* iNewKey;
        HBufC8* iNewValue;
        TInt iKeyNumber;   
        TInt iCheckState;         
        RPointerArray<MPresenceBuddyInfo2> iBuddyArray;  
        TInt iBuddiesNumber;
        TInt  iWriteManyTimes;
        HBufC* iExpiredBuddyId;
    };

#endif      // __BrandedIconsTester_H__

// End of File
