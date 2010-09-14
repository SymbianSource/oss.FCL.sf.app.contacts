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
#include "CPcsDebug.h"

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
    TInt compareRes = 0;

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

    // Check if Sort Order is identical (it must be for same cache)
    TBool sameIndexOrder = ETrue;
    if ( indexOrder1.Count() != indexOrder2.Count() )
        {
        sameIndexOrder = EFalse;
        }
    else
        {
        for ( TInt i = 0; i < indexOrder1.Count(); i++ )
            {
            if (indexOrder1[i] != indexOrder2[i])
                {
                sameIndexOrder = EFalse;
                break;
                }
            }
        }

    // Sort Orders among different caches should be the same, anyway
    // if caches and SO are different we compare by cache URI index
    if (!sameIndexOrder)
        {
        compareRes = aObject1.UriId() - aObject2.UriId();

        CleanupStack::PopAndDestroy(&indexOrder2);
        CleanupStack::PopAndDestroy(&indexOrder1);
        return compareRes;
        }

    // The comparison between contacts data is done for the sort order
    // fields skipping the ones that are empty
    TInt indexCount = indexOrder1.Count();
    TInt idx1 = 0;
    TInt idx2 = 0;
    while ( compareRes == 0 && idx1 < indexCount && idx2 < indexCount )
        {
        // Get contact field of 1st contact
        TInt object1Idx = indexOrder1[idx1];
        HBufC* strCompare1 = aObject1.Data(object1Idx)->Des().AllocLC();
        TPtr strCompare1Ptr( strCompare1->Des() );
        CPcsAlgorithm1Utils::MyTrim( strCompare1Ptr );

        // Get contact field of 2nd contact
        TInt object2Idx = indexOrder2[idx2];
        HBufC* strCompare2 = aObject2.Data(object2Idx)->Des().AllocLC();
        TPtr strCompare2Ptr( strCompare2->Des() );
        CPcsAlgorithm1Utils::MyTrim( strCompare2Ptr );

        if ( strCompare1->Length() > 0 && strCompare2->Length() > 0 )
            {
            compareRes = CPcsAlgorithm1Utils::MyCompareC(*strCompare1, *strCompare2);
            idx1++;
            idx2++;
            }
        else // Increment only the index of the contact with empty field
            {
            if ( strCompare1->Length() == 0 )
                idx1++;
            if ( strCompare2->Length() == 0 )
                idx2++;
            }

        CleanupStack::PopAndDestroy(strCompare2);
        CleanupStack::PopAndDestroy(strCompare1);
        }

    // We do not return that contacts are equal by SO
    if ( compareRes == 0 )
        {
        if ( idx1 != idx2 )
            {
            // Compare by index position
            // If idx1 > idx2 and SO is "FN LN" it means for instance that:
            //     Contact1=[FN:"",      LN:"Smith"],  idx1=2
            //     Contact2=[FN:"Smith", LN:"Donald"], idx2=1
            // Therefore Contact1="Smith" is < than Contact2="Smith Donald"
            // and the return value of this method has to be < 0 (idx2-idx1)

            compareRes = idx2 - idx1;
            }
        else
            {
            // Compare by URI ID as 1st choice and Contact ID as 2nd choice

            compareRes == ( aObject1.UriId() != aObject2.UriId() ) ?
                aObject1.UriId() - aObject2.UriId() : aObject1.Id() - aObject2.Id();
            }
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

