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
* Description:  Phonebook 2 server application document.
*
*/


#ifndef CPBK2SERVERAPPDOCUMENT_H
#define CPBK2SERVERAPPDOCUMENT_H

// INCLUDES
#include <CPbk2DocumentBase.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionManager;
class MPbk2ApplicationServices;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 server application document.
 * Responsible for creating application UI and creating and owning
 * application services object and UI extension manager.
 */
class CPbk2ServerAppDocument : public CPbk2DocumentBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aApp  Reference to EIK application object.
         * @return  A new instance of this class.
         */
        static CPbk2ServerAppDocument* NewL(
                CEikApplication& aApp );

        /**
         * Destructor.
         */
        ~CPbk2ServerAppDocument();

    public: // Interface

        /**
         * Initializes Phonebook 2 server application globals.
         */
        void CreateGlobalsL();

        /**
         * Initializes Phonebook 2 application services.
         *
         * @return  Phonebook 2 application services.
         */
        MPbk2ApplicationServices* ApplicationServicesL();

    public: // From CPbk2DocumentBase
        CEikAppUi* CreateAppUiL();
        CPbk2UIExtensionManager& ExtensionManager() const;

    private: // Implementation
        CPbk2ServerAppDocument(
                CEikApplication& aApp );

    private: // Data
        /// Own: Extension manager
        CPbk2UIExtensionManager* iExtensionManager;
        /// Own: Application services
        CPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2SERVERAPPDOCUMENT_H

// End of File
