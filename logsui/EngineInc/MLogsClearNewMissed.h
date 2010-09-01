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
*     
*
*/


#ifndef __MLogsClearNewMissed_H_
#define __MLogsClearNewMissed_H_

// INCLUDES
//#include <logwrap.h>
//#include "LogsEng.hrh"
#include "MLogsEventGetter.h"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES


// CLASS DECLARATION

/**
 *	Defines abstract interface for setting logs missed calls events to read status.
 */
class MLogsClearNewMissed
    {
    public:
    
        virtual ~MLogsClearNewMissed() {};

    public:
       /**
        *   Delete model data
        *
        * @param aModel
        */
        virtual void SetNewMissedCallRead(TLogId aLogId, TInt aDuplicates ) = 0;    
          
    };
#endif

// End of File  __MLogsClearNewMissed_H_
