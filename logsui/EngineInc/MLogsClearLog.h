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
*     Defines abstract interface for deleting data from log db.
*
*/


#ifndef __MLogsClearLog_H_
#define __MLogsClearLog_H_

// INCLUDES
#include <logwrap.h>
#include "LogsEng.hrh"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MLogsObserver;
// CLASS DECLARATION

/**
 *	Defines abstract interface for deleting events from log db.
 */
class MLogsClearLog
    {
    public:
    
        virtual ~MLogsClearLog() {};

    public:
       /**
        *   Delete model data
        *
        * @param aModel
        */
        virtual void ClearModelL( const TLogsModel aModel ) = 0;       

       /**
        *   Delete single event from recent list.
        * @param aId
        */
        virtual void DeleteEventL( TLogId aId ) = 0;       
       

       /**
        *   Set observer
        *
        * @param aObserver pointer to observer
        */
        virtual void SetObserver( MLogsObserver* aObserver ) = 0;         
    };
#endif

// End of File  __MLogsClearLog_H_
