/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class that reads one contact at a time using ETel
*                RMobilePhoneStore Read
*
*/



#ifndef VPBKSIMSTOREIMPL_CSINGLEREADCMD_H
#define VPBKSIMSTOREIMPL_CSINGLEREADCMD_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimCommand.h>
#include <etelmm.h>

// FORWARD DECLARATIONS

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CStoreBase;

// CLASS DECLARATION

/**
*  A class that reads one contact at a time using ETel RMobilePhoneStore Read
*  @precond GetInfoCmd must have been run before this if all contacts are read.
*/
template<class ETelContact>
NONSHARABLE_CLASS(CSingleReadBase) : public CActive,
                                     public MVPbkSimCommand
    {
    public: // Functions from base classes

        /**
        * From MVPbkSimCommand
        */
        void Execute();
        
        /**
        * From MVPbkSimCommand
        */
        void AddObserverL( MVPbkSimCommandObserver& aObserver );

        /**
        * From MVPbkSimCommand
        */
        void CancelCmd();
        
    private: // Functions from base classes
        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        void DoCancel();
        
        /**
        * From CActive
        */
        TInt RunError( TInt aError );
        
    protected:

        /**
        * C++ constructor.
        *
        * @param aStore the store to use for reading
        * @param aIndexToRead the sim index to read
        */
        CSingleReadBase( CStoreBase& aStore, TInt aIndexToRead );

        /**
        * C++ constructor.
        *
        * The store information is used the get the amount of sim slots 
        * in the store. All slots are read.
        * @param aStore the store to use for reading
        */
        CSingleReadBase( CStoreBase& aStore );
 
        /**
        * C++ constructor.
        *
        * This is used when the sim index is not known when creating the command
        * @param aIndexToReadRef a reference to sim index to read.
        * @param aStore the store to use for reading
        */
        CSingleReadBase( TInt& aIndexToReadRef, CStoreBase& aStore );

    
    private:    // New functions
        
        /// Starts reading next contact
        TBool ReadNext();
        /// Called from RunL. sub class must overwrite this to handle
        /// contact that was read
        virtual void HandleRunL() = 0;
        
    protected: // Data
        /// The ETel store that supports multiple read
        CStoreBase& iStore;
        /// The ETel contact
        ETelContact iETelContact;
        
    private:    // Data
        /// A package for the ETel contact
        TPckg<ETelContact> iETelContactPckg;
        /// The current index to read
        TInt iCurrentIndex;
        /// The amount contacts that is tried to read at once
        TInt iLastIndexToRead;
        /// a reference to the sim index
        const TInt& iSimIndexRef;
        /// Ref: only one observer supported
        MVPbkSimCommandObserver* iObserver;
    };

/**
*  A class that reads own number contacts
*
*/
NONSHARABLE_CLASS(CSingleONReadCmd) : 
        public CSingleReadBase<RMobileONStore::TMobileONEntryV1>
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
        *
        * @param aStoreBase the parent store for the contacts
        * @param aIndexToRead the ETel index to read ( >=1 )
        */
        CSingleONReadCmd( CStoreBase& aStoreBase, TInt aIndexToRead );

        /**
        * C++ constructor.
        * Read all contacts.
        *
        * @param aStoreBase the parent store for the contacts
        */
        CSingleONReadCmd( CStoreBase& aStoreBase );
        
        /**
        * C++ constructor.
        * This is used when the sim index is not known when creating the command
        *
        * @param aIndexToReadRef a reference to sim index to read.
        * @param aStore the store to use for reading
        */
        CSingleONReadCmd( TInt& aIndexToReadRef, CStoreBase& aStore );
            
        /**
        * Destructor.
        */
        virtual ~CSingleONReadCmd();

    private: // Functions from base classes

        /**
        * From CSingleReadBase
        */
        void HandleRunL();

    private:    // New functions
        /// Adds new contact to the contact array
        void AddContactL();
    };
} // namespace VPbkSimStoreImpl 
#endif      // VPBKSIMSTOREIMPL_CSINGLEREADCMD_H
            
// End of File
