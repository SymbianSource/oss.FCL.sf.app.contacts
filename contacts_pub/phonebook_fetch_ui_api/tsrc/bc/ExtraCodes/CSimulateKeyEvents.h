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








#ifndef __CSIMULATEKEYEVENTS_H__
#define __CSIMULATEKEYEVENTS_H__

#include "CSimulateKeyBase.h"

const	TKeyCode    enterCode(EKeyEnter);
const	TKeyCode    upCode(EKeyUpArrow);
const	TKeyCode    downCode(EKeyDownArrow);
const	TKeyCode    nullCode(EKeyNull);
const	TKeyCode    leftSoftKeyCode(EKeyDevice0); // Select 
const	TKeyCode    rightSoftKeyCode(EKeyDevice1); // Cancel
const	TInt        KSoftKeys = 2; // Number of tested soft keys
const	TInt        KInactiveKeyEvents = 37;
const	TInt        KSelectKeys = 3;
const	TInt 		KFocusTests = 2;

/**
 *
 * Class is for testing key events
 *
 */

class CSimulateKeyEvents : public CSimulateKeyBase
	{
	public:
		static CSimulateKeyEvents* NewL();
        ~CSimulateKeyEvents();

        /**
        *   Selects right key code
        */
		void EmailSelectKeyCodeL( const TInt aIndex, TKeyCode& aKeyCode );

        /**
        *   Selects right key code
        */
		void SelectKeyCodeL( const TInt aIndex, TKeyCode& aKeyCode );

        /**
        *   Inactive key codes
        */
		void InactiveKeyCodeL( const TInt aIndex, TKeyCode& aKeyCode );
		
	private:	// from CSimulateKeyBase
		void ExecuteKeySequenceL();

	private:	// implementation
		CSimulateKeyEvents();

	};

#endif // __CSIMULATEKEYEVENTS_H__

// End of File
