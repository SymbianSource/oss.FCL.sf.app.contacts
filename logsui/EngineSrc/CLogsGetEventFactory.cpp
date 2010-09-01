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


// INCLUDE FILES
#include "CLogsGetEventFactory.h"
#include "CLogsGetEvent.h"
#include "CLogsEventArray.h"
#include "MLogsEventArray.h"

#include "CLogsEvent.h"
//#include "TLogsEvent.h"
//#include "TLogsRecentEvent.h"
//#include "TLogsRecentMissedEvent.h"
// CONSTANTS

MLogsGetEvent* CLogsGetEventFactory::LogsGetEventL( RFs& aFsSession,
                                                   TLogsEventStrings& aStrings,
                                                   MLogsObserver* aObserver )
    {
    return CLogsGetEvent::NewL( aFsSession, aStrings, aObserver );
    }

MLogsEventArray* CLogsGetEventFactory::LogsEventArrayL( TLogsModel /* aModel */, 
													TInt aGranularity )
	{
	
    return CLogsEventArray<CLogsEvent>::NewL( aGranularity );	

	//OLD IMPL: FIXME: WE'LL USE CLogsEvent instead of the 4 below types
	/**********************************************************
	switch ( aModel )
		{
		case ELogsMainModel:
			{
		    return CLogsEventArray<TLogsEvent>::NewL( aGranularity );
			}
		case ELogsDialledModel:	// fallthrough
		case ELogsReceivedModel:
			{
			return CLogsEventArray<TLogsRecentEvent>::NewL( aGranularity );
			}
		case ELogsMissedModel:
			{
			return CLogsEventArray<TLogsRecentMissedEvent>::NewL( aGranularity );
			}
		default:
			{
			//Illegal model
			return NULL;
			}
		}
    **********************************************************/
	}

