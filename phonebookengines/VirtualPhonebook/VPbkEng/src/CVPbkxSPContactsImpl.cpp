/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  xSP Contact fetcher implementation.
 *
*/


// INCLUDES
#include "CVPbkxSPContactsImpl.h"
#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>
#include "CVPbkxSPContactsOperation.h"


CVPbkxSPContactsImpl* CVPbkxSPContactsImpl::NewL(
        CVPbkContactManager& aContactManager)
    {
    CVPbkxSPContactsImpl* self = new (ELeave) CVPbkxSPContactsImpl();
    CleanupStack::PushL(self);
    self->iContactManager = &aContactManager;
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CVPbkxSPContactsImpl::ConstructL()
    {
    }

CVPbkxSPContactsImpl::CVPbkxSPContactsImpl()
    {
    }

CVPbkxSPContactsImpl::~CVPbkxSPContactsImpl()
    {
    }

MVPbkContactOperationBase* CVPbkxSPContactsImpl::GetxSPContactLinksL(
        const MVPbkStoreContact& aContact,
        MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aResultObserver,
        MVPbkOperationErrorObserver& aErrorObserver)
    {
    return CVPbkxSPContactsOperation::NewGetxSPLinksOperationL(
            aContact,
            *iContactManager,
            aResultObserver,
            aErrorObserver,
            CVPbkxSPContactsOperation::EGetxSPContactLinks );
    
    }

// end of file
