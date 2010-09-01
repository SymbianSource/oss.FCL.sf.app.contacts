/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook asynchronous single contact retriever 
                 operation.
*
*/



// INCLUDES
#include "CContactRetriever.h"
#include "CContactLink.h"
#include <CVPbkAsyncOperation.h>
#include <MVPbkSingleContactOperationObserver.h>
#include "CContact.h"
#include "CContactStore.h"
#include <cntitem.h>
#include <CVPbkAsyncCallback.h>

namespace VPbkCntModel {

// CContactRetriever implementation

inline void CContactRetriever::BaseConstructL()
    {
    iAsyncOperation = new(ELeave) VPbkEngUtils::CVPbkAsyncOperation;
    }

inline void CContactRetriever::ConstructL(const MVPbkContactLink& aLink)
    {
    BaseConstructL();
    
    iLink = static_cast<CContactLink*>(aLink.CloneLC());
    CleanupStack::Pop();
    }

inline void CContactRetriever::ConstructL(MVPbkContactLink* aLink)
    {
    BaseConstructL();
    
    iLink = static_cast<CContactLink*>(aLink);
    }

CContactRetriever* CContactRetriever::NewLC(
        const MVPbkContactLink& aLink,
        CContactStore& aContactStore,
        MVPbkSingleContactOperationObserver& aObserver)
    {
    CContactRetriever* self = new(ELeave) CContactRetriever(aContactStore, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aLink);
    return self;
    }

CContactRetriever* CContactRetriever::NewLC(
        MVPbkContactLink* aLink,
        CContactStore& aContactStore,
        MVPbkSingleContactOperationObserver& aObserver)
    {
    CContactRetriever* self = new(ELeave) CContactRetriever(aContactStore, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aLink);
    return self;
    }

CContactRetriever::~CContactRetriever()
    {
    delete iAsyncOperation;
    delete iLink;
    }

void CContactRetriever::StartL()
    {
    VPbkEngUtils::MAsyncCallback* retrieveCallback =
        VPbkEngUtils::CreateAsyncCallbackLC(
            *this, 
            &CContactRetriever::DoReadL, 
            &CContactRetriever::RetrieveError, 
            iObserver);
    iAsyncOperation->CallbackL(retrieveCallback);
    CleanupStack::Pop(retrieveCallback);
    }

void CContactRetriever::Cancel()
    {
    iAsyncOperation->Purge();
    }

CContactRetriever::CContactRetriever(
        CContactStore& aContactStore,
        MVPbkSingleContactOperationObserver& aObserver) :
    iContactStore(aContactStore),
    iObserver(aObserver)
    {
    }

void CContactRetriever::DoReadL(MVPbkSingleContactOperationObserver& /*aObserver*/)
    {
    CContactItem* item = iContactStore.NativeDatabase().ReadContactLC(iLink->ContactId());
    CContact* contact = CContact::NewL(iContactStore, item);
    CleanupStack::Pop(item);
    // Takes ownership of contact
    iObserver.VPbkSingleContactOperationComplete(*this, contact);
    }

void CContactRetriever::RetrieveError(
        MVPbkSingleContactOperationObserver& /*aObserver*/,
        TInt aError)
    {
    iObserver.VPbkSingleContactOperationFailed(*this, aError);
    }

}  // namespace VPbkCntModel

// end of file
