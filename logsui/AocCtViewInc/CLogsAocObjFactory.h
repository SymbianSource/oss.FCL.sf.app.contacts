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
*     Object factory for Logs. Instantiates Aoc related objects.
*
*/


#ifndef __CLogsAocObjFactory_H
#define __CLogsAocObjFactory_H

//  INCLUDE FILES  
#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATIONS

class MLogsAocUtil;
class MLogsCallStatus;

//  CLASS DECLARATIONS 

class CLogsAocObjFactory: public CBase
    {
    public:

        /**
         * Instantiate MAocUtil object
         * 
         * @return MAocUtil object
         */
        static IMPORT_C MLogsAocUtil* AocUtilL();

        /**
         * Instantiate MCallStatus object
         * 
         * @return MCallStatus object
         */
        static IMPORT_C MLogsCallStatus* CallStatusLC();

    };

#endif  // __CLogsAocObjFactory_H

// End of file
