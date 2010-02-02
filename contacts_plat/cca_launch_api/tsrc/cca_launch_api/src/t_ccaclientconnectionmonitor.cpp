/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES

#include <e32std.h>
#include <s32mem.h>

// own project
#include "ccaclientheaders.h"
//#include "ccaclientconnectionmonitor.h"
//#include "ccauids.h"
//#include "ccaparameter.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CCCAConnectionMonitor::CCCAConnectionMonitor
// ----------------------------------------------------------
//
CCCAConnectionMonitor::CCCAConnectionMonitor()
    {
    }

// ----------------------------------------------------------
// CCCAConnectionMonitor::~CCCAConnectionMonitor
// ----------------------------------------------------------
//
CCCAConnectionMonitor::~CCCAConnectionMonitor()
    {
    delete iExitAppServerMonitor;
    iExitAppServerMonitor = NULL;
    }

// ----------------------------------------------------------
// CCCAConnectionMonitor::ConstructL
// ----------------------------------------------------------
//
void CCCAConnectionMonitor::ConstructL(RApaAppServiceBase& /*aClient*/,
        MApaServerAppExitObserver& /*aObserver*/, TInt /*aPriority*/)
    {
    }

// ----------------------------------------------------------
// CCCAConnectionMonitor::NewL
// ----------------------------------------------------------
//
CCCAConnectionMonitor* CCCAConnectionMonitor::NewL(
        RCCAClientUIService& aClient, MApaServerAppExitObserver& aObserver,
        TInt aPriority)
    {
    CCCAConnectionMonitor* self = new (ELeave) CCCAConnectionMonitor( );
    CleanupStack::PushL(self);
    self->ConstructL(aClient, aObserver, aPriority);
    CleanupStack::Pop(self);
    return self;
    }

// End of file
