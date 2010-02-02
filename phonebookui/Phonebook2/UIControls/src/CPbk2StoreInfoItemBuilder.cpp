/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class for building store info items
*
*/


#include "CPbk2StoreInfoItemBuilder.h"

// Phonebook2
#include "MPbk2StoreInfoUiItem.h"
#include <CPbk2StoreInfoItemArray.h>
#include <CPbk2StoreInfoItem.h>
#include <CPbk2StorePropertyArray.h>
#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>
#include <CPbk2StoreProperty.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2StorePropertyContentStoreName.h>
#include <Pbk2StoreProperty.hrh>

// Virtual Phonebook
#include <MVPbkContactStoreInfo.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactStoreUriArray.h>

// System includes
#include <barsread.h>
#include <coemain.h>
#include <StringLoader.h>
#include <avkon.rsg>

// CONSTANTS

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EBuildSingleStoreItemsL_PreCond
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2StoreInfoItemBuilder");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

const TInt KMaxIntSize = 16;
const TInt KKByte = 1024;
typedef TInt (MVPbkContactStoreInfo::*StoreInfoFunction)() const;

// Create mappings that doesn't interfere with definitions
// in Pbk2StoreProperty.hrh -> they start from 0x01
// There is no "max number of contact" item in the UI but
// this is needed to calculate free slots
const TUint32 KPbk2StoreInfoMaxNumberOfContacts = 0x10000000;
const TUint32 KPbk2StoreInfoLastMapping = 0xffffffff;

/// Function mapping definitions
struct TPbk2StoreInfoFunctionMapping
    {
    TUint32 iType;
    StoreInfoFunction iInfoFunctionL;
    };

/// Mappings for MVPbkContactStoreInfo functions that return TInt
static const TPbk2StoreInfoFunctionMapping KInfoFunctionMappings[] =
    {
        {
        KPbk2StoreInfoMaxNumberOfContacts,
        &MVPbkContactStoreInfo::MaxNumberOfContactsL
        },
        {
        KPbk2StoreInfoNumberOfContacts,
        &MVPbkContactStoreInfo::NumberOfContactsL
        },
        {
        KPbk2StoreInfoNumberOfGroups,
        &MVPbkContactStoreInfo::NumberOfGroupsL
        },
        {
        KPbk2StoreInfoLastMapping,
        NULL
        }
    };

/**
 * Gets info data.
 *
 * @param aType     Type of info.
 * @param aInfo     Contact store info interface.
 * @return  Requested info data.
 */
TInt GetInfoDataL( TUint32 aType, const MVPbkContactStoreInfo& aInfo )
    {
    TInt ret = KErrNotFound;

    for ( TInt i = 0;
        KInfoFunctionMappings[i].iType != KPbk2StoreInfoLastMapping; ++i)
        {
        if (KInfoFunctionMappings[i].iType & aType)
            {
            ret = (aInfo.*KInfoFunctionMappings[i].iInfoFunctionL)();
            }
        }

    if ( ret == KErrNotFound )
        {
        if (aType & KPbk2StoreInfoReservedMemory)
            {
            ret = I64LOW((aInfo.ReservedMemoryL() + KKByte/2) / KKByte);
            }
        else if (aType & KPbk2StoreInfoFreeMemory)
            {
            ret = I64LOW(aInfo.FreeMemoryL() / KKByte);
            }
        }

    return ret;
    }

/**
 * Store info UI item representation.
 */
NONSHARABLE_CLASS(CUiItem) : public CBase, public MPbk2StoreInfoUiItem
    {
    public: // Destruction

        /**
         * Destructor.
         */
        ~CUiItem();

    public: // Interface

        /**
         * Sets heading text.
         *
         * @param aText     The text to set.
         */
        void SetHeadingText(
                TDesC* aText );

        /**
         * Sets item text.
         *
         * @param aText     The text to set.
         */
        void SetItemText(
                TDesC* aText );

    public: // From MPbk2StoreInfoUiItem
        const TDesC& HeadingText() const;
        const TDesC& ItemText() const;

    private: // Data
        /// Own: Heading text
        TDesC* iHeadingText;
        /// Own: Item
        TDesC* iItem;
    };

// --------------------------------------------------------------------------
// CUiItem::~CUiItem
// --------------------------------------------------------------------------
//
CUiItem::~CUiItem()
    {
    delete iHeadingText;
    delete iItem;
    }

// --------------------------------------------------------------------------
// CUiItem::SetHeadingText
// --------------------------------------------------------------------------
//
void CUiItem::SetHeadingText( TDesC* aText )
    {
    iHeadingText = aText;
    }

// --------------------------------------------------------------------------
// CUiItem::SetItemText
// --------------------------------------------------------------------------
//
void CUiItem::SetItemText( TDesC* aText )
    {
    iItem = aText;
    }

// --------------------------------------------------------------------------
// CUiItem::HeadingText
// --------------------------------------------------------------------------
//
const TDesC& CUiItem::HeadingText() const
    {
    return *iHeadingText;
    }

// --------------------------------------------------------------------------
// CUiItem::ItemText
// --------------------------------------------------------------------------
//
const TDesC& CUiItem::ItemText() const
    {
    return *iItem;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2StoreInfoItemBuilder::CPbk2StoreInfoItemBuilder
// --------------------------------------------------------------------------
//
CPbk2StoreInfoItemBuilder::CPbk2StoreInfoItemBuilder
        ( CVPbkContactManager& aContactManager,
          CPbk2StorePropertyArray& aStoreProperties ) :
            iContactManager( aContactManager ),
            iStoreProperties( aStoreProperties )
    {
    }

// --------------------------------------------------------------------------
// CPbk2StoreInfoItemBuilder::~CPbk2StoreInfoItemBuilder
// --------------------------------------------------------------------------
//
CPbk2StoreInfoItemBuilder::~CPbk2StoreInfoItemBuilder()
    {
    delete iSingleStoreItemDefs;
    delete iAllStoresItemDefs;
    }

// --------------------------------------------------------------------------
// CPbk2StoreInfoItemBuilder::NewL
// --------------------------------------------------------------------------
//
CPbk2StoreInfoItemBuilder* CPbk2StoreInfoItemBuilder::NewL
        ( CVPbkContactManager& aContactManager,
          CPbk2StorePropertyArray& aStoreProperties )
    {
    CPbk2StoreInfoItemBuilder* self = new( ELeave ) CPbk2StoreInfoItemBuilder
        ( aContactManager, aStoreProperties );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StoreInfoItemBuilder::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2StoreInfoItemBuilder::ConstructL()
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (reader, R_PBK2_SINGLE_STORE_INFO_ITEMS);
    iSingleStoreItemDefs = CPbk2StoreInfoItemArray::NewL(reader);
    CleanupStack::PopAndDestroy(); // reader

    CCoeEnv::Static()->CreateResourceReaderLC
        (reader, R_PBK2_ALL_STORES_INFO_ITEMS);
    iAllStoresItemDefs = CPbk2StoreInfoItemArray::NewL(reader);
    CleanupStack::PopAndDestroy(); // reader
    }

// --------------------------------------------------------------------------
// CPbk2StoreInfoItemBuilder::BuildSingleStoreItemsL
// --------------------------------------------------------------------------
//
CArrayPtr<MPbk2StoreInfoUiItem>*
        CPbk2StoreInfoItemBuilder::BuildSingleStoreItemsL
            ( const TVPbkContactStoreUriPtr& aUriPtr )
    {
    MVPbkContactStore* store =
        iContactManager.ContactStoresL().Find(aUriPtr);
    __ASSERT_DEBUG(store, Panic(EBuildSingleStoreItemsL_PreCond));

    const TInt granularity = 10;
    CArrayPtrFlat<MPbk2StoreInfoUiItem>* results =
        new(ELeave) CArrayPtrFlat<MPbk2StoreInfoUiItem>(granularity);
    CleanupStack::PushL(results);
    const CPbk2StoreProperty* prop = iStoreProperties.FindProperty(aUriPtr);
    const TInt count = iSingleStoreItemDefs->Count();
    for (TInt i = 0; i < count; ++i)
        {
        const CPbk2StoreInfoItem& item = iSingleStoreItemDefs->At(i);
        if (prop->MemoryInfoTypes() & item.ItemType())
            {
            MPbk2StoreInfoUiItem* uiItem =
                CreateSingleStoreUiItemLC(*store, *prop, item);
            results->AppendL(uiItem);
            CleanupStack::Pop(); // uiItem
            }
        }
    CleanupStack::Pop(results);
    return results;
    }

// --------------------------------------------------------------------------
// CPbk2StoreInfoItemBuilder::BuildSingleStoreItemsL
// --------------------------------------------------------------------------
//
CArrayPtr<MPbk2StoreInfoUiItem>*
        CPbk2StoreInfoItemBuilder::BuildAllStoresItemsL
            ( CVPbkContactStoreUriArray& aStoreUris )
    {
    const TInt granularity = 10;
    CArrayPtrFlat<MPbk2StoreInfoUiItem>* results =
        new(ELeave) CArrayPtrFlat<MPbk2StoreInfoUiItem>(granularity);
    CleanupStack::PushL(results);

    const TInt storeCount = aStoreUris.Count();
    const TInt itemCount = iAllStoresItemDefs->Count();
    for (TInt i = 0; i < itemCount; ++i)
        {
        TInt sumValue = KErrNotFound;
        const CPbk2StoreInfoItem& item = iAllStoresItemDefs->At(i);
        for (TInt j = 0; j < storeCount; ++j)
            {
            // Get store property
            const CPbk2StoreProperty* prop =
                iStoreProperties.FindProperty(aStoreUris[j]);
            // Get store
            MVPbkContactStore* store =
                iContactManager.ContactStoresL().Find(aStoreUris[j]);

            if (prop->MemoryInfoTypes() & item.ItemType())
                {
                if (item.Flags() & KPbk2StoreInfoOneItemForEachStore)
                    {
                    // Create own item for each store
                    MPbk2StoreInfoUiItem* uiItem =
                        CreateSingleStoreUiItemLC(*store, *prop, item);
                    results->AppendL(uiItem);
                    CleanupStack::Pop(); // uiItem
                    }
                else if (sumValue == KErrNotFound)
                    {
                    // Init sum value with first stores value
                    sumValue = GetInfoDataL(item.ItemType(),
                        store->StoreInfo());
                    }
                else
                    {
                    // Add to sumValue
                    sumValue += GetInfoDataL(item.ItemType(),
                        store->StoreInfo());
                    }
                }

            }

        if (sumValue != KErrNotFound)
            {
            MPbk2StoreInfoUiItem* uiItem =
                CreateUiItemLC(NULL, item, sumValue);
            results->AppendL(uiItem);
            CleanupStack::Pop(); // uiItem
            }
        }

    CleanupStack::Pop(results);
    return results;
    }

// --------------------------------------------------------------------------
// CPbk2StoreInfoItemBuilder::CreateSingleStoreUiItemLC
// --------------------------------------------------------------------------
//
MPbk2StoreInfoUiItem* CPbk2StoreInfoItemBuilder::CreateSingleStoreUiItemLC
        ( MVPbkContactStore& aStore, const CPbk2StoreProperty& aProperty,
          const CPbk2StoreInfoItem& aItem )
    {
    TInt value = 0;
    const MVPbkContactStoreInfo& info = aStore.StoreInfo();
    if (aItem.ItemType() & KPbk2StoreInfoFreeLocations)
        {
        value = GetInfoDataL(KPbk2StoreInfoMaxNumberOfContacts, info) -
            GetInfoDataL(KPbk2StoreInfoNumberOfContacts, info);
        }
    else
        {
        value = GetInfoDataL(aItem.ItemType(), info);
        }

    return CreateUiItemLC(&aProperty, aItem, value);
    }

// --------------------------------------------------------------------------
// CPbk2StoreInfoItemBuilder::CreateUiItemLC
// --------------------------------------------------------------------------
//
MPbk2StoreInfoUiItem* CPbk2StoreInfoItemBuilder::CreateUiItemLC
        ( const CPbk2StoreProperty* aProperty,
          const CPbk2StoreInfoItem& aItem, TInt aIntValue )
    {
    CUiItem* uiItem = new(ELeave) CUiItem;
    CleanupStack::PushL(uiItem);

    HBufC* itemValue = FormatUiItemValueL(aItem, aIntValue);
    uiItem->SetItemText(itemValue); // takes ownership

    HBufC* heading = CreateHeadingL(aProperty, aItem, aIntValue);
    uiItem->SetHeadingText( heading ); // takes ownership

    return uiItem;
    }

// --------------------------------------------------------------------------
// CPbk2StoreInfoItemBuilder::FormatUiItemValueL
// --------------------------------------------------------------------------
//
HBufC* CPbk2StoreInfoItemBuilder::FormatUiItemValueL
        ( const CPbk2StoreInfoItem& aItem,  TInt aValue )
    {
    HBufC* result = NULL;

    TUint memoryItemTypes =
        KPbk2StoreInfoFreeMemory | KPbk2StoreInfoReservedMemory;
    if (aItem.ItemType() & memoryItemTypes)
        {
        result = StringLoader::LoadL(R_QTN_SIZE_KB, aValue);
        }
    else
        {
        TBuf<KMaxIntSize> intBuf;
        intBuf.Num(aValue);
        result = intBuf.AllocL();
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2StoreInfoItemBuilder::CreateHeadingL
// --------------------------------------------------------------------------
//
HBufC* CPbk2StoreInfoItemBuilder::CreateHeadingL
        ( const CPbk2StoreProperty* aProperty,
          const CPbk2StoreInfoItem& aItem, TInt aValue )
    {
    HBufC* retVal = NULL;

    const TInt singleValue = 1;

    if ( aItem.TextType() == EPbk2StoreInfoStoreName && aProperty )
        {
        CPbk2Content* content = aProperty->RetrieveContentLC
            ( EPbk2MemInfoPhone );

        CPbk2StorePropertyContentStoreName* propContent =
            dynamic_cast<CPbk2StorePropertyContentStoreName*>( content );

        HBufC* storeName = NULL;
        if (propContent && propContent->StoreName() != KNullDesC())
            {
            storeName = propContent->StoreName().AllocL();
            }
        else
            {
            // Backup plan -> use store name
            storeName = aProperty->StoreName().AllocL();
            }

        CleanupStack::PopAndDestroy( content );
        retVal = storeName;
        }

    else if (aValue == singleValue)
        {
        retVal = aItem.ItemTextSingular().AllocL();
        }
    else
        {
        retVal = aItem.ItemTextPlural().AllocL();
        }

    return retVal;
    }

// End of File
