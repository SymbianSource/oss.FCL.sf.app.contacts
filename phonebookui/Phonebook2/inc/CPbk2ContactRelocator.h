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
* Description:  Phonebook 2 contacts relocator.
*
*/


#ifndef CPBK2CONTACTRELOCATOR_H
#define CPBK2CONTACTRELOCATOR_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactStoreObserver.h>
#include <MVPbkContactCopyObserver.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkContactOperationBase;
class CVPbkContactLinkArray;
class MVPbkContactStore;
class MPbk2ContactRelocatorObserver;
class MVPbkContactCopyPolicy;
class CVPbkContactCopyPolicyManager;
class MVPbkContactLinkArray;
class CAknQueryDialog;
class CPbk2ApplicationServices;

// CLASS DECLARATION

namespace Pbk2ContactRelocator {

/**
 * Query policy.
 */
enum TPbk2ContactRelocationQueryPolicy
    {
    /// Do not display any queries
    EPbk2DisplayNoQueries,
    /// Display 'copy %U to phone memory' query
    EPbk2DisplayBasicQuery,
    /// Display 'store does not support, copy/move to %U memory' query
    EPbk2DisplayStoreDoesNotSupportQuery
    };
    
    /**
     * Relocation policy.
     */
enum TPbk2ContactRelocationPolicy
    {
    /// Copy contact to phone memory, value used from CenRep
    EPbk2CopyContactToPhoneMemory,
    /// Move contact to phone memory, value used from CenRep
    EPbk2MoveContactToPhoneMemory,
    /// Copy contact to phone memory and lock it
    EPbk2CopyContactToPhoneMemoryAndLock,
    /// Move contact to phone memory and lock it
    EPbk2MoveContactToPhoneMemoryAndLock
    };
} /// namespace

/**
 * Phonebook 2 contacts relocator.
 */
class CPbk2ContactRelocator : public CActive,
                              public MVPbkContactStoreObserver,
                              public MVPbkContactCopyObserver,
                              public MVPbkSingleContactOperationObserver,
                              public MVPbkContactObserver
    {
    public: // Data structure

        enum TFlags
            {
            /// The contact exists in the store and can be deleted
            /// if the relocation policy is to move contact.
            EPbk2RelocatorExistingContact   = 0x01,
            /// Relocator assumes that contact is new and doesn't try to
            /// delete the contact if the policy is move contact.
            EPbk2RelocatorNewContact        = 0x02
            };

    public:  // Constructors and destructor

        /**
         * Creates and returns a new instance of this class.
         *
         * @return A new instance of this class.
         */
        IMPORT_C static CPbk2ContactRelocator* NewL();

        /**
         * Destructor.
         */
        ~CPbk2ContactRelocator();

    public: // Interface

        /**
         * Relocates given contact to phone memory store.
         *
         * @param aContact      The contact to relocate. Ownership of
         *                      the contact is taken.
         * @param aObserver     Observer for the relocation process.
         * @param aQueryPolicy  Query policy.
         * @param aFlags        A set of TFlags
         * @return  ETrue if user has accepted relocation, otherwise EFalse.
         *          NOTE! Do not delete relocator instance. Relocator will
         *                be completed asynchronusly through aObserver.
         */
        IMPORT_C TBool RelocateContactL(
                MVPbkStoreContact* aContact,
                MPbk2ContactRelocatorObserver& aObserver,
                Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy 
                    aQueryPolicy,
                TUint32 aFlags );

        /**
         * Relocates given contact to phone memory store.
         * After the contact has been relocated, the contact
         * is locked.
         *
         * @param aContact      The contact to relocate. Ownership of
         *                      the contact is taken.
         * @param aObserver     Observer for the relocation process.
         * @param aQueryPolicy  Query policy.
         * @param aFlags        A set of TFlags
         * @return  ETrue if user has accepted relocation, otherwise EFalse.
         *          NOTE! Do not delete relocator instance. Relocator will
         *                be completed asynchronusly through aObserver.
         */
        IMPORT_C TBool RelocateAndLockContactL(
                MVPbkStoreContact* aContact,
                MPbk2ContactRelocatorObserver& aObserver,
                Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy 
                    aQueryPolicy,
                TUint32 aFlags );

        /**
         * Relocates given contacts to phone memory store.
         *
         * @param aContacts     The contacts to relocate. Ownership of
         *                      the link array is taken.
         * @param aObserver     Observer for the relocation process.
         * @param aQueryPolicy  Query policy, this function ignores
         *                      this parameter currently.
         * @return  ETrue if user has accepted relocation, otherwise EFalse.
         *          NOTE! Do not delete relocator instance. Relocator will
         *                be completed asynchronusly through aObserver.
         */
        IMPORT_C TBool RelocateContactsL(
                CVPbkContactLinkArray* aContacts,
                MPbk2ContactRelocatorObserver& aObserver,
                Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy 
                    aQueryPolicy );

        /**
         * Checks is the given contact a phone memory contact.
         *
         * @param aContact  The contact to inspect.
         * @return  ETrue if aContact belongs to phone memory store.
         */
        IMPORT_C TBool IsPhoneMemoryContact(
                const MVPbkStoreContact& aContact ) const;

        /**
         * Inspects is phone memory part of the current Phonebook 2
         * store configuration.
         *
         * @return  ETrue if phone memory belongs to configuration.
         */
        IMPORT_C TBool IsPhoneMemoryInConfigurationL() const;

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

    private: // From MVPbkContactCopyObserver
        void FieldAddedToContact(
                MVPbkContactOperationBase& aOperation );
        void FieldAddingFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );
        void ContactsSaved(
                MVPbkContactOperationBase& aOperation,
                MVPbkContactLinkArray* aResults );
        void ContactsSavingFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MVPbkContactObserver
        void ContactOperationCompleted(
                TContactOpResult aResult );
        void ContactOperationFailed(
                TContactOp aOpCode,
                TInt aErrorCode,
                TBool aErrorNotified );

    private: // Data structures
        /**
         * Relocaltion query type
         */
        enum TPbk2RelocationQueryType
            {
            /// No relocation query
            EPbk2NoRelocationQuery,
            /// §qtn.phob.query.copy.to.store§ query
            EPbk2CopyOneToStoreQuery,
            /// §qtn.phob.query.move.to.store§ query
            EPbk2MoveOneToStoreQuery,
            /// §qtn.phob.query.copy.to.phone.mem§ query            
            EPbk2CopyOneToPhoneQuery,
            /// §qtn.phob.query.move.to.phone.mem§ query
            EPbk2MoveOneToPhoneQuery,
            /// §qtn.phob.query.copy.many.to.store§ query
            EPbk2CopyManyToStoreQuery,
            /// §qtn.phob.query.move.many.to.store§
            EPbk2MoveManyToStoreQuery
            };

    private: // Implementation
        CPbk2ContactRelocator();
        void ConstructL();
        Pbk2ContactRelocator::TPbk2ContactRelocationPolicy ReadRelocationPolicyL();
        void OpenTargetStoreL();
        void DoRelocateContactL();
        void InitCopyPolicyL();
        void DoHandleContactRelocated();
        void DeleteSourceContact();
        TBool AskConfirmationL(
                MVPbkStoreContact* aContact,
                CVPbkContactLinkArray* aContacts,
                TPbk2RelocationQueryType aRelocationQueryType );
        void RetrieveSavedContactL(
                const MVPbkContactLink& aLink );
        void VerifyPolicy(
                MVPbkStoreContact* aContact, TUint32 aFlags );
        TBool PrepareToRelocateContactL(
                MVPbkStoreContact* aContact,
                CVPbkContactLinkArray* aContacts,
                MPbk2ContactRelocatorObserver& aObserver,
                Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy 
                    aQueryPolicy,
                TUint32 aFlags );
        void CopyContactL(
                MVPbkStoreContact& aContact );
        void FinalizeIfReady();
        TPbk2RelocationQueryType SelectRelocationQueryType(
                MVPbkStoreContact* aContact,
                CVPbkContactLinkArray* aContacts,
                Pbk2ContactRelocator::TPbk2ContactRelocationQueryPolicy
                aQueryPolicy );

    private: // Data
        /// Own: Relocation policy
    	Pbk2ContactRelocator::TPbk2ContactRelocationPolicy iRelocationPolicy;
        /// Own: Relocation policy
    	Pbk2ContactRelocator::TPbk2ContactRelocationPolicy iActivePolicy;
        /// Ref: Target store for copied contacts
        MVPbkContactStore* iTargetStore;
        /// Own: Indication of target store state
        TBool iTargetStoreOpen;
        /// Own: Copy policy manager
        CVPbkContactCopyPolicyManager* iCopyPolicyManager;
        /// Ref: Copy policy
        MVPbkContactCopyPolicy* iCopyPolicy;
        /// Own: Contact retriever
        MVPbkContactOperationBase* iSavedContactsRetriever;
        /// Own: Relocatable contact retriever
        MVPbkContactOperationBase* iContactRetriever;
        /// Own: Copy operation
        MVPbkContactOperationBase* iContactCopier;
        /// Own: Relocated contact
        MVPbkStoreContact* iRelocatedContact;
        /// Ref: Observer for the relocation process
        MPbk2ContactRelocatorObserver* iObserver;
        /// Own: The contact to relocate
        MVPbkStoreContact* iSourceContact;
        /// Own: The contacts to relocate
        CVPbkContactLinkArray* iSourceContacts;
        /// Own: Error code for asynchronous error notification
        TInt iErrorCode;
        /// Own: Query dialog
        CAknQueryDialog* iQueryDialog;
        /// Own: To check is this destroyed
        TBool* iDestroyedPtr;
        /// Own: Application services
        CPbk2ApplicationServices* iAppServices; 
        };

#endif // CPBK2CONTACTRELOCATOR_H

// End of File
