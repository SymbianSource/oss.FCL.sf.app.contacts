/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An class for store URI. Used as a member in the store.
*
*/


#ifndef CVPBKCONTACTSTOREURI_H
#define CVPBKCONTACTSTOREURI_H


// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TVPbkContactStoreUriPtr;

// CLASS DECLARATIONS

/**
 * Virtual Phonebook domain specific URI.
 *
 * @lib VPbkEng.lib
 */
class CVPbkContactStoreUri : public CBase
    {
    public:  // constructor and destructor
        /**
         * @param aStoreUri a pointer to URI that is copied to this instance.
         * @return A new instance of this class
         */
        IMPORT_C static CVPbkContactStoreUri* NewL( 
            const TVPbkContactStoreUriPtr& aStoreUri );

        /**
         * Destructor.
         */
        ~CVPbkContactStoreUri();

    public: // interface
        /**
         * Returns a pointer to URI.
         *
         * @return A pointer to URI.
         */
        IMPORT_C TVPbkContactStoreUriPtr Uri() const;

    private: // implementation
        CVPbkContactStoreUri();
        void ConstructL(const TDesC& aStoreUri);

    private: // Data
        /// Own: URI buffer
        HBufC* iUriBuffer;

    };

#endif  // CVPBKCONTACTSTOREURI_H

// End of file
