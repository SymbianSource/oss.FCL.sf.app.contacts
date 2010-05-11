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
                                                     RPointerArray<CPsData>& SearchResults)
    {
    CleanupClosePushL( SearchResults );
    
    TInt maxIndex = 0;
    TInt maxValue = aSearchResultsArr[maxIndex]->Count();
    TLinearOrder<CPsData> rule(CPcsAlgorithm2Utils::CompareDataBySortOrder);

    // Find the largest array in aSearchResultsArr
    for (TInt i = 1; i < aSearchResultsArr.Count(); i++)
        {
        if (aSearchResultsArr[i]->Count() > maxValue)
            {
            maxIndex = i;
            maxValue = aSearchResultsArr[i]->Count();
            }
        }

    // Assign the largets array to searchresults 
    for (TInt i = 0; i < aSearchResultsArr[maxIndex]->Count(); i++)
        {
        SearchResults.Append((*(aSearchResultsArr[maxIndex]))[i]);
        }

    // Merge the remaining result arrays to the largest array in sequential order
    for (TInt i = 0; i < aSearchResultsArr.Count(); i++)
        {
        // Check if we are not copying again the largest array
        if ((i != maxIndex) && ((aSearchResultsArr[i])->Count() != 0))
            {
            TInt numElements = (aSearchResultsArr[i])->Count();
            for (TInt j = 0; j < numElements; j++)
                {
                SearchResults.InsertInOrderAllowRepeatsL((*(aSearchResultsArr[i]))[j], rule);
                }
            }
        }
    CleanupStack::Pop();
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
// CPcsAlgorithm2Utils::CompareDataBySortOrder()
// TLinearOrder rule for comparison of data objects
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm2Utils::CompareDataBySortOrder(const CPsData& aObject1,
                                                 const CPsData& aObject2)
    {
    _LIT(KSpace, " ");

    // Fetch the cache list stored in TLS to recover the sort order
    typedef RPointerArray<CPcsCache> PTR;
    PTR* pcsCache = static_cast<PTR*> (Dll::Tls());

    // Data1
    TBuf<255> data1(KNullDesC);
    TInt uriId1 = aObject1.UriId();
    CPcsCache* cache = (*pcsCache)[uriId1];

    RArray<TInt> indexOrder;

    // Get the index order based on sort order from the cache
    cache->GetIndexOrder(indexOrder);

    // Append sort order elements first
    for (int i = 0; i < indexOrder.Count(); i++)
        {
        TInt index = indexOrder[i];
        if (index < aObject1.DataElementCount() && aObject1.Data(index))
            {
            // Trim the unnecessary white spaces/special chars before we compare
            TBuf<255> str(KNullDesC);

            str = aObject1.Data(index)->Des();
            CPcsAlgorithm2Utils::MyTrim(str);

            data1 += str;
            data1 += KSpace;
            }
        }

    /* --- NOT SURE IF THIS BEHAVIOR IS REQUIRED ---
     // Append remaining elements in order
     for ( int i = 0; i < aObject1.DataElementCount(); i++ )
     {
     if ( indexOrder.Find(i) == KErrNone && aObject1.Data(i) )
     {
     data1 += aObject1.Data(i)->Des();
     data1 += KSpace;
     }	     
     }
     */

    // Data2
    TBuf<255> data2(KNullDesC);
    TInt uriId2 = aObject2.UriId();
    cache = (*pcsCache)[uriId2];

    indexOrder.Reset();

    // Get the index order based on sort order from the cache
    cache->GetIndexOrder(indexOrder);

    // Append sort order elements first
    for (int i = 0; i < indexOrder.Count(); i++)
        {
        TInt index = indexOrder[i];
        if (index < aObject2.DataElementCount() && aObject2.Data(index))
            {
            // Trim the unnecessary white spaces/special chars before we compare
            TBuf<255> str(KNullDesC);

            str = aObject2.Data(index)->Des();
            CPcsAlgorithm2Utils::MyTrim(str);

            data2 += str;
            data2 += KSpace;
            }
        }

    /* --- NOT SURE IF THIS BEHAVIOR IS REQUIRED ---
     // Append remaining elements in order
     for ( int i = 0; i < aObject2.DataElementCount(); i++ )
     {
     if ( indexOrder.Find(i) == KErrNone && aObject2.Data(i) )
     {
     data2 += aObject2.Data(i)->Des();
     data2 += KSpace;
     }	     
     }
     */

    indexOrder.Reset();
    data1.TrimAll();
    data2.TrimAll();
    return (CPcsAlgorithm2Utils::MyCompareC(data1, data2));
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

// End of File

