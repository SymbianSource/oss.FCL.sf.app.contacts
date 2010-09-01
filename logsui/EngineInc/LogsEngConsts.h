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
*     Contains constants used in Logs Engine
*
*/


#ifndef __LogsEngConsts_H__
#define __LogsEngConsts_H__



// CONSTANT LITERALS

// Used by CLogsGetEvent
_LIT( KLogsEmergencyCall911, "911" );   // Number in event considered to be emergency call 
_LIT( KLogsEmergencyCall, "112" );      // Number in event considered to be emergency call (in CDMA 
                                        // KLogsDataFldTag_Emergency is used instead 


// CONSTANT NUMBERS

// Used by CLogsGetEvent and by  CLogsMainReader
//const TInt KUndefinedIndex = 65535; // 2^16 - 1
const TLogFlags KLogEventALS = 0x4;

// MACROS

// DATA TYPES


#endif  // __LogsEngConsts_H__
           
// End of File
