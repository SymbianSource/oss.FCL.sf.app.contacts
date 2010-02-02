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
* Description:  Wait note process decorator.
*
*/


// INCLUDE FILES
#include "CPbk2WaitNoteDecorator.h"
#include <eikprogi.h>
#include <AknWaitDialog.h>

// ================= MEMBER FUNCTIONS =======================

CPbk2WaitNoteDecorator* CPbk2WaitNoteDecorator::NewL
        (TInt aResourceId, TBool aVisibilityDelay)
    {
	return new(ELeave)
        CPbk2WaitNoteDecorator(aResourceId, aVisibilityDelay);
    }


CPbk2WaitNoteDecorator::CPbk2WaitNoteDecorator(
        TInt aResourceId, TBool aVisibilityDelay ): 
    iDialogResource(aResourceId),
    iVisibilityDelayOff(!aVisibilityDelay)
    {
    // Do nothing
    }


CPbk2WaitNoteDecorator::~CPbk2WaitNoteDecorator()
    {
    if (iWaitDialog)
    	{
    	iWaitDialog->SetCallback( NULL );
    	}
    ProcessStopped();
    }


void CPbk2WaitNoteDecorator::CreateWaitNoteL()
    {
    delete iWaitDialog;
    iWaitDialog = NULL;

    iWaitDialog = new(ELeave) CAknWaitDialog
        (reinterpret_cast<CEikDialog**>(&iWaitDialog), iVisibilityDelayOff);
    iWaitDialog->SetCallback(this);
    iWaitDialog->SetTone(CAknNoteDialog::ENoTone);
    // Should not be popped from cleanup stack --> PrepareLC
    iWaitDialog->PrepareLC(iDialogResource);
    iWaitDialog->RunLD();
    }


void CPbk2WaitNoteDecorator::ProcessStartedL(TInt /*aSteps*/)
    {
    CreateWaitNoteL();
    }


void CPbk2WaitNoteDecorator::ProcessAdvance(TInt /*aIncrement*/)
    {
    // Do nothing
    }


void CPbk2WaitNoteDecorator::ProcessStopped()
    {
    if (iWaitDialog)
        {
        TRAPD(err, iWaitDialog->ProcessFinishedL());
        if (err != KErrNone)
            {
            delete iWaitDialog;
            }
        iWaitDialog = NULL;
        }
    }


void CPbk2WaitNoteDecorator::DialogDismissedL(TInt aButtonId)
    {
    iWaitDialog = NULL;
    if ( iObserver )
        {
        iObserver->ProcessDismissed(aButtonId);
        }
    }


void CPbk2WaitNoteDecorator::SetObserver
        (MPbk2ProcessDecoratorObserver& aObserver)
    {
    iObserver = &aObserver;
    }

// End of File  
