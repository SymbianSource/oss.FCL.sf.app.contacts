/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CContactStoreUri - Store URI.
*
*/


#ifndef __CONTACTSTOREURI_H__
#define __CONTACTSTOREURI_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TVPbkContactStoreUriPtr;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

/**
*  CContactStoreUri - store uri.
*  
*/
class CContactStoreUri : public CBase
    {
    public:     // CContactStoreUri public constructors and destructor
        /**
        @function   NewL
        @discussion Create CContactStoreUri object
        @param      aStoreUri Store URI
        @return     Pointer to instance of CContactStoreUri
        */
        static CContactStoreUri* NewL(const TVPbkContactStoreUriPtr& aStoreUri);

        /**
        @function   NewLC
        @discussion Create CContactStoreUri object
        @param      aStoreUri Store URI
        @return     Pointer to instance of CContactStoreUri
        */
        static CContactStoreUri* NewLC(const TVPbkContactStoreUriPtr& aStoreUri);

        /**
        @function   ~CContactStoreUri
        @discussion CContactStoreUri destructor
        */
        ~CContactStoreUri();

    public:     // CContactStoreUri public methods

        /**
        @function   Uri
        @discussion Returns store URI
        @return     Store URI
        */
        const TVPbkContactStoreUriPtr Uri() const;

    private:    // CContactStoreUri private constructors
        /**
        @function   CContactStoreUri
        @discussion CContactStoreUri default contructor
        */
        CContactStoreUri();

        /**
        @function   ConstructL
        @discussion Perform the second stage construction of CContactStoreUri 
        @param      aStoreUri Store URI
        */
        void ConstructL(const TDesC& aStoreUri);

    private:    // CContactStoreUri private member variables

        /// URI buffer
        HBufC* iUriBuffer;
    };

} // End of namespace LDAPStore
#endif // __CONTACTSTOREURI_H__
