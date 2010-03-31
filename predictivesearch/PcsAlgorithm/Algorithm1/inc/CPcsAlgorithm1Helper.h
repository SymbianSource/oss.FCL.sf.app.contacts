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
* Description:  Predictive Contact Search Algorithm 1 main class
*
*/


#ifndef C_PCS_ALGORITHM_1_HELPER
#define C_PCS_ALGORITHM_1_HELPER

// INCLUDES
#include <e32cmn.h>
#include <e32math.h>
#include <e32hashtab.h>

#include "CPcsPlugin.h"
#include "CPsData.h"
#include "CPcsCache.h"
#include "CPcsKeyMap.h"
#include "CPsQuery.h"
#include "CPcsAlgorithm1FilterHelper.h"
#include "mdatastoreobserver.h"

// FORWARD DECLARATION
class CPcsAlgorithm1;
class CPsData;
class CPsDataPluginInterface;
class CPsQuery;

// CLASS DECLARATION
class CPcsAlgorithm1Helper : public CBase
{
    private:

        /**
         * Constructor
         */
        CPcsAlgorithm1Helper();

        /**
         * 2nd phase construtor
         */
        void ConstructL(CPcsAlgorithm1* aAlgorithm);

    public:

        /**
         * Two phase construction
         */
        static CPcsAlgorithm1Helper* NewL(CPcsAlgorithm1* aAlgorithm);

        /**
         * Destructor
         */
        virtual ~CPcsAlgorithm1Helper();

    private:

        /**
         * Filter subset results for ITU-T mode, QWERTY mode, or Mixed (ITU-T and QWERTY) mode.
         */
        void FilterResultsSingleL(CPcsAlgorithm1FilterHelper* aAlgorithmFilterHelper,
                                  RPointerArray<CPcsPoolElement>& searchSet,
                                  CPsQuery& aPsQuery,
                                  TUint8 aFilteredDataMatch,
                                  TBool isSearchInGroup,
                                  RArray<TInt>& aContactsInGroup);

    public:

        /**
         * Search function for query in ITU-T mode, QWERTY mode, or Mixed (ITU-T and QWERTY) mode.
         */
        void SearchSingleL(const CPsSettings& aSettings,
                           CPsQuery& aPsQuery,
                           TBool aIsGroupSearch,
                           RArray<TInt>& aContactsInGroup,
                           RPointerArray<CPsData>& aSearchResults,
                           RPointerArray<CPsPattern>& aSearchSeqs );

        /**
         * Funciton to search matching sequences in the input text.
         */
        void SearchMatchSeqL(CPsQuery& aPsQuery,
                             const TDesC& aData,
                             RPointerArray<TDesC>& aMatchSet,
                             RArray<TPsMatchLocation>& aMatchLocation );

        /**
         * Sort search sequences.
         */
        void SortSearchSeqsL(RPointerArray<TDesC>& aMatchSet);

    private:

        /**
         * Pointer to algorithm instance. Not owned.
         */
        CPcsAlgorithm1* iAlgorithm;

        /**
         * Pointer to key map instance. Not owned.
         */
        CPcsKeyMap* iKeyMap;

        /**
         * Array of result sets from different data stores.
         */
        typedef RPointerArray<CPsData> CPSDATA_R_PTR_ARRAY;
        RPointerArray<CPSDATA_R_PTR_ARRAY> iSearchResultsArr;
};

#endif // C_PCS_ALGORITHM_1_HELPER
