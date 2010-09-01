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
* Description:  CContactRetriever - Contact retriever implementation
*
*/


// INCLUDE FILES
#include "contactretriever.h"
#include "contact.h"
#include "contactstore.h"
#include <mvpbksinglecontactoperationobserver.h>

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CContactRetriever::CContactRetriever
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CContactRetriever::CContactRetriever(CContactStore& aStore,
                                MVPbkSingleContactOperationObserver& aObserver)
:   CActive(EPriorityStandard), iStore(aStore), iObserver(aObserver)
    {
    }

// -----------------------------------------------------------------------------
// CContactRetriever::ConstructL
// CContactRetriever constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CContactRetriever::ConstructL(TInt aIndex)
    {
    CActiveScheduler::Add(this);
    // New contact
    iContact = iStore.CreateContactL(aIndex);
    }
    
// -----------------------------------------------------------------------------
// CContactRetriever::NewL
// CContactRetriever two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactRetriever* CContactRetriever::NewL(CContactStore& aStore,TInt aIndex,
                                MVPbkSingleContactOperationObserver& aObserver)
    {
    CContactRetriever* self = new (ELeave) CContactRetriever(aStore,aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aIndex);
    CleanupStack::Pop(self);
    return self;
    }
    
// -----------------------------------------------------------------------------
// CContactRetriever::~CContactRetriever
// CContactRetriever Destructor
// -----------------------------------------------------------------------------
//
CContactRetriever::~CContactRetriever()
    {
    Cancel();
    }
    
// -----------------------------------------------------------------------------
//                          MVPbkContactOperation implementation
// -----------------------------------------------------------------------------
// CContactRetriever::StartL
// -----------------------------------------------------------------------------
//
void CContactRetriever::StartL()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status,KErrNone);
    SetActive();
    }
// -----------------------------------------------------------------------------
// CContactRetriever::Cancel
// -----------------------------------------------------------------------------
//
void CContactRetriever::Cancel()
    {
    CActive::Cancel();
    }

// -----------------------------------------------------------------------------
//                          CActive implementation
// -----------------------------------------------------------------------------
// CContactRetriever::DoCancel
// -----------------------------------------------------------------------------
//    
void CContactRetriever::DoCancel()
    {
    }
// -----------------------------------------------------------------------------
// CContactRetriever::RunL
// -----------------------------------------------------------------------------
//
void CContactRetriever::RunL()
    {
    // Operation completition
    iObserver.VPbkSingleContactOperationComplete(*this,iContact);
    // Ownership of contact is given above operation complete
    iContact = NULL;
    }

}  // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
