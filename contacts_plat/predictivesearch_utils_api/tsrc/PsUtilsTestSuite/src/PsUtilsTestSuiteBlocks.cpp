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
* Description:  Ps Utils test suite source file
*
*/


// SYSTEM INCLUDES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <S32MEM.H>
#include <CPsClientData.h>

// USER INCLUDES
#include "PsUtilsTestSuite.h"
#include "psutilstestsuitedefs.h"
#include "TestSuiteInputData.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
void CPsUtilsTestSuite::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
TInt CPsUtilsTestSuite::RunMethodL( 
    CStifItemParser& aItem ) 
    {

     TStifFunctionInfo const KFunctions[] =
        {  
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 

        ENTRY( "PsQueryItem_SetAndGetL", CPsUtilsTestSuite::CPsQueryItem_SetAndGetL),
		ENTRY( "PsQuery_SetAndGetL", CPsUtilsTestSuite::CPsQuery_SetAndGetL),
		ENTRY( "PsClientData_SetAndGetL", CPsUtilsTestSuite::PsClientData_SetAndGetL),
		ENTRY( "PsData_SetAndGetL", CPsUtilsTestSuite::PsData_SetAndGetL),
		ENTRY( "PsSettings_SetAndGetL", CPsUtilsTestSuite::PsSettings_SetAndGetL),
		ENTRY( "PsPattern_SetAndGetL", CPsUtilsTestSuite::PsPattern_SetAndGetL),
	      
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// This test case performs all the  operations on the CPsQueryItem
// -----------------------------------------------------------------------------
TInt CPsUtilsTestSuite::CPsQueryItem_SetAndGetL( CStifItemParser& aItem )
{
	TInt errorStatus = KErrGeneral;
	RPointerArray<TDesC> inputParameterArray;
	Parse_StifItem(aItem,inputParameterArray);
	
	
	//Parse data for CPsQueryItem
    TKeyboardModes inputKeyboardMode = EModeUndefined;
    TChar inputCharacter = '=';
    ParseForCPsQueryItemL(inputParameterArray,inputKeyboardMode,inputCharacter);
    inputParameterArray.ResetAndDestroy();
	// Create the soure CPsQueryItem
	CPsQueryItem* itemSrc = NULL;
	itemSrc = CPsQueryItem::NewL(); 
	CleanupStack::PushL(itemSrc);
    
    //Set the mode and character
    itemSrc->SetMode(inputKeyboardMode);
    itemSrc->SetCharacter(inputCharacter);

    // Externalize to a stream of type RBufWriteStream
    CBufFlat *buf = CBufFlat::NewL(KBufferMaxLen); 
    CleanupStack::PushL(buf);
    RBufWriteStream stream(*buf);
	stream.PushL();
	itemSrc->ExternalizeL(stream);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream

    // Create the destination CPsQueryItem
	CPsQueryItem* itemDest = NULL;
	itemDest = CPsQueryItem::NewL();
	CleanupStack::PushL(itemDest);
	 
	// Internalize from the stream
	HBufC8* destBuf = HBufC8::NewLC(buf->Size()); 
    TPtr8 ptrdestBuf(destBuf->Des()); 
    buf->Read(0, ptrdestBuf, buf->Size()); 

	RDesReadStream rdStream(destBuf->Des()); 
	CleanupClosePushL(rdStream);
    itemDest->InternalizeL(rdStream);
    CleanupStack::PopAndDestroy(2);//rdStream, destBuf
	
    // Check if internalized and externalized values are same.
    if (   (itemDest->Mode() != itemSrc->Mode())
        || (itemDest->Character() != itemSrc->Character()) )
    {
    	errorStatus = KErrGeneral;
        
    }
    else
    {
    	errorStatus = KErrNone;
    }
        
	CleanupStack::PopAndDestroy(3); //itemDest,buf,itemSrc
	
	// return the errorStauts
	return errorStatus;
}

// -----------------------------------------------------------------------------
// This test case performs all the  operations on the CPsQuery
// -----------------------------------------------------------------------------

TInt CPsUtilsTestSuite::CPsQuery_SetAndGetL( CStifItemParser& aItem )
{
		TInt errorStatus = KErrNone;
       	RPointerArray<TDesC> inputParameterArray;
		Parse_StifItem(aItem,inputParameterArray);

		//Parse data 
	    TKeyboardModes inputKeyboardMode = EModeUndefined;
	    TChar inputCharacter = '=';
	    //Create the query
	    CPsQuery* inputSearchQuery = CPsQuery::NewL(); 
	    CleanupStack::PushL(inputSearchQuery); 
	    TInt removeIndex;
   
        // Parse data for CPsQuery
	    ParseForCPsQueryL(inputParameterArray,inputKeyboardMode,*inputSearchQuery,removeIndex);
	    
	    
	    // Create the destination CPsQuery
		CPsQuery* destSearchQuery = NULL;
		destSearchQuery = CPsQuery::NewL();
		CleanupStack::PushL(destSearchQuery); 
		
        // Externalize to a stream of type RBufWriteStream
	    CBufFlat *buf = CBufFlat::NewL(KBufferMaxLen); 
	    CleanupStack::PushL(buf);
	    RBufWriteStream stream(*buf);
		stream.PushL(); 
		inputSearchQuery->ExternalizeL(stream);
		stream.CommitL();
		CleanupStack::PopAndDestroy(); // stream

	    // Internalize from the stream
		HBufC8* destBuf = HBufC8::NewLC(buf->Size()); 
	    TPtr8 ptrdestBuf(destBuf->Des()); 
	    buf->Read(0, ptrdestBuf, buf->Size()); 

		RDesReadStream rdStream(destBuf->Des()); 
		CleanupClosePushL(rdStream);
	    destSearchQuery->InternalizeL(rdStream);
	    CleanupStack::PopAndDestroy(3);//rdStream, destBuf, buf
	
        
        // Check if internalized and externalized values are same.
	    if (  !ComparePsQueryL(*inputSearchQuery, *destSearchQuery)  )
	    {
	    	errorStatus = KErrGeneral;
	    }
	        
		// Perform other operations (not performed yet)
        
        // Get the item at removeindex
        CPsQueryItem* item = CPsQueryItem::NewL();
        CPsQueryItem &tempitem = inputSearchQuery->GetItemAtL(removeIndex);
        item->SetMode(tempitem.Mode());
        item->SetCharacter(tempitem.Character());
        
        //Remove the item
        inputSearchQuery->Remove(removeIndex);
        
        //Insert the item back
        inputSearchQuery->InsertL(*item,removeIndex);
        
        //Get the query and check with original query
        TPtrC queryPtr ( inputSearchQuery->QueryAsStringLC() );
        if (queryPtr.Compare(*(inputParameterArray[1])) != 0)
        {
            errorStatus = KErrGeneral;
        }
        CleanupStack::PopAndDestroy(3); // queryPtr,destSearchQuery,inputSearchQuery
        
        //Clean up
        inputParameterArray.ResetAndDestroy();
        
        // Return errorStatus
        return errorStatus;
}



// -----------------------------------------------------------------------------
// This test case performs all the  operations on the CPsClientData
// -----------------------------------------------------------------------------
TInt CPsUtilsTestSuite::PsClientData_SetAndGetL( CStifItemParser& aItem )

{
	TInt errorStatus = KErrNone;
	CTestSuiteInputData* iInputParsedData = CTestSuiteInputData::NewL(aItem);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;
  	
  	
  	// Create a new source PS Query object
    CPsClientData* psSrcData = CPsClientData::NewL();
    CleanupStack::PushL(psSrcData);
        
    // Set the unique Id
    psSrcData->SetId(iInputParsedData->Id());
        
    // Set the unique URI Id
	psSrcData->SetUriL( iInputParsedData->CacheUriAt(0));
        
    RPointerArray<TDesC> contactDataArray;
    iInputParsedData->Data(contactDataArray)  ; 
    // Set the data elements
    for (TInt i(0); i < contactDataArray.Count(); i++)
    {
            psSrcData->SetDataL(i, *(contactDataArray[i]) );
    }
    
    //set data extension
    if(contactDataArray.Count() > 0)
    psSrcData->SetDataExtensionL(contactDataArray[0]);
    
    //set IsMarked
    psSrcData->SetMark();
    
    // Create the destination CPsQuery
	CPsClientData* destObject = NULL;
	destObject = CPsClientData::NewL();
	CleanupStack::PushL(destObject);
	
    // Externalize to a stream of type RBufWriteStream
    CBufFlat *buf = CBufFlat::NewL(KBufferMaxLen); 
    CleanupStack::PushL(buf);
    RBufWriteStream stream(*buf);
	stream.PushL(); 
	psSrcData->ExternalizeL(stream);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream

    // Internalize from the stream
	HBufC8* destBuf = HBufC8::NewLC(buf->Size()); 
    TPtr8 ptrdestBuf(destBuf->Des()); 
    buf->Read(0, ptrdestBuf, buf->Size()); 

	RDesReadStream rdStream(destBuf->Des()); 
	CleanupClosePushL(rdStream);

    destObject->InternalizeL(rdStream);
    CleanupStack::PopAndDestroy(3);//rdStream, destBuf, buf
	    
    //Compare the internalized and externalized object
    TInt ret1 = iInputParsedData->CompareByDataL(*psSrcData,*destObject);
    TBool ret2 = ComparePsClientDataL(*psSrcData,*destObject);
    if(ret1 !=0 && ret2)
    {
    
    	errorStatus = KErrGeneral;
    	
    }
	CleanupStack::PopAndDestroy(2);
	
	
	return errorStatus;

}


// -----------------------------------------------------------------------------
// This test case performs all the  operations on the CPsData
// -----------------------------------------------------------------------------
TInt CPsUtilsTestSuite::PsData_SetAndGetL( CStifItemParser& aItem )

{
	TInt errorStatus = KErrNone;
	CTestSuiteInputData* iInputParsedData = CTestSuiteInputData::NewL(aItem);
	
	// Parse the input data
  	iInputParsedData->ParseInputL(aItem) ;
  	
  	
  	// Create a new source PS Query object
    CPsData* psSrcData = CPsData::NewL();
    CleanupStack::PushL(psSrcData);
        
    // Set the unique Id
    psSrcData->SetId(iInputParsedData->Id());
        
    if(iInputParsedData->Id())
    {
	    // Set the unique URI Id
	    psSrcData->SetUriId(iInputParsedData->Id());
	    
	    //set data matches
	    psSrcData->SetDataMatch(iInputParsedData->Id());
	    
	    //add Extended Data field of Integers
	    psSrcData->AddIntDataExtL(iInputParsedData->Id());
    }    
        
    RPointerArray<TDesC> contactDataArray;
    iInputParsedData->Data(contactDataArray)  ; 
    // Set the data elements
    for (TInt i(0); i < contactDataArray.Count(); i++)
    {
            psSrcData->SetDataL(i, *(contactDataArray[i]) );
    }
    
     //set data extension
    if(contactDataArray.Count() > 0)
    psSrcData->SetDataExtension(contactDataArray[0]);
    
    // Create the destination CPsQuery
		CPsData* destObject = NULL;
		destObject = CPsData::NewL();
		CleanupStack::PushL(destObject);
		
        // Externalize to a stream of type RBufWriteStream
	    CBufFlat *buf = CBufFlat::NewL(KBufferMaxLen); 
	    CleanupStack::PushL(buf);
	    RBufWriteStream stream(*buf);
		stream.PushL(); 
		psSrcData->ExternalizeL(stream);
		stream.CommitL();
		CleanupStack::PopAndDestroy(); // stream

	    // Internalize from the stream
		HBufC8* destBuf = HBufC8::NewLC(buf->Size()); 
	    TPtr8 ptrdestBuf(destBuf->Des()); 
	    buf->Read(0, ptrdestBuf, buf->Size()); 

		RDesReadStream rdStream(destBuf->Des()); 
		CleanupClosePushL(rdStream);
		 destObject->InternalizeL(rdStream);
	    CleanupStack::PopAndDestroy(3);//rdStream, destBuf, buf
	    
	    //Compare the internalized and externalized object
	    TInt ret1 = CPsData::CompareByData(*psSrcData,*destObject);
	    TInt ret2 = CPsData::CompareById(*psSrcData,*destObject);
	    TBool ret3 = ComparePsDataL(*psSrcData,*destObject);
	    
	    if((ret1 !=0) && (ret2 != 0) && !ret3 )
	    {
	    
	    	errorStatus = KErrGeneral;
	    	
	    }
	    else
	    {
	    	if(iInputParsedData->Id())
	    	{
		    	if(psSrcData->IsDataMatch(iInputParsedData->Id()))
		    	{
		    		errorStatus = KErrNone;
		    	}
		    	else
		    	{
		    		errorStatus = KErrGeneral;
		    	}
	    	}
	    	else
	    	{
	    	
	    		errorStatus = KErrNone;
	    	}
	    	
	    }
	    
	    psSrcData->RemoveIntDataExt(iInputParsedData->Id());
	    psSrcData->ClearDataMatches();
  		CleanupStack::PopAndDestroy(2);
  		return errorStatus;

}

// -----------------------------------------------------------------------------
// This test case performs all the  operations on the CPsSettings
// -----------------------------------------------------------------------------
TInt CPsUtilsTestSuite::PsSettings_SetAndGetL(CStifItemParser& aItem )
{
	TInt errorStatus = KErrNone;
	TInt err1= KErrGeneral;
	TInt err2 = KErrGeneral;
	
	CTestSuiteInputData* inputParsedData = CTestSuiteInputData::NewL(aItem);
	
	// Parse the input data
  	inputParsedData->ParseInputL(aItem) ;
  	
	RArray<TInt> displayFields;
	inputParsedData->DisplayFields(displayFields);
	
	RPointerArray<TDesC> uriArray;
    inputParsedData->CacheUris(uriArray)  ;    
    
    TInt maxResult = inputParsedData->Id();
    
    // Create a new source CPsSettings object
    CPsSettings* srcPsSettings = CPsSettings::NewL();
    CleanupStack::PushL(srcPsSettings);  
    
    // set the search uris
    srcPsSettings->SetSearchUrisL(uriArray);
    
    // set display fields
    srcPsSettings->SetDisplayFieldsL(displayFields);
    
    // set max results
    srcPsSettings->SetMaxResults(maxResult);
    
    // set sort type
    srcPsSettings->SetSortType(inputParsedData->SortType());
    
    // Create the destination CPsSettings
	CPsSettings* destPsSettings = NULL;
	destPsSettings = CPsSettings::NewL();
	CleanupStack::PushL(destPsSettings);  
	
    // Externalize to a stream of type RBufWriteStream
    CBufFlat *buf = CBufFlat::NewL(KBufferMaxLen); 
    CleanupStack::PushL(buf);  
    RBufWriteStream stream(*buf);
	stream.PushL(); 
	srcPsSettings->ExternalizeL(stream);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream

    // Internalize from the stream
	HBufC8* destBuf = HBufC8::NewLC(buf->Size()); 
    TPtr8 ptrdestBuf(destBuf->Des()); 
    buf->Read(0, ptrdestBuf, buf->Size()); 

	RDesReadStream rdStream(destBuf->Des()); 
	CleanupClosePushL(rdStream);
    destPsSettings->InternalizeL(rdStream);
    CleanupStack::PopAndDestroy(3);//rdStream, destBuf, buf
    
    // Create a new settings instance use CloneL
    CPsSettings *tempSettings = srcPsSettings->CloneL();
    
    RArray<TInt> groupId;
    //check for group id
    srcPsSettings->GetGroupIdsL(groupId);
    RArray<TInt> expGroupId;
	inputParsedData->GroupIds(expGroupId);
	
    if(groupId.Count() == expGroupId.Count() )
    {
    	for(TInt i = 0; i < groupId.Count(); i++)
	    {
	    	if(groupId[i] == expGroupId[i])
	    	{
	    		err1 = KErrNone;
	    	}
	    	else
	    	{
	    		err1 = KErrGeneral;
	    		break;
	    	}
	    }
    }
    else
    {
    	err1 = KErrGeneral;
    }
    
    //Compare the internalized and externalized object    
    //Compare the clone object with original object
	if( ComparePsSettingsL(*srcPsSettings, *destPsSettings) && 
		ComparePsSettingsL(*srcPsSettings, *tempSettings) )
    {
    	err2 = KErrNone;
    }
    else
    {
    	err2 = KErrGeneral;
    }
    
    if(err1 == KErrNone && err2 == KErrNone)
    {
    	errorStatus = KErrNone;
    }
    else
    {
    	errorStatus = KErrGeneral;
    }
    
    
    CleanupStack::PopAndDestroy(2);  // destPsSettings, srcPsSettings
  	
  	// clean up
  	delete inputParsedData;
  	
  	// return error code
  	return errorStatus;
}

// -----------------------------------------------------------------------------
// This test case performs all the  operations on the CPsSettings
// -----------------------------------------------------------------------------
TInt CPsUtilsTestSuite::PsPattern_SetAndGetL(CStifItemParser& aItem )
{
	TInt errorStatus = KErrNone;
	CTestSuiteInputData* inputParsedData = CTestSuiteInputData::NewL(aItem);
	
	// Parse the input data
  	inputParsedData->ParseInputL(aItem) ;
  	
	RPointerArray<TDesC> dataArray;
    inputParsedData->Data(dataArray)  ;    
    
    TInt index = inputParsedData->Id();

	// Create a new source CPsSettings object
    CPsPattern* srcPsPattern = CPsPattern::NewL();
    CleanupStack::PushL(srcPsPattern);  
    
    // set the match pattern
    srcPsPattern->SetPatternL(*dataArray[0]);
    
    // set first index
    srcPsPattern->SetFirstIndex(index);
        
	// Create the destination CPsPattern
	CPsPattern* destPsPattern = NULL;
	destPsPattern = CPsPattern::NewL();
	CleanupStack::PushL(destPsPattern);  
	
    // Externalize to a stream of type RBufWriteStream
    CBufFlat *buf = CBufFlat::NewL(KBufferMaxLen); 
    CleanupStack::PushL(buf);  
    RBufWriteStream stream(*buf);
	stream.PushL(); 
	srcPsPattern->ExternalizeL(stream);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream

    // Internalize from the stream
	HBufC8* destBuf = HBufC8::NewLC(buf->Size()); 
    TPtr8 ptrdestBuf(destBuf->Des()); 
    buf->Read(0, ptrdestBuf, buf->Size()); 

	RDesReadStream rdStream(destBuf->Des()); 
	CleanupClosePushL(rdStream);
    destPsPattern->InternalizeL(rdStream);
    CleanupStack::PopAndDestroy(3);//rdStream, destBuf, buf
    
    //Compare the internalized and externalized object
	if( ( (srcPsPattern->Pattern()).Compare(destPsPattern->Pattern()) == 0 ) &&
    	( srcPsPattern->FirstIndex() == destPsPattern->FirstIndex() )
       )
    {
    	errorStatus = KErrNone;
    }
    else
    {
    	errorStatus = KErrGeneral;
    }
    
    
    CleanupStack::PopAndDestroy(2);  // destPsPattern, srcPsPattern
  	// clean up
  	delete inputParsedData;
  	
  	// return error code
  	return errorStatus;

	
}

// -----------------------------------------------------------------------------
// Parses the input stif parameters 
// -----------------------------------------------------------------------------
void CPsUtilsTestSuite::Parse_StifItem(CStifItemParser& aItem,RPointerArray<TDesC>& aInputParameter)
{
	TPtrC string; 
	aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing ); 
	// Get each token and append to aInputParameter
	while ( KErrNone == aItem.GetNextString ( string ) ) 
	{ 
		aInputParameter.Append(string.AllocL()); 
	} 

}

// -----------------------------------------------------------------------------
// Parses the input stif parameters for CPsQueryItem
// -----------------------------------------------------------------------------
void CPsUtilsTestSuite::ParseForCPsQueryItemL(RPointerArray<TDesC>& aInputParameterArray,TKeyboardModes& aCurrentMode,TChar& aCurrentChar)
{

	//There should be only 2 arguments, else it is an error in input
	if(aInputParameterArray.Count() != 2)
	{
		User::Leave(KErrArgument);
	}
	
	// Get the keyboard mode  
    if ( (*(aInputParameterArray[0])).Compare(KItut) == 0)
    {
    	aCurrentMode = EItut;
    }
    else if ( (*(aInputParameterArray[0])).Compare(KQwerty) == 0)
    {
    	aCurrentMode = EQwerty;
    }
    else
    {
       	aCurrentMode = EModeUndefined;
    }
    
    
    //Get the current character
    TDesC* tmp = aInputParameterArray[1];
    if (tmp!=NULL)
    {
        if(tmp->Length() > 1)
        {
        	User::Leave(KErrArgument);
        }
        aCurrentChar = (*tmp)[0];
    }

}	

// -----------------------------------------------------------------------------
// Parses the input stif parameters for CPsQuery
// -----------------------------------------------------------------------------
void CPsUtilsTestSuite::ParseForCPsQueryL(RPointerArray<TDesC>& aInputParameterArray,
														TKeyboardModes& aCurrentMode,
														CPsQuery& aSearchQuery, 
														TInt& aRemoveIndex)
{
	//There should be only 3 arguments, else an error in input
	if(aInputParameterArray.Count()!= 3)
	{
		User::Leave(KErrArgument);
	}
	
	// Get the keyboard mode 
    if ( (*(aInputParameterArray[0])).Compare(KItut) == 0)
    {
    	aCurrentMode = EItut;
    }
    else if ( (*(aInputParameterArray[0])).Compare(KQwerty) == 0)
    {
    	aCurrentMode = EQwerty;
    }
    else
    {
       	aCurrentMode = EModeUndefined;
    }
    
 	// Create the search query as required by ps engine
	TDesC* buf = aInputParameterArray[1];
    // Append the query items to the query
    for ( int i = 0; i < buf->Length(); i++ )
    {    
        // Add a query item
        CPsQueryItem* item = CPsQueryItem::NewL();
        item->SetCharacter((*buf)[i]);   
        item->SetMode(aCurrentMode);
        aSearchQuery.AppendL(*item);
    }
   
   
   // Get the removeIndex 
   TInt removeIndex = 0;
   TLex myDocId(*(aInputParameterArray[2]));
   TInt err = myDocId.Val(removeIndex);
			 
  	if(err == KErrNone)
  	{
  		aRemoveIndex = removeIndex;
  	}
  	else
  	{
  		User::Leave(err);
  	} 

}	

// -----------------------------------------------------------------------------
// Compares two CPsQuery objects
// -----------------------------------------------------------------------------

TBool CPsUtilsTestSuite::ComparePsQueryL(CPsQuery& aInputSearchQuery, CPsQuery& aDestSearchQuery)
{
	// The count and keyboardmode  of both objects should be equal
	if( (aInputSearchQuery.Count() != aDestSearchQuery.Count()) ||
	    (aInputSearchQuery.KeyboardModeL() != aDestSearchQuery.KeyboardModeL())
	  )
	{
		return EFalse;

	}
	
	// Compare each CPsQueryItem from both the objects 
	for(TInt i =0; i < aInputSearchQuery.Count(); i++)
	{
	
	    CPsQueryItem& src =  aInputSearchQuery.GetItemAtL(i);
	    CPsQueryItem& dest =  aDestSearchQuery.GetItemAtL(i);
	    // The mode and Character should be equal
	 	if (   (src.Mode() != dest.Mode()) || (src.Character() != dest.Character()) )
    	{
    		return EFalse;
        }
 
	}
	// All ok, return ETrue
	return ETrue;
}

// -----------------------------------------------------------------------------
// Compares two CPsSettings objects
// -----------------------------------------------------------------------------
TBool CPsUtilsTestSuite::ComparePsSettingsL(CPsSettings& aSrcPsSettings, CPsSettings& aDestPsSettings)
{
	TBool matched = EFalse;
	
	// compare max results and sort type
	if( (aSrcPsSettings.MaxResults() == aDestPsSettings.MaxResults()) &&
		(aSrcPsSettings.GetSortType() == aDestPsSettings.GetSortType())	   
	  )
	{
		matched = ETrue;
	}
	
	// compare search Uris
	RPointerArray<TDesC> srcSearchUri;
	aSrcPsSettings.SearchUrisL(srcSearchUri);
	RPointerArray<TDesC> destSearchUri;
	aDestPsSettings.SearchUrisL(destSearchUri);
	
	if(srcSearchUri.Count() == destSearchUri.Count())
	{
		for(TInt i = 0; i < srcSearchUri.Count(); i++)
		{
			if((*srcSearchUri[i]).Compare(*destSearchUri[i]) == 0)
			{
				matched = ETrue;
			}
			else
			{
				matched = EFalse;
				break;
			}
		}
	}
	
	// compare display fields
	RArray<TInt> srcDisplayFields;
	aSrcPsSettings.DisplayFieldsL(srcDisplayFields);
	RArray<TInt> destDisplayFields;
	aDestPsSettings.DisplayFieldsL(destDisplayFields);
	
	if(srcDisplayFields.Count() == destDisplayFields.Count())
	{
		for(TInt i = 0; i < srcDisplayFields.Count(); i++)
		{
			if(srcDisplayFields[i] == destDisplayFields[i])
			{
				matched = ETrue;
			}
			else
			{
				matched = EFalse;
				break;
			}
		}
	}
	
	
	
	return matched;
}

// -----------------------------------------------------------------------------
// Compares two CPsClientData objects
// -----------------------------------------------------------------------------
TBool CPsUtilsTestSuite::ComparePsClientDataL( CPsClientData& aObject1, CPsClientData& aObject2 )
{
	TBool isMarked = aObject1.IsMarked() && aObject2.IsMarked() ;
	TInt compareUri = (aObject1.Uri())->Compare(*(aObject2.Uri()));
	if( (aObject1.Id() == aObject2.Id() ) &&
		( compareUri == 0) && isMarked 	  )
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}

// -----------------------------------------------------------------------------
// Compares two CPsData objects
// -----------------------------------------------------------------------------
TBool CPsUtilsTestSuite::ComparePsDataL( CPsData& aObject1, CPsData& aObject2 )
{
	TBool compareUrid = ( aObject1.UriId() ==  aObject2.UriId());
	TBool compareDataMatch = ( aObject1.DataMatch() ==  aObject2.DataMatch());
	
	RArray<TInt> object1DataExtArray;
	aObject1.IntDataExt(object1DataExtArray);
	RArray<TInt> object2DataExtArray;
	aObject2.IntDataExt(object2DataExtArray);
	
	TBool compareDataExtArray = EFalse;
	
	if(object1DataExtArray.Count() == object2DataExtArray.Count() )
	{
		for(TInt i = 0; i < object1DataExtArray.Count(); i++)
		{
			if(object1DataExtArray[i] == object2DataExtArray[i])
			{
				compareDataExtArray = ETrue;
			}
			else
			{
				compareDataExtArray = EFalse;
				break;
			}
		}
	}
	
	if( compareUrid &&  compareDataMatch && compareDataExtArray )
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}

//End of files
