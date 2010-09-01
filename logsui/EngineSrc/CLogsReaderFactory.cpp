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


// INCLUDE FILES
#include "CLogsReaderFactory.h"
#include "CLogsMainReader.h"
#include "CLogsRecentReader.h"
#include "CLogsSMSEventUpdater.h"
#include "CLogsEventUpdater.h"
#include "MLogsEventArray.h"

// CONSTANTS

MLogsReader* CLogsReaderFactory::LogsReaderL( RFs& aFsSession,
            MLogsEventArray& aEventArray,
            TLogsEventStrings& aStrings,
            TLogsModel aModelId, MLogsObserver* aObserver,
            CLogsEngine* aLogsEngineRef )
    {
    switch( aModelId )
        {
        case ELogsMainModel:
            return CLogsMainReader::NewL( aFsSession, aEventArray,
                                            aStrings, aModelId, aObserver,
                                            aLogsEngineRef );         
            //break;

        case ELogsDialledModel: // fall through
        case ELogsReceivedModel:
        case ELogsMissedModel:
            return CLogsRecentReader::NewL( aFsSession, aEventArray,
                                            aStrings, aModelId, aObserver,
                                            aLogsEngineRef ); 
            //break;

        default:
            return NULL;
            //break;
        }
    }

/**********
MLogsReader* CLogsReaderFactory::LogsEventUpdaterL( RFs& aFsSession,
          CPbkContactEngine* aPbkEngine, MLogsObserver* aObserver, TBool aSMS )
    {
    if( aSMS )
        {        
        return CLogsSMSEventUpdater::NewL( aFsSession, aPbkEngine ,aObserver );
        }
    else
        {
        return CLogsEventUpdater::NewL( aFsSession, aPbkEngine, aObserver );
        }
    }
**********/    

                    
                    
