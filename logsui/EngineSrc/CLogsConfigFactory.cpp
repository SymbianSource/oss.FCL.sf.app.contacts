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
*     Produces objects that implement MLogsConfig
*
*/


// INCLUDE FILES
#include "CLogsConfigFactory.h"
#include "CLogsConfig.h"

// CONSTANTS
//For Log db settings manipulation
MLogsConfig* CLogsConfigFactory::LogsConfigL( RFs& aFsSession,
                                                   MLogsObserver* aObserver )
    {
    return CLogsConfig::NewL( aFsSession, aObserver );
    }
