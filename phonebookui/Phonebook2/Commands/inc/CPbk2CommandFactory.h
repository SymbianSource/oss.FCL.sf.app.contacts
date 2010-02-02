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
* Description:  Creates Phonebook 2 command objects.
*
*/


#ifndef CPBK2COMMANDFACTORY_H
#define CPBK2COMMANDFACTORY_H

//  INCLUDES
#include <e32base.h>
#include "MPbk2CommandFactory.h"

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class CPbk2UIExtensionManager;

//  CLASS DECLARATION

/**
 * Factory to create Phonebook 2 command objects.
 */
NONSHARABLE_CLASS(CPbk2CommandFactory) :
        public CBase,
        public MPbk2CommandFactory
    {
    public: // Constructor and destructor
        /**
         * Static constructor.
         *
         * @return A new instance of this class.
         */
        static CPbk2CommandFactory* NewL();

        /**
         * Destructor.
         */
        ~CPbk2CommandFactory();

    public: // From MPbk2CommandFactory
        MPbk2Command* CreateCommandForIdL(
                TPbk2CommandId aCommandId,
                MPbk2ContactUiControl& aUiControl) const;

    public: // Command creation functions
        MPbk2Command* CreateSetToneCommandL(
            MPbk2ContactUiControl& aUiControl) const;
        MPbk2Command* CreateActivateCntInfoViewCommandL(
            MPbk2ContactUiControl& aUiControl) const;
            
    private: // Implementation
        CPbk2CommandFactory();
        void ConstructL();

    private: // Data
        /// Own: Extension manager
        CPbk2UIExtensionManager* iExtensionManager;
    };

#endif // CPBK2COMMANDFACTORY_H

// End of File
