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
* Description: Supports initial search feature. 
*
*/

// INCLUDES
#include "CPcsAlgorithm2Utils.h"
#include "CPsData.h"
#include "CPsQuery.h"
#include "CPcsDefs.h"
#include "CPcsCache.h"
#include <collate.h>
// CONSTANTS
_LIT(KSpace, ' ');
// Search contacts in a group URI template
_LIT(KGroupIdUri, "cntdb://c:contacts.gdb?id=");

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Utils::FormCompleteSearchResultsL()
// Merges all the respective data store result sets to single set in sorted order.
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Utils::FormCompleteSearchResultsL(RPointerArray<CPSDATA_R_PTR_ARRAY>& aSearchResultsArr,
                                                     RPointerArray<CPsData>& aSearchResults)
    {
    TInt maxIndex = 0;
    TInt maxValue = aSearchResultsArr[maxIndex]->Count();
    TLinearOrder<CPsData> rule(CPcsAlgorithm2Utils::CompareDataBySortOrderL);

    // Find the largest array in aSearchResultsArr
    const TInt searchResultsArrCount = aSearchResultsArr.Count(); 
    for (TInt i = 1; i < searchResultsArrCount; i++)
        {
        if (aSearchResultsArr[i]->Count() > maxValue)
            {
            maxIndex = i;
            maxValue = aSearchResultsArr[i]->Count();
            }
        }

    // Assign the largets array to searchresults 
    const TInt cnt = aSearchResultsArr[maxIndex]->Count();
    for (TInt i = 0; i < cnt; i++)
        {
        aSearchResults.Append((*(aSearchResultsArr[maxIndex]))[i]);
        }

    // Merge the remaining result arrays to the largest array in sequential order

    for (TInt i = 0; i < searchResultsArrCount; i++)
        {
        // Check if we are not copying again the largest array
        if ((i != maxIndex) && ((aSearchResultsArr[i])->Count() != 0))
            {
            TInt numElements = (aSearchResultsArr[i])->Count();
            for (TInt j = 0; j < numElements; j++)
                {
                aSearchResults.InsertInOrderAllowRepeatsL((*(aSearchResultsArr[i]))[j], rule);
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Utils::MyCompareC()
// Lang specific MyCompareC
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm2Utils::MyCompareC(const TDesC& aLeft, const TDesC& aRight)
    {
    // Get the current language
    TLanguage lang = User::Language();

    // Get the standard method
    TCollationMethod meth = *Mem::CollationMethodByIndex(0);
    meth.iFlags |= TCollationMethod::EIgnoreNone;
    meth.iFlags |= TCollationMethod::EFoldCase;

    if (lang == ELangHindi || lang == ELangMarathi)
        {
        meth.iFlags |= TCollationMethod::EIgnoreCombining;
        }

    // Collation level 3 is used
    TInt comparison(aLeft.CompareC(aRight, 3, &meth));
    return comparison;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Utils::CompareDataBySortOrderL()
// TLinearOrder rule for comparison of data objects
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm2Utils::CompareDataBySortOrderL(const CPsData& aObject1,
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

    // Sort Orders among diffent caches should be the same, anyway
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
        CPcsAlgorithm2Utils::MyTrim( strCompare1Ptr );

        // Get contact field of 2nd contact
        TInt object2Idx = indexOrder2[idx2];
        HBufC* strCompare2 = aObject2.Data(object2Idx)->Des().AllocLC();
        TPtr strCompare2Ptr( strCompare2->Des() );
        CPcsAlgorithm2Utils::MyTrim( strCompare2Ptr );

        if ( strCompare1->Length() > 0 && strCompare2->Length() > 0 )
            {
            compareRes = CPcsAlgorithm2Utils::MyCompareC(*strCompare1, *strCompare2);
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
// CPcsAlgorithm2Utils::CompareExact()
// 
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm2Utils::CompareExact(const TDesC& aFirst, const TDesC& aSecond)
    {
    return aFirst == aSecond;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Utils::CompareLength()
// 
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm2Utils::CompareLength(const CPsQuery& aFirst, const CPsQuery& aSecond)
    {
    CPsQuery& first = const_cast<CPsQuery&> (aFirst);
    CPsQuery& second = const_cast<CPsQuery&> (aSecond);
    
    return (first.Count() - second.Count());
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Utils::MyTrim()
// Trim off all white spaces and special characters
// This behavior is required to mimic the current phonebook sort sequence
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Utils::MyTrim(TDes& aString)
    {
    for (TInt i = aString.Length(); --i >= 0;)
        {
        TChar c = (TChar) aString[i];

        if (!c.IsAlphaDigit())
            {
            aString.Replace(i, 1, KSpace);
            }
        }

    aString.TrimAll();
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Utils::IsGroupUri()
// Check if the input URI is of contact search in a group template form
// ----------------------------------------------------------------------------                         
TBool CPcsAlgorithm2Utils::IsGroupUri(TDesC& aURI)
    {
    TBuf<255> uri(aURI);
    uri.LowerCase();

    TInt index = uri.FindF(KGroupIdUri);

    if (index == KErrNotFound)
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
TUint8 CPcsAlgorithm2Utils::FilterDataFieldsL(const RArray<TInt>& aRequiredDataFields,
                                              const RArray<TInt>& aSupportedDataFields)
{
    TUint8 filteredMatch = 0x0;
   const TInt supportedDataFieldsCount = aSupportedDataFields.Count();
   const TInt requiredDataFieldsCount = aRequiredDataFields.Count(); 
    for ( TInt i = 0; i < supportedDataFieldsCount; i++ )
    {
        for ( TInt j = 0; j < requiredDataFieldsCount; j++ )
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
// CPcsAlgorithm2Utils::AppendMatchToSeqL
// ----------------------------------------------------------------------------
void CPcsAlgorithm2Utils::AppendMatchToSeqL( 
        RPointerArray<TDesC>& aMatchSeq, const TDesC& aMatch )
    {
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
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2Utils::MatchesOverlap
// Check if two match location items have overlapping indices.
// ----------------------------------------------------------------------------
TBool CPcsAlgorithm2Utils::MatchesOverlap( const TPsMatchLocation& aFirst, 
                                           const TPsMatchLocation& aSecond )
    {
    TBool overlap = EFalse;
    
    if ( aFirst.index == aSecond.index )
        {
        overlap = ETrue;
        }
    else
        {
        // give arguments alias names where first begins before the second
        TInt firstPos( aFirst.index );
        TInt firstLen( aFirst.length );
        TInt secondPos( aSecond.index );
        if ( firstPos > secondPos )
            {
            firstPos = aSecond.index;
            firstLen = aSecond.length;
            secondPos = aFirst.index;
            }
        
        // there is an overlap if the end of the first comes after
        // beginning of the second
        if ( firstPos + firstLen > secondPos )
            {
            overlap = ETrue;
            }
        }
    
    return overlap;
    }

// End of File

