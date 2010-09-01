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
* Description:  The base class for sim stores
*
*/



#ifndef VPBKSIMSTOREIMPL_CSTOREBASE_H
#define VPBKSIMSTOREIMPL_CSTOREBASE_H

//  INCLUDES
#include <e32base.h>
#include <etelmm.h>
#include "MVPbkSimCntStore.h"
#include "VPbkSimStoreCommon.h"
#include "MVPbkSimCommandObserver.h"
#include "MVPbkSimContactObserver.h"
#include <MVPbkSimPhoneObserver.h>
#include "MSimRefreshObject.h"
#include "METelStoreNotificationObserver.h"
#include "MBtSapObserver.h"
#include "CContactArray.h" // MContactArrayObserver

// FORWARD DECLARATIONS
class RMobilePhoneStore;
class MVPbkSimCommand;
class CVPbkSimCommandStore;
class CVPbkSimStateInformation;
class CVPbkSimFieldTypeFilter;
template <class MVPbkSimStoreObserver> class CVPbkAsyncObjectOperation;
class CVPbkSimContactView;

namespace VPbkSimStoreImpl {

// DATA TYPES
enum TStoreState
    {
    /// Store is not open
    EStoreNotOpen,
    /// Store is opening, init command is running
    EStoreIsOpening,
    /// Store is open
    EStoreOpen,
    /// Store is not available, e.g SAT refresh can cause this.
    EStoreNotAvailable
    };

// FUNCTION PROTOTYPES
TUint32 ConvertETelStoreCaps( TUint32 aETelStoreCaps );

// FORWARD DECLARATIONS
class CPhone;
class CContactArray;
class CETelStoreNotification;
class CSimPhoneNumberIndex;
class CSatRefreshNotifier;
class TStoreParams;
class CBtSapNotification;
class CCompositeCmdBase;
class CSharedViewArray;

// CLASS DECLARATION

/**
*  The base class for sim stores
*
*/
NONSHARABLE_CLASS(CStoreBase) : public CBase,
                   public MVPbkSimCntStore,
                   public METelStoreNotificationObserver,
                   public MSimRefreshObject,
                   protected MVPbkSimCommandObserver,
                   protected MContactArrayObserver,
                   private MBtSapObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~CStoreBase();

    public: // New functions

        /**
        * Returns the ETel name of the store
        * @return Returns the ETel name of the store
        */
        virtual const TDesC& ETelName() const = 0;
        
        /**
        * Returns the ETel base store
        * @return the ETel base store
        */
        virtual RMobilePhoneStore& ETelStoreBase() = 0;
                
        /**
        * Returns the ETel phonebook information structure
        * Sub class must have the info as a member.
        * @return the ETel phonebook information structure
        */
        virtual RMobilePhoneStore::TMobilePhoneStoreInfoV1& ETelStoreInfo() = 0;
        
        /**
        * Returns the ETel phonebook information structure package
        * Sub class must have the info and the info package as a member.
        * @return the ETel phonebook information structure package
        */
        virtual TDes8& ETelStoreInfoPackge() = 0;
        
        /**
        * Creates a command to initialize store.
        * @param aObserver the observer for the command
        */
        virtual MVPbkSimCommand* CreateInitCommandL( 
            MVPbkSimCommandObserver& aObserver ) = 0;

        /**
        * Creates a command to update a contact from ETel
        * @param aSimIndex the index of the changed sim contact
        */
        virtual MVPbkSimCommand* CreateUpdateCommandL( TInt aSimIndex ) = 0;
        
        /**
        * Creates a command to update a contact from ETel. Uses a reference to
        * the sim index. Client must update the index before the command is executed
        * @param aSimIndex a reference to the index of the changed sim contact
        */
        virtual MVPbkSimCommand* CreateUpdateCommandUsingRefL( 
            TInt& aSimIndexRef ) = 0;
        
        /**
        * Called after store has been refreshed due to SAT refresh. The store
        * implementation must completed the refresh with correct parameters.
        *
        * @param aSimRefreshCompletion the refresh to complete
        */
        virtual void CompleteSATRefresh( 
            MSimRefreshCompletion& aSimRefreshCompletion ) = 0;
        
        /**
        * Called in the beginning of the OpenL. The subclass should overwrite
        * this and not OpenL itself.
        * Default implelention returns ETrue
        * @return EFalse if the store can not be opened. The state
        *                will be set to EStoreNotAvailable in that case
        */
        virtual TBool AllowOpeningL();
        
        /**
        * Returns the contact array
        * @return the contact array
        */
        inline CContactArray& Contacts();

        /**
        * Returns the ETel notification instance if the ETel store
        * supports notification. The ownership of the instance is not given.
        * @return the ETel notification reference or NULL
        */
        inline CETelStoreNotification* ETelNotification() const;
        
        /**
        * Returns SIM state information
        *
        * @return SIM state information
        */
        inline CVPbkSimStateInformation& SimStateInformation() const;

    public: // Functions from base classes

        /**
        * From MVPbkSimCntStore
        */
        TVPbkSimStoreIdentifier Identifier() const;

        /**
        * From MVPbkSimCntStore
        * 
        * Must be called from the sub class if it overwrites this.
        * It's better to overwrite AllowOpeningL than this.
        * Behavior:
        *   EStoreNotOpen: starts constructing the store
        *   EStoreIsOpening: does nothing
        *   EStoreOpen: sends store ready event to observer
        *   EStoreNotAvailable: sends store unavailable event to observer
        */
        void OpenL( MVPbkSimStoreObserver& aObserver );

        /**
        * From MVPbkSimCntStore
        *
        * Must be called from the sub class if it overwrites this
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
        virtual const MVPbkSimContact* ContactAtL( TInt aSimIndex );
        
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
        
        /**
        * From METelStoreNotificationObserver
        */
        TBool ETelStoreChanged( TInt aSimIndex, TUint32 aEvents );

        /**
        * From METelStoreNotificationObserver
        */
        TBool ETelStoreChangeError( TInt aError );
        
        /**
        * From MContactArrayObserver
        */
        void ContactAdded( MVPbkSimContact& aSimContact );

        /**
        * From MContactArrayObserver
        */
        void ContactRemoved( MVPbkSimContact& aRemovedContact );
        
        /**
        * From MContactArrayObserver
        */
        void ContactChanged( MVPbkSimContact& aOldContact,
            MVPbkSimContact& aNewContact );        
        
    protected:  // New functions
        
        /**
        * Returns the phone subsession
        * @return the phone
        */
        inline CPhone& Phone();
        
        /**
        * Returns the command store
        * @return the command store
        */
        inline CVPbkSimCommandStore& CommandStore();
        
        /**
        * Returns the current state of the store
        * @return the current state of the store
        */
        inline TStoreState CurrentState();
        
        /**
        * Cancels all requests. Must be called from
        * the subclass destructor before closing any ETel sessions.
        */
        void CancelRequests();

        /**
        * Subclasses can call this to add an GetInfo command to the
        * command chain.
        * @param aCmdChain the command chain created by the subclass
        */
        void AddGetInfoCmdToChainL( CCompositeCmdBase& aCmdChain );
        
        /**
        * Resets this store
        */
        void ResetStore();

        /**
        * Starts initializing store
        */
        void ConstructStoreL();
        
        /**
        * Sends store ready event to all observers
        * Sets store state to EStoreOpen
        */
        void SendStoreReadyEvent();
        
        /**
        * Sends store unavailable event to all observers
        * Sets store state to EStoreNotOpen
        */
        void SendStoreUnavailableEvent();
        
        /**
        * Sends store error event to all observers
        * Sets store state to EStoreNotOpen
        */
        void SendStoreErrorEvent(TInt aError);
        
    protected:  // Functions from base classes

        /**
        * From MVPbkSimCommandObserver
        */
        void CommandDone( MVPbkSimCommand& aCommand );

        /**
        * From MVPbkSimCommandObserver
        */
        void CommandError( MVPbkSimCommand& aCommand, TInt aError );

        /**
        * From MSimRefreshObject
        */
        void SimRefreshed( TUint32 aSimRefreshFlags,
            MSimRefreshCompletion& aSimRefreshCompletion );

        /**
        * From MSimRefreshObject
        */
        TBool AllowRefresh( TUint32 aSimRefreshFlags );
        
        /**
        * From MSimRefreshObject
        */
        TUint32 ElementaryFiles();
        
        /**
        * From MBtSapObserver
        */
        void BtSapActivated();

        /**
        * From MBtSapObserver
        */
        void BtSapDeactivated();
        
        /**
        * From MBtSapObserver
        */
        void BtSapNotificationError( TInt aError );
    
    protected:  // Construction

        /**
        * C++ constructor.
        * @param aParams store parameters
        */
        CStoreBase( TStoreParams& aParams );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // New functions

        /**
        * Opens the ETel store sub sub session.
        * Subclasses must overwrite this to open the store
        */
        virtual void OpenETelStoreL() = 0;

        /**
        * Closes the ETel store
        */
        virtual void CloseETelStore() = 0;
    
    private:    // Implementation
        
        /// Construction after contacts are read
        void PostConstructStoreL();
        /// Handles the store event. Creates an update command.
        void HandleStoreEventL( TInt aSimIndex );
        /// Handles bluetooth SIM access profile dactivation
        void HandleBtSapDeactivationL();
        /// A function called by iOpenOp
        void DoStoreOpenCallbackL( MVPbkSimStoreObserver& aObserver );
        /// A function called by iOpenOp
        void DoStoreUnavailableCallbackL( MVPbkSimStoreObserver& aObserver );
        /// A function called by iOpenOp
        void DoStoreErrorCallback( MVPbkSimStoreObserver& aObserver, 
                TInt aError );
    
    private:    // Data
        /// The identifier of the store
        TVPbkSimStoreIdentifier iIdentifier;
        /// a phone subsession for the store
        CPhone& iPhone;
        /// Ref: a SAT refresh notifier
        CSatRefreshNotifier& iSatNotifier;
        /// Ref a bluetooth SIM access profile notifier
        CBtSapNotification& iBtSapNotifier;
        /// Ref: SIM state information
        CVPbkSimStateInformation& iSimStateInfo;
        /// Ref: an observer for the store
        RPointerArray<MVPbkSimStoreObserver> iStoreObservers;
        /// Own: a command store that handles lifetime of the commands
        CVPbkSimCommandStore* iCmdStore;
        /// Own: the array of contacts
        CContactArray* iContactArray;
        /// Own: the ETel store notication for updating store
        CETelStoreNotification* iETelNotification;
        /// The state of the store;
        TStoreState iStoreState;
        /// Own: an async call back support
        CVPbkAsyncObjectOperation<MVPbkSimStoreObserver>* iOpenOp;
        /// Own: a phone number index for fast matching
        CSimPhoneNumberIndex* iPhoneNumberIndex;
        /// Own: a contact converter
        CVPbkETelCntConverter* iCntConverter;
        /// An array of shared views.
        CSharedViewArray* iSharedViewArray;
        /// Ref: SAT refresh completion
        MSimRefreshCompletion* iSimRefreshCompletion;
        /// Ref: The system max phone number length defined by Phonebook
        TInt iSystemMaxPhoneNumberLength;
        /// The flag judge whether the BtSap is activeted.
        TBool iIsBtSapActivated;
    };

// INLINE FUNCTIONS
inline CContactArray& CStoreBase::Contacts()
    {
    return *iContactArray;
    }

inline CETelStoreNotification* CStoreBase::ETelNotification() const
    {
    return iETelNotification;
    }

inline CVPbkSimStateInformation& CStoreBase::SimStateInformation() const
    {
    return iSimStateInfo;
    }
    
inline CPhone& CStoreBase::Phone()
    {
    return iPhone;
    }

inline CVPbkSimCommandStore& CStoreBase::CommandStore()
    {
    return *iCmdStore;
    }

inline TStoreState CStoreBase::CurrentState()
    {
    return iStoreState;
    }

} /// namespace VPbkSimStoreImpl

#endif  // VPBKSIMSTOREIMPL_CSTOREBASE_H
            
// End of File
