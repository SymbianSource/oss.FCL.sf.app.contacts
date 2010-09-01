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
* Description:  CContactRetriever - Contact retriever.
*
*/


#ifndef __CONTACTRETRIEVER_H__
#define __CONTACTRETRIEVER_H__

//  INCLUDES
#include <e32base.h>
#include <mvpbkcontactoperation.h>

// FORWARD DECLARATIONS
class MVPbkSingleContactOperationObserver;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// FORWARD DECLARATIONS
class CContactStore;
class CContact;

/**
*  CContactRetriever - contact data retriever.
* 
*/
class CContactRetriever : public CActive,
                          public MVPbkContactOperation
    {
    public:     // CContactRetriever public constructors and destructor
        
        /**
        @function   NewL
        @discussion Create CContactLink object
        @param      aStore Contact store
        @param      aIndex Contact index
        @param      aObserver Operation observer
        @return     Pointer to instance of CContactRetriever
        */
        static CContactRetriever* NewL(CContactStore& aStore,TInt aIndex,
                        MVPbkSingleContactOperationObserver& aObserver);
        
        /**
        @function  ~CContactRetriever
        @discussion CContactRetriever destructor
        */
        virtual ~CContactRetriever();
       
    public:     // Methods from MVPbkContactOperation
        /**
        @function   StartL
        @discussion Start contact retrieve operation.
        */
        void StartL();

        /**
        @function   Cancel
        @discussion Cancel contact retrieve operation.
        */
        void Cancel();
    
    protected:  // Methods from CActive
        
        /**
        @function   DoCancel
        @discussion Cancel any outstanding operation
        */
        void DoCancel();

        /**
        @function   RunL
        @discussion Called when operation complete
        */
        void RunL();

    private:    // CConnection private constructors

        /**
        @function   CContactRetriever
        @discussion CContactRetriever default contructor
        @param      aStore Store to retrieve contact.
        @param      aObserver Operation observer
        */
        CContactRetriever(CContactStore& aStore,
                MVPbkSingleContactOperationObserver& aObserver);
        
        /**
        @function   ConstructL
        @discussion Perform the second stage construction of CContactRetriever 
        */
        void ConstructL(TInt aIndex);

    private:    // CContactRetriever private member variables

        /// Contact store reference
        CContactStore& iStore;

        /// Contact
        CContact* iContact;

        /// Operation observer
        MVPbkSingleContactOperationObserver& iObserver;

    };
} // End of namespace LDAPStore
#endif // __CONTACTRETRIEVER_H__
// End of File
