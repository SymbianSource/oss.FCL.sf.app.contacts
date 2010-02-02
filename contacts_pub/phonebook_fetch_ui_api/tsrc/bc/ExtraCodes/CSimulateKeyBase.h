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








#ifndef __CSIMULATEKEYBASE_H__
#define __CSIMULATEKEYBASE_H__

class CEikonEnv;

class CSimulateKeyBase : public CBase
	{
public:
	void CallBackStartL();
	void ClearAllKeyEventsL();
    void ReleaseMemory();
	void AddEventL(const TKeyEvent& aKeyEvent,
		const TEventCode& aEventCode);
	void AddKeyEventL(const TKeyEvent& aKeyEvent);
	void AddKeyEventCodeL(const TKeyCode& aKeyCode,
		TInt aRepeats=0);
	void AddKeyEventAsciiCodeL(const TUint& aKeyCode,
		TInt aRepeats=0);
	
public:
	static TBool CallBackL(TAny* aThis);

protected:
	CSimulateKeyBase();
	virtual ~CSimulateKeyBase();
	void ConstructL();

protected:
	virtual void ExecuteKeySequenceL()=0;

protected:
	CIdle*	iIdle;
	TInt iReason;
	CEikonEnv* iEnv;
	CArrayFix<TKeyEvent>* iKeyEvents;
	CArrayFix<TEventCode>* iEventCodes;
	};
#endif // __CSIMULATEKEYBASE_H__

// End of File
