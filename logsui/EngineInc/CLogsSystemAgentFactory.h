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
*     Produces objects that implement MLogsSystemAgent
*
*/


#ifndef __Logs_Engine_CLogsSystemAgentFactory_H__
#define __Logs_Engine_CLogsSystemAgentFactory_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATION
class MLogsSystemAgent;
class MLogsObserver;
class MLogsCallObserver;

// CLASS DECLARATION

/**
 *  Produces objects that implement MLogsSystemAgent
 */
class CLogsSystemAgentFactory :  public CBase
    {

    public:

       /**
        * produces MLogsSystemAgent implementation object
        *
        * @param    aObserver observer
        * @return   MLogsSystemAgent implementation object
        */
        static MLogsSystemAgent* LogsSystemAgentL( 
                                        MLogsObserver* aObserver = NULL,
                                        MLogsCallObserver* aCallObserver = NULL );       
    };

#endif      // __Logs_Engine_CLogsSystemAgentFactory_H__



// End of File
