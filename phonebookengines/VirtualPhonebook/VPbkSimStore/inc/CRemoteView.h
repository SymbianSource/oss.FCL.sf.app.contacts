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
* Description:  The remote SIM view implementation.
*
*/



#ifndef VPBKSIMSTORE_CREMOTEVIEW_H
#define VPBKSIMSTORE_CREMOTEVIEW_H

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>
#include <MVPbkSimCntView.h>
#include <MVPbkSimStoreObserver.h>
#include <RVPbkStreamedIntArray.h>
#include <MVPbkSimStoreOperation.h>
#include "RVPbkSimCntView.h"

// FORWARD DECLARATIONS
class RVPbkSimFieldTypeArray;
class CVPbkSimContactBuf;
class MVPbkSimStoreOperation;
template<class MVPbkSimViewObserver> class CVPbkAsyncObjectOperation;

namespace VPbkSimStore {

// FORWARD DECLARATIONS
class CRemoteStore;

// CLASS DECLARATION

/**
 *  The remote SIM view implementation.
 *
 */
NONSHARABLE_CLASS( CRemoteView ): 
        public CActive,
        public MVPbkSimCntView,
        public MVPbkSimStoreObserver
    {
    public:  // Constructors and destructor
        
        /**
         * Two-phased constructor.
         * @param aStore                The parent store.
         * @param aSortOrder            The sort order for the view.
         * @param aConstructionPolicy   Defines the way the view
         *                              is constructed.
         * @param aViewName             A name if a shared view is
         *                              desired or KNullDesC.
         * @param aFilter               Field type filter.
         * @return  A new instance of this class.
         */
        static CRemoteView* NewL(
                CRemoteStore& aStore,
                const RVPbkSimFieldTypeArray& aSortOrder,
                TVPbkSimViewConstructionPolicy aConstructionPolicy,
                const TDesC& aViewName,
                CVPbkSimFieldTypeFilter* aFilter );
        
        /**
         * Destructor.
         */
        virtual ~CRemoteView();

    public: // From CActive
        void RunL();
        void DoCancel();

    public: // From MVPbkSimCntView
        const TDesC& Name() const;
        MVPbkSimCntStore& ParentStore() const;
        void OpenL(
                MVPbkSimViewObserver& aObserver );
        void Close(
                MVPbkSimViewObserver& aObserver );
        TInt CountL() const;
        MVPbkSimContact& ContactAtL(
                TInt aIndex );
        void ChangeSortOrderL(
                const RVPbkSimFieldTypeArray& aSortOrder );
        TInt MapSimIndexToViewIndexL(
                TInt aSimIndex );
        MVPbkSimStoreOperation* ContactMatchingPrefixL(
                const MDesCArray& aFindStrings, 
                MVPbkSimViewFindObserver& aObserver );
        const RVPbkSimFieldTypeArray& SortOrderL() const;

    public: // From MVPbkSimStoreObserver
        void StoreReady(
                MVPbkSimCntStore& aStore );
        void StoreError(
                MVPbkSimCntStore& aStore,
                TInt aError );
        void StoreNotAvailable(
                MVPbkSimCntStore& aStore );
        void StoreContactEvent(
                TEvent aEvent,
                TInt aSimIndex );
        
    private: // Implementation
        CRemoteView(
                CRemoteStore& aStore,
                TVPbkSimViewConstructionPolicy aConstructionPolicy,
                CVPbkSimFieldTypeFilter* aFilter );
        void ConstructL(
                const RVPbkSimFieldTypeArray& aSortOrder,
                const TDesC& aViewName );
        /// Activates view notification request
        void ActivateViewNotification();
        /// Copies sort order to the member
        void SetSortOrderL(
                const RVPbkSimFieldTypeArray& aSortOrder );
        void IssueRequest(
                TInt aError );
        void DoViewUnavailableL( MVPbkSimViewObserver& aObserver );
        void DoViewUnavailableError( MVPbkSimViewObserver& aObserver, 
                TInt aError );

    private: // Data
        /// Own: The name of the view
        HBufC* iViewName;
        /// Ref: The parent store of the view
        CRemoteStore& iStore;
        /// Own: The view subsession
        RVPbkSimCntView iSimView;
        /// Ref: An observer for the view
        RPointerArray<MVPbkSimViewObserver> iObservers;
        /// Own: The current view contact
        CVPbkSimContactBuf* iCurrentContact;
        /// Own: Contact event data structure filled by the server
        TVPbkSimContactEventData iEventData;
        /// Own: The sort order of the view
        mutable RVPbkSimFieldTypeArray iSortOrder;
        /// Own: A policy for the view construction
        TVPbkSimViewConstructionPolicy iConstructionPolicy;
        /// Own: Async operation for observers
        CVPbkAsyncObjectOperation<MVPbkSimViewObserver>* iObserverOp;
        /// Own: Field type filter
        CVPbkSimFieldTypeFilter* iFilter;
        /// A view find operation class
        class CViewFindOperation;        
    };

/**
* An active object for contact matching
*/
NONSHARABLE_CLASS( CRemoteView::CViewFindOperation ): 
        public CActive,
        public MVPbkSimStoreOperation
    {
    public: // Construction and destruction
        static CViewFindOperation* NewL( 
            const MDesCArray& aFindStrings, 
            MVPbkSimViewFindObserver& aObserver,
            RVPbkSimCntView& aSimCntViewSession,
            MVPbkSimCntView& aSimCntView );
        ~CViewFindOperation();

    public: // New functions
        void ActivateL();

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
        CViewFindOperation( 
            const MDesCArray& aFindStrings,             
            MVPbkSimViewFindObserver& aObserver,
            RVPbkSimCntView& aSimCntViewSession,
            MVPbkSimCntView& aSimCntView );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:                        
        const MDesCArray& iFindStrings;
        MVPbkSimViewFindObserver& iObserver;  
        RVPbkSimCntView& iSimCntViewSession;
        MVPbkSimCntView& iSimCntView;
        TPtr8 iSimMatchResultBufPtr;      
        HBufC8* iSimMatchResultBuf;
        TInt iResultBufferSize;
    };    

} // namespace VPbkSimStore

#endif // VPBKSIMSTORE_CREMOTEVIEW_H
            
// End of File
