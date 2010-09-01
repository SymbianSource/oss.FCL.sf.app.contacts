/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       
*
*/


#ifndef __CMSSESSION_H__
#define __CMSSESSION_H__

// INCLUDES
#include <e32std.h>
#include <f32file.h>

//Constants
#ifdef _DEBUG
    _LIT( KCmsClientLogDir,                       "CCA" );
    _LIT( KCmsClientLogFile,                      "cmsclient.txt" );
    const TInt KClientLogBufferMaxSize            = 2000;
#endif

//FORWARD DECLARATIONS

//CLASS DECLARATION
class RCmsSession : public RSessionBase
    {
    public:  // New functions
        
        /**
        * C++ Constructor.
        */
        IMPORT_C RCmsSession();
        
        /**
        * Connect to the server side session object
        *
        * @return TInt Error code
        */
        IMPORT_C TInt Connect();

        /**
        * Return the version of this component
        *
        * @return TVersion The version number
        */
        IMPORT_C TVersion Version() const;

        /**
        * Destructor
        */
        IMPORT_C ~RCmsSession();
        
    public:   //Not exported
        
        /**
        * Start the server
        *
        * @return TInt Error code
        */
        TInt StartServer();
        
        /**
        * Is the server already running
        *
        * @return TBool Is the server running
        */
        TBool IsRunning();

    public:  //Static

        /**
        * A global logging function for 8 bit data.
        *
        * @param aCommand command to be handled
        */
        static void WriteToLog( TRefByValue<const TDesC8> aFmt,... );

    private: //Data
        
        TBool                                            iConnected;
    };

#endif      //__CMSCLIENT_H__



