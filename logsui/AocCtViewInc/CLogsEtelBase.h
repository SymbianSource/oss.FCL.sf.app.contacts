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
*     Logs etel base class
*
*/


#ifndef __CLogsEtelBase_H
#define __CLogsEtelBase_H

//  INCLUDE FILES  
#include <e32base.h> // CActive
#include <etel.h> // RTelServer
#include <etelmm.h> // RMobilePhone

// CONSTANTS

// times to try in PIN2 operations
const TInt KPin2Max = 3;


// FORWARD DECLARATIONS

//  CLASS DECLARATIONS 


class CLogsEtelBase: public CActive
    {
        
    protected:
        /**
         *  C++ Constructor
         */
        CLogsEtelBase();

        /**
         *  C++ Destructor
         */
        ~CLogsEtelBase();

    protected: // EPOC Telephony Server connection related functions
        /**
         * Connect ETel connection
         */
        TInt OpenEtelConnection();

        /**
         * Disconnect ETel connection
         */
        void CloseEtelConnection();

    protected: 
        void ShowSecUiNoteLD( TInt aResourceId );

    protected: // data
        /// Own: connection status
        TBool iConnectionOk;
              
        /// Own: Telephone server, for notifications
        RTelServer iServer;
        
        /// Own: custom phone for line handling, for notifications
        RMobilePhone iPhone;

    };

#endif  // __CLogsEtelBase_H

// End of file
