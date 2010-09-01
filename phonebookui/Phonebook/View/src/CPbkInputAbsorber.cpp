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
*      Methods for control which absorbs all keyboard and CBA events.
*
*/


// INCLUDE FILES
#include "CPbkInputAbsorber.h"
#include <eikenv.h>
#include <eikappui.h>
#include <eikbtgpc.h>
#include <avkon.rsg>


// ================= MEMBER FUNCTIONS =======================

inline CPbkInputAbsorber::CPbkInputAbsorber()
    {
    }

inline void CPbkInputAbsorber::InitControlL()
    {
    MakeVisible(EFalse);
	SetExtent(TPoint(0,0), TSize(0,0));
	CreateWindowL();
    }

inline void CPbkInputAbsorber::CaptureEventsL()
    {
    SetPointerCapture(ETrue);
	ClaimPointerGrab(ETrue);
    CEikAppUi* appUi = iEikonEnv->EikAppUi();
    appUi->AddToStackL(this, ECoeStackPriorityDialog);
    iAppUi = appUi;
    }

inline void CPbkInputAbsorber::InitCbaL(TInt aCbaResource)
    {
    if (!aCbaResource)
        {
        aCbaResource = R_AVKON_SOFTKEYS_EMPTY;
        }
    iCba = CEikButtonGroupContainer::NewL
        (CEikButtonGroupContainer::ECba, 
        CEikButtonGroupContainer::EHorizontal, 
        this, 
        aCbaResource, 
        *this);
	const TSize screenSize = iCoeEnv->ScreenDevice()->SizeInPixels();
	iCba->SetBoundingRect(TRect(screenSize));
    }

inline void CPbkInputAbsorber::ConstructL(TInt aCbaResource)
    {
    InitControlL();
    CaptureEventsL();
    InitCbaL(aCbaResource);
	iWait = new (ELeave) CActiveSchedulerWait();
    }

CPbkInputAbsorber* CPbkInputAbsorber::NewL(TInt aCbaResource/*=0*/)
    {
    CPbkInputAbsorber* self = CPbkInputAbsorber::NewLC(aCbaResource);
    CleanupStack::Pop(self);
    return self;
    }

CPbkInputAbsorber* CPbkInputAbsorber::NewLC(TInt aCbaResource/*=0*/)
    {
    CPbkInputAbsorber* self = new (ELeave) CPbkInputAbsorber;
    CleanupStack::PushL(self);
    self->ConstructL(aCbaResource);
    return self;
    }

CPbkInputAbsorber::~CPbkInputAbsorber()
    {
    StopWait();
    delete iCba;
	delete iWait;
	if (iCoeEnv && iAppUi)
        {
		iAppUi->RemoveFromStack(this);
        }
    }

void CPbkInputAbsorber::SetCommandObserver(MEikCommandObserver* aCmdObserver)
    {
    iCmdObserver = aCmdObserver;
    }

void CPbkInputAbsorber::Wait()
    {
    if (iWait->IsStarted())
        {
        iWait->AsyncStop();
        }
    iWait->Start();
    }

void CPbkInputAbsorber::StopWait()
    {
    if ((iWait) && (iWait->IsStarted()))
        {
        iWait->AsyncStop();
        }
    }

TBool CPbkInputAbsorber::IsWaiting() const
    {
    return iWait->IsStarted();
    }

TKeyResponse CPbkInputAbsorber::OfferKeyEventL
        (const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasConsumed;
    }

void CPbkInputAbsorber::ProcessCommandL(TInt aCommandId)
    {
    if (iCmdObserver)
        {
        iCmdObserver->ProcessCommandL(aCommandId);
        }
    }

//  End of File
