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
* Description: 
*     Field ordering manager for a Phonebook 2 field properties.
*
*/


// INCLUDES
#include "CPbk2FieldOrderingManager.h"

// From VirtualPhonebook
#include <MVPbkFieldType.h>
#include <barsread.h>

// LOCAL

#ifdef _DEBUG
namespace {

    enum TPanic
        {
        EPanic_PostCond_ItemIndex = 1
        };
        
    void Panic( TPanic aPanicCode )
        {
        _LIT( KPanicCat, "CPbk2FieldOrderingManager" );
        User::Panic( KPanicCat, aPanicCode );
        }
}
#endif // _DEBUG


// --------------------------------------------------------------------------
// CPbk2FieldOrderingManager::TFieldTypeOrder::TFieldTypeOrder
// --------------------------------------------------------------------------
//
CPbk2FieldOrderingManager::TFieldTypeOrder::TFieldTypeOrder( 
    TResourceReader& aReader )
    {
    iFieldTypeResId = aReader.ReadInt32();
    iOrderingItem = aReader.ReadInt8();
    iAddItemOrdering = aReader.ReadInt8();
    }
    
// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CPbk2FieldOrderingManager::CPbk2FieldOrderingManager
// --------------------------------------------------------------------------
//
CPbk2FieldOrderingManager::CPbk2FieldOrderingManager()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2FieldOrderingManager::~CPbk2FieldOrderingManager
// --------------------------------------------------------------------------
//
CPbk2FieldOrderingManager::~CPbk2FieldOrderingManager()
    {
    iOrderingItems.Close();
    }
        
// --------------------------------------------------------------------------
// CPbk2FieldOrderingManager::NewL
// --------------------------------------------------------------------------
//
CPbk2FieldOrderingManager* CPbk2FieldOrderingManager::NewL(
        TResourceReader& aReader )
    {
    CPbk2FieldOrderingManager* self = 
        new (ELeave) CPbk2FieldOrderingManager();
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    CleanupStack::Pop( self ); 
    return self;
    }
        
// --------------------------------------------------------------------------
// CPbk2FieldOrderingManager::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2FieldOrderingManager::ConstructL( TResourceReader& aReader )
    {
    const TInt itemCount = aReader.ReadInt16();
    for ( TInt i = 0; i < itemCount; ++i )
        {
        iOrderingItems.AppendL( TFieldTypeOrder( aReader ) );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldOrderingManager::OrderingItem
// --------------------------------------------------------------------------
//
TInt CPbk2FieldOrderingManager::OrderingItem
    ( const MVPbkFieldType& aFieldType )
    {
    TInt fieldTypeResId = aFieldType.FieldTypeResId();
    TInt index = ItemIndex( aFieldType.FieldTypeResId() );
    return iOrderingItems[index].iOrderingItem;
    }

// --------------------------------------------------------------------------
// CPbk2FieldOrderingManager::AddItemOrdering
// --------------------------------------------------------------------------
//
TInt CPbk2FieldOrderingManager::AddItemOrdering
    ( const MVPbkFieldType& aFieldType )
    {
    TInt fieldTypeResId = aFieldType.FieldTypeResId();
    TInt index = ItemIndex( aFieldType.FieldTypeResId() );
    return iOrderingItems[index].iAddItemOrdering;
    }

// --------------------------------------------------------------------------
// CPbk2FieldOrderingManager::ItemIndex
// --------------------------------------------------------------------------
//
TInt CPbk2FieldOrderingManager::ItemIndex( TInt aFieldTypeResId )
    {
    const TInt count = iOrderingItems.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iOrderingItems[i].iFieldTypeResId == aFieldTypeResId )
            {
            return i;
            }
        }
    /// Must not come here. If it does then a new field type has been
    /// added to the system but not to Pbk2FieldOrdering.rss
    __ASSERT_DEBUG( EFalse, Panic( EPanic_PostCond_ItemIndex ) );
    return KErrNotFound;
    }

// End of file
