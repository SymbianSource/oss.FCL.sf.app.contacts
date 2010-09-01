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
* Description:   Progress dialog process decorator.
*
*/


// INCLUDE FILES
#include "CPbk2ProgressDialogDecorator.h"
#include <eikprogi.h>

// ================= MEMBER FUNCTIONS =======================

CPbk2ProgressDialogDecorator* CPbk2ProgressDialogDecorator::NewL
        (TInt aResourceId, TBool aVisibilityDelay)
    {
	return new(ELeave)
        CPbk2ProgressDialogDecorator(aResourceId, aVisibilityDelay);
    }


CPbk2ProgressDialogDecorator::CPbk2ProgressDialogDecorator(
        TInt aResourceId, TBool aVisibilityDelay): 
    iDialogResource(aResourceId),
    iVisibilityDelayOff(!aVisibilityDelay)
    {
    // Do nothing
    }


CPbk2ProgressDialogDecorator::~CPbk2ProgressDialogDecorator()
    {
    if (iProgressDialog)
    	{
        iProgressDialog->SetCallback( NULL );
    	}
    ProcessStopped();
    }


void CPbk2ProgressDialogDecorator::CreateProgressNoteL
        (TInt aFinalValue)
    {
    delete iProgressDialog;
    iProgressDialog = NULL;
    
    iProgressDialog = new(ELeave) CAknProgressDialog
        (reinterpret_cast<CEikDialog**>(&iProgressDialog), iVisibilityDelayOff);
    // Should not be popped from cleanup stack --> PrepareLC   
    iProgressDialog->PrepareLC(iDialogResource);
    iProgressDialog->SetCallback(this);
    iProgressInfo = iProgressDialog->GetProgressInfoL();
    iProgressInfo->SetFinalValue(aFinalValue);
    iProgressDialog->RunLD();    
    }

void CPbk2ProgressDialogDecorator::ProcessStartedL(TInt aSteps)
    {
    CreateProgressNoteL(aSteps);
    }


void CPbk2ProgressDialogDecorator::ProcessAdvance(TInt aIncrement)
    {
    if ( iProgressDialog && iProgressInfo )
        {
        iProgressInfo->IncrementAndDraw(aIncrement);
        }
    }


void CPbk2ProgressDialogDecorator::ProcessStopped()
    {
    if (iProgressDialog)
        {
        TRAPD(err, iProgressDialog->ProcessFinishedL());
        if (err != KErrNone)
            {
            delete iProgressDialog;
            }
        iProgressDialog = NULL;
        }
    }


void CPbk2ProgressDialogDecorator::DialogDismissedL(TInt aButtonId)
    {
    iProgressDialog = NULL;
    if ( iObserver )
        {
        iObserver->ProcessDismissed(aButtonId);
        }
    }


void CPbk2ProgressDialogDecorator::SetObserver
        (MPbk2ProcessDecoratorObserver& aObserver)
    {
    iObserver = &aObserver;
    }

// End of File  
