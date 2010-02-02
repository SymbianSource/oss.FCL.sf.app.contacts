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
* Description:  CStoreBookmark - Contact store information.
*
*/


#ifndef __STOREBOOKMARK_H__
#define __STOREBOOKMARK_H__

// INCLUDES
#include <e32base.h>
#include <mvpbkcontactbookmark.h>

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CContactStore;

/**
*  CStoreBookmark - contact store bookmark
*  
*/
class CStoreBookmark : public CBase, public MVPbkContactBookmark
    {
    public:     // CStoreBookmark public constructors and destructor

        /**
        @function   NewL
        @discussion Create CStoreBookmark object
        @param      aStore Contact store
        @param      aIndex Contact index
        @return     Pointer to instance of CStoreBookmark
        */
        static CStoreBookmark* NewL(CContactStore& aStore,TInt aIndex = 0);
 
        /**
        @function   NewLC
        @discussion Create CStoreBookmark object
        @param      aStore Contact store
        @param      aIndex Contact index
        @return     Pointer to instance of CStoreBookmark
        */
        static CStoreBookmark* NewLC(CContactStore& aStore,TInt aIndex = 0);

        /**
        @function   ~CStoreBookmark
        @discussion CStoreBookmark destructor
        */
        ~CStoreBookmark();

    public:     // CStoreBookmark public methods

        /**
        @function   Index
        @discussion Returns bookmarked contact index.
        @return     Bookmarked contact index
        */
        TInt Index() const;

        /**
        @function   Store
        @discussion Returns bookmarked store.
        @return     Bookmarked store
        */
        CContactStore& Store() const;

    private:    // CStoreBookmark private constructors

        /**
        @function   CStoreBookmark
        @discussion CStoreBookmark default contructor
        @param      aStore Contact store
        @param      aIndex Contact index
        */
        CStoreBookmark(CContactStore& aStore,TInt aIndex = 0);

    private:    // CStoreBookmark private member variables

        /// Contact store reference
        CContactStore& iStore;

        /// Contact index
        TInt iIndex;
    };

} // End of namespace LDAPStore
#endif // __STOREBOOKMARK_H__
