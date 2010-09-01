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
* Description:  A base class for write operations
*
*/



#ifndef VPBKSIMSTOREIMPL_CWRITECMDBASE_H
#define VPBKSIMSTOREIMPL_CWRITECMDBASE_H

//  INCLUDES
#include "CStoreBase.h"
#include "METelStoreNotificationObserver.h"

#include <e32base.h>
#include <MVPbkSimStoreOperation.h>
#include <MVPbkSimCommandObserver.h>
#include <VPbkDebug.h>

// FORWARD DECLARATIONS
class MVPbkSimContactObserver;

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CStoreBase;

// CLASS DECLARATION

/**
*  An operation that writes a contact to (U)SIM card
*
*/
NONSHARABLE_CLASS(CWriteCmdBase) : public CActive,
                                   public MVPbkSimStoreOperation,
                                   protected METelStoreNotificationObserver,
                                   protected MVPbkSimCommandObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        ~CWriteCmdBase();

    public: // Functions from base classes

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

    protected:    // Functions from base classes
        
        /**
        * From METelStoreNotificationObserver
        */
        TBool ETelStoreChanged( TInt aSimIndex, TUint32 aEvents );
        
        /**
        * From METelStoreNotificationObserver
        */
        TBool ETelStoreChangeError( TInt aError );
        
        /**
        * From MVPbkSimCommandObserver
        */
        void CommandDone( MVPbkSimCommand& aCommand );

        /**
        * From MVPbkSimCommandObserver
        */
        void CommandError( MVPbkSimCommand& aCommand, TInt aError );
        
    protected:    // Construction

        /**
        * C++ constructor
        */
        CWriteCmdBase( CStoreBase& aStore, TInt& aSimIndex,
            MVPbkSimContactObserver& aObserver );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();
        
    protected:    // New functions
        
        /**
        * Must be called from the subclass Execute to
        * active the command
        */
        void Activate();
        
        /**
        * Handles error case
        * @param aError the error to notify observers
        */
        
        void HandleError( TInt aError );
        
        /**
        * Removes this instance from the ETel notification
        */
        void RemoveNotificationObserver();
        
        /**
        * @return the store
        */
        inline CStoreBase& StoreBase() const;
        
        /**
        * Activates the object and completes command using aResult.
        * @param aResult The completion code.
        */
        void CompleteCommandAsynchronously( TInt aResult );
    
    protected:  // Data
        /// a refence to the sim index which is updated by the
        /// ETel if the value is -1
        TInt& iSimIndex;
    private:    // Data
        /// the store for making the ETel request
        CStoreBase& iStoreBase;
        /// an observer to notify in error case
        MVPbkSimContactObserver& iObserver;
        /// Own: the update command to execute
        MVPbkSimCommand* iUpdateCommand;
        /// an observer for command framework
        MVPbkSimCommandObserver* iCommandObserver;
    };

// INLINE FUNCTIONS
inline CStoreBase& CWriteCmdBase::StoreBase() const
    {
    return iStoreBase;
    }
    
/**
* A write operation template for RMobilePhoneStore::Write
*/
template<class ETelContact>
NONSHARABLE_CLASS( CBasicWriteCmd ) : public CWriteCmdBase
    {
    public:  // New functions 
        
        /**
        * Starts this operation
        */
        void Execute();
        
    protected:  // Functions from base classes
        
        /**
        * From CActive
        */
        void RunL();
        
    protected:    // Construction

        /**
        * C++ constructor
        */
        CBasicWriteCmd( CStoreBase& aStore, TInt& aSimIndex,
            MVPbkSimContactObserver& aObserver );
            
    protected: // Data
        /// The ETel contact
        ETelContact iETelContact;
        
    private: // Data
        /// A package for the ETel contact
        TPckg<ETelContact> iETelContactPckg;
        /// a reference to the index to write
        TInt& iSimIndex;
    };

// -----------------------------------------------------------------------------
// CBasicWriteCmd::CBasicWriteCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
template<class ETelContact>    
CBasicWriteCmd<ETelContact>::CBasicWriteCmd( CStoreBase& aStore,
    TInt& aSimIndex, MVPbkSimContactObserver& aObserver )
    :   CWriteCmdBase( aStore, iETelContact.iIndex, aObserver ),
        iETelContactPckg( iETelContact ),
        iSimIndex( aSimIndex )
    {
    }

// -----------------------------------------------------------------------------
// CBasicWriteCmd::Execute
// -----------------------------------------------------------------------------
//
template<class ETelContact>
void CBasicWriteCmd<ETelContact>::Execute()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::Write h%d request slot=%d"),
        StoreBase().ETelStoreBase().SubSessionHandle(),iSimIndex);
    StoreBase().ETelStoreBase().Write( iStatus, iETelContactPckg );
    CWriteCmdBase::Activate();
    }

// -----------------------------------------------------------------------------
// CBasicWriteCmd::RunL
// -----------------------------------------------------------------------------
//
template<class ETelContact>
void CBasicWriteCmd<ETelContact>::RunL()
    {
    // ETel writes to iETelContact.iIndex. It must be copied to the index
    // reference offered by the client of this dll
    iSimIndex = iETelContact.iIndex;
    CWriteCmdBase::RunL();
    }

} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CWRITECMDBASE_H
            
// End of File
