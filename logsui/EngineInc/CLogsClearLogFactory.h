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
*     Produces objects that implement MLogsClearLog
*
*/


#ifndef __Logs_Engine_CLogsClearLogFactory_H__
#define __Logs_Engine_CLogsClearLogFactory_H__

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATION

// CLASS DECLARATION
class MLogsClearLog;
class MLogsObserver;

/**
 *  Produces objects that implement MLogsClearLog
 */
class CLogsClearLogFactory :  public CBase
    {

    public:

       /**
        * produces MLogsClearLog implementation object
        *
        * @param aFsSession file server session
        * @param aObserver observer
        * @return   MLogsClearLog implementation object
        */
        static MLogsClearLog* LogsClearLogL( RFs& aFsSession, 
                                    MLogsObserver* aObserver = NULL );     
    };

#endif      // __Logs_Engine_CLogsClearLogFactory_H__



// End of File
