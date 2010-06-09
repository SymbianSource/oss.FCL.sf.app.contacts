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
#include "CPsQuery.h"
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
												     RPointerArray<CPsData>& aSearchResults)
{
    CleanupClosePushL( aSearchResults );
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
    	aSearchResults.Append((*(aSearchResultsArr[maxIndex]))[i]);
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
    			aSearchResults.InsertInOrderAllowRepeatsL((*(aSearchResultsArr[i]))[j], rule);
    		}
    	}
    }
    CleanupStack::Pop();
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::CompareByCharacter()
// Compare by character.
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm1Utils::CompareByCharacter( const TChar& aFirst, const TChar& aSecond )
{
    TInt retValue;
    if ( aFirst == aSecond )
        {
        retValue = 0;
        }
    else if ( aFirst < aSecond )
        {
        retValue = -1;
        }
    else 
        {
        retValue = 1;
        }

    return retValue;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::CompareByLength()
// Compare by length.
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm1Utils::CompareByLength( const HBufC& aFirst, const HBufC& aSecond )
{
    return ( aFirst.Length() - aSecond.Length() );
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::CompareExact()
// Compare strings exactly case sensitively.
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm1Utils::CompareExact( const TDesC& aFirst, const TDesC& aSecond )
{
    return aFirst == aSecond;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::CompareCollate()
// Compare strings with collate rules depending on locale.
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm1Utils::CompareCollate( const TDesC& aFirst, const TDesC& aSecond )
{
    return CPcsAlgorithm1Utils::MyCompareC(aFirst, aSecond);
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::MyCompareKeyAndString()
// Compare for keys and strings:
// - Case sensitive compare for keys,
// - Case insensitive and language dependent compare for Contact Data and Query.
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm1Utils::MyCompareKeyAndString(const TDesC& aContactString,
                                                 const TDesC& aQueryAsString,
                                                 CPsQuery& aPsQuery)
{
    TBool comparison = MyCompareK(aContactString, aQueryAsString, aPsQuery); 
        
    if (comparison)
        {
        // The aContactString can be longer than aQueryAsString and we want a match to be
        // successful if both strings are equal for the length of the query (aQueryAsString)
        comparison = (MyCompareC(aContactString.Left(aQueryAsString.Length()), aQueryAsString) == 0);
        }
    
    return comparison;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::MyCompareK()
// Case sensitive compare for keys of first 2 input params.
// INFO: We have some cases that TPtiKey "Z" (uppercase) is mapping chars "Zz...."
//       and TPtiKey "z" (lowercase) is mapping chars ".," or "Ää".
//       The comparison of "Z" and "z" should fail for the keys.
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
            CPsQueryItem* currentItemPtr;
            TRAPD ( err, currentItemPtr = &(aPsQuery.GetItemAtL(i)) );
            if ( err != KErrNone
                 || ( CPcsKeyMap::IsModePredictive(currentItemPtr->Mode())
                    && aLeft[i] != aRight[i] ) )
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

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Helper::FilterDataFieldsL()
// Constructs a bit pattern using the required/supported data fields
// For example, 6, 4 and 27 are supported fields <-- 00000111
//              6 and 4 are required fields      <-- 00000011
// Bit pattern returned is 00000011.
// ----------------------------------------------------------------------------
TUint8 CPcsAlgorithm1Utils::FilterDataFieldsL(const RArray<TInt>& aRequiredDataFields,
                                              const RArray<TInt>& aSupportedDataFields)
{
    TUint8 filteredMatch = 0x0;

    for ( TInt i = 0; i < aSupportedDataFields.Count(); i++ )
    {
        for ( TInt j = 0; j < aRequiredDataFields.Count(); j++ )
        {
            if ( aSupportedDataFields[i] == aRequiredDataFields[j] )
            {
                TUint8 val = 1 << i;
                filteredMatch |= val;
            }
        }
    }

    return filteredMatch;
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::AppendMatchToSeqL
// ----------------------------------------------------------------------------
void CPcsAlgorithm1Utils::AppendMatchToSeqL( 
        RPointerArray<TDesC>& aMatchSeq, const TDesC& aMatch )
    {
    CleanupResetAndDestroyPushL( aMatchSeq );
    
    HBufC* seq = aMatch.AllocLC();
    seq->Des().UpperCase();
    TIdentityRelation<TDesC> rule(CompareExact);
    if ( aMatchSeq.Find(seq, rule) == KErrNotFound )
        {
        aMatchSeq.AppendL(seq);
        CleanupStack::Pop( seq );
        }
    else 
        {
        CleanupStack::PopAndDestroy( seq );
        }
    CleanupStack::Pop( &aMatchSeq );
    }

// End of File

