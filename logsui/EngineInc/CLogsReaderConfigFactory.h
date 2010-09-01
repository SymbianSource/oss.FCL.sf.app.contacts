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


#ifndef __Logs_Engine_CLogsReaderConfigFactory_H__
#define __Logs_Engine_CLogsReaderConfigFactory_H__

//  INCLUDES
#include <e32base.h>
#include "LogsEng.hrh"

// FORWARD DECLARATION

// CLASS DECLARATION
class MLogsReaderConfig;

/**
 *  Produces objects that implement MLogsReaderConfig
 */
class CLogsReaderConfigFactory :  public CBase
    {

    public:

       /**
        * Produces MLogsReaderConfig implementation object.Reader configuration 
        * for filtering all events by event type.
        *
        * @param    aFilter filter
        * @return   MLogsReaderConfig implementation object
        */
        IMPORT_C static MLogsReaderConfig* LogsReaderConfigLC( 
                                                const TLogsFilter aFilter );
       /**
        * Produces MLogsReaderConfig implementation object.Reader configuration 
        * for filtering events from specified name and number.
        *
        * @param    aNumber number
        * @param    aRemoteParty remote party
        * @return   MLogsReaderConfig implementation object
        */
        IMPORT_C static MLogsReaderConfig* LogsReaderConfigLC( 
                                                const TDesC* aNumber,
                                                const TDesC* aRemoteParty );


       /**
        * Produces MLogsReaderConfig implementation object. Reader configuration 
        * for filtering all events with same number.
        *
        * @param    aNumber number
        * @return   MLogsReaderConfig implementation object
        */
        IMPORT_C static MLogsReaderConfig* LogsReaderConfigLC( 
                                                const TDesC* aNumber );

       /**
        * Produces MLogsReaderConfig implementation object.Reader configuration 
        * for filtering events from specified name, number or contact link.
        *
        * @param    aNumber number
        * @param    aRemoteParty remote party
        * @param    aContactLink contact link
        * @return   MLogsReaderConfig implementation object
        */
        IMPORT_C static MLogsReaderConfig* LogsReaderConfigLC( 
                                                        const TDesC* aNumber,
                                                        const TDesC* aRemoteParty,
                                                        const TDesC8* aContactLink );
        
    };

#endif      // __Logs_Engine_CLogsReaderConfigFactory_H__



// End of File
