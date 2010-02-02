// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __T_CURRENTDB_H__
#define __T_CURRENTDB_H__

// System includes
#include <cntdb.h>
#include <cntitem.h>
#include <cntfield.h>
#include <cntfldst.h>

// Classes referenced
class RTest;
class CConsoleBase;
class CCntTest;
class CTestBase;
class CTestManager;


//
// -------> CTestBase (header)
//
class CTestBase : public CActive, public MContactDbObserver
	{
//
public:							// ENUMERATIONS SHARED BETWEEN DRIVER & TEST
//
	enum
		{
		EStartTests = 0,
		ETest1 = EStartTests,
		ETest2,
		ETest3,
		ETest4,
		ETest5,
		ETest6,
		ETest7,
		ETest8,
		ETest9,
		EEndTests
		};
	
//
public:							// STATIC CONSTRUCT / DESTRUCT
//
	~CTestBase();

//
public:							// ACCESS
//
	void						Complete(TInt aReason);
	inline TInt					CurrentTest() const				{ return iCurrentTest; }
	inline RThread&				Thread()						{ return iThread; }
	inline TBool				Waiting() const					{ return iWaiting || iWaitingForAnyEvent; }

//
protected:						// INTERNAL CONSTRUCTION
//
	CTestBase(CTestManager& aTester, TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority);
	virtual void				ConstructL(const TDesC& aName);
	//
	static TInt					ThreadFunction(TAny *aParam);
	void						RunTestsL();

//
protected:						// TEST FRAMEWORK
//
	virtual void				OpenDatabaseL() = 0;
	virtual TInt				HandleThreadCreationL();
	virtual void				HandleThreadDeletionL();

//
protected:						// FROM MContactDbObserver
//
	void						HandleDatabaseEventL(TContactDbObserverEvent aEvent);

//
private:						// FROM CActive
//
	void						RunL();
	void						DoCancel();

//
protected:						// TEST RELATED
//
	void						ReadyForNextTest();
	void						WaitForContactsEvent(TContactDbObserverEventType aEvent, TInt aTimeInSecondsToWaitFor = 30);
	void						WaitForAnyContactsEvent(TInt aTimeInSecondsToWaitFor = 30);

	virtual void				DoTestL(TInt aTestNumber) = 0;
	void 						OpenSemaphoreL();
	void						CloseSemaphore();

//
protected:						// MEMBER DATA
//
	CTestManager&				iTestCoordinator;

	// Owned by this class after the thread has been created
	CCntTest*					iContactsTest;
	CConsoleBase*				iConsole;
	RTest*						iTest;

	//
	TInt						iCurrentTest;
	TInt						iClientNumber;
	RThread						iThread;
	TThreadPriority				iThreadPriority;
	
	//
	TBool						iWaiting;
	TBool						iWaitingForAnyEvent;
	TContactDbObserverEventType	iEvent;
	TContactDbObserverEvent		iLastEvent;
	RTimer						iWaitTimer;
	RSemaphore					iSemaphoreOne;
	RSemaphore					iSemaphoreTwo;
	};


//
// -------> CTestThreadOne (header)
//
class CTestThreadOne : public CTestBase
	{
//
public:							// FROM CTestBase
//
	static CTestThreadOne*		NewL(TInt aClientNumber, const TDesC& aName, CTestManager& aTester, TThreadPriority aThreadPriority = EPriorityNormal, TInt aPriority = EPriorityStandard);
	void						DoTestL(TInt aTestNumber);
	void						OpenDatabaseL();
	
//
private:						// INTERNAL
//
	CTestThreadOne(CTestManager& aTester, TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority);
	//
	void						DoTest1L();
	void						DoTest2L();
	void						DoTest3L();
	void						DoTest4L();
	void						DoTest5L();
	void						DoTest6L();
	void						DoTest7L();
	void						DoTest8L();
	void						DoTest9L();
	};


//
// -------> CTestThreadTwo (header)
//
class CTestThreadTwo : public CTestBase
	{
//
public:							// FROM CTestBase
//
	static CTestThreadTwo*		NewL(TInt aClientNumber, const TDesC& aName, CTestManager& aTester, TThreadPriority aThreadPriority, TInt aPriority = EPriorityStandard);
	void						DoTestL(TInt aTestNumber);
	void						OpenDatabaseL();
	
//
private:						// INTERNAL
//
	CTestThreadTwo(CTestManager& aTester, TInt aClientNumber, TThreadPriority aThreadPriority, TInt aPriority);
	//
	void						DoTest1L();
	void						DoTest2L();
	void						DoTest3L();
	void						DoTest4L();
	void						DoTest5L();
	void						DoTest6L();
	void						DoTest7L();
	void						DoTest8L();
	void						DoTest9L();
	};


//
// -------> CTestManager (header)
//
class CTestManager : public CTimer
	{
//
public:							// STATIC CONSTRUCT
//
	static CTestManager*		NewLC();
	static CTestManager*		NewL();
	~CTestManager();

//
private:						// INTERNAL CONSTRUCT
//
	CTestManager(TInt aPriority = EPriorityStandard);
	void						ConstructL();

//
private:						// FROM CActive
//
	void						RunL();

//
private:						// NEW FUNCTIONS
//
	void						IssueTimerRequest();

//
private:						// MEMBER DATA
//
    TInt						iTestNumber;
	//
	CTestThreadOne*				iClient1;
	CTestThreadTwo*				iClient2;
	};



#endif
