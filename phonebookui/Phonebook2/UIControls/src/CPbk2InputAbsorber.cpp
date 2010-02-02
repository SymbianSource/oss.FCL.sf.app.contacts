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
*      Methods for control which absorbs all keyboard and CBA events.
*
*/


// INCLUDE FILES
#include "CPbk2InputAbsorber.h"
#include <eikenv.h>
#include <eikappui.h>
#include <eikbtgpc.h>
#include <avkon.rsg>


// ================= MEMBER FUNCTIONS =======================

inline CPbk2InputAbsorber::CPbk2InputAbsorber()
    {
    }

inline void CPbk2InputAbsorber::InitControlL()
    {
    MakeVisible(EFalse);
	SetExtent(TPoint(0,0), TSize(0,0));
	CreateWindowL();
    }

inline void CPbk2InputAbsorber::CaptureEventsL()
    {
    SetPointerCapture(ETrue);
	ClaimPointerGrab(ETrue);
    CEikAppUi* appUi = iEikonEnv->EikAppUi();
    appUi->AddToStackL(this, ECoeStackPriorityDialog);
    iAppUi = appUi;
    }

inline void CPbk2InputAbsorber::InitCbaL(TInt aCbaResource)
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

inline void CPbk2InputAbsorber::ConstructL(TInt aCbaResource)
    {
    InitControlL();
    CaptureEventsL();
    InitCbaL(aCbaResource);
	iWait = new (ELeave) CActiveSchedulerWait();
    }

EXPORT_C CPbk2InputAbsorber* CPbk2InputAbsorber::NewL(TInt aCbaResource/*=0*/)
    {
    CPbk2InputAbsorber* self = CPbk2InputAbsorber::NewLC(aCbaResource);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CPbk2InputAbsorber* CPbk2InputAbsorber::NewLC(TInt aCbaResource/*=0*/)
    {
    CPbk2InputAbsorber* self = new (ELeave) CPbk2InputAbsorber;
    CleanupStack::PushL(self);
    self->ConstructL(aCbaResource);
    return self;
    }

CPbk2InputAbsorber::~CPbk2InputAbsorber()
    {
    StopWait();
    delete iCba;
	delete iWait;
	if (iCoeEnv && iAppUi)
        {
		iAppUi->RemoveFromStack(this);
        }
    }

EXPORT_C void CPbk2InputAbsorber::SetCommandObserver(MEikCommandObserver* aCmdObserver)
    {
    iCmdObserver = aCmdObserver;
    }

EXPORT_C void CPbk2InputAbsorber::Wait()
    {
    // Check that scheduler is not started already.
    if ( !iWait->IsStarted() )
        {            
        iWait->Start();
        }
    }

EXPORT_C void CPbk2InputAbsorber::StopWait()
    {
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

EXPORT_C TBool CPbk2InputAbsorber::IsWaiting() const
    {
    return iWait->IsStarted();
    }

TKeyResponse CPbk2InputAbsorber::OfferKeyEventL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TKeyResponse response = EKeyWasConsumed;
    
    // Allow Send Key events to open Dialer application
    if ( ( aType == EEventKeyUp || aType == EEventKeyDown ) 
        && aKeyEvent.iScanCode == EStdKeyYes )
        {
        response = EKeyWasNotConsumed;
        }
    
    return response;
    }

void CPbk2InputAbsorber::ProcessCommandL(TInt aCommandId)
    {
    if (iCmdObserver)
        {
        iCmdObserver->ProcessCommandL(aCommandId);
        }
    }

//  End of File
