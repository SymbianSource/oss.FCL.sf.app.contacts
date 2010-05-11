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

#include "CPsData.h"
#include "CPcsCache.h"
#include "CPcsDebug.h"
#include "CWords.h"
#include "CPcsAlgorithm1Utils.h"


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsCache::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsCache* CPcsCache::NewL(const TDesC& aURI, CPcsKeyMap& aKeyMap, TUint8 aUriId)
{
    PRINT ( _L("Enter CPcsCache::NewL") );
    
	CPcsCache* instance= new ( ELeave ) CPcsCache();

	CleanupStack::PushL( instance );

	instance->ConstructL(aURI, aKeyMap, aUriId);

	CleanupStack::Pop( instance );

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
void CPcsCache::ConstructL(const TDesC& aURI, CPcsKeyMap& aKeyMap, TUint8 aUriId)
{
    PRINT ( _L("Enter CPcsCache::ConstructL") );

    PRINT2 ( _L("CPcsCache::ConstructL: aURI=%S, aUriId=%d"), &aURI, aUriId);
    
    iURI = aURI.AllocL();
    iUriId = aUriId;
    //Update the caching status for this cache
    iCacheStatus = ECachingNotStarted;
    
    iKeyMap = &aKeyMap;        

    // Populate iKeyArr
    for(TInt i= 0; i <aKeyMap.PoolCount();i++ )
        {
        RPointerArray<CPcsPoolElement> *keyMap = new (ELeave) RPointerArray<CPcsPoolElement>(1);
        iKeyArr.InsertL(keyMap,i);
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
    CleanupClosePushL( aData );
    if ( aKeyId >= 0 && aKeyId < iKeyArr.Count() )
        {
        const RPointerArray<CPcsPoolElement>& arr = *iKeyArr[aKeyId];
        for ( TInt i = 0; i < arr.Count(); i++ )
            {
            aData.AppendL(arr[i]);
            }
        }
    CleanupStack::Pop();
    PRINT ( _L("End CPcsCache::GetContactsForKeyL") );
    }

// ----------------------------------------------------------------------------
// CPcsCache::GetAllContentsL
// Get all data elements in this cache
// ----------------------------------------------------------------------------     
void CPcsCache::GetAllContentsL(RPointerArray<CPsData>& aData)
{
    PRINT ( _L("Enter CPcsCache::GetAllContentsL") );
    CleanupClosePushL( aData );	
	for ( TInt i = 0; i < iMasterPool.Count(); i++ )
	{
        aData.AppendL(iMasterPool[i]);
	}
	CleanupStack::Pop();
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
     TLinearOrder<CPcsPoolElement> rule( CPcsPoolElement::CompareByData );

     // Parse thru each data element
     for ( TInt dataIndex = 0; dataIndex < aData.DataElementCount(); dataIndex++ )
     {
        // Find store all the pool IDs where this contact should be
		RArray<TUint> poolIds;
		CleanupClosePushL( poolIds );
		
		// Recover the first character
		if ( aData.Data(dataIndex) && aData.Data(dataIndex)->Length() != 0 )
		{
		    // Split the data into words
		    CWords* words = CWords::NewLC(*aData.Data(dataIndex));

		    // Store the first numeric key for each word
		    for ( TInt i = 0; i < words->MdcaCount(); i++ )
		    {
		    	TChar firstChar = (words->MdcaPoint(i))[0];
		    	
		    	// Pool ID according to ITU-T mappings
		    	if ( iKeyMap->IsItutPredictiveAvailable() )
		    	    {
                    TInt itutPoolId = iKeyMap->PoolIdForCharacter(firstChar, EPredictiveItuT);
                    if ( itutPoolId != KErrNotFound )
                        {
                        poolIds.Append(itutPoolId);
                        }
		    	    }
		    	
		    	// Pool ID according to QWERTY mappings
                if ( iKeyMap->IsQwertyPredictiveAvailable() )
                    {
                    TInt qwertyPoolId = iKeyMap->PoolIdForCharacter(firstChar, EPredictiveQwerty);
                    if ( qwertyPoolId != KErrNotFound )
                        {
                        poolIds.Append(qwertyPoolId);
                        }
                    }
		    }
		    
		    CleanupStack::PopAndDestroy(words); 
		}
		
		for ( TInt poolIdIndex = 0; poolIdIndex < poolIds.Count(); poolIdIndex++ )
		{
            TUint poolId = poolIds[poolIdIndex];
		    CPcsPoolElement* element = NULL;
		    
		    // Check if an element already exists in the pool for this data
		    TInt* loc = NULL;
		    loc = elementHash.Find(poolId);
		    if ( loc != NULL )
		    {
		        // Exists. Then recover ...
		        const RPointerArray<CPcsPoolElement>& tmpKeyMap = *(iKeyArr[poolId]);
		    	element = tmpKeyMap[*loc];
		    }
		
			if ( element == NULL ) // Pool element doesn't exist. Create new ...
			{
		        element = CPcsPoolElement::NewL(aData);
		        CleanupStack::PushL( element );
		    	element->ClearDataMatchAttribute();
				element->SetDataMatch(dataIndex);
				
				// Insert to pool
				iKeyArr[poolId]->InsertInOrderAllowRepeatsL(element, rule);
				CleanupStack::Pop( element ); // ownership transferred
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
		
		CleanupStack::PopAndDestroy( &poolIds );
		
     } // for 1 loop
     
     CleanupStack::PopAndDestroy( &elementHash );
}

// ---------------------------------------------------------------------
// CPcsCache::AddToCacheL
// 
// ---------------------------------------------------------------------
void CPcsCache::AddToCacheL( CPsData& aData )
{
    // Protect against duplicate items getting added
    if ( iCacheInfo.Find(aData.Id()) != NULL )
    {
        return;
    }
    
    // Include this element in the pool
    TUint64 poolMap = 0;
    AddToPoolL(poolMap, aData);
    iCacheInfo.InsertL(aData.Id(), poolMap);

    // Include this element in master pool
    TLinearOrder<CPsData> rule( CPcsAlgorithm1Utils::CompareDataBySortOrderL );
    
    iMasterPool.InsertInOrderAllowRepeatsL(&aData, rule);
}

// ---------------------------------------------------------------------
// CPcsCache::RemoveContactL
// 
// ---------------------------------------------------------------------
void CPcsCache::RemoveFromCacheL( TInt aItemId )
{
    CPsData* data = NULL;
    
    TUint64* poolMap = iCacheInfo.Find(aItemId);
    
    if ( poolMap == NULL )
    {
    	return;
    }
    
    // Remove this element from pools
    for ( TInt keyIndex = 0; keyIndex < iKeyArr.Count(); keyIndex++ )
    {
        TBool present = GetPoolMap(*poolMap, keyIndex); 

        if ( !present )
        {
        	continue;
        }

        const RPointerArray<CPcsPoolElement>& tmpKeyMap = *(iKeyArr[keyIndex]);
        for ( TInt arrayIndex = 0; 
              arrayIndex < tmpKeyMap.Count();
              arrayIndex++ )
        {
		    CPcsPoolElement *element = tmpKeyMap[arrayIndex];
		    TInt id = element->GetPsData()->Id();
		    if ( id == aItemId )
		    {
		        data = element->GetPsData();
		    	delete element;
		    	iKeyArr[keyIndex]->Remove(arrayIndex);  
		    }
        }
    }
        
    // Remove this element from master pool
    TInt arrayIndex = 0;
    while ( arrayIndex < iMasterPool.Count() )
    {
        if ( iMasterPool[arrayIndex]->Id() == aItemId )
        {
            iMasterPool.Remove(arrayIndex);  
        }
        else
        {
            arrayIndex++;
        }
    }
    
    // Delete data 
    delete data;
    data = NULL;

    // Clear up cache information
    iCacheInfo.Remove(aItemId);
}

// ---------------------------------------------------------------------
// CPcsCache::RemoveAllFromCache
// 
// ---------------------------------------------------------------------
void CPcsCache::RemoveAllFromCache()
{
    PRINT ( _L("Enter CPcsCache::RemoveAllFromCache") );
    
    for ( TInt i = 0 ; i < iKeyArr.Count() ; i++ )
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
    
	for ( TInt i = 0; i < iSortOrder.Count(); i++ )
	{
		for ( TInt j = 0; j < iDataFields.Count(); j++ )
		{
			if ( iSortOrder[i] == iDataFields[j] )
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
    for (TInt i = 0; i < iMasterPool.Count(); i++ )
        {
        iMasterPoolBackup.Append( iMasterPool[i] );
        }
    //Now reset the key array
    for (TInt i = 0; i < iKeyArr.Count(); i++ )
        {
        iKeyArr[i]->ResetAndDestroy();
        }
    iMasterPool.Reset();
    iCacheInfo.Close();
    //now add data again from the iMasterPoolBackup
    for (TInt i = 0; i < iMasterPoolBackup.Count(); i++ )
        {
        CPsData* temp = iMasterPoolBackup[i];
        AddToCacheL( *temp );
        }
    iMasterPoolBackup.Reset();
    }

// End of file
