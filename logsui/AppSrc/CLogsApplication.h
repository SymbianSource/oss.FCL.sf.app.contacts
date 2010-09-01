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
*     Logs Application class
*
*/


#ifndef     __Logs_App_CLogsApplication_H__
#define     __Logs_App_CLogsApplication_H__

#include <aknapp.h>


//  INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Logs Application class. This class is created by the Symbian OS framework by a 
 * call to NewApplication() function when the application is started. The main
 * purpose of the application class is to create the application-specific 
 * document object (CLogsDocument in this case) via a call to virtual 
 * CreateDocumentL().
 *
 * @see NewApplication
 */
class   CLogsApplication : public CAknApplication
    {
    public: // New functions
        // Default constructor and destructor are ok for this class.

        /**
         * Raises a panic (internal error) and terminates the program.
         *
         * @param aPanic panic code. This code will be reported to the user.
         *
         * @see CLogsApplication#TPanicCode
         */
        static void Panic( TInt aPanic );

    private: // from CApaApplication
        /**
         * Creates the Logs document object (CLogsDocument). Called by the application 
         * framework.
         * 
         * @return New Logs document object (CLogsDocument).
         *
         * @exception KErrNoMemory if out of memory.
         * @exception anything thrown by CLogsDocument construction.
         */
        CApaDocument* CreateDocumentL();

    public: // from CApaApplication
        /**
         * Returns the UID of this application.
         * Override from CApaApplication.
         */
        TUid AppDllUid() const;
    };

#endif  // __Logs_App_CLogsApplication_H__

// End of File

