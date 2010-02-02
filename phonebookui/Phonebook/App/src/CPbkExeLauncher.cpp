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
*     Application launcher factory.
*
*/


#include "CPbkExeLauncher.h"
#include <apgcli.h>
#include <apacmdln.h>

MPbkApplicationLauncher* CPbkExeLauncher::NewLC()
    {
    CPbkExeLauncher* self = new(ELeave) CPbkExeLauncher;
    CleanupStack::PushL(self);
	return self;
	}

void CPbkExeLauncher::LaunchApplicationL(TUid aAppUid)
    {
    // Connect to application architure server
    RApaLsSession appArch;
    User::LeaveIfError(appArch.Connect());
    CleanupClosePushL(appArch);
        
    // Get application info
    TApaAppInfo appInfo;
    User::LeaveIfError(appArch.GetAppInfo(appInfo, aAppUid));

    TFileName appName = appInfo.iFullName;

    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    cmdLine->SetExecutableNameL(appName);
    cmdLine->SetCommandL(EApaCommandRun);
    User::LeaveIfError(appArch.StartApp(*cmdLine));

    CleanupStack::PopAndDestroy(cmdLine);
    CleanupStack::PopAndDestroy(); // appArch
	}

// End of File
