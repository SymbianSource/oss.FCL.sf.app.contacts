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
*           Provides phonebook settings command object methods.
*
*/


// INCLUDE FILES
#include "CPbkSettingsCmd.h"

#include <CPbkContactEngine.h>
#include <CPbkSettingsDlg.h>

#include <pbkdebug.h>

// ================= MEMBER FUNCTIONS =======================

// Default constructor
CPbkSettingsCmd::CPbkSettingsCmd (CPbkContactEngine& aEngine,
        MObjectProvider& aParent)
        : iEngine(aEngine), iParent(aParent)
	{
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSettingsCmd::CPbkSettingsCmd(0x%x)"), this);
    }


// Static constructor
CPbkSettingsCmd* CPbkSettingsCmd::NewL (CPbkContactEngine& aEngine,
        MObjectProvider& aParent)
    {
    CPbkSettingsCmd* self = new(ELeave) CPbkSettingsCmd(aEngine, aParent);
    CleanupStack::PushL(self);
    CleanupStack::Pop(); // self
    return self;
    }

// Destructor
CPbkSettingsCmd::~CPbkSettingsCmd()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSettingsCmd::~CPbkSettingsCmd(0x%x)"), this);
    }

void CPbkSettingsCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkSettingsCmd::ExecuteLD(0x%x)"), this);
    
	CleanupStack::PushL(this);

    CPbkSettingsDlg* settings = CPbkSettingsDlg::NewL(iEngine);
	settings->SetMopParent(&iParent);
	settings->ExecuteLD();

	// Destroy itself as promised
    CleanupStack::PopAndDestroy(); // this
    }

//  End of File  
