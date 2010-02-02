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
* Description:  A class that reads PBK2_STORE_INFO_ITEM resource
*
*/



// INCLUDE FILES
#include "CPbk2StoreInfoItem.h"
#include <barsread.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbk2StoreInfoItem::CPbk2StoreInfoItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2StoreInfoItem::CPbk2StoreInfoItem()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2StoreInfoItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPbk2StoreInfoItem::ConstructL(TResourceReader& aReader)
    {
    iItemType = aReader.ReadInt32();
    iItemTextSingular = aReader.ReadHBufC16L();
    iItemTextPlural = aReader.ReadHBufC16L();
    iTextType = static_cast<TPbk2StoreInfoItemTextType>(aReader.ReadInt8());
    iFlags = aReader.ReadUint32();
    }

// -----------------------------------------------------------------------------
// CPbk2StoreInfoItem::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreInfoItem* CPbk2StoreInfoItem::NewLC(
        TResourceReader& aReader)
    {
    CPbk2StoreInfoItem* self = new( ELeave ) CPbk2StoreInfoItem;
    CleanupStack::PushL( self );
    self->ConstructL(aReader);
    return self;
    }

    
// Destructor
CPbk2StoreInfoItem::~CPbk2StoreInfoItem()
    {
    delete iItemTextSingular;
    delete iItemTextPlural;
    }

// -----------------------------------------------------------------------------
// CPbk2StoreInfoItem::ItemType
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2StoreInfoItem::ItemType() const
    {
    return iItemType;
    }
    
// -----------------------------------------------------------------------------
// CPbk2StoreInfoItem::ItemTextSingular
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPbk2StoreInfoItem::ItemTextSingular() const
    {
    if (iItemTextSingular)
        {
        return *iItemTextSingular;
        }
    else
        {
        return KNullDesC;
        }
    }

// -----------------------------------------------------------------------------
// CPbk2StoreInfoItem::ItemTextPlural
// -----------------------------------------------------------------------------
//    
EXPORT_C const TDesC& CPbk2StoreInfoItem::ItemTextPlural() const
    {
    if (iItemTextPlural)
        {
        return *iItemTextPlural;
        }
    else
        {
        return KNullDesC;
        }
    }
    
// -----------------------------------------------------------------------------
// CPbk2StoreInfoItem::TextType
// -----------------------------------------------------------------------------
//
EXPORT_C TPbk2StoreInfoItemTextType CPbk2StoreInfoItem::TextType() const
    {
    return iTextType;
    }
    
// -----------------------------------------------------------------------------
// CPbk2StoreInfoItem::Flags
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CPbk2StoreInfoItem::Flags() const
    {
    return iFlags;
    }
    
//  End of File  
