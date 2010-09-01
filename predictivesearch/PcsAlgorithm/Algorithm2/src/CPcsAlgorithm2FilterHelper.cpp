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
* Description: Helper class to filter results
*
*/

// INCLUDES
#include "CPcsAlgorithm2FilterHelper.h"
#include "CPcsDebug.h"

// Compare functions
TInt ComparePsPattern(const TPsPatternDetails& aFirst,
                      const TPsPatternDetails& aSecond)
    {
    return (CPcsAlgorithm2Utils::MyCompareC(*(aFirst.matchPattern),
                                            *(aSecond.matchPattern)));

    }

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm2FilterHelper* CPcsAlgorithm2FilterHelper::NewL(TSortType aSortType)
    {
    PRINT ( _L("Enter CPcsAlgorithm2FilterHelper::NewL") );

    CPcsAlgorithm2FilterHelper* self = new (ELeave) CPcsAlgorithm2FilterHelper();
    CleanupStack::PushL(self);
    self->ConstructL(aSortType);
    CleanupStack::Pop(self);

    PRINT ( _L("End CPcsAlgorithm2FilterHelper::NewL") );

    return self;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::CPcsAlgorithm2FilterHelper
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAlgorithm2FilterHelper::CPcsAlgorithm2FilterHelper()
    {
    PRINT ( _L("Enter CPcsAlgorithm2FilterHelper::CPcsAlgorithm2FilterHelper") );
    PRINT ( _L("End CPcsAlgorithm2FilterHelper::CPcsAlgorithm2FilterHelper") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPcsAlgorithm2FilterHelper::ConstructL(TSortType aSortType)
    {
    PRINT ( _L("Enter CPcsAlgorithm2FilterHelper::ConstructL") );

    iSortType = aSortType;
    iResultCount = -1;

    PRINT ( _L("End CPcsAlgorithm2FilterHelper::ConstructL") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::~CPcsAlgorithm2FilterHelper
// Destructor
// ----------------------------------------------------------------------------
CPcsAlgorithm2FilterHelper::~CPcsAlgorithm2FilterHelper()
    {
    PRINT ( _L("Enter CPcsAlgorithm2FilterHelper::~CPcsAlgorithm2FilterHelper") );

    // delete match pattern pools
    for (TInt i = 0; i < iMatchPatternPools.Count(); i++)
        {
        iMatchPatternPools[i]->Reset();
        delete iMatchPatternPools[i];
        iMatchPatternPools[i] = NULL;
        }
    iMatchPatternPools.Reset();

    // delete match pattern details
    for (TInt i = 0; i < iMatchPatternDetails.Count(); i++)
        {
        delete iMatchPatternDetails[i]->matchPattern;
        for (TInt j = 0; j < iMatchPatternDetails[i]->subPatternArray.Count(); j++)
            {
            delete iMatchPatternDetails[i]->subPatternArray[j]->matchPattern;
            iMatchPatternDetails[i]->subPatternArray[j]->matchPattern = NULL;
            }
        iMatchPatternDetails[i]->subPatternArray.ResetAndDestroy();
        }
    iMatchPatternDetails.ResetAndDestroy();

    PRINT ( _L("End CPcsAlgorithm2FilterHelper::~CPcsAlgorithm2FilterHelper") );
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::AddL
// 
// ----------------------------------------------------------------------------
void CPcsAlgorithm2FilterHelper::AddL(CPsData* aPsData, RPointerArray<TDesC>& aPatternSequence)
    {
    if (iSortType == EAlphabetical)
        {
        // Create pool if not already present 
        if (iMatchPatternPools.Count() == 0)
            {
            RPointerArray<CPsData> *tempPsDataArray = new (ELeave) RPointerArray<CPsData> ();
            CleanupStack::PushL( tempPsDataArray );
            iMatchPatternPools.AppendL(tempPsDataArray);
            CleanupStack::Pop( tempPsDataArray );
            }
        // Add result to the result set
        iMatchPatternPools[0]->AppendL(aPsData);

        // Update the count
        iResultCount++;

        // Update sequence list
        const TInt patternSequenceCount = aPatternSequence.Count();
        for (TInt i = 0; i < patternSequenceCount; i++)
            {
            TInt index = FindSequence(*aPatternSequence[i]);
            if (index == KErrNotFound)
                {
                // sequence not found, add it to array
                TPsPatternDetails* temp = new (ELeave) TPsPatternDetails;
                CleanupStack::PushL( temp );
                
                temp->matchPattern = aPatternSequence[i]->AllocL();

                // First occurence should be -1 for alphabetical sort
                // and pool index will be 0 as only one pool will be created
                temp->firstOccurence = -1;
                temp->poolIndex = 0;

                iMatchPatternDetails.AppendL(temp);
                CleanupStack::Pop( temp );
                }
            }
        }
    else
        {
        // PatternBased sort
        TInt index = AddToPoolL(aPsData, aPatternSequence);

        // Increment the total count
        iResultCount++;
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::AddToPoolL
// 
// ----------------------------------------------------------------------------

TInt CPcsAlgorithm2FilterHelper::AddToPoolL(CPsData* aPsData, RPointerArray<TDesC>& aPatternSequence)
    {

    // Sort the pattern sequence
    TLinearOrder<TDesC> rule(CPcsAlgorithm2Utils::MyCompareC);
    aPatternSequence.Sort(rule);

    // Sort rule        
    TLinearOrder<CPsData> psDataRule(CPcsAlgorithm2Utils::CompareDataBySortOrder);

    // Search Rule
    TIdentityRelation<CPsData> identitySearchRule(CPsData::CompareById);

    TInt poolItemCount = -1;
    TInt matchpatterpoolIndexToAppend = -1;

    const TInt patternSequenceCount = aPatternSequence.Count();
    for (TInt cnt = 0; cnt < patternSequenceCount; cnt++)
        {

        // Create the pattern for aPatternSequence[cnt] in  iMatchPatternDetails
        // and return the index
        TInt indexInMatchPatternDetails = CreateMatchPatternDetailsAndPoolsL(*aPatternSequence[cnt]);

        //Add the data to the pool the first pattern pool.
        // The data should be added only once
        if (cnt == 0)
            {
            TInt poolIndex = iMatchPatternDetails[indexInMatchPatternDetails]->poolIndex;
            matchpatterpoolIndexToAppend = indexInMatchPatternDetails;
            iMatchPatternPools[poolIndex]->InsertInOrderAllowRepeats(aPsData, psDataRule);
            TInt findposition = iMatchPatternPools[poolIndex]->Find(aPsData, identitySearchRule);
            if (findposition != KErrNotFound)
                {
                poolItemCount = findposition;
                }
            else
                {
                poolItemCount = -1;
                }

            }
        else if (cnt > 0)
            {
            // Check if aPatternSequence[cnt] is listed in subpatterns of aPatternSequence[0]
            // If not, then create a sub pattern and apped it to  iMatchPatternDetails for aPatternSequence[0]
            TInt subSeq = FindSubSequence(*aPatternSequence[cnt], matchpatterpoolIndexToAppend);
            if (subSeq == KErrNotFound)
                {
                //Create the subpattern and append it.
                TPsSubPattern* tempSubPattern = new (ELeave) TPsSubPattern;
                tempSubPattern->matchPattern = HBufC::NewL(aPatternSequence[cnt]->Length());
                tempSubPattern->matchPattern->Des().Copy(*(aPatternSequence[cnt]));
                tempSubPattern->firstOccurenceInThisPattern = poolItemCount;
                iMatchPatternDetails[matchpatterpoolIndexToAppend]->subPatternArray.AppendL(tempSubPattern);
                }
            }
        }

    return 0;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::CreateMatchPatternDetailsAndPoolsL
// Creates the Matchpattern aSeq in  iMatchPatternDetails.
// Returns the index of the sequence in iMatchPatternDetails
// ----------------------------------------------------------------------------

TInt CPcsAlgorithm2FilterHelper::CreateMatchPatternDetailsAndPoolsL(const TDesC& aSeq)
    {

    TInt indexInMatchPatternDetails = FindSequence(aSeq);
    if (indexInMatchPatternDetails == KErrNotFound)
        {
        // sequence not found, add it to array
        TPsPatternDetails* tempPatternDetailsInstance = new (ELeave) TPsPatternDetails;

        //TInt len = aPatternSequence[cnt]->Length();
        tempPatternDetailsInstance->matchPattern = aSeq.Alloc();
        tempPatternDetailsInstance->firstOccurence = -1; //RAVIKIRAN

        // Pools doesn't exist for this sequence..
        RPointerArray<CPsData> *tempPsDataArray = new (ELeave) RPointerArray<CPsData> ();

        tempPatternDetailsInstance->poolIndex = iMatchPatternPools.Count();
        iMatchPatternDetails.Append(tempPatternDetailsInstance);

        iMatchPatternPools.Append(tempPsDataArray);

        return (iMatchPatternDetails.Count() - 1);
        }
    else
        {
        // Pool already exists for this sequence, get the correct index and add the result
        return indexInMatchPatternDetails;
        }

    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::GetResults
// 
// ----------------------------------------------------------------------------

void CPcsAlgorithm2FilterHelper::GetResults(RPointerArray<CPsData>& aSearchResults)
    {
    // No results, simply return
    if (iResultCount == -1)
        {
        return;
        }

    // Sort sequences
    TLinearOrder<TPsPatternDetails> rule(ComparePsPattern);
    iMatchPatternDetails.Sort(rule);

    if (iSortType == EAlphabetical)
        {
        // Return the results stored in single array
        const TInt cnt = iMatchPatternPools[0]->Count();
        for (TInt i = 0; i < cnt; i++)
            {
            aSearchResults.Append((*(iMatchPatternPools[0]))[i]);
            }

        // Clear the match pattern pool for the next data store
        iMatchPatternPools[0]->Reset();
        }
    else
        {
        TInt firstOccurenceTracker = 0;
        const TInt matchPatternDetailsCount = iMatchPatternDetails.Count();
        for (TInt i = 0; i < matchPatternDetailsCount; i++)
            {
            // Get the mapping pool index for this sequence
            TInt index = iMatchPatternDetails[i]->poolIndex;
            TInt j = 0;
            const TInt poolCount = iMatchPatternPools[index]->Count();
            for (; j < poolCount; j++)
                {
                aSearchResults.Append((*(iMatchPatternPools[index]))[j]);
                }

            // Check the sub patterns
            const TInt subPatternArrayCount =  iMatchPatternDetails[i]->subPatternArray.Count();
            for (TInt k = 0; k < subPatternArrayCount; k++)
                {
                HBufC* temporseq = iMatchPatternDetails[i]->subPatternArray[k]->matchPattern;
                TRAP_IGNORE( UpdateForSubSequencesL( *temporseq, 
                    firstOccurenceTracker + 
                    iMatchPatternDetails[i]->subPatternArray[k]->firstOccurenceInThisPattern ) );
                }

            if (iMatchPatternDetails[i]->firstOccurence == -1)
                {
                iMatchPatternDetails[i]->firstOccurence = firstOccurenceTracker;
                }

            firstOccurenceTracker = firstOccurenceTracker + j;

            }
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::GetPatternsL
// 
// ----------------------------------------------------------------------------
void CPcsAlgorithm2FilterHelper::GetPatternsL(
        RPointerArray<CPsPattern>& aPatternSet)
    {
    // Sort sequences
    TLinearOrder<TPsPatternDetails> rule(ComparePsPattern);
    iMatchPatternDetails.Sort(rule);

    const TInt matchPatternDetailsCount = iMatchPatternDetails.Count(); 
    for (TInt i = 0; i < matchPatternDetailsCount; i++)
        {
        CPsPattern *tempPattern = CPsPattern::NewL();

        // Copy the sequence
        tempPattern->SetPatternL(*(iMatchPatternDetails[i]->matchPattern));
        tempPattern->SetFirstIndex(iMatchPatternDetails[i]->firstOccurence);

        aPatternSet.Append(tempPattern);
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::FindSequence
// Searches for for aSeq in iMatchPatternDetails and returns index 
//  where input sequence is found. Returns -1 if not found
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm2FilterHelper::FindSequence(const TDesC& aSeq)
    {
    TInt j = 0;
    const TInt matchPatternDetailsCount =  iMatchPatternDetails.Count();
    for (; j < matchPatternDetailsCount; j++)
        {
        const TDesC& matchPattern = *(iMatchPatternDetails[j]->matchPattern);
        if ( CPcsAlgorithm2Utils::MyCompareC(aSeq, matchPattern) == 0 )
            break;
        }

    if (j == iMatchPatternDetails.Count())
        return KErrNotFound;
    else
        return j;
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::FindSubSequence
// Searches for for seq in subpatterns of iMatchPatternDetails[aPatternIndex]
// and returns index where input sequence is found. 
// Returns -1 if not found
// ----------------------------------------------------------------------------
TInt CPcsAlgorithm2FilterHelper::FindSubSequence(const TDesC& aSeq, TInt aPatternIndex)
    {
    TInt j = 0;
    const TInt subPatternArraycount =  iMatchPatternDetails[aPatternIndex]->subPatternArray.Count();
    for (; j < subPatternArraycount; j++)
        {
        const TDesC& matchPattern = *(iMatchPatternDetails[aPatternIndex]->subPatternArray[j]->matchPattern);
	    if ( CPcsAlgorithm2Utils::MyCompareC(aSeq, matchPattern) == 0 ) 
            break;
        }

    if (j == iMatchPatternDetails[aPatternIndex]->subPatternArray.Count())
        {
        return KErrNotFound;
        }
    else
        {
        return j;
        }
    }

// ----------------------------------------------------------------------------
// CPcsAlgorithm2FilterHelper::UpdateForSubSequencesL
// Searches for all the subsequences of aSeq and updates the aFirstOccrVal in 
// iMatchPatternDetails.
//
// For ex: if aSeq = "ABC" and aFirstOccrVal= 2, 
// then, firstOccurence of all substring sequences ("A", "AB", "ABC") 
//       in iMatchPatternDetails will be set to 2
// ----------------------------------------------------------------------------
void CPcsAlgorithm2FilterHelper::UpdateForSubSequencesL(const TDesC& aSeq, TInt aFirstOccrVal)
    {
    TPtrC tempSequence;
    for ( TInt i = 1; i <= aSeq.Length(); i++ )
        {
        // Get the next substring in tempSequence
        tempSequence.Set( aSeq.Left(i) );

        // Find if this sequence exist in iMatchPatternDetails
        TInt patternIndex = FindSequence(tempSequence);

        //If the sequence exits and firstOccurence == -1, then set it to aFirstOccrVal 
        if ((patternIndex != KErrNotFound)&& (iMatchPatternDetails[patternIndex]->firstOccurence == -1))
            {
            iMatchPatternDetails[patternIndex]->firstOccurence = aFirstOccrVal;
            }

        }
    }
// END OF FILE

