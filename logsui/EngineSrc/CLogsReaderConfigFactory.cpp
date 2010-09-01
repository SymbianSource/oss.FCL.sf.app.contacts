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
*     Produces objects that implement MLogsReaderConfig
*
*/


// INCLUDE FILES
#include "CLogsReaderConfigFactory.h"
#include "CLogsReaderConfig.h"

// CONSTANTS

EXPORT_C MLogsReaderConfig* CLogsReaderConfigFactory::LogsReaderConfigLC( 
                                                    const TLogsFilter aFilter )
    {
    return CLogsReaderConfig::NewLC( aFilter, NULL, NULL );
    }

EXPORT_C MLogsReaderConfig* CLogsReaderConfigFactory::LogsReaderConfigLC( 
                                                    const TDesC* aNumber,
                                                    const TDesC* aRemoteParty )
    {
    return CLogsReaderConfig::NewLC( ELogsFilterPerson, aNumber, aRemoteParty );
    }
       
EXPORT_C MLogsReaderConfig* CLogsReaderConfigFactory::LogsReaderConfigLC( 
                        const TDesC* aNumber ) 
    {
    return CLogsReaderConfig::NewLC( aNumber );
    }

EXPORT_C MLogsReaderConfig* CLogsReaderConfigFactory::LogsReaderConfigLC( 
                                                    const TDesC* aNumber,
                                                    const TDesC* aRemoteParty,
                                                    const TDesC8* aContackLink ) 
    {
    return CLogsReaderConfig::NewLC( aNumber, aRemoteParty, aContackLink );
    }
