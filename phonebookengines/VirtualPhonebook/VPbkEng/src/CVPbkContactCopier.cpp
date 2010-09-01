/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An class for handling different type of copying logic
*
*/


#include <CVPbkContactCopier.h>

#include "CVPbkCopyContactsOperation.h"


// ---------------------------------------------------------------------------
// CVPbkContactCopier::CVPbkContactCopier
// ---------------------------------------------------------------------------
//
CVPbkContactCopier::CVPbkContactCopier( CVPbkContactManager& aContactManager )
    :   iContactManager( aContactManager )
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactCopier::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactCopier* CVPbkContactCopier::NewL( 
        CVPbkContactManager& aContactManager )
    {
    return new( ELeave ) CVPbkContactCopier( aContactManager );
    }

// ---------------------------------------------------------------------------
// CVPbkContactCopier::~CVPbkContactCopier
// ---------------------------------------------------------------------------
//
CVPbkContactCopier::~CVPbkContactCopier()
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactCopier::CopyContactsL
// ---------------------------------------------------------------------------
//
EXPORT_C MVPbkContactOperationBase* CVPbkContactCopier::CopyContactsL(
        TUint32 aContactCopyFlags,
        const MVPbkContactLinkArray& aSourceContactLinks,
        MVPbkContactStore* aTargetStore,
        CVPbkContactLinkArray& aCopiedContactLinks,
        MVPbkBatchOperationObserver& aObserver )
    {
    MVPbkContactOperation* operation = CVPbkCopyContactsOperation::NewLC(
            aContactCopyFlags, iContactManager, aSourceContactLinks,
            aTargetStore, &aCopiedContactLinks, aObserver );
    operation->StartL();
    CleanupStack::Pop(); // operation
    return operation;
    }

