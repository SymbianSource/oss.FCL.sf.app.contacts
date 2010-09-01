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
* Description:  Phonebook 2 OviSync command object.
*
*/

#ifndef CPBK2OVISYNCCMD_H
#define CPBK2OVISYNCCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <AiwCommon.h>

// FORWARD DECLARATIONS
class CAiwServiceHandler;

// CLASS DECLARATION

/**
 * Phonebook 2 ovisync command object.
 */
NONSHARABLE_CLASS(CPbk2OviSyncCmd) : 
        public CBase,
        public MPbk2Command
    {
    public: // Construction and destruction
        /**
         * Creates a new instance of this class.
         *
         * @param aCommandId        Command id.
         * @param aServiceHandler   Service handler.
         * @return  A new instance of this class.
         */
        static CPbk2OviSyncCmd* NewL (
                TInt aCommandId,
                CAiwServiceHandler& aServiceHandler );

        /**
         * Destructor.
         */
        ~CPbk2OviSyncCmd();

    public: // From MPbk2Command
        void ExecuteLD();
        void ResetUiControl(
                MPbk2ContactUiControl& aUiControl );
        void AddObserver(
                MPbk2CommandObserver& aObserver );

    private:  // Implementation
        CPbk2OviSyncCmd (
                TInt aCommandId,
                CAiwServiceHandler& aServiceHandler );

    private: // Data
        /// Own: Command id
        TInt iCommandId;
        /// Ref: AIW service handler
        CAiwServiceHandler& iServiceHandler;
    };

#endif // CPBK2OVISYNCCMD_H

// End of File
