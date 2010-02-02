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
*     Defines abstract interface for Logs configuration
*
*/


#ifndef __MLogsConfig_H_
#define __MLogsConfig_H_

// INCLUDES
#include <logcli.h>
#include "LogsEng.hrh"

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class MLogsObserver;

// CLASS DECLARATION

/**
 *	Defines abstract interface for Logs configuration
 */
class MLogsConfig
    {
    public:
    
        virtual ~MLogsConfig() {};

    public:

        /**
         * Set configuration
         *
         * @param aConfig reference to configuration structure
         */
        virtual void SetConfig( const TLogConfig& aConfig ) = 0;

        /**
         * Get configuration
         *
         * @param aConfig reference to configuration structure
         */
        virtual void GetConfig( TLogConfig& aConfig ) = 0;

        /**
         *   Observer setter
         *
         *   @param  aObserver   pointer to observer
         */
        virtual void SetObserver( MLogsObserver* aObserver ) = 0; 
    };

            
#endif

// End of File  __MLogsConfig_H_
