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
*     Phonebook 2 default saving store monitor.
*
*/


// INCLUDE FILES
#include "CPbk2DefaultSavingStoreMonitor.h"

// From Phonebook2
#include <Phonebook2InternalCRKeys.h>
#include <MPbk2DefaultSavingStoreObserver.h>

// From Virtual phonebook
#include <TVPbkContactStoreUriPtr.h>

// From System
#include <centralrepository.h>

// Unnamed namespace for local definitions
namespace {

// Definition for initial store uri size
const TInt KInitialStoreUriSize = 1;

}// namespace


// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStoreMonitor::CPbk2DefaultSavingStoreMonitor
// --------------------------------------------------------------------------
//
CPbk2DefaultSavingStoreMonitor::CPbk2DefaultSavingStoreMonitor(
        CRepository& aRepository,
        MPbk2DefaultSavingStoreObserver& aObserver ) :
    CActive(EPriorityHigh),
    iRepository(aRepository),
    iObserver( aObserver )
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStoreMonitor::~CPbk2DefaultSavingStoreMonitor
// --------------------------------------------------------------------------
//
CPbk2DefaultSavingStoreMonitor::~CPbk2DefaultSavingStoreMonitor()
    {
    Cancel();
    delete iDefaultStoreUri;
    }
    
// --------------------------------------------------------------------------
// CPbk2DefaultSavingStoreMonitor::NewL
// --------------------------------------------------------------------------
//
CPbk2DefaultSavingStoreMonitor* CPbk2DefaultSavingStoreMonitor::NewL(
        CRepository& aRepository, MPbk2DefaultSavingStoreObserver& aObserver)
    {
    CPbk2DefaultSavingStoreMonitor* self =
        new(ELeave) CPbk2DefaultSavingStoreMonitor(aRepository, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStoreMonitor::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2DefaultSavingStoreMonitor::ConstructL()
    {
    RenewRequestL();
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStoreMonitor::DefaultSavingStoreL
// --------------------------------------------------------------------------
//
TVPbkContactStoreUriPtr
        CPbk2DefaultSavingStoreMonitor::DefaultSavingStoreL() const
    {
    if ( !iDefaultStoreUri )
        {
        iDefaultStoreUri = UpdateDefaultSavingStoreL();
        }
    return *iDefaultStoreUri;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStoreMonitor::UpdateDefaultSavingStoreL
// --------------------------------------------------------------------------
//
HBufC* CPbk2DefaultSavingStoreMonitor::UpdateDefaultSavingStoreL() const
    {
    HBufC* defaultStoreUri = HBufC::NewL(KInitialStoreUriSize);
    TPtr ptr = defaultStoreUri->Des();
    TInt actualSize = 0;
    TInt ret = iRepository.Get
        (KPhonebookDefaultSavingStoreUri, ptr, actualSize);
    if (ret == KErrOverflow)
        {
        delete defaultStoreUri;
        defaultStoreUri = NULL;
        defaultStoreUri = HBufC::NewL(actualSize);
        CleanupStack::PushL( defaultStoreUri );
        ptr.Set(defaultStoreUri->Des());
        User::LeaveIfError(iRepository.Get
            (KPhonebookDefaultSavingStoreUri, ptr));
        CleanupStack::Pop( defaultStoreUri ); 
        }        
    return defaultStoreUri;        
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStoreMonitor::RenewRequestL
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStoreMonitor::RenewRequestL()
    {
    User::LeaveIfError(iRepository.NotifyRequest(
        KPhonebookDefaultSavingStoreUri, iStatus));
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2DefaultSavingStoreMonitor::RunL
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStoreMonitor::RunL()
    {
    HBufC* temp = UpdateDefaultSavingStoreL();
    delete iDefaultStoreUri;
    iDefaultStoreUri = NULL;
    iDefaultStoreUri = temp;
    iObserver.SavingStoreChanged();
    RenewRequestL();
    }
    
// --------------------------------------------------------------------------
// CPbk2DefaultSavingStoreMonitor::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2DefaultSavingStoreMonitor::DoCancel()
    {
    iRepository.NotifyCancelAll();
    }

// End of File
