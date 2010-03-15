/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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


//SYSTEM INCLUDES
#include <vpbkeng.rsg>
#include <collate.h>

//USER INCLUDES
#include "pstestqueryhandler.h"
#include "CPsQuery.h"
#include "CPsQueryItem.h"
#include "CPsSettings.h"

// ---------------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------------
CPsTestQueryHandler* CPsTestQueryHandler::NewL()
{
	CPsTestQueryHandler* self = new ( ELeave ) CPsTestQueryHandler();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
	return self;
}

// ---------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------
//
CPsTestQueryHandler::CPsTestQueryHandler() 
{
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::ConstructL() 
// 2nd phase constructor
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler::ConstructL()
{
  	iAsyncHandler = CPSRequestHandler::NewL();
    iAsyncHandler->AddObserverL(this); 
  	iSettings = CPsSettings::NewL();   
  	iPsQuery = CPsQuery::NewL();
  	iCurrentTestCase = EDefaultCase;
  	iCurrentErrorCode = KErrNone;
}

// ---------------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------------
CPsTestQueryHandler::~CPsTestQueryHandler() 
{
	if(iAsyncHandler)
	{
		delete iAsyncHandler;
		iAsyncHandler = NULL;
	}
	if(iSettings)
	{
		delete iSettings;
		iSettings = NULL;
	}
	if(iPsQuery)
	{
		delete iPsQuery;
		iPsQuery = NULL;
	} 
	iContactsList.Reset();
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::SetCurrentTestCase() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: SetCurrentTestCase(TTestCase aCurrentTestCase)
{
	iCurrentTestCase = aCurrentTestCase;
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::SetCurrentErrorCode() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: SetCurrentErrorCode(TInt aCurrentErrorCode)
{
	iCurrentErrorCode = aCurrentErrorCode;
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::CurrentErrorCode() 
// 
// ---------------------------------------------------------------------------------
TInt CPsTestQueryHandler :: CurrentErrorCode()
{
	return(iCurrentErrorCode);
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::GetAllContentsL() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler::GetAllContentsL()
{	
	iAsyncHandler->GetAllContentsL();
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::StartSearchL() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: StartSearchL(const CPsQuery& aSearchQuery)
{
    // Send a search request
	iAsyncHandler->SearchL(aSearchQuery);
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::TestSearchL() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: TestSearchL(const TDesC& aBuf, TKeyboardModes aMode)
{
	FormTestSearchQueryL(aBuf, aMode);

    // Send a search request
	iAsyncHandler->SearchL(*iPsQuery);
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::InputSearchL() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: InputSearchL(const CPsQuery& aSearchQuery, const TDesC& aInput, RPointerArray<TDesC>& aResultSet)
{
	SetCurrentErrorCode(KErrNotFound);
	
	//to obtain the result;
	RPointerArray<TDesC> matchSet;
	RArray<TPsMatchLocation> matchLocation;
	
	// send a search request
	iAsyncHandler->SearchL(aSearchQuery, aInput, matchSet, matchLocation);
	
	// compare the obtained result with desired result set
	if(matchSet.Count() == aResultSet.Count())
	{
		TInt count = matchSet.Count();
		if(count == 0)
		{
			SetCurrentErrorCode(KErrNone);
		}
		for(TInt i = 0; i < count; i++)	
		{
			TBufC<20> buf1(*(aResultSet[i]));
			for(TInt j = 0; j < count; j++)
			{
				TBufC<20> buf2(*(matchSet[j]));
				if(buf1.Compare(buf2) == 0)
				{
					SetCurrentErrorCode(KErrNone);
					break;
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::SetSearchSettingsL() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: SetSearchSettingsL(RPointerArray<TDesC>& aSearchUri, RArray<TInt>& aDisplayFields, TInt aMaxResults)
{
    iSettings->SetSearchUrisL(aSearchUri);
    iSettings->SetDisplayFieldsL(aDisplayFields);
    iSettings->SetMaxResults(aMaxResults);

    iAsyncHandler->SetSearchSettingsL(*iSettings);
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::FormTestSearchQueryL() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: FormTestSearchQueryL(const TDesC& aBuf, TKeyboardModes aMode)
{
	for ( int i = 0; i < aBuf.Length(); i++ )
	{
	    CPsQueryItem* item = CPsQueryItem::NewL();
	    item->SetCharacter(aBuf[i]);
	 	if(aMode != EModeUndefined)
	 	{
	 		item->SetMode(aMode);	 		
	 	}
		iPsQuery->AppendL(*item);			
	}
	
	CPsQueryItem* item1 = CPsQueryItem::NewL();
	item1->SetCharacter(aBuf[0]);
	item1->SetMode(EItut);
	
	iPsQuery->InsertL(*item1,0);
	
	iPsQuery->Remove(0);
	
	iPsQuery->InsertL(*item1,aBuf.Length()+2);
	
	iPsQuery->Remove(-1);
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::HandlePsResultsUpdate() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: HandlePsResultsUpdate(RPointerArray<CPsClientData>& searchResults, RPointerArray<TDesC>& /*searchSeqs*/)
{
	// search results should always be a subset of iContactslist
	TInt itemstoshow = searchResults.Count();	
    switch(iCurrentTestCase)
    {
    	case EAddContact:
    	{
			VerifyResult(searchResults, ETrue);
    		break;
    	}
    	case EDeleteContact:
    	case EAddContactNeg:
    	case EFilter:
    	{
    		VerifyResult(searchResults, EFalse);
    		break;
    	}
    	/*
    	case EPsData:
    	{
    		SwitchEPsData(searchResults);
    		break;
    	}
    	*/
    	case ETruncateResult:
    	{
    		SetCurrentErrorCode(KErrGeneral);
    		if(itemstoshow == (iContactsList.Count() - 2))
    		{
    		//	SetCurrentErrorCode(KErrNone);
    			VerifyResult(searchResults, ETrue);
    		}
    		break;
    	}
    	case ESort:
    	{
    		SetCurrentErrorCode(KErrNotFound);

    		// first verify if the contact items created are present in result set
    		VerifyResult(searchResults, ETrue);
    		
    		if(CurrentErrorCode() == KErrNone)
    		{
    			// then verify if the result is sorted or not
	    		if( !VerifySortResult(searchResults) )
	    		{
	    			SetCurrentErrorCode(KErrGeneral);
	    		}
    		}
    		
    		break;
    	}
    	default:
    	{
    		SetCurrentErrorCode(KErrNone);
    		break;
    	}
    }
	CActiveScheduler::Stop();
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::ShutDownL() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: ShutDownL()
{
	iAsyncHandler->ShutdownServerL();
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::Cancel() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: Cancel()
{
	iAsyncHandler->CancelSearch();
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::Version() 
// 
// ---------------------------------------------------------------------------------
TVersion CPsTestQueryHandler :: Version()
{
	return (iAsyncHandler->Version());
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::IsLanguageSupportedL() 
// 
// ---------------------------------------------------------------------------------
TBool CPsTestQueryHandler::IsLanguageSupportedL(TLanguage aLang)
{
	return(iAsyncHandler->IsLanguageSupportedL(aLang));
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::HandlePsErrorL() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: HandlePsError(TInt /*aErrorCode*/)
{
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::SetContactsDetails() 
// 
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: SetContactsDetails(RPointerArray<CPsClientData>& aContactsList)
{
	for(TInt i =0 ; i < aContactsList.Count();i++)
	{
		iContactsList.Append(aContactsList[i]);
	}
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::VerifyResult() 
// 
// This function verifies 
// 	for positive test case: if the contacts/ groups created are present in result set 
//  for negative test case: contacts/ groups created are not be present in result set
// ---------------------------------------------------------------------------------
void CPsTestQueryHandler :: VerifyResult(RPointerArray<CPsClientData>& aSearchResults, TBool aResultType)
{
	SetCurrentErrorCode(KErrGeneral);
	if(aSearchResults.Count() == 0 && !aResultType)
	{
		// result match is 0 AND is negative test case 
		SetCurrentErrorCode(KErrNone);
		return;
	}
	
	for(TInt i = 0; i < aSearchResults.Count(); i++)
	{
		for(TInt j = 0; j < iContactsList.Count(); j++)
		{
			if(CompareByData(*(aSearchResults[i]), *(iContactsList[j])) == 0)
			{
				iContactsList[j]->SetId(aSearchResults[i]->Id());
				if(aResultType)
				{
					SetCurrentErrorCode(KErrNone);
					break;
				}
				else
				{
					SetCurrentErrorCode(KErrGeneral);
					break;
				}
			}
			else
			{
				if(aResultType)
				{
					SetCurrentErrorCode(KErrGeneral);
				}
				else
				{
					SetCurrentErrorCode(KErrNone);
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::IndexForGroupDataL() 
// 
// 
// ---------------------------------------------------------------------------------
TInt CPsTestQueryHandler::IndexForGroupDataL()
{
	TInt grpIndex;
	RArray<TInt> dataOrder;
	TBufC<30> groupUri(KGroupUri);
	iAsyncHandler->GetDataOrderL(groupUri, dataOrder);
	
	for(TInt i=0; i < dataOrder.Count(); i++)
	{
		if(dataOrder[i] == R_VPBK_FIELD_TYPE_LASTNAME)
		{
			grpIndex = i;
		}
	}
	
	return grpIndex;
}

// ---------------------------------------------------------------------------------
// CPsTestQueryHandler::VerifySortResult() 
// 
// This function verifies if the result set is sorted or not
// ---------------------------------------------------------------------------------
TBool CPsTestQueryHandler::VerifySortResult(RPointerArray<CPsClientData>& aSearchResults)
{
	TBool isSorted = EFalse;
	TInt i = 0;
	
	for(i = 0; i < aSearchResults.Count() - 1 ; i++)
	{
		if(CompareByData(*(aSearchResults[i]), *(aSearchResults[i+1])) > 0)	
		{
			break;
		}
	}
	
	if( i == aSearchResults.Count() - 1)
	{
		isSorted = ETrue;
	}
	return isSorted;
}

void CPsTestQueryHandler::SwitchEPsData(RPointerArray<CPsClientData>& searchResults)
{
	TInt itemstoshow = searchResults.Count();		
	SetCurrentErrorCode(KErrNotFound);
	for(TInt i = 0; i < itemstoshow; i++)
	{
        /*
        InsertContactId(searchResults[i]->Id(),i);
        if((iFirstName.Compare(searchResults[i]->Data(0)->Des()) == 0) && (iLastName.Compare(searchResults[i]->Data(1)->Des())==0) && (iCompanyName.Compare(searchResults[i]->Data(2)->Des())==0))
        {
            if(searchResults[i]->IsDataMatch(0) == EFalse && searchResults[i]->IsDataMatch(1) == EFalse && searchResults[i]->IsDataMatch(2))
            {
                SetCurrentErrorCode(KErrNone);
            }

            TBool fl = searchResults[i]->IsWordMatch(2,0);
        }
        */
		for(TInt j = 0; j < iContactsList.Count(); j++)
		{
			// compare searchResult[i] with iContactsList[j]
            /*
		 	if((CompareByData(*(searchResults[i]), *(iContactsList[j])) == 0) &&
			(searchResults[i]->IsDataMatch(0) == EFalse && searchResults[i]->IsDataMatch(1) == EFalse && searchResults[i]->IsDataMatch(2)))
			{
				SetCurrentErrorCode(KErrNone);
				iContactsList[j]->SetId(searchResults[i]->Id());
				break;
			}
			else
			{
				SetCurrentErrorCode(KErrNotFound);
			}
            */
		}
	}
}

void CPsTestQueryHandler::CachingStatus(TCachingStatus& /*aStatus*/, TInt& /*aError*/)
{
	// implement later
}

TInt CPsTestQueryHandler::CompareByData ( const CPsClientData& aObject1, const CPsClientData& aObject2 )
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
