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
#include "CSimulateKeyEvents.h"

CSimulateKeyEvents::CSimulateKeyEvents()
	{
	}

CSimulateKeyEvents::~CSimulateKeyEvents()
	{
	}

CSimulateKeyEvents* CSimulateKeyEvents::NewL()
	{//static
	CSimulateKeyEvents* self=new(ELeave) CSimulateKeyEvents();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); //self

	return self;
	}

void CSimulateKeyEvents::ExecuteKeySequenceL()
	{
	const TInt count=iKeyEvents->Count();
	
	for (TInt index=0;index<count;index++)
		{
		TKeyEvent event;
		TEventCode code; 
		event=(*iKeyEvents)[index];
		code=(*iEventCodes)[index];
		CCoeEnv::Static()->SimulateKeyEventL(event, code);
		}
	}

/*******************************************************************
*
* Selects right key code
*
*******************************************************************/
void CSimulateKeyEvents::EmailSelectKeyCodeL( const TInt aIndex, TKeyCode& aKeyCode )
	{

	switch(aIndex)
		{
		case 0:
			{
			aKeyCode = EKeyEnter;
			break;
			}
		case 1:
			{
			// Left soft key
			aKeyCode = EKeyDevice0;
			break;
			}
		case 2:
			{
			// Ok key
			aKeyCode = EKeyDevice3;
			break;
			}
		default:
			{
			aKeyCode = EKeyNull;
			break;
			}
		}// switch(aIndex) ends
	}

/*******************************************************************
*
* Selects right key code
*
*******************************************************************/
void CSimulateKeyEvents::SelectKeyCodeL( const TInt aIndex, TKeyCode& aKeyCode )
	{

	switch(aIndex)
		{
		case 0:
			{
			aKeyCode = EKeyEnter;
			break;
			}
		case 1:
			{
			// Left soft key
			aKeyCode = EKeyDevice0;
			break;
			}
		case 2:
			{
			// Ok key
			aKeyCode = EKeyDevice3;
			break;
			}
		default:
			{
			aKeyCode = EKeyNull;
			break;
			}
		}// switch(aIndex) ends
	}

/*******************************************************************
*
* Inactive key code
*
*******************************************************************/
void CSimulateKeyEvents::InactiveKeyCodeL( const TInt aIndex, TKeyCode& aKeyCode )
	{

    if ( aIndex > KInactiveKeyEvents || aIndex < 0)
        {
        aKeyCode = EKeyNull;
        }

    else
        {
        TKeyCode keyCodes[KInactiveKeyEvents] = 
            {
            EKeyLeftArrow, EKeyRightArrow,EKeyYes,EKeyLeftShift, EKeyRightShift, 
            EKeyOff, EKeyNo, EKeyApplication0, EKeyApplication1, EKeyApplication2, 
            EKeyApplication3, EKeyApplication4, EKeyApplication5, EKeyApplication6,
            EKeyApplication7, EKeyApplication8, EKeyApplication9, EKeyApplicationA, 
            EKeyApplicationB, EKeyApplicationC, EKeyApplicationD, EKeyApplicationE, 
            EKeyApplicationF, TKeyCode('1'), TKeyCode('2'), TKeyCode('3'), TKeyCode('4'),
            TKeyCode('5'), TKeyCode('6'), TKeyCode('7'), TKeyCode('8'), TKeyCode('9'), 
            TKeyCode('0'), TKeyCode('*'), TKeyCode('#'),
            EKeyDevice2,    //EKeyDevice2 == EKeyPowerOff
            EKeyLeftArrow,  //for EClearKey
            };

            aKeyCode = keyCodes[aIndex];
        }
	}
// End of File
