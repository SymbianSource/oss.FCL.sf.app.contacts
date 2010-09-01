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
* Description:  A class that reads an arrya of PBK2_STORE_INFO_ITEM resource
*
*/



// INCLUDE FILES
#include "CPbk2StoreInfoItemArray.h"

#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>
#include "CPbk2StoreInfoItem.h"
#include <MVPbkContactStoreInfo.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <barsread.h>
#include <coemain.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPbk2StoreInfoItemArray::CPbk2StoreInfoItemArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPbk2StoreInfoItemArray::CPbk2StoreInfoItemArray()
    {
    }

// -----------------------------------------------------------------------------
// CPbk2StoreInfoItemArray::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPbk2StoreInfoItemArray::ConstructL(TResourceReader& aReader)
    {
    const TInt count = aReader.ReadInt16();
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2StoreInfoItem* item = CPbk2StoreInfoItem::NewLC(aReader);
        iItems.AppendL(item);
        CleanupStack::Pop(item);
        }
    }

// -----------------------------------------------------------------------------
// CPbk2StoreInfoItemArray::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreInfoItemArray* CPbk2StoreInfoItemArray::NewL(
        TResourceReader& aReader)
    {
    CPbk2StoreInfoItemArray* self = new( ELeave ) CPbk2StoreInfoItemArray;
    CleanupStack::PushL(self);
    self->ConstructL(aReader);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPbk2StoreInfoItemArray::~CPbk2StoreInfoItemArray()
    {
    iItems.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CPbk2StoreInfoItemArray::Count
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2StoreInfoItemArray::Count() const
    {
    return iItems.Count();
    }

// -----------------------------------------------------------------------------
// CPbk2StoreInfoItemArray::At
// -----------------------------------------------------------------------------
//
EXPORT_C const CPbk2StoreInfoItem& CPbk2StoreInfoItemArray::At(
        TInt aIndex) const
    {
    return *iItems[aIndex];
    }

//  End of File
