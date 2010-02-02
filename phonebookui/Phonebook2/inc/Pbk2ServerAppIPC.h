/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 server application IPC command set.
*
*/


#ifndef PBK2SERVERAPPIPC_H
#define PBK2SERVERAPPIPC_H

// INCLUDES
#include <apaserverapp.h>

/**
 * Phonebook 2 server app IPC command set.
 */
enum TPbk2AppServerIPC
    {
    EPbk2LaunchFetch = RApaAppServiceBase::KServiceCmdBase,
    EPbk2LaunchAssign,
    EPbk2LaunchAttributeAssign,
    EPbk2LaunchAttributeUnassign,
    EPbk2CancelFetch,
    EPbk2CancelAssign,
    /// Accepts or denies the service
    EPbk2AcceptService,
    /// Accepts or denies exit request
    EPbk2ExitService,
    /// Request exit notification
    EPbk2ExitRequest,
    /// Cancels exit request notification
    EPbk2CancelExitRequest,
    /// Request accept notification
    EPbk2AcceptRequest,
    /// Cancels accept request notification
    EPbk2CancelAcceptRequest,
    EPbk2GetResults,
    EPbk2GetResultSize
    };

/**
 * Phonebook 2 attribute assign data structure.
 */
struct TPbk2AttributeAssignData
    {
    /// Uid of the attribute to assign
    TUid iAttributeUid;
    /// Value of the attribute (for example a speed dial index)
    TInt iAttributeValue;
    };

#endif // PBK2SERVERAPPIPC_H

// End of File
