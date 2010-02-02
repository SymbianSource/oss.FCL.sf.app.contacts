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
*     Produces objects that implement MLogsEvent
*
*/


#ifndef __Logs_Engine_CLogsModelFactory_H__
#define __Logs_Engine_CLogsModelFactory_H__

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "MLogsModel.h"
#include "CLogsEngine.h"

// FORWARD DECLARATION
struct TLogsEventStrings;
// CLASS DECLARATION

/**
 *  Produces objects that implement MLogsEvent
 */
class CLogsModelFactory :  public CBase
    {
    public:
       /**
        * produces MLogsModel implementation object
        *
        * @param    aModel  model type
        * @param aStrings     log client constants
        * @return   MLogsModel implementation object
        */
        static MLogsModel* ModelL( 
            RFs& aFsSession, 
            TLogsModel aModel,
            TLogsEventStrings& aStrings,
            CLogsEngine* aLogsEngine );                                
    };

#endif      // __Logs_Engine_CLogsModelFactory_H__



// End of File
