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
* Description:  Phonebook 2 application services interface.
*
*/


#ifndef MPBK2APPLICATIONSERVICES2_H
#define MPBK2APPLICATIONSERVICES2_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CPbk2StoreManager;
class CPbk2ServiceManager;

// CLASS DECLARATION

/**
 * This class is an extension to MPbk2ApplicationServices.
 * See documentation of MPbk2ApplicationServices from header
 * MPbk2ApplicationServices.h 
 * 
 * You can access this extension by calling
 * MPbk2ApplicationServices->MPbk2ApplicationServicesExtension()
 */
class MPbk2ApplicationServices2
    {
    public: // Interface
        /**
         * Returns the concrete store manager instance.
         *
         * @return  Store manager.
         */
        virtual CPbk2StoreManager& StoreManager() const = 0;
        
        /**
         * Returns service manager.
         *
         * @return  Service manager.
         */
        virtual CPbk2ServiceManager& ServiceManager() const = 0;
        
     protected: // Protected destructor
        ~MPbk2ApplicationServices2()
            {}
    };

#endif // MPBK2APPLICATIONSERVICES2_H

// End of File
