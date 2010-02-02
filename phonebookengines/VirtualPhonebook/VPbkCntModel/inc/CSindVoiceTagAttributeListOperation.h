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


#ifndef CSINDVOICETAGATTRIBUTELISTOPERATION_H
#define CSINDVOICETAGATTRIBUTELISTOPERATION_H

// INCLUDES
#include <mvpbkcontactoperation.h>
#include <nssvasapi.h>
#include <e32base.h>
#include <cntdb.h>

// FORWARD DECLARATIONS
class MVPbkContactFindObserver;
class CVPbkContactLinkArray;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CContactStore;

/**
 * Contact model store Voice tag attribute operation.
 */
NONSHARABLE_CLASS( CSindVoiceTagAttributeListOperation ) : 	
        public CBase,
		public MVPbkContactOperation,
		private MNssGetContextClient,
		private MNssGetTagClient
    {
    public:
        /**
         * Creates a new instance of this class.
         *
         * @param aContactStore, Contact store
         * @param aAttribute, Voice tag attribute
         * @param aObserver, Setting attribute observer
         *
         * @return a new instance of this class             
         */     
        static CSindVoiceTagAttributeListOperation* NewListLC(
                CContactStore& aContactStore,
                MNssContextMgr& aNssContextManager,
                MNssTagMgr& aNssTagManager,
                MVPbkContactFindObserver& aObserver );

        /**
         * Destructor.
         */                
        ~CSindVoiceTagAttributeListOperation();
        
    public: // From MVPbkContactOperation
        void StartL();
        void Cancel();
    
    private: // From MNssGetContextClient
        void GetContextCompleted( MNssContext* aContext );
        void GetContextListCompleted( MNssContextListArray *aContextList );
        void GetContextFailed( TNssGetContextClientFailCode aFailCode );
        
    private: // From MNssGetTagClient
        void GetTagListCompleted( MNssTagListArray* aTagList );
        void GetTagFailed( TNssGetTagClientFailCode aFailCode );
        
    private: // Implementation
        
        CSindVoiceTagAttributeListOperation(
                CContactStore& aContactStore,
                MNssContextMgr& aNssContextManager,
                MNssTagMgr& aNssTagManager,
                MVPbkContactFindObserver& aObserver );
        void ConstructL();
        void CompleteOperation();
        void HandleGetTagListCompletedL( MNssTagListArray* aTagList );
        void GetNextTagL();
        
    private: // Data
        ///Own: Contact store
        CContactStore& iContactStore;
        ///Not Own: a reference for NSS context manager for retrieving 
        // voice tags context
        MNssContextMgr& iNssContextManager;
        ///Now Own: A reference to the NSS tag manager for retrieving
        /// voice tags
        MNssTagMgr& iNssTagManager;
        ///Own: Find observer
        MVPbkContactFindObserver& iFindObserver;
        ///Own: The results of the operation
        CVPbkContactLinkArray* iContactLinks;
        ///Own: An iterator for Contacts Model ids.
        TContactIter iContactIterator;
        ///Own: A contact who is currently checked for voice tags
        TContactItemId iCurrentId;
        ///Own: A NSS voice tag context
        MNssContext* iNssContext;
    };
    
} // namespace VPbkCntModel

#endif // CSINDVOICETAGATTRIBUTELISTOPERATION_H

// End of File
