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
* Description:  Contact model store Voice tag attribute operation.
*
*/


#ifndef CSINDVOICETAGATTRIBUTEFINDOPERATION_H
#define CSINDVOICETAGATTRIBUTEFINDOPERATION_H

// INCLUDES
#include <mvpbkcontactoperation.h>
#include <nssvasapi.h>
#include <e32base.h>
#include <cntdb.h>

// FORWARD DECLARATIONS
class MVPbkMultiAttributePresenceObserver;
class CVPbkContactFieldCollection;
class MVPbkStoreContact;
class MVPbkFieldType;
class MVPbkStoreContactField;
class CVPbkContactManager;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;

/**
 * Contact model store Voice tag attribute operation.
 */
NONSHARABLE_CLASS( CSindVoiceTagAttributeFindOperation ) : 	
        public CBase,
		public MVPbkContactOperation,
		private MNssGetContextClient,
		private MNssGetTagClient
    {
    public:
        /**
         * Creates a new instance of this class.
         *
         * @param aContactStore Contact store.
         * @param aNssContextManager    Voice tag context manager.
         * @param aNssTagManager   Tag manager.
         * @param aContact  Store contact.
         * @param aContactManager, Contact manager
         * @param aObserver Operation observer.
         *
         * @return a new instance of this class             
         */     
        static CSindVoiceTagAttributeFindOperation* NewFindLC(
                CContactStore& aContactStore,
                MNssContextMgr& aNssContextManager,
                MNssTagMgr& aNssTagManager,
                MVPbkStoreContact& aStoreContact,
                CVPbkContactManager& aContactManager,
                MVPbkMultiAttributePresenceObserver& aObserver );
                
        /**
         * Destructor.
         */
        ~CSindVoiceTagAttributeFindOperation();
        
    public: // From MVPbkContactOperation
        void StartL();
        void Cancel();
    
    public: // From MNssGetContextClient
        void GetContextCompleted( MNssContext* aContext );
        void GetContextListCompleted( MNssContextListArray *aContextList );
        void GetContextFailed( TNssGetContextClientFailCode aFailCode );
        
    public: // From MNssGetTagClient
        void GetTagListCompleted( MNssTagListArray* aTagList );
        void GetTagFailed( TNssGetTagClientFailCode aFailCode );
        
    private: // Implementation
        CSindVoiceTagAttributeFindOperation(
                CContactStore& aContactStore,
                MNssContextMgr& aNssContextManager,
                MNssTagMgr& aNssTagManager,
                MVPbkStoreContact& aStoreContact,
                CVPbkContactManager& aContactManager,
                MVPbkMultiAttributePresenceObserver& aObserver );
        void ConstructL();
        void HandleGetTagListCompletedL( MNssTagListArray* aTagList );
        void GetTagL();
        void MapTaggedFieldsL(
                MNssTagListArray* aTagList );
        TBool DoesFieldIdMatchMVPbkFieldType(
              const TInt aFieldId,
              const MVPbkFieldType* aFieldType,
              TInt& aFieldTypeResId)  const;
        TBool IsFirstFieldOfTypeL(
                const MVPbkStoreContactField& aField,
                TInt aFieldTypeResId ) const;
        TInt DefaultCallNumberNativeFieldIndexL(
                const MVPbkStoreContact& aContact) const;

    private: // Data
        /// Ref: Contact store
        CContactStore& iContactStore;        
        /// Ref: NSS context manager for retrieving voice tags context
        MNssContextMgr& iNssContextManager;
        /// Ref: NSS tag manager for retrieving voice tags
        MNssTagMgr& iNssTagManager;
        /// Ref: Store contact
        MVPbkStoreContact& iStoreContact;
        /// Ref: Contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Operation observer
        MVPbkMultiAttributePresenceObserver& iOperationObserver;        
        /// Own: The results of the operation
        CVPbkContactFieldCollection* iVoiceTagFields;
        /// Own: NSS voice tag context
        MNssContext* iNssContext;
        

    };
    
} // namespace VPbkCntModel

#endif // CSINDVOICETAGATTRIBUTEFINDOPERATION_H

// End of File
