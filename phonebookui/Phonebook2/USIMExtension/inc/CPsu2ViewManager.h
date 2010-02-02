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
* Description:  Phonebook 2 USIM UI Extension view manager.
*
*/


#ifndef CPSU2VIEWMANAGER_H
#define CPSU2VIEWMANAGER_H

// INCLUDES
#include <e32base.h>
#include "Pbk2USimUI.hrh"
#include <MVPbkContactStoreObserver.h>
#include <MVPbkContactViewObserver.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class MVPbkContactViewBase;
class TVPbkContactStoreUriPtr;
class MVPbkContactStoreList;
class MVPbkSimPhone;
class CPsu2SecUi;
class CVPbkSortOrder;
class MVPbkContactStore;
class MVPbkContactStoreObserver;
class MVPbkContactViewObserver;
class MPsu2ViewLaunchCallback;

// CLASS DECLARATION

/**
 * Phonebook 2 USIM UI Extension view manager.
 */
class CPsu2ViewManager : public CBase,
                         private MVPbkContactStoreObserver,
                         private MVPbkContactViewObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPsu2ViewManager* NewL();

        /**
         * Destructor.
         */
        virtual ~CPsu2ViewManager();

    public: // Interface

        /**
         * Returns the current engine side SIM view.
         *
         * @return  Current engine side SIM view.
         */
        inline MVPbkContactViewBase& CurrentSimView();

        /**
         * Returns the PIN 2 handler.
         *
         * @return  PIN 2 handler.
         */
        inline CPsu2SecUi& SecUi() const;

        /**
         * Resets current store and view configuration and loads
         * opens the store identified by given URI.
         * Opens the contact view identified by given view id.
         *
         * @param aURI                  URI of contact store to open.
         * @param aViewId               Id of the view to open.         
         * @param aViewLaunchCallback   Reference to receive contact
         *                               view reference.
         */
        void ResetAndInitiateStoreAndViewL(
                const TVPbkContactStoreUriPtr& aURI,
                TPsu2ViewId aViewId,
                MPsu2ViewLaunchCallback& aViewLaunchCallback);
                
        /**
         * Remove view's launch callback
         */                
        void RemoveViewLaunchCallBack(); 

        /**
         * Registers store and view.
         */
        void RegisterStoreAndView();

        /**
         * Deregisters store and view.
         */
        void DeregisterStoreAndView();

        /**
         * Returns current contact store.
         */
         MVPbkContactStore& ContactStore();

        /**
         * Returns ETrue if store is available. EFalse if not.
         * This function also shows unavailable note if store is not available
         */
        TBool StoreAvailableL();

    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent );

    private: // From MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        void ContactAddedToView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactViewError(
                MVPbkContactViewBase& aView,
                TInt aError,
                TBool aErrorNotified );


    private: // Implementation
        CPsu2ViewManager();
        void ConstructL();
        MVPbkContactViewBase* CreateViewL(
                TInt aViewDefResourceId );
        MVPbkContactViewBase* OpenViewL(
                TPsu2ViewId aViewId );
        void DoOpenContactViewL();

    private: // Data
        /// Ref: Contact manager
        CVPbkContactManager* iContactManager;
        /// Own: Currently active view
        MVPbkContactViewBase* iCurrentSimView;
        /// Ref: Contact store list
        MVPbkContactStoreList* iContactStores;
        /// Own: Sim phone
        MVPbkSimPhone* iPhone;
        /// Own: PIN 2 handler
        CPsu2SecUi* iSecUi;
        /// Own: FDN sort order
        CVPbkSortOrder* iFdnSortOrder;
        /// Own: contact store
        MVPbkContactStore* iContactStore;
        /// Own: view id
        TPsu2ViewId iViewId;
        /// Ref: View LaunchCallback
        MPsu2ViewLaunchCallback* iViewLaunchCallback;
        /// Own: Store available indicator
        TBool iStoreAvailable;
        /// Own: Reference count
        TInt iRefCount;
    };

// INLINE FUNCTIONS

// --------------------------------------------------------------------------
// CPsu2ViewManager::CurrentSimView
// --------------------------------------------------------------------------
//
inline MVPbkContactViewBase& CPsu2ViewManager::CurrentSimView()
    {
    return *iCurrentSimView;
    }

// --------------------------------------------------------------------------
// CPsu2ViewManager::SecUi
// --------------------------------------------------------------------------
//
inline CPsu2SecUi& CPsu2ViewManager::SecUi() const
    {
    return *iSecUi;
    }

#endif // CPSU2VIEWMANAGER_H

// End of File
