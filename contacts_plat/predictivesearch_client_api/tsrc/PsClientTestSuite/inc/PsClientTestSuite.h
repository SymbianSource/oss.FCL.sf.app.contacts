/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for client test suite 
*
*/


#ifndef PS_CLIENT_TEST_SUITE_H
#define PS_CLIENT_TEST_SUITE_H

// SYSTEM INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <MPsResultsObserver.h>
#include <CPcsDefs.h>

// USER INCLUDES
#include "TestSuiteInputData.h"
#include "psclienttestsuitedefs.h"

// FORWARD DECLARATIONS
class CPcsTestContactsHandler;
class CPsQuery;
class CPSRequestHandler;
class CPsSettings;


// CLASS DECLARATION

/**
*  CPsClientTestSuite test class for STIF Test Framework TestScripter.
*  @since S60 v3.2
*/

NONSHARABLE_CLASS(CPsClientTestSuite) : public CScriptBase,  public MPsResultsObserver
{
public:  // Constructors and destructor

        /**
        * First phase constructor constructor.
        */
        static CPsClientTestSuite* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CPsClientTestSuite();



public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * 
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
	
public: // Function from base class MPsResultsObserver
	    /**
	    * HandlePsResultsUpdateL.
	    * Handle updates from the Ps server.
	    * 
	    * @param searchResults Search results returned from the server.
	    * @param searchSeqs List of matching character sequences.
	    */
		void HandlePsResultsUpdate(RPointerArray<CPsClientData>& searchResults,
		                                    RPointerArray<CPsPattern>& searchSeqs);
		
		/**
	    * HandlePsError.
	    * Handle errors from the server.
	    * 
	    * @param aErrorCode Search errors returned from the server.	    
	    */	                                        
        void HandlePsError(TInt aErrorCode);	  
        
        /**
        * CachingStatus
        * Gets called when caching is completed.
        * Observers need to implement this function accordingly
        * to take action after cache completion
        *
        * @param aStatus - caching status, 
        *   20 - Caching completes succesfully, 
        *   30 - Caching completed with errors
        * @param aError - Any error that occurred while caching. KErrNone if no error else the error code
        */
        void CachingStatus(TCachingStatus& aStatus, TInt& aError);   
        

        
private:

        /**
        * C++ default constructor.
        */
        CPsClientTestSuite( CTestModuleIf& aTestModuleIf );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * 
        */
        void Delete();
        
    	
      
private:  //Test cases
    
        /**
        * This method writes the test case title to the log file
        */
        TInt WriteTitleToLog( );

        /**
		* This test case creates contacts in the phonebook
		*
		* @param aItem Script line containing parameters.
		*/
		TInt CreateContactsL( CStifItemParser& aItem );
		
		/**
		* This test case creates groups in the phonebook. It creats only
		* empty groups, i.e the groups do not have any members.
		*
		* @param aItem Script line containing parameters.
		*/
		TInt CreateGroupsInPhoneL( CStifItemParser& aItem );
		
		/**
		* This test case fires a search query to the psengine 
		*
		* @param aItem Script line containing parameters.
		* @return - error status - KErrNone if test case is successful
		*/
        TInt SearchCacheL( CStifItemParser& aItem );
        
        /**
		* This test case cancels a search query to the psengine
		*
		*
		* @param aItem Script line containing parameters.
		* @return - error status - KErrNone if test case is successful
		*/
        TInt CancelSearchL( CStifItemParser& aItem );
        
		/**
		* This test case searches for a query in the given input string
		*
		* @param aItem Script line containing parameters.
		* @return - error status - KErrNone if test case is successful
		*/
		TInt SearchOnInputStringL(CStifItemParser& aItem);

		/**
        * This test case searches for a query with LookupMatch
        * and ensures the result string is correct
        *
        * @param aItem Script line containing parameters.
        * @return - error status - KErrNone if test case is successful
        */
		TInt SearchLookupMatchStringL(CStifItemParser& aItem);
		
		/**
		* This method deletes all the contacts in the phonebook
		*
		* @param aItem Script line containing parameters.
		* @return - error status - KErrNone if test case is successful
		*/
		TInt DeleteAllContactsInPhonebookL(CStifItemParser& aItem);
    
    	/**
		* This method checks the caching status
		* 
		* @return - error status - KErrNone if test case is successful
		*/
		TInt CheckCachingStatusL();
    
		
    	/**
		* This method checks the version of the ps server
		* 
		* @return - error status - KErrNone if test case is successful
		*/
        TInt CheckVersion();                                 

		/**
		* This method checks if a particular language is supported by the ps server
		* 
		* @return - error status - KErrNone if test case is successful
		*/
		TInt TestIsLanguageSupportedL(CStifItemParser& aItem);
		
        /**
		* This method waits for time equal to the input parameter
		* @param aInterval  The wait interval
		*/        	
		void RTimerWait(TTimeIntervalMicroSeconds32 aInterval);
		
		/**
		* This method checks gets all the contents from a particular URI (as specified in the configuration)
		* 
		* @return - error status - KErrNone if test case is successful
		*/
		TInt GetAllContentsL( CStifItemParser& aItem );
		
		/**
		* This method searches in a particular group. The group name is given as a
		* configuration parameter.
		* 
		* @return - error status - KErrNone if test case is successful
		*/
		TInt SearchWithInGroupL(CStifItemParser& aItem );
		
		
		/**
		* This method tests the sort order
		* 
		* @return - error status - KErrNone if test case is successful
		*/
		TInt  TestSortOrderL( CStifItemParser& aItem );
	
		/**
		* This method tests the sort order for error case
		* 
		* @return - error status - KErrNone if test case is successful
		*/
		TInt TestSortOrderErrCaseL( CStifItemParser& aItem );
		/**
		* This method removes the observer from ps searchengine
		* 
		* @return - error status - KErrNone if test case is successful
		*/
		TInt RemoveObserverL(CStifItemParser& aItem);
		
		/**
        *  Tests if contact Id returns correct vpbklink
        *
        * @return -  - error status - KErrNone if test case is successful
        */
		TInt ConvertToVpbkLinkL(CStifItemParser& aItem);
		
		/**
        *  Tests for bookmarked contacts
        *
        * @return error status - KErrNone if test case is successful
        */
		TInt AddMarkedContactsTestL(CStifItemParser& aItem);
	
		/**
        *  Tests for data order 
        *
        * @return error status - KErrNone if test case is successful
        */
		TInt TestDataOrderL(CStifItemParser& aItem);
		
		/**
        *  Tests for data order 
        *
        * @return error status - KErrNone if test case is successful
        */
		TInt TestDataOrderForErrL(CStifItemParser& aItem);
	
		
		/**
		* This method shuts the server down
		* 
		* @return - error status - KErrNone if test case is successful
		*/
		TInt ShutDownL();

private:    // Data
   
    /**
    * The input data parser
    * own
    */ 
    CTestSuiteInputData* iInputParsedData;
    
    /**
    * The search query
    */
    CPsQuery *iPsQuery;
    
    /**
    * The CPSRequestHandler instance
    */
    CPSRequestHandler* iPsClientHandler;
    
    /**
    * The search settings 
    */
    CPsSettings* iSettings ;
    
    /**
    * The error return status
    */
    TInt iReturnStatus;
    
    /**
    * The group Id of the group name entered in the configuration file
    */
    TInt iGroupToBeSearchedId;
	
	/**
    * The current operation being performed
    */
    TTestCase iCurrentOperation;
    
    /**
    * RTimer variable to set the timer before RunL calls any function.
    * This is required to allow other threads to run since contact fetch 
    * is CPU intensive task. 
    */
    RTimer iTimer;
        
    /**
    * The caching status of the psengine
    */
    TCachingStatus iCachingStatus;
    
    /**
    * The marked contacts
    */
   RPointerArray<CPsClientData> iMarkedContacts;

};

#endif      // PS_CLIENT_TEST_SUITE_H

// End of File
