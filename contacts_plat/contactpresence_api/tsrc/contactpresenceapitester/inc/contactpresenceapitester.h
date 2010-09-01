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



#ifndef __CONTACTPRESENCEAPITESTER_H__
#define __CONTACTPRESENCEAPITESTER_H__

//  INCLUDES
#include <fbs.h>
#include <badesca.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

// service table
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spdefinitions.h>


_LIT( KCmsTesterLogPath,              "\\logs\\testframework\\CP\\" ); 
_LIT( KCmsTesterLogFile,              "cp_tester.txt" );

//Forward declarations

class CStifItemParser;
class MContactPresence;
class CIconInfoContainer;
class CContactPresenceApiHandler;
class CContactPresencePhonebook;
class CContactPresencePbHandler;
class CVPbkContactManager;

class CMyTimer;
// class MPresenceFeeder;
class MPresenceCacheWriter2;
class CCreatorServer2;


NONSHARABLE_CLASS( CContactPresenceApiTester ) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CContactPresenceApiTester* NewL( CTestModuleIf& aTestModuleIf );
        
        /**
        * Two-phased constructor.
        */
        void RequestComplete( TInt aStatus );
      
        /**
        * Two-phased constructor.
        */
        void StoreOneContactLinkL( HBufC8* aContactLink, CVPbkContactManager* aManager );

        /**
        * Destructor.
        */
        virtual ~CContactPresenceApiTester();
        
        
        
    // _______________________ Methods for tester observer __________________________  
    public:
    
    	void LogThis( const TDesC& aText );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private:

        /**
        * C++ default constructor.
        */
        CContactPresenceApiTester( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();
        
        void AddServiceEntryL(const TDesC& aServiceName, const TDesC& aBrandingId,  TInt& aPluginUid,  
            TInt& aServiceAttribute, TInt& aLaunchId );        
        
        // _______________________ Test Methods __________________________
                        
        TInt CreateTestContactsL( CStifItemParser& aItem ); 
        
        TInt CreateTestServicesL( CStifItemParser& aItem );         
        
        TInt OpenStoresL( CStifItemParser& aItem );
        
        TInt FetchContactL( CStifItemParser& aItem );
               
        TInt CreateContactPresenceL( CStifItemParser& aItem );
        
        TInt GetIconInfoL( CStifItemParser& aItem );
                      
        TInt GetBrandedIconL( CStifItemParser& aItem );
        
        TInt FeedPresenceL( CStifItemParser& aItem ); 
        
        TInt CleanFeedPresenceL( CStifItemParser& aItem );  
        
        TInt CancelAll( CStifItemParser& aItem );
        
        // Other methods
        
        TInt TesterAtoi( const TDesC& aDescInteger );  
        
        virtual TInt DoWaitSec( CStifItemParser& aItem );        
        
        void DoDoWaitSec( TInt aSec );        
             
    private:    //Data
        
        RFs                                         iFs;
        RFbsSession                                 iFbsSession;
        TInt                                        iExpectedResult;
        CPtrC8Array*                                iLinkArray;
        MContactPresence*                           iContactPresence;
        CContactPresenceApiHandler*                 iContactPresenceHandler;
        CContactPresencePhonebook*                  iPhonebook;
        CContactPresencePbHandler*                  iPhonebookHandler;      
                
        CMyTimer* iMyTimer; 
        // MPresenceFeeder* iPresenceFeeder;  
        MPresenceCacheWriter2* iWriter;        
        CVPbkContactManager* iManager;   
        
        /** 
         * OWN
         */
        CSPSettings* iSettings;        
    };
    
    /**
     *  Expiry timer
     */
    class CMyTimer : public CActive
        {
    public:

        CMyTimer(
            CContactPresenceApiTester & aTester );

        virtual ~CMyTimer();

        /**
         * Start timer.
         */
        void Start( TInt aSec );

    protected:

    // from base class CActive

        void DoCancel( );

        void RunL( );

        TInt RunError(TInt aError);

    private: // data


        RTimer                  iTimer;
             
        TInt                    iSeconds;
        
        CContactPresenceApiTester& iTester; 
         
        };            

#endif      // __CONTACTPRESENCETESTER_H__

// End of File
