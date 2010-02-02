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
* Description:  ?Description
*
*/

// SYSTEM INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "PsClientTestSuite.h"
#include <CPsQueryItem.h>
#include <CPcsDefs.h>
#include <CPsQuery.h>
#include <CPsRequestHandler.h>
#include <CPsClientData.h>
#include <badesca.h>

// Defines
#define KOneSecDelay 1000000
_LIT(groupformat,"cntdb://c:contacts.gdb?id=");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPsClientTestSuite::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CPsClientTestSuite::Delete() 
{
}

// -----------------------------------------------------------------------------
// CPsClientTestSuite::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CPsClientTestSuite::RunMethodL( CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] = {
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.

        ENTRY( "WriteTitleToLog",              CPsClientTestSuite::WriteTitleToLog ),
        ENTRY( "CreateContactsInPhone",        CPsClientTestSuite::CreateContactsL ),
	    ENTRY( "ADDGroups",                    CPsClientTestSuite::CreateGroupsInPhoneL ),
	    ENTRY( "DeleteAllContactsInPhonebook", CPsClientTestSuite::DeleteAllContactsInPhonebookL ),
        ENTRY( "SearchOnInputString",          CPsClientTestSuite::SearchOnInputStringL ),
        ENTRY( "TC_ITU_SearchOnInputString",   CPsClientTestSuite::SearchOnInputStringL ), // For easy handling of .cgf file
        ENTRY( "TC_N97_SearchOnInputString",   CPsClientTestSuite::SearchOnInputStringL ), // For easy handling of .cgf file
        ENTRY( "SearchLookupMatchString",      CPsClientTestSuite::SearchLookupMatchStringL ),
        ENTRY( "TC_ITU_SearchLookupMatchString",CPsClientTestSuite::SearchLookupMatchStringL ), // For easy handling of .cgf file
        ENTRY( "TC_N97_SearchLookupMatchString",CPsClientTestSuite::SearchLookupMatchStringL ), // For easy handling of .cgf file
        ENTRY( "TestSearchingInCache",         CPsClientTestSuite::SearchCacheL ),
        ENTRY( "TC_ITU_TestSearchingInCache",  CPsClientTestSuite::SearchCacheL ), // For easy handling of .cgf file
        ENTRY( "TC_N97_TestSearchingInCache",  CPsClientTestSuite::SearchCacheL ), // For easy handling of .cgf file
	    ENTRY( "CheckCachingStatus",           CPsClientTestSuite::CheckCachingStatusL ),
	    ENTRY( "CheckVersion",                 CPsClientTestSuite::CheckVersion),
	    ENTRY( "CheckLanguageSupport",         CPsClientTestSuite::TestIsLanguageSupportedL),
	    ENTRY( "GetAllContents",               CPsClientTestSuite::GetAllContentsL),
	    ENTRY( "TestSortOrder",                CPsClientTestSuite::TestSortOrderL),
	    ENTRY( "TestSortOrderErrCaseL",        CPsClientTestSuite::TestSortOrderErrCaseL),	    
	    ENTRY( "TestSearchingWithInGroup",     CPsClientTestSuite::SearchWithInGroupL),
	    ENTRY( "ConvertToVpbkLink",            CPsClientTestSuite::ConvertToVpbkLinkL), 
	    ENTRY( "RemoveObserver",               CPsClientTestSuite::RemoveObserverL ),
	    ENTRY( "ShutDown",                     CPsClientTestSuite::ShutDownL ),
	    ENTRY( "AddMarkedContactsTest",        CPsClientTestSuite::AddMarkedContactsTestL ),
	    ENTRY( "TestDataOrder",                CPsClientTestSuite::TestDataOrderL ),
	    ENTRY( "TestDataOrderForErrL",         CPsClientTestSuite::TestDataOrderForErrL ),
	    ENTRY( "CancelSearch",                 CPsClientTestSuite::CancelSearchL )
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// This writes a log line to ease search of the execution logs
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::WriteTitleToLog( )
{
	iReturnStatus = KErrNone;

    TName title;
    TestModuleIf().GetTestCaseTitleL(title);

	// Write the log
#ifdef _DEBUG

#ifdef __WINS__
    RDebug::Print( _L("[PCS] ################################################################") );
    RDebug::Print( _L("[PCS] ##### STIF: %S"), &title );
    RDebug::Print( _L("[PCS] ################################################################") );
#endif // __WINS__

#endif // _DEBUG

	// return error code
    return iReturnStatus;
}

// -----------------------------------------------------------------------------
// This test case creates contacts in the phonebook
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::CreateContactsL( CStifItemParser& aItem )
{
    iReturnStatus = KErrNone;
    
    // Create the input parser
    iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
    
    // Parse the input data
    iInputParsedData->ParseInputL(aItem) ;  
    
    // Create the contacts
    iInputParsedData->CreateContactsL();

    // return error code
    return iReturnStatus;
}

TInt CPsClientTestSuite::CreateGroupsInPhoneL( CStifItemParser& aItem )
{
	iReturnStatus = KErrNone;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;	
  	
	// return error code
    return iReturnStatus;
}

// -----------------------------------------------------------------------------
// This test case creates a new CPsQueryItem object and checks 
// if the creation is successfull.
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::SearchCacheL( CStifItemParser& aItem )
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrGeneral;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;	
  	
	// Create the contacts
	iInputParsedData->CreateContactsL();

	// Perform the search settings
	iInputParsedData->PerformSearchSettingsL(*iSettings);
	
	// set search settings
	iPsClientHandler->SetSearchSettingsL(*iSettings); 
	 
	// Create the search query 
	iInputParsedData->CreateSearchQueryL(*iPsQuery,iInputParsedData->Query());
	
	//Wait for one second
	RTimerWait(KOneSecDelay);
	
	//Perform the search
	iPsClientHandler->SearchL(*iPsQuery);
	CActiveScheduler::Start();

   	// return error code
    return iReturnStatus;
}

// -----------------------------------------------------------------------------
// This test case creates a new CPsQueryItem object and checks 
// if search cancelling doesn't crash.
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::CancelSearchL( CStifItemParser& aItem )
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrGeneral;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;	
  	
	// Perform the search settings
	iInputParsedData->PerformSearchSettingsL(*iSettings);
	
	// set search settings
	iPsClientHandler->SetSearchSettingsL(*iSettings); 
	 
	// Create the search query 
	iInputParsedData->CreateSearchQueryL(*iPsQuery,iInputParsedData->Query());
	
	//Wait for one second
	RTimerWait(KOneSecDelay);
	
	//Perform the search
	iPsClientHandler->SearchL(*iPsQuery);

    //Cancel search
    iPsClientHandler->CancelSearch();

	//Wait for one second
	RTimerWait(KOneSecDelay);
   
   	// return error code
    return KErrNone;
}

void CPsClientTestSuite::HandlePsResultsUpdate(RPointerArray<CPsClientData>& searchResults,
		                                       RPointerArray<CPsPattern>& /*searchSeqs*/)
{
	RArray<TInt> contactIdsArray;
	TInt leaveErrorCode(KErrNone);
	switch(iCurrentOperation)
	{
		case EDeleteSearchedResults:
		    // Deleted all the contacts contained in the search results
		    for(TInt i =0 ; i< searchResults.Count();i++)
		        {
		        contactIdsArray.Append(searchResults[i]->Id());
		        }
		    TRAP( iReturnStatus, iInputParsedData->DeleteContactsWithIdsL(contactIdsArray) );
		    break;
					
		case EGetGroupId:
		    // Get the group Id. If there are more than one results,
		    // then there is an error
		    if(searchResults.Count() == 1)
		        {
		        iGroupToBeSearchedId = searchResults[0]->Id();
		        }
		    else if(searchResults.Count() > 1)
		        {
		        iReturnStatus = KErrArgument;
		        }
		    CActiveScheduler::Stop();
		    break;

		case EConvertToVpbkLink:
		    // Get the contact link for the first element.
		    if(searchResults.Count() )
		        {
		        iReturnStatus = KErrNone;
		        TRAP ( leaveErrorCode, 
		                TBool ret = iInputParsedData->TestVpbkLinkForIdL(*iPsClientHandler,*(searchResults[0]));
                        if (!ret)
                            iReturnStatus = KErrGeneral;
                        );
                if ( leaveErrorCode )
                    {
                    iReturnStatus = leaveErrorCode;
                    }
		        }
		    else
		        {
		        iReturnStatus = KErrGeneral;
		        }

		    CActiveScheduler::Stop();
		    break;
	
		case EMarkedContacts:
		    TRAP( leaveErrorCode,
                for(TInt i =0; i < searchResults.Count(); i++)
                    {
                    CPsClientData* markContact = searchResults[i]->CloneL();
                    iMarkedContacts.Append(markContact);
                    }
                );
		    if ( leaveErrorCode )
		        {
		        iReturnStatus = leaveErrorCode;
		        }
		    CActiveScheduler::Stop();
		    break;
					
		case EDefaultCase:
		    // Search is complete. Compare the results with expected results
		    TRAP( leaveErrorCode, iReturnStatus = iInputParsedData->CompareResWithExpResL(searchResults));
		    if ( leaveErrorCode )
		        {
		        iReturnStatus = leaveErrorCode;
		        }
		    CActiveScheduler::Stop();
		    break;
		default :
		    break;
	} // End Switch	
}

// -----------------------------------------------------------------------------
// Searches for a query in input string.
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::SearchOnInputStringL(CStifItemParser& aItem)
{
    RTimerWait(KOneSecDelay);
	iReturnStatus = KErrNone;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem);
  	
  	//Get the input string
	HBufC* inputStr = iInputParsedData->InputSearchString().AllocL();
	
	//To obtain the result;
	CDesCArray* matchSet = new( ELeave ) CDesCArrayFlat(1);
	RArray<TPsMatchLocation> matchLocation;
	
	// Create the search query
	iInputParsedData->CreateSearchQueryL(*iPsQuery,iInputParsedData->Query());
	//Wait for one second
	RTimerWait(KOneSecDelay);
		
	//Perform the search
	iPsClientHandler->LookupL(*iPsQuery, *inputStr, *matchSet, matchLocation);
	
	// Retrieve the expected result
	RPointerArray<cacheData> expResultData;
	iInputParsedData->ResultData(expResultData);
	
	// If the expected search result has more than one element, then
	// it is an error since we have only one input string to be searched.
	if(expResultData.Count() > 1)
	{
		iReturnStatus = KErrArgument;
	}
	
	// Perform the comparison of matched sequences
	TInt expResCount = 0;
	if(expResultData.Count())
	{
		expResCount = expResultData[0]->data.Count();
	}
	else
	{
		expResCount = 0;
	}

	// compare the obtained result with desired result set
	if( (matchSet->Count() == expResCount) && (KErrNone == iReturnStatus) )
	{
		TInt totalMatches = 0;
		for(TInt i = 0; i < matchSet->Count(); i++)	
		{
			for(TInt j = 0; j < expResultData[0]->data.Count(); j++)
			{
				HBufC* temp1 = ((*matchSet)[i]).AllocL();
				HBufC* temp2 = (*(expResultData[0]->data[j])).AllocL();
				temp1->Des().LowerCase();
				temp2->Des().LowerCase();
				if((*temp1).Compare(*temp2) == 0 )
				{
					totalMatches++;
					expResultData[0]->data.Remove(j);
					break;
				} 
			}
		}
		
		// If the totalmatches == total number of results in match set,
		// then testcase is successful
		if(matchSet->Count() == totalMatches)
		{
			iReturnStatus = KErrNone;
		}
		else
		{
			iReturnStatus = KErrArgument;
		}
	}
	else
	{
		iReturnStatus = KErrArgument;
	}
	
	//compare match Locations
	const RArray<TPsMatchLocation>& expectedLocations = iInputParsedData->ExpectedLocations();
	if ( expectedLocations.Count() != matchLocation.Count() )
	    {
	    iReturnStatus = KErrArgument; //matchLocations differs from expected result
	    }
	for ( TInt i(0); i < expectedLocations.Count() && KErrNone == iReturnStatus; ++i )
	    {
	    if ( (expectedLocations[i].index != matchLocation[i].index) ||
	         (expectedLocations[i].length != matchLocation[i].length)   )
	        {
	        iReturnStatus = KErrArgument; //matchLocations differs from expected result
	        }
	    }
	
	matchLocation.Close();
	matchLocation.Reset();
	matchSet->Reset();
	
	// return error code
    return iReturnStatus;
}


// -----------------------------------------------------------------------------
// Searches for a query in input string.
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::SearchLookupMatchStringL(CStifItemParser& aItem)
{
    RTimerWait(KOneSecDelay);
    iReturnStatus = KErrNone;
    
    // Create the input parser
    iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
    
    // Parse the input data
    iInputParsedData->ParseInputL(aItem);
    
    //Get the input string
    HBufC* inputStr = iInputParsedData->InputSearchString().AllocL();
    
    // Create the search query
    iInputParsedData->CreateSearchQueryL(*iPsQuery,iInputParsedData->Query());
    
    TBuf<KPsQueryMaxLen> result;
    //Wait for one second
    RTimerWait(KOneSecDelay);
        
    //Perform the search
    iPsClientHandler->LookupMatchL(*iPsQuery, *inputStr, result);
    
    if ( iInputParsedData->ExpectedResultString() != result )
    {
        iReturnStatus = KErrArgument;
    }

    return iReturnStatus;
}

// -----------------------------------------------------------------------------
// Stores the caching status of the psengine
// -----------------------------------------------------------------------------
void CPsClientTestSuite::CachingStatus(TCachingStatus& aStatus, TInt& /*aError*/)
{
	// implement later
	iCachingStatus = aStatus ;
}

// -----------------------------------------------------------------------------
// Handle the error from the ps engine
// -----------------------------------------------------------------------------
void CPsClientTestSuite::HandlePsError(TInt /*aErrorCode*/)
{
}

// -----------------------------------------------------------------------------
// This function introduces a delay of the specified time in microseconds
// -----------------------------------------------------------------------------
void CPsClientTestSuite::RTimerWait(TTimeIntervalMicroSeconds32 aInterval)
{
	TRequestStatus timerStatus;
	iTimer.Cancel();
	iTimer.After( timerStatus, aInterval); 
	User::WaitForRequest( timerStatus );
}

// -----------------------------------------------------------------------------
// Delete all contacts from the phonebook
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::DeleteAllContactsInPhonebookL(CStifItemParser& aItem)
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrGeneral;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;	
  	
  	// Perform the search settings
	iInputParsedData->PerformSearchSettingsL(*iSettings);
	
	// set search settings
	iPsClientHandler->SetSearchSettingsL(*iSettings); 
	
	//Wait for one second
	RTimerWait(KOneSecDelay);
	
	// Get all the contents and delete them
	iCurrentOperation = EDeleteSearchedResults;
	iPsClientHandler->GetAllContentsL();
	CActiveScheduler::Start();
    iReturnStatus = KErrNone;
    
	// return error code
	return iReturnStatus;
}

// -----------------------------------------------------------------------------
// Checks the caching status of the psengine
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::CheckCachingStatusL()
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrGeneral;
	TCachingStatus cachingStatus;
	
	// We keep checking the caching status every second.
	// Loop every second 100 times. If still caching is not complete,
	// then there is some issue (Ideally the cahing should finish within a 
	// few seconds for 10-20 contacts.)
	for(TInt i=0; i<100; i++)
	{
		iPsClientHandler->GetCachingStatusL(cachingStatus);
		if(cachingStatus == ECachingComplete)
		{
			iReturnStatus = KErrNone;
			break;
		}
		
		//Wait for one second
		RTimerWait(KOneSecDelay);
	}

	// Return the status
	return iReturnStatus;
}

// -----------------------------------------------------------------------------
// This test case performs a check for the version.
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::CheckVersion()
{
	RTimerWait(KOneSecDelay);
	TInt result = KErrNotFound;
	
	// Get the version from the ps engine
	TVersion ver = iPsClientHandler->Version(); // ver contains the version info of server
	
	TInt major = ver.iMajor; // the major version number
	TInt minor = ver.iMinor; // the minor version number
	TInt build = ver.iBuild; // the build number
	
	// compare the obtained value with the required value
	if(major == 1 && minor == 0 && build == 0)
	{
		result = KErrNone;
	}
	
	return result;  // return error code
}

// -----------------------------------------------------------------------------
// Tests the language support
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::TestIsLanguageSupportedL(CStifItemParser& aItem)
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrGeneral;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;	
  	
  	// Compare with expected result
	if(iInputParsedData->LanguageInputExpectedResult() == iPsClientHandler->IsLanguageSupportedL(iInputParsedData->LanguageInput()))
	{
		iReturnStatus =  KErrNone;
	}
	else
	{
		iReturnStatus =  KErrGeneral;
	
	}
	
	return iReturnStatus;
}

// -----------------------------------------------------------------------------
// Gets all contacts from the phonebook
// -----------------------------------------------------------------------------

TInt CPsClientTestSuite::GetAllContentsL( CStifItemParser& aItem )
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrGeneral;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;	
  	
	// Perform the search settings
	iInputParsedData->PerformSearchSettingsL(*iSettings);
	
	// set search settings
	iPsClientHandler->SetSearchSettingsL(*iSettings); 
	
	//Wait for one second
	RTimerWait(KOneSecDelay);
	iPsClientHandler->GetAllContentsL();
	CActiveScheduler::Start();
   
	// return error code
    return iReturnStatus;


}

// -----------------------------------------------------------------------------
// Tests the sort order of the result data
// -----------------------------------------------------------------------------

TInt CPsClientTestSuite::TestSortOrderL( CStifItemParser& aItem )
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrNone;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;
	
	//If more than one cacheuris are configured, then it is an error
	HBufC* datastore = HBufC::NewL(50);
	if(iInputParsedData->CacheUrisCount() ==1)
	{
		datastore->Des().Copy(iInputParsedData->CacheUris(0)); 
	}
	else
	{
		User::Leave(KErrArgument);
	}
	
	// Get the config file sort order
	RArray<TInt> inputSortOrder;
	iInputParsedData->SortOrder(inputSortOrder);
	
	// If there are no elements in the sort order, then the existing sort order is 
	// used to set the sort order. This is added for test coverage
	if(inputSortOrder.Count() == 0 )
	{
		iPsClientHandler->GetSortOrderL(*datastore,inputSortOrder);
	}
	
	//Set the sort order
	iPsClientHandler->ChangeSortOrderL(*datastore,inputSortOrder);
	
	RArray<TInt> newSortOrder;
	// Get the sort order
	iPsClientHandler->GetSortOrderL(*datastore,newSortOrder);
	if(inputSortOrder.Count() != newSortOrder.Count())
	{
		iReturnStatus = KErrArgument;
		delete datastore;
		datastore = NULL;
		return iReturnStatus;
	}
	for(TInt j =0; j < inputSortOrder.Count() ; j++ )
	{
		if(inputSortOrder[j] != newSortOrder[j] )
			iReturnStatus = KErrArgument;
	}
	
	delete datastore;
	datastore = NULL;
	return iReturnStatus;
}

// -----------------------------------------------------------------------------
// Tests the sort order for error case
// -----------------------------------------------------------------------------

TInt CPsClientTestSuite::TestSortOrderErrCaseL( CStifItemParser& aItem )
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrNone;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;
	
	//If more than one cacheuris are configured, then it is an error
	HBufC* datastore = HBufC::NewL(50);
	if(iInputParsedData->CacheUrisCount() ==1)
	{
		datastore->Des().Copy(iInputParsedData->CacheUris(0)); 
	}
	else
	{
		User::Leave(KErrArgument);
	}
	
	// Get the config file sort order
	RArray<TInt> inputSortOrder;
	iInputParsedData->SortOrder(inputSortOrder);
	
	
	//Set the sort order
	iPsClientHandler->ChangeSortOrderL(*datastore,inputSortOrder);
	
	RArray<TInt> newSortOrder;
	// Get the sort order
	iPsClientHandler->GetSortOrderL(*datastore,newSortOrder);
	TChar equal = '=';
	TInt pos = datastore->Des().Locate(equal);
	if(pos != KErrNotFound)
	{
		HBufC* temp = datastore->Des().Left(pos + 1).AllocL();
		if(temp->Des().Compare(groupformat) == 0 )
		{
			if(newSortOrder.Count() > 0)
			{
				iReturnStatus = KErrNone;
			}
			else
			{
				iReturnStatus = KErrArgument;
			}
		
			delete temp; temp = NULL;
	
		}
	}
	else
	{
		if(newSortOrder.Count() ==  0)
		{
			iReturnStatus = KErrNone;
		}
		else
		{
			iReturnStatus = KErrArgument;
		}
	}
	delete datastore;
	datastore = NULL;

	return iReturnStatus;
}

// -----------------------------------------------------------------------------
// Searches within a particular group.
// First get the group id from the group name. Then perform a search on the group
// -----------------------------------------------------------------------------

TInt CPsClientTestSuite::SearchWithInGroupL(CStifItemParser& aItem )
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrGeneral;
	
	iGroupToBeSearchedId =0;
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;
  	
  	// Search on the groups cache to get the group id of the input group
  	HBufC* grpDbUri = HBufC::NewL(50);
  	grpDbUri->Des().Copy(KVPbkDefaultGrpDbURI);
  	
  	RPointerArray<TDesC> cacheUris;
  	cacheUris.AppendL(grpDbUri);
  	iInputParsedData->ApendCacheUrisL(cacheUris);
  	
  	// Perform the search settings
  	iInputParsedData->PerformSearchSettingsL(*iSettings);
	
	// set search settings
	iPsClientHandler->SetSearchSettingsL(*iSettings); 
	

	// Create the search query
	iInputParsedData->CreateSearchQueryL(*iPsQuery, iInputParsedData->GroupToBeSearched());
	iCurrentOperation = EGetGroupId;
	
	//Wait for one second
	RTimerWait(KOneSecDelay);
	
	//Perform the search
	iPsClientHandler->SearchL(*iPsQuery);
	CActiveScheduler::Start();
	
	// Now the group Id is available, so create the search query.
    // the group id is available in iGroupToBeSearchedId.
    // Create a search uri using the group id
	HBufC* grpSpecificUri = HBufC::NewL(50);
  	
	grpSpecificUri->Des().Copy(groupformat);
  	grpSpecificUri->Des().AppendNum(iGroupToBeSearchedId);
  	
  	// Set the search group uri
  	RPointerArray<TDesC> grpCacheUris;
  	grpCacheUris.Append(grpSpecificUri);
  	iInputParsedData->ApendCacheUrisL(grpCacheUris);
  	
  	// Perform the search settings
  	iInputParsedData->PerformSearchSettingsL(*iSettings);
	
	// set search settings
	iPsClientHandler->SetSearchSettingsL(*iSettings); 

	// Create the search query
	if(iPsQuery)
	{
		delete iPsQuery;
		iPsQuery = CPsQuery::NewL();
	}
	iInputParsedData->CreateSearchQueryL(*iPsQuery, iInputParsedData->Query());
	iCurrentOperation = EDefaultCase;
	
	//Wait for one second
	RTimerWait(KOneSecDelay);
	
	//Perform the search
	iPsClientHandler->SearchL(*iPsQuery);
	CActiveScheduler::Start();
   
   // return the result
   return iReturnStatus;
}

// -----------------------------------------------------------------------------
// This test case removes the observer for CPSRequestHandler
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::RemoveObserverL(CStifItemParser& /*aItem*/)
{
	RTimerWait(KOneSecDelay);
 	TInt ret1 = KErrGeneral;
 	TInt ret2 = KErrGeneral;
 	
	//Remove the observer
	ret1 = iPsClientHandler->RemoveObserver(this); 
	ret2 = iPsClientHandler->RemoveObserver(this); 
	if((ret1 == KErrNone)&& (ret2== KErrNotFound))
		iReturnStatus = KErrNone;
	else
	    iReturnStatus = KErrGeneral;
	
	//return the result
	return iReturnStatus;
}

// -----------------------------------------------------------------------------
// This test case converts and Id to vpbk link
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::ConvertToVpbkLinkL(CStifItemParser& aItem)
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrNone;
	iCurrentOperation = EConvertToVpbkLink;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;	
  	
  	// Perform the search settings
	iInputParsedData->PerformSearchSettingsL(*iSettings);
	
	// set search settings
	iPsClientHandler->SetSearchSettingsL(*iSettings); 
	
	//Get all the contents of the contacts
	iPsClientHandler->GetAllContentsL();
	CActiveScheduler::Start();
   
   	return iReturnStatus;
}

// -----------------------------------------------------------------------------
// This test case checks addition of marked contacts
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::AddMarkedContactsTestL(CStifItemParser& aItem)
{
	RTimerWait(KOneSecDelay);
	iReturnStatus = KErrGeneral;
	iCurrentOperation = EMarkedContacts;
	
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;	
  	
	// Perform the search settings
	iInputParsedData->PerformSearchSettingsL(*iSettings);
	
	// set search settings
	iPsClientHandler->SetSearchSettingsL(*iSettings); 
	
	//Wait for one second
	RTimerWait(KOneSecDelay);
	iPsClientHandler->GetAllContentsL();
	CActiveScheduler::Start();
	
	//Now fire normal search query
	iCurrentOperation = EDefaultCase;
	// Create the search query 
	iInputParsedData->CreateSearchQueryL(*iPsQuery,iInputParsedData->Query());
	
	//Wait for one second
	RTimerWait(KOneSecDelay);
	
	//Perform the search
	iPsClientHandler->SearchL(*iPsQuery,iMarkedContacts,NULL);
	CActiveScheduler::Start();
   return iReturnStatus;
}

// -----------------------------------------------------------------------------
// This test case tests the data order. This case cannot be checked from test 
// suite. Added here for code coverage
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::TestDataOrderL(CStifItemParser& aItem)
{
	RTimerWait(KOneSecDelay);
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;	
  	
	//If more than one cacheuris are configured, then it is an error
	HBufC* datastore = HBufC::NewL(50);
	if(iInputParsedData->CacheUrisCount() ==1)
	{
		datastore->Des().Copy(iInputParsedData->CacheUris(0)); 
	}
	else
	{
		User::Leave(KErrArgument);
	}
	
	// Get the data order
	RArray<TInt> myDataOrder;
	iPsClientHandler->GetDataOrderL(*datastore, myDataOrder);
	
	if(myDataOrder.Count() > 0)
	{
		iReturnStatus = KErrNone;
	}
	else
	{
		iReturnStatus = KErrGeneral;
	}
	
	delete datastore;
	datastore = NULL;
	//Return the error code
	return iReturnStatus;
}

// -----------------------------------------------------------------------------
// This test case tests the data order. This case cannot be checked from test 
// suite. Added here for code coverage
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::TestDataOrderForErrL(CStifItemParser& aItem)
{
	RTimerWait(KOneSecDelay);
	// Create the input parser
	iInputParsedData = CTestSuiteInputData::NewL(aItem,*iPsClientHandler);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;	
  	
	//If more than one cacheuris are configured, then it is an error
	HBufC* datastore = HBufC::NewL(50);
	if(iInputParsedData->CacheUrisCount() ==1)
	{
		datastore->Des().Copy(iInputParsedData->CacheUris(0)); 
	}
	else
	{
		User::Leave(KErrArgument);
	}
	
	// Get the data order
	RArray<TInt> myDataOrder;
	iPsClientHandler->GetDataOrderL(*datastore, myDataOrder);
	
	if(myDataOrder.Count() == 0)
	{
		iReturnStatus = KErrNone;
	}
	else
	{
		iReturnStatus = KErrGeneral;
	}
	
	delete datastore;
	datastore = NULL;
	//Return the error code
	return iReturnStatus;
}

// -----------------------------------------------------------------------------
// This test case performs shut down of the server
// -----------------------------------------------------------------------------
TInt CPsClientTestSuite::ShutDownL()
{
	//Wait for one second
	RTimerWait(KOneSecDelay);  // wait for one sec
    
    // Shut down the server
    iPsClientHandler->ShutdownServerL();  // shutdown predictive search engine
    
    // return the result
	return KErrNone;	
}

//End of files
