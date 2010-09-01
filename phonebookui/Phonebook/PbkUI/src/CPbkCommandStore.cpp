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
*       Component which owns all phonebook command objects.
*
*/


// INCLUDE FILES
#include <CPbkCommandStore.h>
#include <MPbkCommand.h>

// ================= MEMBER FUNCTIONS =======================

inline CPbkCommandStore::CPbkCommandStore()
    {
    }

inline void CPbkCommandStore::ConstructL()
    {
	iIdleDestroyer = CIdle::NewL(CActive::EPriorityIdle);
    }

EXPORT_C CPbkCommandStore* CPbkCommandStore::NewL()
    {
    CPbkCommandStore* self = new(ELeave) CPbkCommandStore;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkCommandStore::~CPbkCommandStore()
    {
    iIdleDestructableCommands.Reset();
    iIdleDestructableCommands.Close();
	iCommandArray.ResetAndDestroy();
    iCommandArray.Close();
	delete iIdleDestroyer;
    }

EXPORT_C void CPbkCommandStore::AddAndExecuteCommandL
        (MPbkCommand* aCommand)
    {
    if (aCommand)
        {
        aCommand->AddObserver(*this);
        aCommand->ExecuteLD();
        User::LeaveIfError(iCommandArray.Append(aCommand));
        }
    }

void CPbkCommandStore::CommandFinished
        (const MPbkCommand& aCommand)
    {
    // Add command to the list of idle destroyable objects
    iIdleDestructableCommands.Append(&aCommand);
    if (iIdleDestroyer->IsActive())
        {
        iIdleDestroyer->Cancel();
        }
    // Request idle destruction. If the append above failed,
    // it doesn't matter. The command gets destructed then
    // at the Phonebook exit, when this objects destructor
    // deletes also commands in iCommandArray.
    iIdleDestroyer->Start(TCallBack((&CPbkCommandStore::IdleDestructorCallback),this));
    }

TInt CPbkCommandStore::IdleDestructorCallback(TAny* aSelf)
    {
    CPbkCommandStore* self = static_cast<CPbkCommandStore*>(aSelf);
    self->IdleDestructor();
    return EFalse;
    }

void CPbkCommandStore::IdleDestructor()
    {
    // Delete objects in idle destroyable array
    for (TInt i = 0; i < iIdleDestructableCommands.Count(); ++i)
        {
        MPbkCommand* idleDestCmd = iIdleDestructableCommands[i];
        for (TInt j = 0; j < iCommandArray.Count(); ++j)
            {
            MPbkCommand* arrayCmd = iCommandArray[j];
            if (idleDestCmd == arrayCmd)
                { 
                // Remove from command array
                iCommandArray.Remove(j);
                iIdleDestructableCommands.Remove(i);
                delete idleDestCmd;
                idleDestCmd = NULL;
                }
            }
        }
    }

// End of File  
