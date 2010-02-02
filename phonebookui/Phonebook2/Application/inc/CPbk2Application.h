/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 application class.
*
*/


#ifndef CPBK2APPLICATION_H
#define CPBK2APPLICATION_H

//  INCLUDES
#include <aknapp.h> // CAknApplication

// CLASS DECLARATION

/**
 * Phonebook 2 application class.
 * This class is created by the Symbian OS framework by a call
 * to NewApplication() function when the application is started.
 * The main purpose of the application class is to create the
 * application-specific document object (CPbk2Document in this case)
 * via a call to virtual CreateDocumentL().
 *
 * @see NewApplication
 */
class CPbk2Application : public CAknApplication
    {
    public: // Interface

        /**
         * Default constructor.
         */
        CPbk2Application();

        /**
         * Returns application's shared file server connection.
         *
         * @return  File server connection.
         */
        RFs& FsSession();

        /**
         * Return applications control environment.
         *
         * @return  Control environment.
         */
        CCoeEnv& CoeEnv();

    private: // From CApaApplication
        ~CPbk2Application();
        CApaDocument* CreateDocumentL();
        TUid AppDllUid() const;
    };

#endif // CPBK2APPLICATION_H

// End of File
