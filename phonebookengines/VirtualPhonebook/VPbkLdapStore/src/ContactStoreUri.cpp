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
* Description:  CContactStoreUri implementation
*
*/


// INCLUDE FILES
#include "contactstoreuri.h"
#include <tvpbkcontactstoreuriptr.h>

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CContactStoreUri::CContactStoreUri
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CContactStoreUri::CContactStoreUri()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CContactStoreUri::ConstructL
// CContactStoreUri constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CContactStoreUri::ConstructL(const TDesC& aStoreUri)
    {
    iUriBuffer = aStoreUri.AllocL();
    }
// -----------------------------------------------------------------------------
// CContactStoreUri::NewLC
// CContactStoreUri two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreUri* CContactStoreUri::NewLC(const TVPbkContactStoreUriPtr& aStoreUri)
    {
    CContactStoreUri* self = new (ELeave)CContactStoreUri();
    CleanupStack::PushL(self);
    self->ConstructL(aStoreUri.UriDes());
    return self;
    }
// -----------------------------------------------------------------------------
// CContactStoreUri::NewL
// CContactStoreUri two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreUri* CContactStoreUri::NewL(const TVPbkContactStoreUriPtr& aStoreUri)
    {
    CContactStoreUri* self = CContactStoreUri::NewLC(aStoreUri);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CContactStoreUri::~CContactStoreUri
// CContactStoreUri Destructor
// -----------------------------------------------------------------------------
//
CContactStoreUri::~CContactStoreUri()
    {
    if (iUriBuffer)
        {
        delete iUriBuffer;
        iUriBuffer = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CContactStoreUri::Uri
// Return Store URI
// -----------------------------------------------------------------------------
//
const TVPbkContactStoreUriPtr CContactStoreUri::Uri() const
    {
    return TVPbkContactStoreUriPtr(*iUriBuffer);
    }

} // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
