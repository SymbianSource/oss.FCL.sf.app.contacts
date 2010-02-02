/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES
#include <StifParser.h>
#include <CPsQueryItem.h>
#include "TestSuiteInputData.h"
#include "pstestcontactshandler.h"
#include "psclienttestsuitedefs.h"
#include <vpbkeng.rsg>

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// Two Phase constructor
// ----------------------------------------------------------------------------
CTestSuiteInputData* CTestSuiteInputData::NewL(CStifItemParser& aItem, CPSRequestHandler& aPsClientHandler)
{
	CTestSuiteInputData* self = new (ELeave) CTestSuiteInputData();	
    CleanupStack::PushL(self);
    self->ConstructL(aItem, aPsClientHandler);
    CleanupStack::Pop();
    return self;
}

// ----------------------------------------------------------------------------
// Second phase construction
// ----------------------------------------------------------------------------
void CTestSuiteInputData::ConstructL(CStifItemParser& /*aItem*/,CPSRequestHandler& aPsClientHandler)
{
    iContactHandler = CPcsTestContactsHandler::NewL();
    iPsClientHandler = &aPsClientHandler;
    iMaxNumberOfResults = 100;
    iSortType = EAlphabetical;
}

// ----------------------------------------------------------------------------
// Default constructor
// ----------------------------------------------------------------------------
CTestSuiteInputData::CTestSuiteInputData()
{
}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
CTestSuiteInputData::~CTestSuiteInputData()
{
    delete iQueryBuf;
    delete iKeyboardModeBuf;
	delete iContactHandler;
	delete iInputSearchString;
	delete iExpectedResultString;
	delete iGroupToBeSearched;
	
	iCacheUris.ResetAndDestroy();
	iResultData.ResetAndDestroy();
	iInputCacheData.ResetAndDestroy();
	iDisplayFields.Close();
	iSortOrder.Close();
	iExpectedLocations.Close();
}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
TDesC& CTestSuiteInputData::KeyboardMode() const
{
	return *iKeyboardModeBuf;
}

// ----------------------------------------------------------------------------
// Sets the keyboard mode
// ----------------------------------------------------------------------------
void CTestSuiteInputData::SetKeyboardModeL(const TDesC& aKeyboardMode)
{
	iKeyboardModeBuf = aKeyboardMode.AllocL();
}

// ----------------------------------------------------------------------------
// Returns the search query (as specified in the config file)
// ----------------------------------------------------------------------------
TDesC& CTestSuiteInputData::Query() const
{
	return *iQueryBuf;
}

// ----------------------------------------------------------------------------
// Returns the Input string (as specified in the config file)
// ----------------------------------------------------------------------------
TDesC& CTestSuiteInputData::InputSearchString() const
{
	return *iInputSearchString;
}

// ----------------------------------------------------------------------------
// Returns the Input string (as specified in the config file)
// ----------------------------------------------------------------------------
const TDesC& CTestSuiteInputData::ExpectedResultString() const
{
    return *iExpectedResultString;
}

// ----------------------------------------------------------------------------
// Sets the search query
// 
// ----------------------------------------------------------------------------
void CTestSuiteInputData::SetQueryL(const TDesC& aQuery)
{
	iQueryBuf = aQuery.AllocL();
}

// ----------------------------------------------------------------------------
// Returns the input cache data
// ----------------------------------------------------------------------------
void CTestSuiteInputData::InputCacheData(TInt aIndex, cacheData& aInputCacheData) const
{
	if(aIndex >= iInputCacheData.Count())
	{
		return;
	}
	
	aInputCacheData = *(iInputCacheData[aIndex]);
}

// ----------------------------------------------------------------------------
// Appends the cachedata to results data 
// ----------------------------------------------------------------------------
void CTestSuiteInputData::AppendResultDataL(const cacheData& aResultData)
{
	iResultData.Append(&aResultData);
}

// ----------------------------------------------------------------------------
// Returns the count for inputcachedata
// ----------------------------------------------------------------------------
TInt CTestSuiteInputData::InputCacheDataCount() const
{
	return( iInputCacheData.Count() );
}

// ----------------------------------------------------------------------------
// Returns the expected results array 
// ----------------------------------------------------------------------------
void CTestSuiteInputData::ResultData(RPointerArray<cacheData>& aResultData) const
{
	for(TInt i=0; i<iResultData.Count(); i++ )
	{
		aResultData.Append(iResultData[i]);
	}
}

// ----------------------------------------------------------------------------
// Returns the sort Type
// ----------------------------------------------------------------------------
TSortType CTestSuiteInputData::GetSortType()
{
	return iSortType;
}

// ----------------------------------------------------------------------------
// Appends the cachedata to cache data 
// ----------------------------------------------------------------------------
void CTestSuiteInputData::AppendInputCacheDataL(const cacheData& aInputCacheData)
{
	iInputCacheData.Append(&aInputCacheData);
}

// ----------------------------------------------------------------------------
// Returns the count of the expected result data
// 
// ----------------------------------------------------------------------------
TInt CTestSuiteInputData::ResultDataCount() const
{
	return( iResultData.Count() );
}

// ----------------------------------------------------------------------------
// Return the cache uri at given index 
// ----------------------------------------------------------------------------
TDesC& CTestSuiteInputData::CacheUris(TInt aIndex) const
{
	if ( aIndex >= iCacheUris.Count() ) 
	{
		//return NULL;
	}
         
    return *(iCacheUris[aIndex]);
}

// ----------------------------------------------------------------------------
// Sets the cache uris
// ----------------------------------------------------------------------------
void CTestSuiteInputData::ApendCacheUrisL(RPointerArray<TDesC>& aCacheUriArray)
{   iCacheUris.ResetAndDestroy();
	for(TInt i =0; i <aCacheUriArray.Count(); i++ ) 
    	iCacheUris.Append(aCacheUriArray[i]);
}

// ----------------------------------------------------------------------------
// Returns the count of the cacheuris
// 
// ----------------------------------------------------------------------------
TInt CTestSuiteInputData::CacheUrisCount() const
{
	return( iCacheUris.Count() );
}

// -----------------------------------------------------------------------------
// This function parses the input parameters
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputL(CStifItemParser& aItem)
{
	TPtrC string;

   	// parsing type to be set before parsing a string 
	// EQuoteStyleParsing: information between quotes is handled as a one string. 
	aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
	
	// loop till there is a token in i/p string
	while ( aItem.GetNextString ( string ) == KErrNone )
	{
		HBufC *item = string.AllocL();
		TChar sqBracketOpen = '[';
		TChar sqBracketClose = ']';
		
		TInt sqBracketStartPos = string.Locate(sqBracketOpen);
		TInt sqBracketEndPos = string.Locate(sqBracketClose);
		
		if ( (sqBracketStartPos!=KErrNotFound) && (sqBracketEndPos!=KErrNotFound) )
		{
			TPtrC16 optionPtr = string.Left(sqBracketStartPos);	
				
			TPtrC16 dataPtr = string.Mid(sqBracketStartPos+1, sqBracketEndPos-sqBracketStartPos-1);	
			
			// Check the type of tag, then do the corresponding parsing
            if(optionPtr.Compare(KCreateContacts) == 0)
            {
                // Contacts to be created
                ParseInputForContactsDataL(dataPtr,0);
            }
			else if(optionPtr.Compare(KSearchQueryTag) == 0)
		    {
		    	// Search query
		    	ParseInputForSearchQueryL(dataPtr);
		    
		    }
		    else if(optionPtr.Compare(KUriTag) == 0)
		    {
		    	// Data store uris
		    	ParseInputForDataStoresUriL(dataPtr);
		    }
		    else if(optionPtr.Compare(KSearchExpectedResult) == 0)
		    {
		    	// Expected results
		    	ParseInputForContactsDataL(dataPtr,1);
		    }
            else if(optionPtr.Compare(KSearchExpectedMatchLocations) == 0)
            {
                // Expected match locations result
                ParseInputForMatchLocationsL(dataPtr);
            }		    
            
		    else if(optionPtr.Compare(KSetDisplayFields) == 0)
		    {
		    	// Display fields
		    	ParseInputForDisplayFieldsL(dataPtr);
		    }
		    else if(optionPtr.Compare(KSearchOnInputString) == 0)
		    {
		    	// Input search string
		    	ParseInputInputSearchStringL(dataPtr);
		    }
            else if(optionPtr.Compare(KExpectedResultString) == 0)
            {
                // Expected result string
                ParseInputExpectedResultStringL(dataPtr);
            }
		    else if(optionPtr.Compare(KSearchExpectedResultForInput) == 0)
		    {
		    	// Expected results for input string search
				ParseInputForContactsDataL(dataPtr,1);
		    }
		    else if(optionPtr.Compare(KSupprotedLanguage) == 0)
		    {
		    	// Languages supported
		    	ParseInputForLanguageSupportL(dataPtr);
		    }
		    else if(optionPtr.Compare(KAddGroup) == 0)
		    {
		    	// Add groups to phonebook
		    	ParseInputForAddingGroupsL(dataPtr);
		    
		    }
		    else if(optionPtr.Compare(KSearchInGroup) == 0)
		    {
		    	// Search within a particular group
		    	ParseInputForSearchWithinGroupL(dataPtr);
		    
		    }
			 else if(optionPtr.Compare(KSortOrderTag) == 0)
		    {
		    	// Search within a particular group
		    	ParseInputForSortOrderL(dataPtr);
		    }	  
		}
		else
		{
			// Wrong Input..
			User::Leave(KErrGeneral);
		}
	}
}

// -----------------------------------------------------------------------------
// Returned parsed expected result locations
// -----------------------------------------------------------------------------
const RArray<TPsMatchLocation>& CTestSuiteInputData::ExpectedLocations()
    {
    return iExpectedLocations;
    }

// -----------------------------------------------------------------------------
// Parses the input parameters for contacts data for creating contacts in 
// the phonebook
// -----------------------------------------------------------------------------
			
void CTestSuiteInputData::ParseInputForContactsDataL(TDesC& aInput,TInt aType)
{
	TChar comma = ',';
	TChar bracketOpen = '(';
	TChar bracketClose = ')';
	TInt commaPos = aInput.Locate(comma);
	TInt numOfContacts = 0;
	
	if(commaPos !=KErrNotFound )
	{
		TPtrC temp = aInput.Left(commaPos);
		TLex num(temp);
		TInt err = num.Val(numOfContacts);  // parse the string to extract integer value
			
		TInt startofBracket = aInput.Locate(bracketOpen);
		for(TInt i =0; i<numOfContacts; i++ )
		{
		    TInt j = startofBracket;
			
			// Get the data for each contact
			while(aInput[j] != bracketClose )
			    j++;
			     
			TPtrC16 dataPtr = aInput.Mid(startofBracket + 1,j - 1 - startofBracket );	
			
			AddSingleContactDataL(dataPtr,aType);
		    if(i<numOfContacts - 1 )
		    {
                while(aInput[j] != bracketOpen )
			    j++;
		
				startofBracket = j;
		    }
		}
	}
}

// -----------------------------------------------------------------------------
// This function adds single contacts data
// if (aType),
//    then it is added to  the iResultData
// else
// 	  it is added to  the iInputCacheData
// -----------------------------------------------------------------------------

void CTestSuiteInputData::AddSingleContactDataL(TDesC& aInput, TInt aType)
{
	TChar comma = ',';
	TChar colon = ':';
	TInt commaPos = -1 ;
	cacheData* tempcacheData = new (ELeave) cacheData();
	
    TInt arrCount = iInputCacheData.Count();	
	for (TInt j = 0 ; j< aInput.Length();)
	{
        TInt Startpos = commaPos + 1;

        while(aInput[j] != colon)
            j++;
        TPtrC16 vpbkfielddataPtr = aInput.Mid(Startpos ,j - Startpos);
			
        // Get the corresponding integer value			
        TInt fieldId  =  GetFieldIdFromVpbkIdentifierL(vpbkfielddataPtr);

        tempcacheData->vpbkId.Append(fieldId);

        Startpos = j + 1;   

        while ( (j < aInput.Length()) && (aInput[j] != comma) )
            j++;

        TPtrC16 dataPtr = aInput.Mid(Startpos ,j - Startpos);	

        // Add data to the class
        tempcacheData->data.Append(dataPtr.AllocL());

        commaPos = j ;  
        j++;   
	}

	// Check and add to appropriate data structure
	if(aType)
    {
        iResultData.Append(tempcacheData);
    }
	else
	{
        iInputCacheData.Append(tempcacheData);
	}
}

// -----------------------------------------------------------------------------
// Parses the input parameters for result match locations
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForMatchLocationsL(TDesC& aInput)
    {
    const TChar KComma(',');
    const TChar KColon(':');
	
    if ( 0 == aInput.Length() )
        {
        return;
        }
    TLex lex(aInput);
    do
        {
        TInt index;
        TInt length;
        User::LeaveIfError( lex.Val( index ) );
        if ( KColon != lex.Get() )
            {
            User::Leave( KErrArgument );
            }
        User::LeaveIfError( lex.Val( length ) );
        TPsMatchLocation newLocation = {index, length, TBidiText::ELeftToRight }; 
        iExpectedLocations.AppendL( newLocation );
        } while ( KComma == lex.Get() );
    }

// -----------------------------------------------------------------------------
// This function returns the vpbk id corresponding to config file strings
// -----------------------------------------------------------------------------

TInt CTestSuiteInputData::GetFieldIdFromVpbkIdentifierL(TDesC& aIdentifierAsString)
{
	if(aIdentifierAsString.Compare(KFirstName) == 0)
	{
	 	return R_VPBK_FIELD_TYPE_FIRSTNAME;
	}
	else if(aIdentifierAsString.Compare(KLastName) == 0)
	{
		return R_VPBK_FIELD_TYPE_LASTNAME;
	}
	else if(aIdentifierAsString.Compare(KCompanyName) == 0)
	{
		return R_VPBK_FIELD_TYPE_COMPANYNAME;
	}
	
	return -1;
}

// -----------------------------------------------------------------------------
// This function parses input for cache uris
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForDataStoresUriL(TDesC& aInput)
{
	RPointerArray<TDesC> Uris;
	TChar separator = ',';
	
	// Get the uris which are separated by commas
	GetCharacterSeparatedDataL( aInput,separator, Uris);
	for(TInt i = 0; i< Uris.Count(); i++)
	{
        HBufC *temp = Uris[i]->AllocL();
        iCacheUris.Append(Uris[i]);
	}
}

// -----------------------------------------------------------------------------
// This function parses the input for display fields
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForDisplayFieldsL(TDesC& aInput)
{
	RPointerArray<TDesC> dispFieldsBuf;
	
	TChar separator = ',';
	GetCharacterSeparatedDataL(aInput, separator, dispFieldsBuf);
	for (TInt i = 0; i< dispFieldsBuf.Count(); i++)
	{
		TInt fieldId = 0;
		TLex num(*(dispFieldsBuf[i]));
		
		TInt err = num.Val(fieldId); // parse the string to extract integer value
		if (err != KErrNone)
		{
            User::Leave(KErrGeneral);
		}
		else
		{
            iDisplayFields.Append(fieldId);
		}
	}
}

// -----------------------------------------------------------------------------
// This function splits the the input separated by particular character
// -----------------------------------------------------------------------------
void CTestSuiteInputData::GetCharacterSeparatedDataL(TDesC& aInput, TChar aSeparator, RPointerArray<TDesC> &aSeparatedData)
{
	TInt SeparatorPos = -1 ;
    for (TInt j = 0 ; j< aInput.Length();)
	{
        TInt Startpos = SeparatorPos + 1;
		// Get the data for each contact
        while ( (j < aInput.Length()) && (aInput[j] != aSeparator) )
            j++;
			      
        TPtrC16 dataPtr = aInput.Mid(Startpos ,j - Startpos);	
			
        // Add data to the output
        HBufC* tempData = dataPtr.AllocL();
        aSeparatedData.Append(tempData);
        SeparatorPos = j;  
        j++;   
	}
}

// -----------------------------------------------------------------------------
// This function parses the input for search query
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForSearchQueryL(TDesC& aInput)
{
	RPointerArray<TDesC> queryData;
	TChar separator = ',';
	GetCharacterSeparatedDataL( aInput,separator, queryData);

    // If number of fields != 4 , then it is an error
    if(queryData.Count() != 4)
    {
    	queryData.ResetAndDestroy();
     	User::Leave(KErrArgument);
    }
    else
    {
    	iQueryBuf = queryData[0]->AllocL();
    	iKeyboardModeBuf = queryData[1]->AllocL();
    	
    	if(queryData[2]->Compare(KPatternSort) == 0)
    	{
    		iSortType = EPatternBased;
    	}
    	else if(queryData[2]->Compare(KAlphaSort) == 0) 
    	{
       		iSortType = EAlphabetical;
    	}
    	else
    	{
    		queryData.ResetAndDestroy();
    		User::Leave(KErrArgument);
    	}
    	
    	// Number of results
    	iMaxNumberOfResults = 0;
	   	TLex num(*(queryData[3]));
		TInt err = num.Val(iMaxNumberOfResults);
		
		if( KErrNone != err)
		{
			queryData.ResetAndDestroy();
     		User::Leave(KErrArgument);
		}
	    queryData.ResetAndDestroy();
    }
}

// -----------------------------------------------------------------------------
// This function creates contacts (contacts data is taken from  iInputCacheData
// -----------------------------------------------------------------------------
void  CTestSuiteInputData::CreateContactsL()
{
	for(TInt i =0 ; i <iInputCacheData.Count(); i++ )
    {
     	// create the specified contact 
		iContactHandler->CreateOneContactL(*(iInputCacheData[i]));
		CActiveScheduler::Start();
    }
}

// -----------------------------------------------------------------------------
// This function creates the search query from the input buffer
// -----------------------------------------------------------------------------
void CTestSuiteInputData::CreateSearchQueryL(CPsQuery& aPsQuery, TDesC& aQueryBuf)
{
	TKeyboardModes keyboardMode = EModeUndefined;

    // Check the keyboard mode
	if(iKeyboardModeBuf->Des().Compare(KItut) == 0)
	{
	    keyboardMode = EItut;
	}
	else if (iKeyboardModeBuf->Des().Compare(KQwerty) == 0)
	{
	    keyboardMode = EQwerty;
	}

	else // mixed mode
	{
		// convert the search query to the required format
		_LIT(KCode,"i");
		for(TInt i = 0; i < aQueryBuf.Length(); i++)
		{
			CPsQueryItem* item = CPsQueryItem::NewL();
	    	item->SetCharacter(aQueryBuf[i]); // set search character
	    	
	    	// set the keyboard mode
	    	if( (iKeyboardModeBuf->Des().Mid(i,1)).Compare(KCode) == 0)
 			{
 				item->SetMode(EItut);
 			}
 			else 
 			{
 				item->SetMode(EQwerty);
 			}
 			
 			aPsQuery.AppendL(*item);
		}
		
		return ;
	}
	
	// create the query as CPsQuery object
	// for EItut and EQwerty mode
	for(TInt j = 0; j < aQueryBuf.Length(); j++)
	{
		CPsQueryItem* item1 = CPsQueryItem::NewL();
	    item1->SetCharacter(aQueryBuf[j]);
	    item1->SetMode(keyboardMode);
	    aPsQuery.AppendL(*item1);
	}
}

// -----------------------------------------------------------------------------
// Perform the search settings
// -----------------------------------------------------------------------------

void CTestSuiteInputData::PerformSearchSettingsL(CPsSettings& aSettings)
{
	RPointerArray<TDesC> databases; // list of databases
	RArray<TInt> displayFields;     // list of display fields

	// add the display fields
    if(iDisplayFields.Count())
    {
		for (TInt i=0; i<iDisplayFields.Count(); i++)
		    displayFields.Append(iDisplayFields[i]);
    }
    else
    {
	    displayFields.Append(R_VPBK_FIELD_TYPE_FIRSTNAME); // Firstname
	    displayFields.Append(R_VPBK_FIELD_TYPE_LASTNAME); // Lastname
	    displayFields.Append(R_VPBK_FIELD_TYPE_COMPANYNAME); // Company name
    }

    // Set the databases to be searched 
	for(TInt i =0; i <iCacheUris.Count(); i++ )
	{
		databases.Append(iCacheUris[i]->AllocL());
	}
	
	// Perform the settings
	aSettings.SetSearchUrisL(databases);
    aSettings.SetDisplayFieldsL(displayFields);
    aSettings.SetMaxResults(iMaxNumberOfResults);
	
	// Set sort type
	aSettings.SetSortType(iSortType);

	// cleanup
	displayFields.Close();
	databases.Reset(); 
	databases.Close(); 
}

// -----------------------------------------------------------------------------
// This function compares the searchResults with expected results
// The expected results are stored in iResultData
// -----------------------------------------------------------------------------

TInt CTestSuiteInputData::CompareResWithExpResL(RPointerArray<CPsClientData>& searchResults)
{
	TInt returnStatus = KErrNone;
	
	if(iResultData.Count() != searchResults.Count() )
	{
		returnStatus = KErrGeneral;
	}
	else
	{
		TInt totalMatches = 0;
		for(TInt expResCnt=0; expResCnt < iResultData.Count();  expResCnt++ )
		{
			for(TInt resCnt=0; resCnt < searchResults.Count();  resCnt++ )
			{
			    CPsClientData* tempPsData = searchResults[resCnt];
			    TBool comapreRes = CompareInputDataWithResultDataL(*tempPsData,*(iResultData[expResCnt])); 
				if(	comapreRes )
				{
					totalMatches++;
				}
			}
		}	
		
		// If totalMatches === searchResults.Count(), then the match exists
		if(totalMatches != searchResults.Count() )
		{
			returnStatus = KErrGeneral;
		}
		else
		{
			returnStatus = KErrNone;
		}
	}
	
	return returnStatus;
}

TInt CTestSuiteInputData::ExactMatchCompareInputDataWithResultDataL(RPointerArray<CPsClientData>& searchResults)
{
	TBool returnStatus = KErrNone;
	
	if (iResultData.Count() != searchResults.Count())
	{
		returnStatus = KErrGeneral;
	}
	else
	{
		for (TInt resCnt=0; resCnt < searchResults.Count(); resCnt++)
		{
		    CPsClientData* tempPsData = searchResults[resCnt];
		    TBool comapreRes = CompareInputDataWithResultDataL(*tempPsData,*(iResultData[resCnt])); 
			if (comapreRes == EFalse)
			{
				returnStatus = KErrGeneral;
				break;
			}
		}
	}	

	return returnStatus;
}

// -----------------------------------------------------------------------------
// This function compares the cachedata with the  CPsClientData
// -----------------------------------------------------------------------------
TBool CTestSuiteInputData::CompareInputDataWithResultDataL(CPsClientData& aResData,cacheData aInputData)
{
    TBool result = ETrue;
	RArray<TInt> dataOrder;
	
	iPsClientHandler->GetDataOrderL(*(iCacheUris[0]), dataOrder);
    
	HBufC* res1 = HBufC::NewL(100);
	HBufC* res2 = HBufC::NewL(100);
	for(TInt i =0; i< dataOrder.Count(); i++)
	{
		res1->Des().Append( *(aResData.Data(i)));
		
		TInt loc = aInputData.vpbkId.Find(dataOrder[i]);
		if( loc != KErrNotFound)
		{
			res2->Des().Append(*(aInputData.data[loc]));
		}
		else
		{
			result = EFalse;
			break;
		}
		
	}	
	if((res1->Des().Compare(*res2)) ==0)
	    result =  ETrue;
	else
	    result = EFalse;	
	return result;
}

// -----------------------------------------------------------------------------
// This function parses the input search string
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputInputSearchStringL(TDesC& aInput)
{
	iInputSearchString = aInput.AllocL();
}

// -----------------------------------------------------------------------------
// This function parses the input expected result string
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputExpectedResultStringL(TDesC& aInput)
{
    iExpectedResultString = aInput.AllocL();
}

// -----------------------------------------------------------------------------
// Deletes all the created contacts
// -----------------------------------------------------------------------------
void CTestSuiteInputData::DeleteAllCreatedContactsL()
{
	iContactHandler->DeleteAllCreatedContactsL();
	CActiveScheduler::Start();
}

// -----------------------------------------------------------------------------
// Deletes contacts with specific contact ids
// -----------------------------------------------------------------------------
void CTestSuiteInputData::DeleteContactsWithIdsL(RArray<TInt>& aContactIdArray)
{
	iContactHandler->DeleteContactsWithIdsL(aContactIdArray);
//	CActiveScheduler::Start();	
}

// -----------------------------------------------------------------------------
// This function parses the input for the languge supported tag
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForLanguageSupportL(TDesC& aInput)
{
	RPointerArray<TDesC> langData;
	TChar separator = ',';
	GetCharacterSeparatedDataL( aInput,separator, langData);
	
	//If there are more than 2 elements, then it is an error
	if(langData.Count() != 2)
	{
	   User::Leave(KErrArgument);
	}
	
	// get Language Id
	TInt lang = -1;
	TLex num(*(langData[0]));
	TInt err = num.Val(lang);
	// for 57 this value will be ELangHindi
	if( KErrNone == err)
	{
		iLangId = (TLanguage)lang;
	}
	else
	{
		User::Leave(KErrArgument);
	}
	
	if(langData[1]->Compare(KTrue) == 0)
	{
		iIsLangSupported = ETrue;
	}
	else if(langData[1]->Compare(KFalse) == 0)
	
	{
		iIsLangSupported = EFalse;
	}
	else
	{
		User::Leave(KErrArgument);
	}
}

// -----------------------------------------------------------------------------
// This function parses the input for addition of group to phonebook
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForAddingGroupsL(TDesC& aInput)
{
	TChar comma = ',';
	TInt commaPos = aInput.Locate(comma);
	HBufC* gropName = HBufC::NewL(commaPos);
	gropName->Des().Copy(aInput.Left(commaPos));
	TInt len = aInput.Length() - commaPos;
	HBufC* temp2 = aInput.Right(len - 1).AllocL();
	ParseInputForContactsDataL(*temp2,0);
	
	//Add the group
	iContactHandler->AddGroupL(*gropName);
	CActiveScheduler :: Start();
	
	//Create Contacts 
	CreateContactsL();
	iContactHandler->AddCreatedContactsToGroup();
}

// -----------------------------------------------------------------------------
// This function parse the input for the group to seach in that particular group
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForSearchWithinGroupL(TDesC& aInput)
{
	iGroupToBeSearched = aInput.AllocL();	
}

// -----------------------------------------------------------------------------
// Return the input language
// -----------------------------------------------------------------------------
TLanguage CTestSuiteInputData::LanguageInput()
{
    return iLangId;
}

// -----------------------------------------------------------------------------
// Return the input language expected result
// -----------------------------------------------------------------------------
TBool CTestSuiteInputData::LanguageInputExpectedResult()
{
	return  iIsLangSupported;
}

// -----------------------------------------------------------------------------
// Return the group to be searched
// -----------------------------------------------------------------------------
TDesC& CTestSuiteInputData::GroupToBeSearched() const
{
	return *iGroupToBeSearched;
}

// -----------------------------------------------------------------------------
// Tests if Id returns correct vpbklink
// -----------------------------------------------------------------------------
TBool CTestSuiteInputData::TestVpbkLinkForIdL(CPSRequestHandler& aPsClientHandler,const CPsClientData& aSearchResult)
{
   return iContactHandler->TestVpbkLinkForIdL(aPsClientHandler,aSearchResult);
}

// -----------------------------------------------------------------------------
// Parses the input for sort order
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForSortOrderL(TDesC& aInput)
{
	TChar aSeparator = ',';
	RPointerArray<TDesC> aSeparatedData;
	GetCharacterSeparatedDataL(aInput,aSeparator,aSeparatedData);

	for (TInt i=0; i < aSeparatedData.Count(); i++)
	{
		TInt fieldId = 0;
		TLex numlex(*(aSeparatedData[i]));
		TInt err = numlex.Val(fieldId);  // parse the string to extract integer value
		if(err != KErrNone)
		{
			User::Leave(KErrGeneral);
		}
		iSortOrder.Append(fieldId);

	}
}

// -----------------------------------------------------------------------------
// Returns the sort order
// -----------------------------------------------------------------------------
void CTestSuiteInputData::SortOrder(RArray<TInt>& aSortOrder)
{
	for (TInt i=0; i < iSortOrder.Count(); i++)
	{
		aSortOrder.Append(iSortOrder[i]);
	}
}

// End of file
