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
#include <collate.h>
#include "TestSuiteInputData.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// Two Phase constructor
// ----------------------------------------------------------------------------
CTestSuiteInputData* CTestSuiteInputData::NewL(CStifItemParser& aItem)
{
	CTestSuiteInputData* self = new (ELeave) CTestSuiteInputData();	
    CleanupStack::PushL(self);
    self->ConstructL(aItem);
    CleanupStack::Pop();
    return self;
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
    	
    if(iQueryBuf)
    {
    	delete iQueryBuf;
		iQueryBuf = NULL;    
    }	
    if(iKeyboardModeBuf)
    {
    	delete iKeyboardModeBuf;
    	iKeyboardModeBuf = NULL;
    }

	
	iCacheUris.ResetAndDestroy();
	

	iDisplayFields.Close();

}

// ----------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------
TDesC& CTestSuiteInputData::KeyboardMode() const
{
	return *iKeyboardModeBuf;
}

// ----------------------------------------------------------------------------
// Returns the Id
// ----------------------------------------------------------------------------
TInt CTestSuiteInputData::Id()
{
	return iId;
}

void CTestSuiteInputData::Data(RPointerArray<TDesC> & aDataArray)
{
	for(TInt i = 0 ; i < iDataArray.Count(); i++)
	{
		aDataArray.Append(iDataArray[i]->AllocL());
	}

}
// ----------------------------------------------------------------------------
// Sets the keyboard mode
// ----------------------------------------------------------------------------
void CTestSuiteInputData::SetKeyboardModeL(const TDesC& aKeyboardMode)
{
	iKeyboardModeBuf = aKeyboardMode.AllocL();
}

TSortType CTestSuiteInputData::SortType() const
{
	return iSortType;
}
		
void CTestSuiteInputData::SetSortType(TSortType aSortType)
{
	iSortType = aSortType;
}

// ----------------------------------------------------------------------------
// Returns the search query (as specified in the config file)
// ----------------------------------------------------------------------------
TDesC& CTestSuiteInputData::Query() const
{
	return *iQueryBuf;
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
// Return the cache uri at given index 
// ----------------------------------------------------------------------------
TDesC& CTestSuiteInputData::CacheUriAt(TInt aIndex) const
{
	if ( aIndex >= iCacheUris.Count() ) 
	{
		HBufC* temp = HBufC::NewL(2);
		temp->Des().Copy(KNullDesC);
		return *temp;
	
	}
         
    return *(iCacheUris[aIndex]);
}


// ----------------------------------------------------------------------------
// Return the cache uri 
// ----------------------------------------------------------------------------
void CTestSuiteInputData::CacheUris(RPointerArray<TDesC> & aUriArray)
{
	for(TInt i = 0 ; i < iCacheUris.Count(); i++)
	{
		aUriArray.Append(iCacheUris[i]->AllocL());
	}
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
// Return the display fields
// ----------------------------------------------------------------------------
void CTestSuiteInputData::DisplayFields(RArray<TInt>& aDisplayFields)
{
	for(TInt i = 0 ; i < iDisplayFields.Count(); i++)
	{
		aDisplayFields.Append(iDisplayFields[i]);
	}
}

// ----------------------------------------------------------------------------
// Return the group ids
// ----------------------------------------------------------------------------
void CTestSuiteInputData::GroupIds(RArray<TInt>& aGroupId)
{
	for(TInt i = 0 ; i < iGroupIds.Count(); i++)
	{
		aGroupId.Append(iGroupIds[i]);
	}
}
// ----------------------------------------------------------------------------
// Second phase construction
// ----------------------------------------------------------------------------
void CTestSuiteInputData::ConstructL(CStifItemParser& /*aItem*/)
{

}

// -----------------------------------------------------------------------------
// This function parses the input parameters
// -----------------------------------------------------------------------------
void CTestSuiteInputData:: ParseInputL(CStifItemParser& aItem)
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
		
		if(  (sqBracketStartPos != KErrNotFound) && ( sqBracketEndPos!=KErrNotFound))
		{
			TPtrC16 optionPtr = string.Left(sqBracketStartPos);	
				
			TPtrC16 dataPtr = string.Mid(sqBracketStartPos + 1,sqBracketEndPos - sqBracketStartPos - 1 );	
			
			
			// Check the type of tag, then do the corresponding parsing
		   if(optionPtr.Compare(KSearchQueryTag) == 0)
		    {
		    	// Search query
		    	ParseInputForSearchQueryL(dataPtr);
		    
		    }
		    else if(optionPtr.Compare(KUriTag) == 0)
		    {
		    	// Data store uris
		    	ParseInputForDataStoresUriL(dataPtr);
		    }
		
		    else if(optionPtr.Compare(KIdTag) == 0)
		    {
		    	// Id
		    	ParseInputForIdL(dataPtr);
		    }
		    else if(optionPtr.Compare(KDataTag) == 0)
		    {
		    	// data
		    	ParseInputForData(dataPtr);
		    }
		    
		    else if(optionPtr.Compare(KSetDisplayFields) == 0)
		    {
		    	// Display fields
		    	ParseInputForDisplayFieldsL(dataPtr);
		    }
		    else if(optionPtr.Compare(KSortTag) == 0)
		    {
		    	// Sorting
		    	ParseInputForSortTypeL(dataPtr);
		    }
		    else if(optionPtr.Compare(KGroupIdTag) == 0)
		    {
		    	// Sorting
		    	ParseInputForGroupIdsL(dataPtr);
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
// This function parses input for number id
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForIdL(TDesC& aInput)
{

		TInt fieldId = 0;
		TLex num(aInput);
		
		TInt err = num.Val(fieldId);
		if(err == KErrNone)
		{
			iId = fieldId;
		}
		else
		{
			User::Leave(KErrArgument);
		}

}

// -----------------------------------------------------------------------------
// This function parses input for sort type
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForSortTypeL(TDesC& aInput)
{
//	TSortType sorttype;
	
	// set the sort type
	if(aInput.Compare(KAlphabetic) == 0)
	{
	    iSortType = EAlphabetical;
	}
	else if (aInput.Compare(KPattern) == 0)
	{
	    iSortType = EPatternBased;
	}
	else
	{
		User::Leave(KErrArgument);
	}
}

// -----------------------------------------------------------------------------
// This function parses input for cache uris
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForData(TDesC& aInput)
{
	RPointerArray<TDesC> data;
	TChar separator = ',';
	
	// Get the uris which are separated by commas
	GetCharacterSeparatedDataL( aInput,separator, data);
	for(TInt i = 0; i< data.Count(); i++)
	{
	        HBufC *temp =  data[i]->AllocL();
			iDataArray.Append(data[i]);
	
	}
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
	        HBufC *temp =  Uris[i]->AllocL();
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
	GetCharacterSeparatedDataL( aInput,separator, dispFieldsBuf);
	for(TInt i = 0; i< dispFieldsBuf.Count(); i++)
	{
		TInt fieldId = 0;
		TLex num(*(dispFieldsBuf[i]));
		
		    TInt err = num.Val(fieldId);  // parse the string to extract integer value
			if(err != KErrNone)
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
// This function parses the input for expected group ids
// -----------------------------------------------------------------------------
void CTestSuiteInputData::ParseInputForGroupIdsL(TDesC& aInput)
{
	RPointerArray<TDesC> grpIdsBuf;
	
	TChar separator = ',';
	GetCharacterSeparatedDataL( aInput,separator, grpIdsBuf);
	for(TInt i = 0; i< grpIdsBuf.Count(); i++)
	{
		TInt groupId = 0;
		TLex num(*(grpIdsBuf[i]));
		
		    TInt err = num.Val(groupId);  // parse the string to extract integer value
			if(err != KErrNone)
			{
				User::Leave(KErrGeneral);
			}
			else
			{
			    iGroupIds.Append(groupId);
			}
	}
}

// -----------------------------------------------------------------------------
// This function splits the the input separated by particular character
// -----------------------------------------------------------------------------
void CTestSuiteInputData::GetCharacterSeparatedDataL(TDesC& aInput,TChar aSeparator, RPointerArray<TDesC> &aSeparatedData)
{
	
	TInt SeparatorPos = -1 ;
    for (TInt j = 0 ; j< aInput.Length();)
	{
			
	        TInt Startpos = SeparatorPos + 1;
		// Get the data for each contact
			while((aInput[j] != aSeparator) && (j < ( aInput.Length()-1) ) )
			     j++;
			if(j == (aInput.Length() - 1))
			 j++; 
			      
			TPtrC16 dataPtr = aInput.Mid(Startpos ,j - Startpos);	
			
			
			// Add data to the output
			HBufC* tempData = dataPtr.AllocL();
			aSeparatedData.Append(tempData);
			SeparatorPos = j ;  
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

    // If there are more than 2 field, then it is an error
    if(queryData.Count() == 2)
    {
    	iQueryBuf = queryData[0]->AllocL();
    	iKeyboardModeBuf = queryData[1]->AllocL();
    	queryData.ResetAndDestroy();
    }
    else
    {
    	queryData.ResetAndDestroy();
     	User::Leave(KErrArgument);
    }
	

}



// -----------------------------------------------------------------------------
// This function creates the search query from the input buffer
// -----------------------------------------------------------------------------
void CTestSuiteInputData::CreateSearchQueryL(CPsQuery& aPsQuery,TDesC& aQueryBuf)
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
	TInt maxResults = 100;

	// add the display fields
    if(iDisplayFields.Count())
    {
		for(TInt i =0; i<iDisplayFields.Count();i++)
		    displayFields.Append(iDisplayFields[i]);
    
    }
    else
    {
	    displayFields.Append(6); // Firstname
	    displayFields.Append(4); // Lastname
	    displayFields.Append(27); // Company name
    }
    
     
    // Set the databases to be searched 
	for(TInt i =0; i <iCacheUris.Count(); i++ )
	{
		databases.Append(iCacheUris[i]->AllocL());
	
	}
	
	// Perform the settings
	aSettings.SetSearchUrisL(databases);
    aSettings.SetDisplayFieldsL(displayFields);
    aSettings.SetMaxResults(maxResults);
	

	// cleanup
	displayFields.Close();
	databases.Reset(); 
	databases.Close(); 


}

TInt CTestSuiteInputData::CompareByDataL ( const CPsClientData& aObject1, const CPsClientData& aObject2 )
{
	TBuf<KBufferMaxLen> data1;
     TBuf<255> data2;
     
     _LIT(KSpace, " ");
     
     data1 = KNullDesC;
     for ( int i = 0; i < aObject1.DataElementCount(); i++ )
     {
     	 if ( aObject1.Data(i) )
	     {
	     	data1 += aObject1.Data(i)->Des();
	     	data1 += KSpace;
	     }	     
     }
     
     // Trim the unnecessary white spaces before we compare
     data1.TrimAll();
     
     data2 = KNullDesC;
     for ( int i = 0; i < aObject2.DataElementCount(); i++ )
     {
     	 if ( aObject2.Data(i) )
	     {
	     	data2 += aObject2.Data(i)->Des();
	     	data2 += KSpace;
	     }	     
     }   
     
     // Trim the unnecessary white spaces before we compare 
     data2.TrimAll();   
     
	 // Get the standard method
	 TCollationMethod meth = *Mem::CollationMethodByIndex( 0 );
  	 meth.iFlags |= TCollationMethod::EIgnoreNone;
  	 meth.iFlags |= TCollationMethod::EFoldCase;

	 // Magic: Collation level 3 is used
	 TInt comparison(data1.CompareC( data2, 3, &meth ));
	 return comparison;
}
// End of file
