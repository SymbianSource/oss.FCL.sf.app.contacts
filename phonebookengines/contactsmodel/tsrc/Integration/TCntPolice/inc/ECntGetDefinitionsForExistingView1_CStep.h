// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ECntGetDefinitionsForExistingView1_H_
#define ECntGetDefinitionsForExistingView1_H_

//Include the suite header
#include "CSuite.h"

#include "CCntServer.h"
#include "CCntIpcCodes.h"


class CECntGetDefinitionsForExistingView1Step: public CCapabilityTestStep
	{

public:

	//Get the version of the server to be called
	TVersion Version()
	{
		return TVersion(KLockSrvMajorVersionNumber, KLockSrvMinorVersionNumber, KLockSrvBuildVersionNumber);
	}	
	
	
	//Constructor called from the respective Suite.cpp from their "AddTestStep" function
	CECntGetDefinitionsForExistingView1Step() ;

	//Always clean your mess
	~CECntGetDefinitionsForExistingView1Step()
	{
		tChildThread.Close();
	}		
	
	//This is the Function called from "doTestStepL" by the test Suite,and it creates an 
	//child thread which internally calls the corresponding Exec_SendReceive_SERVERNAME fn.
	TVerdict MainThread();

	//Here's where the connection and testing the message takes place
	TInt Exec_SendReceive();
	
	};

#endif
