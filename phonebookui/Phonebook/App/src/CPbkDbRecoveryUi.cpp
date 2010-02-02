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
*       Methods for Database recovery UI class.
*
*/


// INCLUDE FILES
#include "CPbkDbRecoveryUi.h"
#include <eikenv.h>
#include <eikappui.h>
#include <avkon.hrh>
#include <CPbkContactEngine.h>
#include <CPbkDbRecovery.h>
#include <pbkdebug.h>
#include "MPbkCtrlDisabler.h"


// ================= MEMBER FUNCTIONS =======================

inline CPbkDbRecoveryUi::CPbkDbRecoveryUi( MPbkCtrlDisabler& aCtrlDisabler )
    : iCtrlDisabler( aCtrlDisabler )
    {
    }

inline void CPbkDbRecoveryUi::ConstructL(CPbkContactEngine& aEngine)
    {
    iDbRecovery = CPbkDbRecovery::NewL(aEngine.Database());
    iDbRecovery->SetErrorHandler(this);
    }

CPbkDbRecoveryUi* CPbkDbRecoveryUi::NewL(
    CPbkContactEngine& aEngine, 
    MPbkCtrlDisabler& aCtrlDisabler)
    {
    CPbkDbRecoveryUi* self = new(ELeave) CPbkDbRecoveryUi( aCtrlDisabler );
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CPbkDbRecoveryUi::~CPbkDbRecoveryUi()
    {
    delete iDbRecovery;
    }

TBool CPbkDbRecoveryUi::HandleDbRecoveryError(TInt aError)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkDbRecoveryUi::HandleDbRecoveryError(0x%x,%d)"),
        this, aError);

    // Get EIKON environment
    CEikonEnv* eikEnv = CEikonEnv::Static();

    if (eikEnv)
        {

        // Display standard error message
        eikEnv->HandleError(aError);

        // Exit Phonebook. There's not much else to do when DB recovery fails.
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING
            ("CPbkDbRecoveryUi::HandleDbRecoveryError(0x%x,%d): exit app"),
            this, aError);
        // we need to disable controller in order to prevent focus 
        // change event called to controller during exiting out
        iCtrlDisabler.DisableController();
        MEikCommandObserver* appUiCmdHandler = eikEnv->EikAppUi();
        TRAP_IGNORE(appUiCmdHandler->ProcessCommandL(EAknCmdExit));
        }

    // Don't retry recovery
    return EFalse;
    }

//  End of File  
