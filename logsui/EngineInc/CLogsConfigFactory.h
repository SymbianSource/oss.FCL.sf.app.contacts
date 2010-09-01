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


#ifndef __Logs_Engine_CLogsConfigFactory_H__
#define __Logs_Engine_CLogsConfigFactory_H__

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATION

// CLASS DECLARATION
class MLogsConfig;
class MLogsObserver;

/**
 *  Produces objects that implement MLogsConfig
 */
class CLogsConfigFactory :  public CBase
    {

    public:

       /**
        * produces MLogsConfig implementation object
        *
        * @param aFsSession file server session
        * @param aObserver observer
        * @return   MLogsConfig implementation object
        */
        static MLogsConfig* LogsConfigL( RFs& aFsSession, 
                                    MLogsObserver* aObserver = NULL );     
    };

#endif      // __Logs_Engine_CLogsConfigFactory_H__



// End of File
