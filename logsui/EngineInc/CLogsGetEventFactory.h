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
*     Produces objects that implement MLogsGetEvent
*
*/


#ifndef __Logs_Engine_CLogsGetEventFactory_H__
#define __Logs_Engine_CLogsGetEventFactory_H__

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "LogsEng.hrh"

// FORWARD DECLARATION

// CLASS DECLARATION
class MLogsGetEvent;
class MLogsObserver;
class MLogsEventArray;
struct TLogsEventStrings;

/**
 *  Produces objects that implement MLogsConfig
 */
class CLogsGetEventFactory :  public CBase
    {

    public:

       /**
        * produces MLogsGetEvent implementation object
        *
        * @param aFsSession file server session
        * @param aStrings     log client constants
        * @param aObserver observer
        * @return   MLogsGetEvent implementation object
        */
        static MLogsGetEvent* LogsGetEventL( RFs& aFsSession, 
                                    TLogsEventStrings& aStrings,
                                    MLogsObserver* aObserver = NULL );     

	   /**
        * produces MLogsEventArray implementation object
        *
        * @param	aModel Logs model for the event array
		* @param	aGranularity granularity for the event array
        * @return   MLogsEventArray implementation object
        */
        static MLogsEventArray* LogsEventArrayL( TLogsModel aModel, 
                                     TInt aGranularity);     

    };

#endif      // __Logs_Engine_CLogsGetEventFactory_H__



// End of File
