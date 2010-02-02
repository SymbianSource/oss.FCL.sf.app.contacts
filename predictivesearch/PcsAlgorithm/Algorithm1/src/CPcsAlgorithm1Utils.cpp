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
* Description:  Supports initial search feature. 
*
*/


// INCLUDES
#include "CPcsAlgorithm1Utils.h"
#include "CPsData.h"
#include "CPcsDefs.h"
#include "CPcsCache.h"
#include "CPsQueryItem.h"

#include <collate.h>

// CONSTANTS
_LIT(KSpace, ' ');

// Search contacts in a group URI template
_LIT(KGroupIdUri, "cntdb://c:contacts.gdb?id="); 

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::FormCompleteSearchResultsL()
// Merges all the respective data store result sets to single set in sorted order.
// ----------------------------------------------------------------------------
void CPcsAlgorithm1Utils::FormCompleteSearchResultsL(RPointerArray<CPSDATA_R_PTR_ARRAY>& aSearchResultsArr, 
												     RPointerArray<CPsData>& SearchResults)
{
	TInt maxIndex = 0;
    TInt maxValue = aSearchResultsArr[maxIndex]->Count();
    TLinearOrder<CPsData> rule( CPcsAlgorithm1Utils::CompareDataBySortOrderL );
    
    // Find the largest array in aSearchResultsArr
    for(TInt i = 1; i < aSearchResultsArr.Count(); i++)
    {
		if(aSearchResultsArr[i]->Count() > maxValue)
		{
			maxIndex = i;
			maxValue =aSearchResultsArr[i]->Count();
		}    
    }
    
    // Assign the largets array to searchresults 
    for(TInt i = 0; i < aSearchResultsArr[maxIndex]->Count(); i++)
    {
    	SearchResults.Append((*(aSearchResultsArr[maxIndex]))[i]);
    }
    
    // Merge the remaining result arrays to the largest array in sequential order
    for(TInt i = 0; i < aSearchResultsArr.Count(); i++)
    {
        // Check if we are not copying again the largest array
    	if(( i != maxIndex) && ((aSearchResultsArr[i])->Count() != 0))
    	{
    		TInt numElements = (aSearchResultsArr[i])->Count();
    		for( TInt j = 0; j < numElements; j++)
    		{
    			SearchResults.InsertInOrderAllowRepeatsL((*(aSearchResultsArr[i]))[j], rule);
    		}
    	}
    }
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::MyCompareK()
// Case sensitive compare for keys of first 2 input params.
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm1Utils::MyCompareK(const TDesC& aLeft, const TDesC& aRight, CPsQuery& aPsQuery)
{
    TBool comparison = ETrue;

    if ( (aLeft.Length() < aPsQuery.Count()) || (aRight.Length() < aPsQuery.Count()) )
        {
        comparison = EFalse;
        }
    else
        {
        for ( TInt i = 0; i < aPsQuery.Count(); i++ )
            {
            CPsQueryItem& currentItem = aPsQuery.GetItemAtL(i);
            if ( (currentItem.Mode() == EItut) && (aLeft[i] != aRight[i]) )
                {
                comparison = EFalse;
                break;
                }
            }
        }

    return comparison;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::MyCompareC()
// Lang specific MyCompareC
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm1Utils::MyCompareC(const TDesC& aLeft, const TDesC& aRight)
{
    // Get the current language
    TLanguage lang = User::Language();
        
	// Get the standard method
	TCollationMethod meth = *Mem::CollationMethodByIndex( 0 );
	meth.iFlags |= TCollationMethod::EIgnoreNone;
	meth.iFlags |= TCollationMethod::EFoldCase;
    
    if ( lang == ELangHindi || lang == ELangMarathi )
    {
    	meth.iFlags |= TCollationMethod::EIgnoreCombining;    	
    }
   
	// Collation level 0 is used
	TInt comparison(aLeft.CompareC( aRight, 0, &meth ));
	return comparison;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::CompareDataBySortOrderL()
// TLinearOrder rule for comparison of data objects
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm1Utils::CompareDataBySortOrderL(const CPsData& aObject1, 
                                                  const CPsData& aObject2)
    {
    TInt compareRes(0);
    
    if( CPsData::CompareById(aObject1, aObject2) )
        {
        return compareRes;
        }
    
    // Fetch the cache list stored in TLS to recover the sort order
    typedef RPointerArray<CPcsCache> PTR;
    PTR* pcsCache = static_cast<PTR*>(Dll::Tls());
    User::LeaveIfNull(pcsCache);
    
	CPcsCache* cache1 = (*pcsCache)[aObject1.UriId()];
    CPcsCache* cache2 = (*pcsCache)[aObject2.UriId()];
	RArray<TInt> indexOrder1;
    RArray<TInt> indexOrder2;	
	  
    // Get the index order based on sort order from the cache
	cache1->GetIndexOrder(indexOrder1);
	CleanupClosePushL(indexOrder1);
	cache2->GetIndexOrder(indexOrder2);
	CleanupClosePushL(indexOrder2);

	TInt idx2 = 0;
	for(TInt idx1 = 0; 
        idx1 < indexOrder1.Count() && idx2 < indexOrder2.Count() && compareRes == 0; 
        idx1++)
        {
        TInt object1Idx = indexOrder1[idx1];
        HBufC* strCompare1 = aObject1.Data(object1Idx)->Des().AllocLC();
        
        if ( object1Idx < aObject1.DataElementCount() 
             && aObject1.Data(object1Idx)
             && strCompare1->Length() )
            {   
            strCompare1->Des().TrimAll();
            
            for(; idx2 < indexOrder2.Count(); idx2++)
                {
                TInt object2Idx = indexOrder2[idx2];
                
                HBufC* strCompare2 = aObject2.Data(object2Idx)->Des().AllocLC();
                
                if( strCompare2->Length() )
                    {
                    strCompare2->Des().TrimAll();
                    
                    compareRes = CPcsAlgorithm1Utils::MyCompareC(*strCompare1, *strCompare2);
                    
                    CleanupStack::PopAndDestroy(strCompare2);
                    
                    if( compareRes == 0 )
                        {
                        idx2++;
                        }
                    break;
                    }
                CleanupStack::PopAndDestroy(strCompare2);
                }
            }
        CleanupStack::PopAndDestroy(strCompare1);
        }
  
	CleanupStack::PopAndDestroy(&indexOrder2);
    CleanupStack::PopAndDestroy(&indexOrder1);

    return compareRes;
    }


// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::MyTrim()
// Trim off all white spaces and special characters
// This behavior is required to mimic the current phonebook sort sequence
// ----------------------------------------------------------------------------
void CPcsAlgorithm1Utils::MyTrim(TDes& aString)
{           
    for ( TInt i = aString.Length(); --i >= 0; )
    {
        TChar c = (TChar) aString[i];
        
        if ( ! c.IsAlphaDigit() )
        {
            aString.Replace( i, 1, KSpace );
        }
    }
    
    aString.TrimAll();        
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::IsGroupUri()
// Check if the input URI is of contact search in a group template form
// ----------------------------------------------------------------------------                         
TBool CPcsAlgorithm1Utils::IsGroupUri(TDesC& aURI)
{
	TBuf<255> uri(aURI);
    uri.LowerCase();
    
    TInt index = uri.FindF(KGroupIdUri);
    
    if ( index == KErrNotFound )
    {
	    return EFalse;
    }
    
    return ETrue;
}

// End of File

