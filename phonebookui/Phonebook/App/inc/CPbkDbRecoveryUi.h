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
*       Database recovery UI class definition.
*
*/


#ifndef __CPbkDbRecoveryUi_H__
#define __CPbkDbRecoveryUi_H__

// INCLUDES
#include <e32base.h>
#include <MPbkRecoveryErrorHandler.h>
#include "MPbkCtrlDisabler.h"

// FORWARD DECLARATIONS
class CPbkDbRecovery;
class CPbkContactEngine;


// CLASS DECLARATION

/**
 * Database recovery UI.
 */
class CPbkDbRecoveryUi : 
        public CBase,
        public MPbkRecoveryErrorHandler
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine reference to a contact engine.
         */
        static CPbkDbRecoveryUi* NewL(
            CPbkContactEngine& aEngine, 
            MPbkCtrlDisabler& aCtrlDisabler );

        /**
         * Destructor.
         */
        ~CPbkDbRecoveryUi();

    private:  // from MPbkRecoveryErrorHandler
        TBool HandleDbRecoveryError(TInt aError);

    private:  // Implementation
        CPbkDbRecoveryUi( MPbkCtrlDisabler& aCtrlDisabler );
        void ConstructL(CPbkContactEngine& aEngine);

    private:  // Data
		/// Own: database recovery handler
        CPbkDbRecovery* iDbRecovery;
        /// Ref: To shutdown controller 
        MPbkCtrlDisabler& iCtrlDisabler;
    };


#endif // __CPbkDbRecoveryUi_H__

// End of File
