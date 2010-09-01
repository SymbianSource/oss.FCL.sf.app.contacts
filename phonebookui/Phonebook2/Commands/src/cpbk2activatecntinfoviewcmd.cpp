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
* Description:  Phonebook 2 change view command.
*
*/

// INCLUDE FILES
#include "cpbk2activatecntinfoviewcmd.h"

// Phonebook 2
#include <MPbk2ContactUiControl.h>
#include <MPbk2ViewExplorer.h>
#include <MPbk2CommandObserver.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2ViewState.h>
#include <CPbk2AppViewBase.h>

// System includes
#include <e32base.h>

// --------------------------------------------------------------------------
// CPbk2ActivateCntInfoViewCmd::CPbk2ActivateCntInfoViewCmd
// --------------------------------------------------------------------------
//
inline CPbk2ActivateCntInfoViewCmd::CPbk2ActivateCntInfoViewCmd(
                MPbk2ContactUiControl& aUiControl )              
    {
    iControl = &aUiControl;
    }

// --------------------------------------------------------------------------
// CPbk2ActivateCntInfoViewCmd::~CPbk2ActivateCntInfoViewCmd
// --------------------------------------------------------------------------
//
CPbk2ActivateCntInfoViewCmd::~CPbk2ActivateCntInfoViewCmd()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ActivateCntInfoViewCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2ActivateCntInfoViewCmd* CPbk2ActivateCntInfoViewCmd::NewL(
                MPbk2ContactUiControl& aUiControl )                                
    {
    CPbk2ActivateCntInfoViewCmd* self = 
                new (ELeave) CPbk2ActivateCntInfoViewCmd(
                                aUiControl );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ActivateCntInfoViewCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ActivateCntInfoViewCmd::ConstructL()
    {
    iViewExplorer = Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer();
    }

// --------------------------------------------------------------------------
// CPbk2ActivateCntInfoViewCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2ActivateCntInfoViewCmd::ExecuteLD()
    {
    CleanupStack::PushL(this);
    if ( iControl && !iControl->ContactsMarked())
        {
        CPbk2ViewState* state = 
            Phonebook2::Pbk2AppUi()->ActiveView()->ViewStateLC();
        iViewExplorer->ActivatePhonebook2ViewL
            (Phonebook2::KPbk2ContactInfoViewUid, state);
        CleanupStack::PopAndDestroy(state);
        iControl->UpdateAfterCommandExecution();       
        }

    if (iCommandObserver)
        {
        iCommandObserver->CommandFinished(*this);
        }        
    CleanupStack::Pop(); // this
    }
    
// --------------------------------------------------------------------------
// CPbk2ActivateCntInfoViewCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2ActivateCntInfoViewCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2ActivateCntInfoViewCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2ActivateCntInfoViewCmd::ResetUiControl(MPbk2ContactUiControl& aUiControl)
    {
    if (iControl == &aUiControl)
        {
        iControl = NULL;
        }    
    }

// End of File
