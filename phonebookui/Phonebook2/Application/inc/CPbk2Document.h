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
* Description:  Phonebook 2 document.
*
*/


#ifndef CPBK2DOCUMENT_H
#define CPBK2DOCUMENT_H

// INCLUDES
#include <CPbk2DocumentBase.h>

// FORWARD DECLARATIONS
class CPbk2Application;
class CPbk2UIExtensionManager;
class MPbk2ApplicationServices;
class CPbk2UIApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 document class.
 * An object of this class is created by the Symbian OS framework by a call
 * to CPbk2Application::CreateDocumentL(). The application framework
 * creates the Phonebook 2 application UI object (CPbk2AppUi) by a call
 * to the virtual function CreateAppUiL().
 *
 * Responsible for creating application UI and creating and owning
 * application services object and UI extension manager.
 */
class CPbk2Document : public CPbk2DocumentBase
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aApp  Reference to Eikon application object.
         * @return  A new instance of this class.
         */
        static CPbk2Document* NewL(
                CEikApplication& aApp );

    public: // Interface

        /**
         * Returns the Phonebook 2 application object.
         *
         * @return  Phonebook 2 application object.
         */
        CPbk2Application* Pbk2Application() const;

        /**
         * Initializes Phonebook 2 application globals.
         */
        void CreateGlobalsL();

        /**
         * Initializes Phonebook 2 application services.
         *
         * @return  Phonebook 2 application services.
         */
        MPbk2ApplicationServices* ApplicationServicesL();


    public: // From CPbk2DocumentBase
        CPbk2UIExtensionManager& ExtensionManager() const;

    private: // From CAknDocument
        CEikAppUi* CreateAppUiL();

    private:  // Implementation
        CPbk2Document(
                CEikApplication& aApp );
        ~CPbk2Document();
        void ConstructL();

    private: // Data
        /// Own: UI Extension manager
        CPbk2UIExtensionManager* iExtensionManager;
        /// Own: Application services
        CPbk2UIApplicationServices* iAppServices;
    };

#endif // CPBK2DOCUMENT_H

// End of File
