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
* Description:  Phonebook Application class. 
*		This class is created by the Symbian OS framework by a call to NewApplication()
*		function when the application is started. The main purpose of the 
*		application class is to create the application-specific document object 
*		(CPbkDocument in this case) via a call to virtual CreateDocumentL().
*
*/


#ifndef __CPbkApplication_H__
#define __CPbkApplication_H__

//  INCLUDES
#include <aknapp.h> // CAknApplication


// CLASS DECLARATION

/**
 * Phonebook Application class. 
 * This class is created by the Symbian OS framework by a call to NewApplication()
 * function when the application is started. The main purpose of the 
 * application class is to create the application-specific document object 
 * (CPbkDocument in this case) via a call to virtual CreateDocumentL().
 *
 * @see NewApplication
 */
class CPbkApplication : 
        public CAknApplication 
    {
    public: // Interface
        /**
         * Default constructor.
         */
        inline CPbkApplication();

        /**
         * Returns application's shared file server connection.
         */
        RFs& FsSession();

        /**
         * Return applications control environment.
         */
        CCoeEnv& CoeEnv();

    private: // from CApaApplication
        ~CPbkApplication();
	    CApaDocument* CreateDocumentL();
        TUid AppDllUid() const;
    };


// INLINE FUNCTIONS

inline CPbkApplication::CPbkApplication()
    {
    }

#endif // __CPbkApplication_H__
            
// End of File
