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
CPcsCache* CPcsCache::NewL(CPcsAlgorithm2* aAlgorithm, TDesC& aURI, 
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
void CPcsCache::ConstructL(CPcsAlgorithm2* aAlgorithm, TDesC& aURI, 
                           CPcsKeyMap& aKeyMap, TUint8 aUriId)
    {
    PRINT ( _L("Enter CPcsCache::ConstructL") );
    iAlgorithm = aAlgorithm;

    iURI = aURI.AllocL();
    iUriId = aUriId;
    //Update the caching status for this cache
    iCacheStatus = ECachingNotStarted;

    keyMap = &aKeyMap;

    // Populate keyArr
    for (TInt i = 0; i < aKeyMap.PoolCount(); i++)
        {
        RPointerArray<CPcsPoolElement> *keyMap = new (ELeave) RPointerArray<CPcsPoolElement> (1);
        keyArr.InsertL(keyMap, i);
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

    if (iURI)
        {
        delete iURI;
        }

    // Loop thru cache info and free and the data elements
    THashMapIter<TInt, TInt> iter(cacheInfo);

    do
        {
        TInt* id = const_cast<TInt*> (iter.NextKey());

        if (id == NULL)
            break;

        TInt* poolMap = iter.CurrentValue();

        if (poolMap == NULL)
            {
            continue;
            }

        CPsData *data = NULL;
        for (int keyIndex = 0; keyIndex <= keyArr.Count(); keyIndex++)
            {
            TBool present = GetPoolMap(*poolMap, keyIndex);

            if (!present)
                {
                continue;
                }

            RPointerArray<CPcsPoolElement> tmpKeyMap = *(keyArr[keyIndex]);
            for (int arrayIndex = 0; arrayIndex < tmpKeyMap.Count(); arrayIndex++)
                {
                CPcsPoolElement *element = tmpKeyMap[arrayIndex];
                TInt localId = element->GetPsData()->Id();
                if (*id == localId)
                    {
                    data = element->GetPsData();
                    delete element;
                    keyArr[keyIndex]->Remove(arrayIndex);
                    }
                }
            };

        // Remove this element from master pool
        for (int arrayIndex = 0; arrayIndex < masterPool.Count(); arrayIndex++)
            {
            CPsData *dataElement = masterPool[arrayIndex];
            TInt localId = dataElement->Id();
            if (*id == localId)
                {
                masterPool.Remove(arrayIndex);
                }
            }

        if (data)
            {
            delete data;
            }

        }
    while (1);

    for (TInt i = 0; i < keyArr.Count(); i++)
        {
        keyArr[i]->ResetAndDestroy();
        delete keyArr[i];
        keyArr[i] = NULL;
        }

    masterPool.ResetAndDestroy();

    cacheInfo.Close();

    keyArr.Reset();
    iDataFields.Reset();
    iSortOrder.Reset();
    iIndexOrder.Reset();

    PRINT ( _L("End CPcsCache::~CPcsCache") );
    }

// ----------------------------------------------------------------------------
// CPcsCache::GetContactsForKeyL
// Get list of pool elements specific to a pool
// ----------------------------------------------------------------------------     
void CPcsCache::GetContactsForKeyL(TInt aKeyId, RPointerArray<CPcsPoolElement>& aData)
    {
    PRINT ( _L("Enter CPcsCache::GetContactsForKeyL") );

    RPointerArray<CPcsPoolElement> arr = *keyArr[aKeyId];
    for (int i = 0; i < arr.Count(); i++)
        {
        CPcsPoolElement* value = arr[i];
        aData.AppendL(value);
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

    for (int i = 0; i < masterPool.Count(); i++)
        {
        CPsData* value = masterPool[i];
        aData.AppendL(value);
        }

    PRINT ( _L("End CPcsCache::GetAllContentsL") );
    }

// ----------------------------------------------------------------------------
// CPcsCache::AddToPool
// Adds a contact to cache
// ----------------------------------------------------------------------------
void CPcsCache::AddToPoolL(TInt& aPoolMap, CPsData& aData)
    {
    // Temp hash to remember the location of pool elements
    // First TInt  = Pool 
    // Second TInt = Location in the pool
    // Required for memory optimization so that more than one pool
    // element doesn't get added for the same data
    RHashMap<TInt, TInt> elementHash;
    TLinearOrder<CPcsPoolElement> rule(CPcsPoolElement::CompareByData);

    // Parse thru each data element    
    for (int dataIndex = 0; dataIndex < aData.DataElementCount(); dataIndex++)
        {
        // Stores first key for each word
        RArray<TChar> firstKey;

        // Recover the first character
        if (aData.Data(dataIndex) && aData.Data(dataIndex)->Length() != 0)
            {
            // Split the data into words	
            CWords* words = CWords::NewLC(*aData.Data(dataIndex));

            // Store the first numeric key for each word
            for (int i = 0; i < words->MdcaCount(); i++)
                {
                TChar firstChar;
                for (int j = 0; j < words->MdcaPoint(i).Length(); j++)
                    {
                    firstChar = (words->MdcaPoint(i))[j];
                    TBuf<20> word;
                    word.Append(firstChar);
                    if (iAlgorithm->FindUtilECE()->IsChineseWord(word))
                        {
                        RPointerArray<HBufC> spellList;
                        if (iAlgorithm->FindUtilECE()->T9ChineseTranslationL(firstChar, spellList))
                            {
                            for (int j = 0; j < spellList.Count(); j++)
                                {
                                firstKey.Append(keyMap->KeyForCharacter(*(spellList[j]->Ptr())));
                                }
                            }
                        else
                            {
                            firstKey.Append( keyMap->KeyForCharacter(firstChar));
                            }
                        spellList.ResetAndDestroy();
                        }
                    else
                        {
                        firstKey.Append(keyMap->KeyForCharacter(firstChar));
                        }
                    }
                }

            CleanupStack::PopAndDestroy(words);
            }

        for (TInt wordIndex = 0; wordIndex < firstKey.Count(); wordIndex++)
            {
            TInt arrayIndex = keyMap->PoolIdForCharacter(firstKey[wordIndex]);

            CPcsPoolElement* element = NULL;

            // Check if an element already exists in the pool for this data
            TInt* loc = NULL;
            loc = elementHash.Find(arrayIndex);
            if (loc != NULL)
                {
                // Exists. Then recover ...
                RPointerArray<CPcsPoolElement> tmpKeyMap = *(keyArr[arrayIndex]);
                element = tmpKeyMap[*loc];
                }

            if (element == NULL) // Pool element doesn't exist. Create new ...
                {
                element = CPcsPoolElement::NewL(aData);
                element->ClearDataMatchAttribute();
                element->SetDataMatch(dataIndex);

                // Insert to pool
                keyArr[arrayIndex]->InsertInOrderAllowRepeatsL(element, rule);
                TInt index = keyArr[arrayIndex]->FindInOrderL(element, rule);

                // Set the bit for this pool					
                SetPoolMap(aPoolMap, arrayIndex);

                // Store the array index in the temp hash
                elementHash.InsertL(arrayIndex, index);
                }
            else // Pool element exists. Just alter the data match attribute
                {
                element->SetDataMatch(dataIndex);

                // Set the bit for this pool					
                SetPoolMap(aPoolMap, arrayIndex);
                }

            } // for 2 loop

        firstKey.Reset();

        } // for 1 loop

    elementHash.Close();
    }

// ---------------------------------------------------------------------
// CPcsCache::AddToCacheL
// 
// ---------------------------------------------------------------------
void CPcsCache::AddToCacheL(CPsData& aData)
    {
    // Protect against duplicate items getting added
    if (cacheInfo.Find(aData.Id()) != NULL)
        {
        return;
        }

    // Include this element in the pool     
    TInt poolMap = 0;
    AddToPoolL(poolMap, aData);
    cacheInfo.InsertL(aData.Id(), poolMap);

    // Include this element in master pool        
    TLinearOrder<CPsData> rule(CPcsAlgorithm2Utils::CompareDataBySortOrder);
    masterPool.InsertInOrderAllowRepeatsL(&aData, rule);
    }

// ---------------------------------------------------------------------
// CPcsCache::RemoveContactL
// 
// ---------------------------------------------------------------------
void CPcsCache::RemoveFromCacheL(TInt aItemId)
    {
    CPsData *data = NULL;

    TInt* poolMap = cacheInfo.Find(aItemId);

    if (poolMap == NULL)
        {
        return;
        }

    // Remove this element from pools
    for (int keyIndex = 0; keyIndex <= keyArr.Count(); keyIndex++)
        {
        TBool present = GetPoolMap(*poolMap, keyIndex);

        if (!present)
            {
            continue;
            }

        RPointerArray<CPcsPoolElement> tmpKeyMap = *(keyArr[keyIndex]);
        for (int arrayIndex = 0; arrayIndex < tmpKeyMap.Count(); arrayIndex++)
            {
            CPcsPoolElement *element = tmpKeyMap[arrayIndex];
            TInt id = element->GetPsData()->Id();
            if (id == aItemId)
                {
                data = element->GetPsData();
                delete element;
                keyArr[keyIndex]->Remove(arrayIndex);
                }
            }
        }

    // Remove this element from master pool
    for (int arrayIndex = 0; arrayIndex < masterPool.Count(); arrayIndex++)
        {
        CPsData *dataElement = masterPool[arrayIndex];
        TInt id = dataElement->Id();
        if (id == aItemId)
            {
            masterPool.Remove(arrayIndex);
            }
        }

    // Delete data 
    if (data)
        {
        delete data;
        data = NULL;
        }

    // Clear up cache information
    cacheInfo.Remove(aItemId);
    }

// ---------------------------------------------------------------------
// CPcsCache::RemoveAllFromCacheL
// 
// ---------------------------------------------------------------------
void CPcsCache::RemoveAllFromCacheL()
    {
    PRINT ( _L("Enter CPcsCache::RemoveAllFromCacheL") );

    for (TInt i = 0; i < keyArr.Count(); i++)
        {
        keyArr[i]->ResetAndDestroy();

        }

    masterPool.ResetAndDestroy();
    cacheInfo.Close();

    PRINT ( _L("End CPcsCache::RemoveAllFromCacheL") );
    }

// ---------------------------------------------------------------------
// CPcsCache::SetPoolMap
// 
// ---------------------------------------------------------------------
void CPcsCache::SetPoolMap(TInt& aPoolMap, TInt arrayIndex)
    {
    TReal val;
    Math::Pow(val, 2, arrayIndex);

    aPoolMap |= (TInt) val;
    }

// ---------------------------------------------------------------------
// CPcsCache::GetPoolMap
// 
// ---------------------------------------------------------------------
TBool CPcsCache::GetPoolMap(TInt& aPoolMap, TInt arrayIndex)
    {
    TReal val;
    Math::Pow(val, 2, arrayIndex);

    return (aPoolMap & (TInt) val);
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
    for (TInt i(0); i < aDataFields.Count(); i++)
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
    for (TInt i(0); i < iDataFields.Count(); i++)
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

    for (TInt i(0); i < aSortOrder.Count(); i++)
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

    for (TInt i(0); i < iSortOrder.Count(); i++)
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

    for (TInt i(0); i < iIndexOrder.Count(); i++)
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

    for (int i = 0; i < iSortOrder.Count(); i++)
        {
        for (int j = 0; j < iDataFields.Count(); j++)
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
    // copy masterPool data into masterPoolBackup
    for (TInt i = 0; i < masterPool.Count(); i++ )
        {
        masterPoolBackup.Append( masterPool[i] );
        }
    //Now reset the key array
    for (TInt i = 0; i < keyArr.Count(); i++ )
        {
        keyArr[i]->ResetAndDestroy();
        }
    masterPool.Reset();
    cacheInfo.Close();
    //now add data again from the masterPoolBackup
    for (TInt i = 0; i < masterPoolBackup.Count(); i++ )
        {
        CPsData* temp = static_cast<CPsData*>(masterPoolBackup[i]);
        AddToCacheL( *temp );
        }
    masterPoolBackup.Reset();
    } 
// End of file
