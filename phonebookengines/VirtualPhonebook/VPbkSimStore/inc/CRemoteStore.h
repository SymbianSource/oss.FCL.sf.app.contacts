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
* Description:  A remote sim store implementation
*
*/



#ifndef VPBKSIMSTORE_CREMOTESTORE_H
#define VPBKSIMSTORE_CREMOTESTORE_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimCntStore.h>
#include <RVPbkStreamedIntArray.h>
#include <MVPbkSimStoreOperation.h>
#include <RVPbkSimStore.h>

// FORWARD DECLARATIONS
class MVPbkSimFindObserver;
class CVPbkSimContactBuf;
class CVPbkSimFieldTypeFilter;
class MVPbkSimViewFindObserver;
class MVPbkSimCntView;
template<typename MVPbkSimStoreObserver> class CVPbkAsyncObjectOperation;

namespace VPbkSimStore {

// CLASS DECLARATION

/**
*  A remote sim store implementation
*
*/
NONSHARABLE_CLASS( CRemoteStore ): 
        public CActive,
        public MVPbkSimCntStore
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aIdentifier the store identifier
        * @return a new instance of this class
        */
        static CRemoteStore* NewL( 
            TVPbkSimStoreIdentifier aIdentifier );
        
        /**
        * Destructor.
        */
        virtual ~CRemoteStore();

    public: // New functions
        
        /**
        * Returns the store session
        * @return the store session
        */
        inline RVPbkSimStore& StoreSession();

        /**
        * Adds an observer for the store
        * @param aObserver a new observer
        */
        void AddObserverL( MVPbkSimStoreObserver& aObserver );

        /**
        * Removes an observer
        * @param aObserver the observer to be removed
        */
        void RemoveObserver( MVPbkSimStoreObserver& aObserver );

        /**
        * @return ETrue if store is open
        */
        TBool IsOpen() const;
        
        /**
         * Opens the store asynchronously. This is used beside MVPbkSimCntStore::OpenL
         * 
         * Calls back the observers attached to this contact store when the
         * opening completes.
         *
         * @param aSecurityInfo Clients security information
         * @param aObserver Observer.
         */
        void OpenL( const TSecurityInfo& aSecurityInfo, 
                    MVPbkSimStoreObserver& aObserver );        

    public: // Functions from base classes

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

        /**
        * From MVPbkSimCntStore
        */
        TVPbkSimStoreIdentifier Identifier() const;

        /**
        * From MVPbkSimCntStore
        */
        void OpenL( MVPbkSimStoreObserver& aObserver );

        /**
        * From MVPbkSimCntStore
        */
        void Close( MVPbkSimStoreObserver& aObserver );

        /**
        * From MVPbkSimCntStore
        */
        MVPbkSimCntView* CreateViewL(
            const RVPbkSimFieldTypeArray& aSortOrder,
            TVPbkSimViewConstructionPolicy aConstructionPolicy,
            const TDesC& aViewName,
            CVPbkSimFieldTypeFilter* aFilter );
        
        /**
        * From MVPbkSimCntStore
        */
        const TDesC8* AtL( TInt aSimIndex );
        
        /**
        * From MVPbkSimCntStore
        */
        const MVPbkSimContact* ContactAtL( TInt aSimIndex );

        /**
        * From MVPbkSimCntStore
        */
        MVPbkSimStoreOperation* SaveL( const TDesC8& aData, TInt& aSimIndex, 
            MVPbkSimContactObserver& aObserver );

        /**
        * From MVPbkSimCntStore
        */
        MVPbkSimStoreOperation* DeleteL( RVPbkStreamedIntArray& aSimIndexes, 
                      MVPbkSimContactObserver& aObserver );
        
        /**
        * From MVPbkSimCntStore
        */
        TInt GetGsmStoreProperties( TVPbkGsmStoreProperty& aGsmProperties ) const;

        /**
        * From MVPbkSimCntStore
        */
        TInt GetUSimStoreProperties( TVPbkUSimStoreProperty& aUSimProperties ) const;

        /**
        * From MVPbkSimCntStore
        */
        const CVPbkETelCntConverter& ContactConverter() const;
        
        /**
        * From MVPbkSimCntStore
        */
        MVPbkSimStoreOperation* CreateMatchPhoneNumberOperationL( 
            const TDesC& aPhoneNumber, TInt aMaxMatchDigits, 
            MVPbkSimFindObserver& aObserver );
        
        /**
        * From MVPbkSimCntStore
        */
        MVPbkSimStoreOperation* CreateFindOperationL( 
            const TDesC& aStringToFind, RVPbkSimFieldTypeArray& aFieldTypes,
            MVPbkSimFindObserver& aObserver );
        
        /**
        * From MVPbkSimCntStore
        */
        TInt SystemPhoneNumberMaxLength() const;
        
    private: // Construction

        /**
        * C++ constructor.
        */
        CRemoteStore( TVPbkSimStoreIdentifier aIdentifier );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // New functions
        /// Resets the store
        void ResetAndCloseStore();
        /// Callback function for OpenL
        void DoOpenL( MVPbkSimStoreObserver& aObserver );
        /// An error handling of DoOpenL
        void DoOpenError( MVPbkSimStoreObserver& aObserver, TInt aError );

    private:    // Data
        /// The store identifier
        TVPbkSimStoreIdentifier iIdentifier;
        /// Ref: the observers of the store
        RPointerArray<MVPbkSimStoreObserver> iObservers;
        /// The sim store subsession
        RVPbkSimStore iSimStore;
        /// The store event updated by the server
        TVPbkSimContactEventData iStoreEvent;
        /// The store event error updated by the server
        TInt iStoreEventError;
        /// A find operation class
        class CFindOperation;
        /// A number matching operation class
        class CNumberMatchOperation;                
        /// Own: native contact versus ETel contact conversion
        CVPbkETelCntConverter* iContactConverter;
        /// Own: the current contact requested with AtL
        CVPbkSimContactBuf* iCurrentETelContact;
        /// Own: the current contact requested with ContactAtL
        CVPbkSimContactBuf* iCurrentContact;
        /// The current state of the store changed by store events
        TVPbkSimEvent iCurrentState;
        /// The system max length for phonenumber
        TInt iSystemPhoneNumberMaxLength;
        /// Does not own. Security information of client.
        const TSecurityInfo* iSecurityInfo;
        CVPbkAsyncObjectOperation<MVPbkSimStoreObserver>* iAsyncOpenOp;
    };

// INLINE FUNCTIONS
inline RVPbkSimStore& CRemoteStore::StoreSession()
    {
    return iSimStore;
    }

/**
* An active object for saving a contact
*/
class CAsyncSave : public CActive,
                   public MVPbkSimStoreOperation
    {
    public: // Construction and destruction
        CAsyncSave( RVPbkSimStore& aStore );
        ~CAsyncSave();

    public: // New functions
        void DoRequestL( const TDesC8& aData, TInt& aSimIndex, 
            MVPbkSimContactObserver& aObserver );

    public: // Functions from base classes
        /**
        * From CActive
        */
        void RunL();
        /**
        * From CActive
        */
        void DoCancel();

    private:
        RVPbkSimStore& iStore;
        MVPbkSimContactObserver* iObserver;
    };

/**
* An active object for deleting a contact
*/
class CAsyncDelete : public CActive,
                     public MVPbkSimStoreOperation
    {
    public: // Construction and destruction
        CAsyncDelete( RVPbkSimStore& aStore );
        ~CAsyncDelete();

    public: // New functions
        void DoRequestL( RVPbkStreamedIntArray& aSimIndexes, 
                        MVPbkSimContactObserver& aObserver );

    public: // Functions from base classes
        /**
        * From CActive
        */
        void RunL();
        /**
        * From CActive
        */
        void DoCancel();

    private:
        RVPbkSimStore& iStore;
        MVPbkSimContactObserver* iObserver;
    };

/**
* An active object for number matching
*/
NONSHARABLE_CLASS( CRemoteStore::CNumberMatchOperation ): 
        public CActive,
        public MVPbkSimStoreOperation
    {
    public: // Construction and destruction
        static CNumberMatchOperation* NewL( CRemoteStore& aStore,
            const TDesC& aPhoneNumber, TInt aMaxMatchDigits, 
            MVPbkSimFindObserver& aObserver );
        ~CNumberMatchOperation();

    public: // New functions
        void Activate();        

    public: // Functions from base classes
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

    private:

        /**
        * C++ constructor
        */
        CNumberMatchOperation( CRemoteStore& aStore, const TDesC& aPhoneNumber, 
            TInt aMaxMatchDigits, MVPbkSimFindObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
        CRemoteStore& iStore;
        const TDesC& iPhoneNumber;
        TInt iMaxMatchDigits;
        MVPbkSimFindObserver& iObserver;
        HBufC8* iSimIndexBuf;
        TPtr8 iSimIndexBufPtr;
    };
    
/**
* An active object for number matching
*/
NONSHARABLE_CLASS( CRemoteStore::CFindOperation ): 
        public CActive,
        public MVPbkSimStoreOperation
    {
    public: // Construction and destruction
        static CFindOperation* NewL( CRemoteStore& aStore, 
            const TDesC& aStringToFind, MVPbkSimFindObserver& aObserver,
            RVPbkSimFieldTypeArray& aFieldTypes );
        ~CFindOperation();

    public: // New functions
        void Activate();

    public: // Functions from base classes
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

    private:

        /**
        * C++ constructor
        */
        CFindOperation( CRemoteStore& aStore, const TDesC& aStringToFind,
            MVPbkSimFindObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RVPbkSimFieldTypeArray& aFieldTypes );

    private:
        CRemoteStore& iStore;
        const TDesC& iStringToFind;
        MVPbkSimFindObserver& iObserver;
        HBufC8* iSimIndexBuf;
        TPtr8 iSimIndexBufPtr;
        HBufC8* iFieldTypes;
    };

} // namespace VPbkSimStore
#endif      // VPBKSIMSTORE_CREMOTESTORE_H
            
// End of File
