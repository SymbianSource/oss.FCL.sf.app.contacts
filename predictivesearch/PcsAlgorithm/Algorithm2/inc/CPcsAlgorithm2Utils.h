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
* Description:  Identifies a contact in cache pools
*
*/

#ifndef C_PCS_ALGORITHM_2_UTILS
#define C_PCS_ALGORITHM_2_UTILS

// INCLUDE FILES

#include <e32hashtab.h>

#include "CPsData.h"

// FORWARD DECLARATION

typedef RPointerArray<CPsData> CPSDATA_R_PTR_ARRAY;

// CLASS DECLARATION
class CPcsAlgorithm2Utils : public CBase
    {
public:

    /**
     * Merge function to merge the search results from different arrays
     * into a single array in sorted order.
     * It merges all the result sets that are there in iSearchResultsArr
     * into single result set. This is a wrapper funciton implemented that
     * works for all type of key board modes
     */
    static void FormCompleteSearchResultsL(RPointerArray<CPSDATA_R_PTR_ARRAY>& aSearchResultsArr,
                                           RPointerArray<CPsData>& SearchResults);

    /**
     * Customized CompareC         
     */
    static TInt MyCompareC(const TDesC& aLeft, const TDesC& aRight);

    /**
     * TLinearOrder rule for comparison of data objects
     */
    static TInt CompareDataBySortOrder(const CPsData& aObject1, const CPsData& aObject2);

    /**
     * Trim off all white spaces and special chars
     */
    static void MyTrim(TDes& aString);

    /**
     * Check if the input URI is of contact search in a group template form
     */
    static TBool IsGroupUri(TDesC& aURI);

    };

// CleanupStack helpers for item owning RPointerArrays
template <class T>
class CleanupResetAndDestroy
    {
public:
    inline static void PushL(T& aRef)
        { CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef)); }
private:
    inline static void ResetAndDestroy(TAny *aPtr)
        { static_cast<T*>(aPtr)->ResetAndDestroy(); }
    };

template <class T>
inline void CleanupResetAndDestroyPushL(T& aRef)
    { CleanupResetAndDestroy<T>::PushL(aRef); }

#endif // C_PCS_ALGORITHM_2_UTILS
