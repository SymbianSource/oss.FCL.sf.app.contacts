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
*     Log event strings
*
*/


#ifndef __Logs_Engine_TLogsEventStrings_H__
#define __Logs_Engine_TLogsEventStrings_H__

#include <logwrap.h>

struct TLogsEventStrings 
	{
	TLogString iInDirection;        // incoming direction text
    TLogString iOutDirection;       // outgoing direction text
	TLogString iMissedDirection;    // missed direction text    
    TLogString iInDirectionAlt;     // Incoming alternate line
    TLogString iOutDirectionAlt;    // Outgoing alternate line
    TLogString iFetched;            // Fetched
	TLogString iUnKnownNumber;      // unknown number text
	};

#endif 
