/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*        Methods for DB recovery.
*
*/


// INCLUDE FILES
#include "CPbkDbRecovery.h"
#include "MPbkRecoveryErrorHandler.h"
#include <PbkDebug.h>


// ==================== MEMBER FUNCTIONS ====================

inline CPbkDbRecovery::CPbkDbRecovery(CContactDatabase& aDb) :
    iDb(aDb)
    {
    }

inline void CPbkDbRecovery::ConstructL()
    {
    iDbChangeNotifier = CContactChangeNotifier::NewL(iDb,this);
    // High priority is used to prevent input events getting through before
    // the recover is started
    iStarter = CIdle::NewL(CActive::EPriorityHigh);
    }

EXPORT_C CPbkDbRecovery* CPbkDbRecovery::NewL(CContactDatabase& aDb)
    {
    CPbkDbRecovery* self = new(ELeave) CPbkDbRecovery(aDb);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;    
    }

CPbkDbRecovery::~CPbkDbRecovery()
    {
    delete iStarter;
    delete iDbChangeNotifier;
    }

EXPORT_C void CPbkDbRecovery::SetErrorHandler
		(MPbkRecoveryErrorHandler* aErrorHandler)
    {
    iErrorHandler = aErrorHandler;
    }

void CPbkDbRecovery::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
    {
    if (aEvent.iType == EContactDbObserverEventRollback)
        {
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING
			("CPbkDbRecovery::HandleDatabaseEventL(0x%x,EContactDbObserverEventRollback)"),
			this);
        StartRecovery();
        }
    }

void CPbkDbRecovery::StartRecovery()
    {
    if(iStarter->IsActive())
        {
        iStarter->Cancel();
        }
    // Start the recovery using an asynchronous callback so that the Contact
    // DB event handler is able to return before starting the recovery.
    iStarter->Start(TCallBack(CPbkDbRecovery::DoRecoveryL,this));
    }

TBool CPbkDbRecovery::DoRecoveryL()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkDbRecovery::DoRecoveryL(0x%x)"),
		this);
    TBool result = EFalse;

    TRAPD(err, iDb.RecoverL());

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkDbRecovery::DoRecoveryL(0x%x) recovery result=%d"), this, err);

    if (err != KErrNone)
        {
        if (iErrorHandler)
            {
            // Use error handler to determine if recovery is to be tried again
            result = iErrorHandler->HandleDbRecoveryError(err);
            }
        else
            {
            // No error handler -> just leave with the error code
            User::Leave(err);
            }
        }

    return result;
    }

TInt CPbkDbRecovery::DoRecoveryL(TAny* aSelf)
    {
    return (static_cast<CPbkDbRecovery*>(aSelf)->DoRecoveryL());
    }


// End of File
