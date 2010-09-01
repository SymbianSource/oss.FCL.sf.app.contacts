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
*     Produces objects that implement MLogsSharedData
*
*/


#ifndef __Logs_Engine_CLogsSharedDataFactory_H__
#define __Logs_Engine_CLogsSharedDataFactory_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATION

// CLASS DECLARATION
class MLogsSharedData;

/**
 *  Produces objects that implement MLogsEvent
 */
class CLogsSharedDataFactory :  public CBase
    {

    public:

       /**
        * produces MLogsSharedData implementation object
        *
        * @return   MLogsSharedData implementation object
        */
        static MLogsSharedData* SharedDataL();                                
    };

#endif      // __Logs_Engine_CLogsSharedDataFactory_H__



// End of File
