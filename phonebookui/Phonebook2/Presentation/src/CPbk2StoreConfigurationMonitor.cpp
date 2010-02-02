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
*     Phonebook 2 contact store configuration monitor.
*
*/


// INCLUDE FILES
#include "CPbk2StoreConfigurationMonitor.h"

// From Phonebook 2
#include <Phonebook2InternalCRKeys.h>
#include <MPbk2StoreConfigurationObserver.h>

// From System
#include <centralrepository.h>


// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationMonitor::CPbk2StoreConfigurationMonitor
// --------------------------------------------------------------------------
//
CPbk2StoreConfigurationMonitor::CPbk2StoreConfigurationMonitor(
        CRepository& aRepository,
        MPbk2StoreConfigurationObserver& aObserver) :
    CActive(EPriorityStandard),
    iRepository(aRepository),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationMonitor::~CPbk2StoreConfigurationMonitor
// --------------------------------------------------------------------------
//
CPbk2StoreConfigurationMonitor::~CPbk2StoreConfigurationMonitor()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationMonitor::NewL
// --------------------------------------------------------------------------
//
CPbk2StoreConfigurationMonitor* CPbk2StoreConfigurationMonitor::NewL(
        CRepository& aRepository,
        MPbk2StoreConfigurationObserver& aObserver)
    {
    CPbk2StoreConfigurationMonitor* self =
        new(ELeave) CPbk2StoreConfigurationMonitor(aRepository, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationMonitor::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2StoreConfigurationMonitor::ConstructL()
    {
    RenewRequestL();
    }
    
// --------------------------------------------------------------------------
// CPbk2StoreConfigurationMonitor::RenewRequestL
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationMonitor::RenewRequestL()
    {
    User::LeaveIfError(iRepository.NotifyRequest(
        KPhonebookCurrentConfigurationPartialKey,
        KPhonebookCurrentConfigurationMask, iStatus));
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationMonitor::RunL
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationMonitor::RunL()
    {
    RenewRequestL();
    iObserver.ConfigurationChanged();
    }
    
// --------------------------------------------------------------------------
// CPbk2StoreConfigurationMonitor::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationMonitor::DoCancel()
    {
    iRepository.NotifyCancelAll();
    }

// End of File
