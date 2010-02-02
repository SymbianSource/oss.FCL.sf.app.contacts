/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  IPC connection parameters.
*
*/


#ifndef T_CCACLIENTSERVERAPPIPC_H
#define T_CCACLIENTSERVERAPPIPC_H

#include <apaserverapp.h>

/** CCA server app IPC command set. */
enum TCCAAppServerIPC
    {
    ECCALaunchApplication = RApaAppServiceBase::KServiceCmdBase,
    ECCACancelLaunchApplication,
    ECCAGetWindowGroupId
    };

#endif // T_CCACLIENTSERVERAPPIPC_H

// End of File
