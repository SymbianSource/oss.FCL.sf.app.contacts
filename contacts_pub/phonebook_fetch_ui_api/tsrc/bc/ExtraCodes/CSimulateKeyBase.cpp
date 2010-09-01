/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/








// INCLUDES
#include <eikenv.h>
#include "CSimulateKeyBase.h"

// CLASS-SPECIFIC PANIC CODES
enum TPanicCodes
    {
	KPanicKeySimulationFailed=0
    };


/**
 * Constructor.
 */
CSimulateKeyBase::CSimulateKeyBase()
	: iReason(KErrNone)
	{}

/**
 * Destructor.
 */
CSimulateKeyBase::~CSimulateKeyBase()
	{
	if (iIdle)
		{
		iIdle->Cancel();
		}
	delete iIdle;
	if (iKeyEvents)
		{
		iKeyEvents->Reset();
		}
	delete iKeyEvents;

	if (iEventCodes)
		{
		iEventCodes->Reset();
		}
	delete iEventCodes;
	}


void CSimulateKeyBase::ConstructL()
	{
	iIdle=CIdle::NewL(CActive::EPriorityIdle);
	iKeyEvents=new(ELeave) CArrayFixFlat<TKeyEvent>(2);
	iEventCodes=new(ELeave) CArrayFixFlat<TEventCode>(2);
	}

void CSimulateKeyBase::CallBackStartL()
	{
	if (!iIdle->IsActive())
		{
		iIdle->Start(TCallBack(CSimulateKeyBase::CallBackL,this));
		}
	}

void CSimulateKeyBase::ClearAllKeyEventsL()
	{
	if (iKeyEvents)
		{
		iKeyEvents->Reset();
		}
	delete iKeyEvents;
	iKeyEvents = NULL;
	iKeyEvents = new(ELeave) CArrayFixFlat<TKeyEvent>(2);

	if (iEventCodes)
		{
		iEventCodes->Reset();
		}
	delete iEventCodes;
	iEventCodes = NULL;
	iEventCodes = new(ELeave) CArrayFixFlat<TEventCode>(2);
	}


void CSimulateKeyBase::ReleaseMemory()
	{
	if (iKeyEvents)
		{
		iKeyEvents->Reset();
		}
	delete iKeyEvents;
	iKeyEvents = NULL;

	if (iEventCodes)
		{
		iEventCodes->Reset();
		}
	delete iEventCodes;
	iEventCodes = NULL;
	}

void CSimulateKeyBase::AddEventL(const TKeyEvent& aKeyEvent,
		const TEventCode& aEventCode)
	{
	iKeyEvents->AppendL(aKeyEvent);
	iEventCodes->AppendL(aEventCode);
	}



void CSimulateKeyBase::AddKeyEventL(const TKeyEvent& aKeyEvent)
	{
	iKeyEvents->AppendL(aKeyEvent);
	iEventCodes->AppendL(EEventKey);
	}


void CSimulateKeyBase::AddKeyEventCodeL(const TKeyCode& aKeyCode,
		TInt aRepeats /*=0*/)
	{
	TKeyEvent event;
	event.iCode = aKeyCode;
	event.iScanCode=0;
	event.iModifiers=0;
	event.iRepeats=aRepeats;
	iKeyEvents->AppendL(event);
	iEventCodes->AppendL(EEventKey);
	}

void CSimulateKeyBase::AddKeyEventAsciiCodeL(const TUint& aKeyCode,
		TInt aRepeats /*=0*/)
	{
	TKeyEvent event;
	event.iCode = aKeyCode;
	event.iScanCode=0;
	event.iModifiers=0;
	event.iRepeats=aRepeats;
	iKeyEvents->AppendL(event);
	iEventCodes->AppendL(EEventKey);
	}

TBool CSimulateKeyBase::CallBackL(TAny* aThis)
	{//static
	
	CSimulateKeyBase* castThis = NULL;
	TRAP_IGNORE( castThis = reinterpret_cast<CSimulateKeyBase*>(aThis));
	
    castThis->iEnv=CEikonEnv::Static();
	castThis->ExecuteKeySequenceL();

	return EFalse;
	}

// End of File
