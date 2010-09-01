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
* Description:  A view subsession class.
*
*/



#ifndef CVPBKVIEWSUBSESSION_H
#define CVPBKVIEWSUBSESSION_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkSimViewObserver.h>
#include <MVPbkSimViewFindObserver.h>
#include "VPbkSimServerCommon.h"
#include <bamdesca.h>

// FORWARD DECLARATIONS
class MVPbkSimCntStore;
class CVPbkSimServerEventQueue;
class MVPbkSimStoreOperation;

// CLASS DECLARATION

/**
 * A sim store subsession class.
 * Handles requests to sim store (RVPbkSimStore)
 */
NONSHARABLE_CLASS(CVPbkViewSubSession) : 
        public CObject,
        protected MVPbkSimViewObserver,
        protected MVPbkSimViewFindObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param the opening message
        * @param aStore the parent store for the view
        * @return a new instance of this class
        */
        static CVPbkViewSubSession* NewL( const RMessage2& aMessage,
            MVPbkSimCntStore& aStore );
        
        /**
        * Destructor.
        */
        virtual ~CVPbkViewSubSession();

    public: // New functions
        
        /**
        * The service function of the view subsession.
        * Handles view messages
        * @param aMessage a message to the view
        */
        void ServiceL( const RMessage2& aMessage );

        /**
        * Called to cancel an asynchronous view request
        * @param aMessage the message containing the canceled request op code
        */
        void CancelRequestL( const RMessage2& aMessage );

    protected: // Functions from base classes

        /**
        * From MVPbkSimViewObserver
        */
        void ViewReady( MVPbkSimCntView& aView );

        /**
        * From MVPbkSimViewObserver
        */
        void ViewError( MVPbkSimCntView& aView, TInt aError );

        /**
        * From MVPbkSimViewObserver
        */
        void ViewNotAvailable( MVPbkSimCntView& aView );

        /**
        * From MVPbkSimViewObserver
        */
        void ViewContactEvent( TEvent aEvent, TInt aIndex, TInt aSimIndex );
        
    protected: // From MVPbkSimViewFindObserver
        void ViewFindCompleted( 
            MVPbkSimCntView& aSimCntView,
            const RVPbkStreamedIntArray& aSimIndexArray );
        void ViewFindError( MVPbkSimCntView& aSimCntView, TInt aError );        
        
    private:

        /**
        * C++ constructor.
        */
        CVPbkViewSubSession( MVPbkSimCntStore& aStore );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const RMessage2& aMessage );

    private:    // New functions

        /// Service handling functions

        /// Saves the message for the view event notification
        void SaveViewEventNotification( const RMessage2& aMessage );
        /// Returns the amount of contacts
        void ReturnContactCountL( const RMessage2& aMessage );
        /// Returns the view contact
        void GetViewContactL( const RMessage2& aMessage );
        /// Starts reordering of contacts
        void ChangeSortOrderL( const RMessage2& aMessage );
        /// Finds the view index related to given simindex
        void FindViewIndexL( const RMessage2& aMessage );
        /// Cancels the view event notification message
        void CancelViewEventNotification();
        /// Contact matching prefix
        void ContactMatchingPrefixL( const RMessage2& aMessage );
        /// Cancels the contact mathing
        void CancelContactMatching();
        /// Deletes the iMatchOperation and completes the iMatchingMsg
        void CompleteAsyncContactMatching( TInt aResult );
        /// Get contact matching results
        void ContactMatchingResultL( const RMessage2& aMessage );
        /// Get externalized sort order size
        void SortOrderSizeL( const RMessage2& aMessage );
        /// Get sort order.
        void SortOrderL( const RMessage2& aMessage );
        
        /// handles the view event
        void HandleEvent( TVPbkSimContactEventData& aEventData );
        /// Sets iErrorEvent
        void SetErrorEvent( TInt aError );
        /// Resets event
        void ResetErrorEvent();
        /// Find completed
        void ViewFindCompletedL( 
            MVPbkSimCntView& aSimCntView,
            const RVPbkStreamedIntArray& aSimIndexArray );
        /// Closes the view and destroyes it if owned
        void CloseAndDestroyView();
        /// Checks that there are no async messages active before
        /// closing the view.
        void AssertNoActiveAsyncRequests();
        
    private:    // Data
        /// The parent store for the view
        MVPbkSimCntStore& iStore;
        /// Ref/Own: the view implementation
        MVPbkSimCntView* iView;
        /// Own: The queue of view events
        CVPbkSimServerEventQueue* iEventQueue;
        /// An error event for the error notifcation
        TVPbkSimContactEventData iErrorEvent;
        /// The notification message from client
        RMessage2 iNotificationMsg;
        /// The matching message from client
        RMessage2 iMatchingMsg;
        /// Own: String array used to find
        MDesCArray* iFindStrings;
        /// Own: Store for match operation
        MVPbkSimStoreOperation* iMatchOperation;
        /// Own: Contact matching result
        HBufC8* iMatchingResults;
        
    };

#endif      // CVPBKVIEWSUBSESSION_H
            
// End of File
