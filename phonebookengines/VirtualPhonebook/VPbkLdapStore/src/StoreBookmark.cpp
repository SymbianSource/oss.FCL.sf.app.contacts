/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CStoreBookmark implementation
*
*/


// INCLUDE FILES
#include "storebookmark.h"

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CStoreBookmark::CStoreBookmark
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CStoreBookmark::CStoreBookmark(CContactStore& aStore,TInt aIndex)
:   iStore(aStore), iIndex(aIndex)
    {
    // No implementation required
    }
// -----------------------------------------------------------------------------
// CStoreBookmark::NewLC
// CStoreBookmark two-phased constructor.
// -----------------------------------------------------------------------------
//
CStoreBookmark* CStoreBookmark::NewLC(CContactStore& aStore,TInt aIndex)
    {
    CStoreBookmark* self = new (ELeave) CStoreBookmark(aStore,aIndex);
    CleanupStack::PushL(self);
    return self;
    }
// -----------------------------------------------------------------------------
// CStoreBookmark::NewL
// CStoreBookmark two-phased constructor.
// -----------------------------------------------------------------------------
//
CStoreBookmark* CStoreBookmark::NewL(CContactStore& aStore,TInt aIndex)
    {
    CStoreBookmark* self = CStoreBookmark::NewLC(aStore,aIndex);
    CleanupStack::Pop(self);
    return self;
    }
// -----------------------------------------------------------------------------
// CStoreBookmark::~CStoreBookmark
// CStoreBookmark Destructor
// -----------------------------------------------------------------------------
//
CStoreBookmark::~CStoreBookmark()
    {
    }

// -----------------------------------------------------------------------------
//                       CStoreBookmark public methods
// -----------------------------------------------------------------------------
// CStoreBookmark::Index
// Returns bookmarked contact index.
// -----------------------------------------------------------------------------
//
TInt CStoreBookmark::Index() const
    {
    return iIndex;
    }

// -----------------------------------------------------------------------------
// CStoreBookmark::Store
// Returns bookmarked store.
// -----------------------------------------------------------------------------
//
CContactStore& CStoreBookmark::Store() const
    {
    return iStore;
    }

}  // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
