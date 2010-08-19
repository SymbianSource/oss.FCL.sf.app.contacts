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
* Description:  Holds the contact information in memory. It maintains a 
*                master array of all contacts. It also has 10 pools corresponding
*                to each key id in the keyboard. Based on numeric key char of 
*                first char of firstname/ lastname a contact is added to one of the
*                pools. Implements MDataStoreObserver interface functions to
*                add/ remove contacts.
*
*/

// INCLUDE FILES
#include <MVPbkContactLink.h>
#include <VPbkEng.rsg>

#include "FindUtilChineseECE.h"
#include "CPsData.h"
#include "CPcsCache.h"
#include "CPcsDebug.h"
#include "CWords.h"
#include "CPcsAlgorithm2Utils.h"
#include "CPcsAlgorithm2.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsCache::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsCache* CPcsCache::NewL(CPcsAlgorithm2* aAlgorithm, const TDesC& aURI, 
                           CPcsKeyMap& aKeyMap, TUint8 aUriId)
    {
    PRINT ( _L("Enter CPcsCache::NewL") );

    CPcsCache* instance = new (ELeave) CPcsCache();

    CleanupStack::PushL(instance);

    instance->ConstructL(aAlgorithm, aURI, aKeyMap, aUriId);

    CleanupStack::Pop(instance);

    PRINT ( _L("End CPcsCache::NewL") );

    return instance;
    }

// ----------------------------------------------------------------------------
// CPcsCache::CPcsCache
// Constructor
// ----------------------------------------------------------------------------
CPcsCache::CPcsCache()
    {
    PRINT ( _L("Enter CPcsCache::CPcsCache") );
    PRINT ( _L("End CPcsCache::CPcsCache") );
    }

// ----------------------------------------------------------------------------
// CPcsCache::ConstructL
// 2nd Phase Constructor
// ----------------------------------------------------------------------------
void CPcsCache::ConstructL(CPcsAlgorithm2* aAlgorithm, const TDesC& aURI, 
                           CPcsKeyMap& aKeyMap, TUint8 aUriId)
    {
    PRINT ( _L("Enter CPcsCache::ConstructL") );
    iAlgorithm = aAlgorithm;

    iURI = aURI.AllocL();
    iUriId = aUriId;
    //Update the caching status for this cache
    iCacheStatus = ECachingNotStarted;

    iKeyMap = &aKeyMap;

    // Populate iKeyArr
    const TInt keyMapPoolcount =  aKeyMap.PoolCount();
    for (TInt i = 0; i < keyMapPoolcount; i++)
        {
        RPointerArray<CPcsPoolElement>* keyMap = new (ELeave) RPointerArray<CPcsPoolElement> (1);
        iKeyArr.InsertL(keyMap, i);
        }

    PRINT ( _L("End CPcsCache::ConstructL") );
    }

// ----------------------------------------------------------------------------
// CPcsCache::~CPcsCache
// Destructor
// ----------------------------------------------------------------------------
CPcsCache::~CPcsCache()
    {
    PRINT ( _L("Enter CPcsCache::~CPcsCache") );

    delete iURI;

    RemoveAllFromCache(); // cleans up iMasterPool and iCacheInfo
   
    iKeyArr.ResetAndDestroy();
    iDataFields.Reset();
    iSortOrder.Reset();
    iIndexOrder.Reset();
    iMasterPoolBackup.Close();

    PRINT ( _L("End CPcsCache::~CPcsCache") );
    }

// ----------------------------------------------------------------------------
// CPcsCache::GetContactsForKeyL
// Get list of pool elements specific to a pool
// ----------------------------------------------------------------------------     
void CPcsCache::GetContactsForKeyL(TInt aKeyId, RPointerArray<CPcsPoolElement>& aData)
    {
    PRINT ( _L("Enter CPcsCache::GetContactsForKeyL") );

    if ( aKeyId >= 0 && aKeyId < iKeyArr.Count() )
        {
        const RPointerArray<CPcsPoolElement>& arr = *iKeyArr[aKeyId];
        const TInt arrCount = arr.Count();
        for (TInt i = 0; i < arrCount; i++)
            {
            CPcsPoolElement* value = arr[i];
            aData.AppendL(value);
            }
        }

    PRINT ( _L("End CPcsCache::GetContactsForKeyL") );
    }

// ----------------------------------------------------------------------------
// CPcsCache::GetAllContentsL
// Get all data elements in this cache
// ----------------------------------------------------------------------------     
void CPcsCache::GetAllContentsL(RPointerArray<CPsData>& aData)
    {
    PRINT ( _L("Enter CPcsCache::GetAllContentsL") );
    
    const TInt masterPoolCount =  iMasterPool.Count(); 
    for (TInt i = 0; i < masterPoolCount; i++)
        {
        CPsData* value = iMasterPool[i];
        aData.AppendL(value);
        }

    PRINT ( _L("End CPcsCache::GetAllContentsL") );
    }

// ----------------------------------------------------------------------------
// CPcsCache::AddToPool
// Adds a contact to cache
// ----------------------------------------------------------------------------
void CPcsCache::AddToPoolL(TUint64& aPoolMap, CPsData& aData)
    {
    // Temp hash to remember the location of pool elements
    // First TInt  = Pool 
    // Second TInt = Location in the pool
    // Required for memory optimization so that more than one pool
    // element doesn't get added for the same data
    RHashMap<TInt, TInt> elementHash;
    CleanupClosePushL( elementHash );
    TLinearOrder<CPcsPoolElement> rule(CPcsPoolElement::CompareByData);

    // Parse thru each data element
    const TInt dataElementCount = aData.DataElementCount();
    for (TInt dataIndex = 0; dataIndex < dataElementCount; dataIndex++)
        {
        // Stores first key for each word
        RArray<TChar> firstCharArr;
        CleanupClosePushL( firstCharArr );

        // Recover the first character
        if ( aData.Data(dataIndex) )
            {
            GetFirstCharsForDataL( *aData.Data(dataIndex), firstCharArr );
            }
        
        // Get the corresponding Pool IDs
        RArray<TInt> poolIdArr;
        CleanupClosePushL( poolIdArr );
        GetPoolIdsForCharsL( firstCharArr, poolIdArr );

        const TInt poolIdArrCount = poolIdArr.Count();
        for (TInt poolArrIndex = 0; poolArrIndex < poolIdArrCount ; poolArrIndex++)
            {
            
            TInt poolId = poolIdArr[poolArrIndex];

            CPcsPoolElement* element = NULL;

            // Check if an element already exists in the pool for this data
            TInt* loc = NULL;
            loc = elementHash.Find(poolId);
            if (loc != NULL)
                {
                // Exists. Then recover ...
                RPointerArray<CPcsPoolElement> tmpKeyMap = *(iKeyArr[poolId]);
                element = tmpKeyMap[*loc];
                }

            if (element == NULL) // Pool element doesn't exist. Create new ...
                {
                element = CPcsPoolElement::NewL(aData);
                element->ClearDataMatchAttribute();
                element->SetDataMatch(dataIndex);

                // Insert to pool
                iKeyArr[poolId]->InsertInOrderAllowRepeatsL(element, rule);
                TInt index = iKeyArr[poolId]->FindInOrderL(element, rule);

                // Set the bit for this pool
                SetPoolMap(aPoolMap, poolId);

                // Store the array index in the temp hash
                elementHash.InsertL(poolId, index);
                }
            else // Pool element exists. Just alter the data match attribute
                {
                element->SetDataMatch(dataIndex);

                // Set the bit for this pool
                SetPoolMap(aPoolMap, poolId);
                }

            } // for 2 loop

        CleanupStack::PopAndDestroy( &poolIdArr );    // Close
        CleanupStack::PopAndDestroy( &firstCharArr ); // Close

        } // for 1 loop

    CleanupStack::PopAndDestroy( &elementHash );  // Close
    }

// ---------------------------------------------------------------------
// CPcsCache::AddToCacheL
// 
// ---------------------------------------------------------------------
void CPcsCache::AddToCacheL(CPsData& aData)
    {
    // Protect against duplicate items getting added
    if (iCacheInfo.Find(aData.Id()) != NULL)
        {
        return;
        }

    // Include this element in the pool     
    TUint64 poolMap = 0;
    AddToPoolL(poolMap, aData);
    iCacheInfo.InsertL(aData.Id(), poolMap);

    // Include this element in master pool        
    TLinearOrder<CPsData> rule(CPcsAlgorithm2Utils::CompareDataBySortOrder);
    iMasterPool.InsertInOrderAllowRepeatsL(&aData, rule);
    }

// ---------------------------------------------------------------------
// CPcsCache::RemoveContactL
// 
// ---------------------------------------------------------------------
void CPcsCache::RemoveFromCacheL(TInt aItemId)
    {
    CPsData *data = NULL;

    TUint64* poolMap = iCacheInfo.Find(aItemId);

    if (poolMap == NULL)
        {
        return;
        }

    // Remove this element from pools
    const TInt keyArrCount = iKeyArr.Count();
    for (TInt keyIndex = 0; keyIndex < keyArrCount; keyIndex++)
        {
        TBool present = GetPoolMap(*poolMap, keyIndex);

        if (!present)
            {
            continue;
            }

        const RPointerArray<CPcsPoolElement>& tmpKeyMap = *(iKeyArr[keyIndex]);
        
        for (TInt arrayIndex = 0; arrayIndex < tmpKeyMap.Count(); arrayIndex++)
            {
            CPcsPoolElement *element = tmpKeyMap[arrayIndex];
            TInt id = element->GetPsData()->Id();
            if (id == aItemId)
                {
                data = element->GetPsData();
                delete element;
                iKeyArr[keyIndex]->Remove(arrayIndex);
                }
            }
        }

    // Remove this element from master pool
    for (TInt arrayIndex = 0; arrayIndex < iMasterPool.Count(); arrayIndex++)
        {
        CPsData *dataElement = iMasterPool[arrayIndex];
        TInt id = dataElement->Id();
        if (id == aItemId)
            {
            iMasterPool.Remove(arrayIndex);
            }
        }

    // Delete data 
    if (data)
        {
        delete data;
        data = NULL;
        }

    // Clear up cache information
    iCacheInfo.Remove(aItemId);
    }

// ---------------------------------------------------------------------
// CPcsCache::RemoveAllFromCacheL
// 
// ---------------------------------------------------------------------
void CPcsCache::RemoveAllFromCache()
    {
    PRINT ( _L("Enter CPcsCache::RemoveAllFromCache") );

    const TInt keyArrCount = iKeyArr.Count();
    for ( TInt i = 0 ; i < keyArrCount ; i++ )
        {
        iKeyArr[i]->ResetAndDestroy();
        }

    iMasterPool.ResetAndDestroy();
    iCacheInfo.Close();

    PRINT ( _L("End CPcsCache::RemoveAllFromCache") );
    }

// ---------------------------------------------------------------------
// CPcsCache::SetPoolMap
// 
// ---------------------------------------------------------------------
void CPcsCache::SetPoolMap(TUint64& aPoolMap, TInt aArrayIndex)
    {
    __ASSERT_DEBUG( aArrayIndex < 64, User::Panic(_L("CPcsCache"), KErrOverflow ) );

    /* Some platforms do not support 64 bits shift operations.
     * Split to two 32 bits operations.
     */
    
    TUint32 poolMapH = I64HIGH(aPoolMap);
    TUint32 poolMapL = I64LOW(aPoolMap);
    
    TUint32 valH = 0;
    TUint32 valL = 0;
    if (aArrayIndex < 32)
        {
        valL = 1 << aArrayIndex;
        }
    else
        {
        valH = 1 << (aArrayIndex-32);
        }

    poolMapH |= valH;
    poolMapL |= valL;
    
    aPoolMap = MAKE_TUINT64(poolMapH, poolMapL);
    }

// ---------------------------------------------------------------------
// CPcsCache::GetPoolMap
// 
// ---------------------------------------------------------------------
TBool CPcsCache::GetPoolMap(TUint64& aPoolMap, TInt aArrayIndex)
    {
    __ASSERT_DEBUG( aArrayIndex < 64, User::Panic(_L("CPcsCache"), KErrOverflow ) );

    /* Some platforms do not support 64 bits shift operations.
     * Split to two 32 bits operations.
     */

    TUint32 poolMapH = I64HIGH(aPoolMap);
    TUint32 poolMapL = I64LOW(aPoolMap);
    
    TUint32 valH = 0;
    TUint32 valL = 0;
    if (aArrayIndex < 32)
        {
        valL = 1 << aArrayIndex;
        }
    else
        {
        valH = 1 << (aArrayIndex-32);
        }

    TBool ret = (poolMapH & valH) || (poolMapL & valL);

    return (ret);
    }

// ---------------------------------------------------------------------
// CPcsCache::GetFirstCharsForDataL
// 
// ---------------------------------------------------------------------
void CPcsCache::GetFirstCharsForDataL( const TDesC& aData, RArray<TChar>& aFirstChars ) const
    {
    // Split the data into words
    CWords* words = CWords::NewLC(aData);

    // Find indexing characters for each word
    for (TInt i = 0; i < words->MdcaCount(); i++)
        {
        TPtrC16 word = words->MdcaPoint(i);
        TBool lastCharIsChinese = EFalse;
        
        // If the word contains any Chinese characters, then it is
        // stored to cache according all the available spellings of
        // all the Chinese characters
        if ( iAlgorithm->FindUtilECE()->IsChineseWordIncluded(word) )
            {
            const TInt wordLength = word.Length();
            for (TInt j = 0; j < wordLength; j++)
                {
                TText curChar = word[j];
                RPointerArray<HBufC> spellList;
                CleanupResetAndDestroyPushL( spellList );
                if ( iAlgorithm->FindUtilECE()->DoTranslationL(curChar, spellList) )
                    {
                    lastCharIsChinese = ETrue;
                    // Append first char of each spelling
                    const TInt spellListCount = spellList.Count();
                    for (TInt k = 0; k < spellListCount; k++)
                        {
                        const HBufC* spelling = spellList[k];
                        aFirstChars.AppendL( (*spelling)[0] );
                        }
                    }
                else
                    {
                    if ( lastCharIsChinese )
                        {
                        aFirstChars.AppendL( word[j] );
                        lastCharIsChinese = EFalse;
                        }
                    }
                CleanupStack::PopAndDestroy( &spellList ); // ResetAndDestroy
                }
            }
        
        // If the first charcter of the word is non-Chinese, then it's stored to the
        // cache according this first character. Other characters of the word may or
        // may not be Chinese.
        if ( !iAlgorithm->FindUtilECE()->IsChineseWordIncluded( word.Left(1) ) )
            {
            aFirstChars.AppendL( word[0] );
            }
        }
    CleanupStack::PopAndDestroy(words);
    }

// ---------------------------------------------------------------------
// CPcsCache::GetPoolIdsForCharsL
// 
// ---------------------------------------------------------------------
void CPcsCache::GetPoolIdsForCharsL( const RArray<TChar>& aChars, RArray<TInt>& aPoolIds ) const
    {
    // There can potentially be two pool IDs for each character. 
    // Reserve memory for this upfront to prevent unnecessary reallocations.
    aPoolIds.ReserveL( aChars.Count() * 2 );
    
    const TInt charsCount = aChars.Count() ;
    for ( TInt i = 0 ; i < charsCount ; ++i )
        {
        TChar character = aChars[i];
        TInt itutPoolId = iKeyMap->PoolIdForCharacter( character, EPredictiveItuT );
        if ( itutPoolId != KErrNotFound )
            {
            aPoolIds.AppendL( itutPoolId );
            }
        
        TInt qwertyPoolId = iKeyMap->PoolIdForCharacter( character, EPredictiveQwerty );
        if ( qwertyPoolId != KErrNotFound )
            {
            aPoolIds.AppendL( qwertyPoolId );
            }
        }
    }

// ---------------------------------------------------------------------
// CPcsCache::GetURI
// 
// ---------------------------------------------------------------------
TDesC& CPcsCache::GetURI()
    {
    return (*iURI);
    }

// ---------------------------------------------------------------------
// CPcsCache::SetDataFields
// 
// ---------------------------------------------------------------------
void CPcsCache::SetDataFields(RArray<TInt>& aDataFields)
    {
    const TInt dataFieldsCount = aDataFields.Count();
    for (TInt i(0); i < dataFieldsCount; i++)
        {
        iDataFields.Append(aDataFields[i]);
        }
    }

// ---------------------------------------------------------------------
// CPcsCache::GetDataFields
// 
// ---------------------------------------------------------------------
void CPcsCache::GetDataFields(RArray<TInt>& aDataFields)
    {
    const TInt dataFieldsCount = iDataFields.Count();
    for (TInt i(0); i < dataFieldsCount; i++)
        {
        aDataFields.Append(iDataFields[i]);
        }
    }

// ---------------------------------------------------------------------
// CPcsCache::UpdateCacheStatus
// 
// ---------------------------------------------------------------------
void CPcsCache::UpdateCacheStatus(TInt aStatus)
    {
    iCacheStatus = aStatus;
    }

// ---------------------------------------------------------------------
// CPcsCache::GetCacheStatus
// 
// ---------------------------------------------------------------------
TInt CPcsCache::GetCacheStatus()
    {
    return iCacheStatus;
    }

// ---------------------------------------------------------------------
// CPcsCache::GetUriId
// 
// ---------------------------------------------------------------------
TUint8 CPcsCache::GetUriId()
    {
    return iUriId;
    }

// ---------------------------------------------------------------------
// CPcsCache::GetUri
// 
// ---------------------------------------------------------------------
HBufC* CPcsCache::GetUri()
    {
    return iURI;
    }

// ---------------------------------------------------------------------
// CPcsCache::SetSortOrder
// 
// ---------------------------------------------------------------------
void CPcsCache::SetSortOrder(RArray<TInt>& aSortOrder)
    {
    PRINT ( _L("Enter CPcsCache::SetSortOrder") );

    iSortOrder.Reset();
    const TInt sortOrderCount = aSortOrder.Count();
    for (TInt i(0); i < sortOrderCount; i++)
        {
        iSortOrder.Append(aSortOrder[i]);
        }

    ComputeIndexOrder();

    PRINT ( _L("End CPcsCache::SetSortOrder") );
    }

// ---------------------------------------------------------------------
// CPcsCache::GetSortOrder
// 
// ---------------------------------------------------------------------
void CPcsCache::GetSortOrder(RArray<TInt>& aSortOrder)
    {
    aSortOrder.Reset();
    const TInt sortOrderCount =  iSortOrder.Count();
    for (TInt i(0); i < sortOrderCount; i++)
        {
        aSortOrder.Append(iSortOrder[i]);
        }
    }

// ---------------------------------------------------------------------
// CPcsCache::GetIndexOrder
// 
// ---------------------------------------------------------------------
void CPcsCache::GetIndexOrder(RArray<TInt>& aIndexOrder)
    {
    aIndexOrder.Reset();
    const TInt indexOrderCount = iIndexOrder.Count();
    for (TInt i(0); i < indexOrderCount; i++)
        {
        aIndexOrder.Append(iIndexOrder[i]);
        }
    }

// ---------------------------------------------------------------------
// CPcsCache::ComputeIndexOrder
// 
// ---------------------------------------------------------------------
void CPcsCache::ComputeIndexOrder()
    {
    iIndexOrder.Reset();

    const TInt sortOrderCount = iSortOrder.Count();
    const TInt dataFieldsCount = iDataFields.Count(); 
    for (TInt i = 0; i < sortOrderCount; i++)
        {
        for (TInt j = 0; j <dataFieldsCount; j++)
            {
            if (iSortOrder[i] == iDataFields[j])
                {
                iIndexOrder.Append(j);
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------
// CPcsCache::ResortdataInPools
// 
// ---------------------------------------------------------------------
void CPcsCache::ResortdataInPoolsL()
    {
    // copy iMasterPool data into iMasterPoolBackup
    const TInt masterPoolCount = iMasterPool.Count();
    for (TInt i = 0; i < masterPoolCount; i++ )
        {
        iMasterPoolBackup.Append( iMasterPool[i] );
        }
    //Now reset the key array
    const TInt keyArrCount = iKeyArr.Count(); 
    for (TInt i = 0; i <keyArrCount; i++ )
        {
        iKeyArr[i]->ResetAndDestroy();
        }
    iMasterPool.Reset();
    iCacheInfo.Close();
    //now add data again from the iMasterPoolBackup
    const TInt masterPoolBackupCount = iMasterPoolBackup.Count();
    for (TInt i = 0; i < masterPoolBackupCount; i++ )
        {
        CPsData* temp = iMasterPoolBackup[i];
        AddToCacheL( *temp );
        }
    iMasterPoolBackup.Reset();
    }

// ---------------------------------------------------------------------
// CPcsCache::GetFirstNameIndex
// 
// ---------------------------------------------------------------------
TInt CPcsCache::GetFirstNameIndex() const
    {
    TInt fnIndex = KErrNotFound;
    const TInt dataFieldsCount = iDataFields.Count();
    for ( TInt i = 0 ; i < dataFieldsCount && fnIndex == KErrNotFound ; ++i )
        {
        if ( iDataFields[i] == R_VPBK_FIELD_TYPE_FIRSTNAME )
            {
            fnIndex = i;
            }
        }
    return fnIndex;
    }

// ---------------------------------------------------------------------
// CPcsCache::GetLastNameIndex
// 
// ---------------------------------------------------------------------
TInt CPcsCache::GetLastNameIndex() const
    {
    TInt lnIndex = KErrNotFound;
    const TInt dataFieldsCount = iDataFields.Count();
    for ( TInt i = 0 ; i < dataFieldsCount && lnIndex == KErrNotFound ; ++i )
        {
        if ( iDataFields[i] == R_VPBK_FIELD_TYPE_LASTNAME )
            {
            lnIndex = i;
            }
        }
    return lnIndex;
    }

// End of file
