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
*     Produces objects that implement MLogsReader
*
*/


#ifndef __Logs_Engine_CLogsReaderFactory_H__
#define __Logs_Engine_CLogsReaderFactory_H__

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "LogsEng.hrh"

// FORWARD DECLARATION

// CLASS DECLARATION
class CPbkContactEngine;
class MLogsObserver;
class MLogsEvent;
class MLogsReader;
class MLogsEventArray;
struct TLogsEventStrings;
class CLogsEngine;

/**
 *  Produces objects that implement MLogsReader
 */
class CLogsReaderFactory :  public CBase
    {

    public:

       /**
        * produces MLogsReader implementation object
        *
        *   @param      aFsSession   ref. to file server session
        *   @param      aEventArray  array of events
        *   @param      aStrings     log client constants
        *   @param      aModelId     model id
        *   @param      aObserver    observer
        *   @return     MLogsReader implementation object
        */
        static MLogsReader* LogsReaderL( RFs& aFsSession,
                    MLogsEventArray& aEventArray,
                    TLogsEventStrings& aStrings,
                    TLogsModel aModelId, MLogsObserver* aObserver,
                    CLogsEngine* aLogsEngineRef ); 
        
       /**
        * produces MLogsReader implementation object
        *
        *   @param      aFsSession   ref. to file server session
        *   @param      aObserver    observer
        *   @param      aPbkEngine   phonebook engine
        *   @param      aSMS if ETrue return SMS event updater
        *   @return     MLogsReader implementation object
        */
        /****        
        static MLogsReader* LogsEventUpdaterL( RFs& aFsSession,
                                               CPbkContactEngine* aPbkEngine, 
                                               MLogsObserver* aObserver,
                                               TBool aSMS = ETrue ); 
        *****/                                               
    };

#endif      // __Logs_Engine_CLogsReaderFactory_H__

// End of File
