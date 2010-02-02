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
*     Defines abstract interface for Logs reader configuration
*
*/


#ifndef __MLogsReaderConfig_H_
#define __MLogsReaderConfig_H_

// INCLUDES
#include "LogsEng.hrh"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *	Defines abstract interface for Logs reader configuration
 */
class MLogsReaderConfig
    {
    public:
    
        virtual ~MLogsReaderConfig() {};

    public:

       /**
        *   Filter
        *
        *   @return filter to be used ( if needed ) 
        */
        virtual TLogsFilter Filter() const = 0;

       /**
        *   Number
        *
        *   @return number to be used ( if needed )
        */
        virtual TDesC& Number() const = 0;

       /**
        *   Remote party
        *
        *   @return remote party to be used ( if needed )
        */
        virtual TDesC& RemoteParty() const = 0;                

        /**
        *   Contact link
        *
        *   @return contact link to be used ( if needed )
        */
        virtual TDesC8& ContactLink() const = 0;
    };

            
#endif

// End of File  __MLogsReaderConfig_H_
