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
*    Implementation of a Phonebook engine compression UI.
*
*/


#ifndef __CPbkCompressUiImpl_H__
#define __CPbkCompressUiImpl_H__

//  INCLUDES
#include <e32base.h>
#include "PbkCompressUi.h"


// FORWARD DECLARATIONS
class CPbkContactEngine;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * This class could be moved to Phonebook.app.
 * Interface for a Phonebook database compression UI.
 */
class CPbkCompressUiImpl : 
        public CBase, 
        private MPbkCompressUi
    {
    public:  // Interface
        /**
         * Creates a new instance of this class.
		 * @param aEngine Phonebook engine
         */
        IMPORT_C static CPbkCompressUiImpl* NewL(CPbkContactEngine& aEngine);

        /**
         * Destructor.
         */
        IMPORT_C ~CPbkCompressUiImpl();

        /**
         * Enables/disables compression.
         *
         * @param aEnable   if ETrue enables compression if EFalse, disables
         *                  compression. If !aEnable and compression is running
         *                  the compression is canceled.
         */
        IMPORT_C void EnableCompression(TBool aEnable);

    private:  // from MPbkCompressUi
        TBool PbkCompressCanStartNow();
        void PbkCompressStartL(TInt aStepsTogo);
        void PbkCompressStep(TInt aStep);
        void PbkCompressComplete();
        void PbkCompressCanceled();
        void PbkCompressError(TInt aError);

    private:  // Implementation
        CPbkCompressUiImpl(CPbkContactEngine& aEngine);

    private:  // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: ETrue if compression is currently enabled
        TBool iCompressionEnabled;
    };

#endif // __CPbkCompressUiImpl_H__

// End of File
