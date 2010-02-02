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
* Description:  Phonebook 2 application view factory.
*
*/


#ifndef CPBK2APPVIEWFACTORY_H
#define CPBK2APPVIEWFACTORY_H

//  INCLUDES
#include <e32std.h>  // for TUid
#include <e32base.h>

//  FORWARD DECLARATIONS
class CAknView;
class CPbk2AppUi;
class CPbk2UIExtensionManager;

// CLASS DECLARATION

/**
 * Phonebook 2 application view factory. CPbk2AppUi uses this class
 * to create the application view objects. Removes direct dependencies
 * from CPbk2AppUi to application view classes.
 */
class CPbk2AppViewFactory : public CBase
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aAppUi    Application UI.
         * @return  A new instance of this class.
         */
        static CPbk2AppViewFactory* NewL(
                CPbk2AppUi& aAppUi );

        /**
         * Creates a new instance of this class.
         *
         * @param aAppUi    Application UI.
         * @return  A new instance of this class.
         */
        static CPbk2AppViewFactory* NewLC(
                CPbk2AppUi& aAppUi );

        /**
         * Destructor.
         */
        ~CPbk2AppViewFactory();

    public: // Interface

        /**
         * Creates and returns application view with the given UID.
         *
         * @param aId               The UID of the view.
         * @exception KErrNoMemory  If out of memory.
         * @exception               Anything thrown during the
         *                          initialisation of the view.
         * @return  A new application view instance.
         */
        CAknView* CreateAppViewL(
                TUid aId );

        /**
         * Creates and returns application view with the given UID.
         *
         * @param aId               The UID of the view.
         * @exception KErrNoMemory  If out of memory.
         * @exception               Anything thrown during the
         *                          initialisation of the view.
         * @return  A new application view instance.
         */
        CAknView* CreateAppViewLC(
                TUid aId );

    private: // Implementation
        CPbk2AppViewFactory(
                CPbk2AppUi& aAppUi );
        void ConstructL();

    private: // Data
        /// Own: Extension manager
        CPbk2UIExtensionManager* iExtensionManager;
        /// Ref: Application UI
        CPbk2AppUi& iAppUi;
    };

#endif // CPBK2APPVIEWFACTORY_H

// End of File
