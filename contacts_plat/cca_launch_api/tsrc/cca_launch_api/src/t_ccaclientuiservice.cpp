/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s32mem.h>
#include "t_ccaclientuiservice.h"
#include "ccauids.h"
#include "ccaparameter.h"

// --------------------------------------------------------------------------
// RCCAClientUIService::RCCAClientUIService
// --------------------------------------------------------------------------
//
RCCAClientUIService::RCCAClientUIService()
    {
    }

// --------------------------------------------------------------------------
// RCCAClientUIService::DoLaunch
// --------------------------------------------------------------------------
//
void RCCAClientUIService::DoLaunch(TRequestStatus& aStatus, TPtr8& aMessagePtr)
    {
    TRequestStatus* status = &aStatus;
    //SHOULD BE ALWAYS COME TO KERRNONE CASE.     
    if (aMessagePtr.Size()>0)
        {
        User::RequestComplete(status, KErrNone);
        }
    else
        {
        User::RequestComplete(status, KErrCorrupt);
        }

    }

// --------------------------------------------------------------------------
// RCCAClientUIService::ConnectL
// --------------------------------------------------------------------------
//
void RCCAClientUIService::DoConnectL()
    {
    }
    
// --------------------------------------------------------------------------
// RCCAClientUIService::DoCloseApplication
// --------------------------------------------------------------------------
//
void RCCAClientUIService::DoCloseApplication()
    {
    }
    
// --------------------------------------------------------------------------
// RCCAClientUIService::ServiceUid
// --------------------------------------------------------------------------
//
TUid RCCAClientUIService::ServiceUid() const
    {
    return TUid::Uid( KCCAClientServiceType);
    }

// End of File
