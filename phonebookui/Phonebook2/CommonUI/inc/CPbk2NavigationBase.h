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
* Description:  Phonebook 2 contact navigator base.
*
*/


#ifndef CPBK2NAVIGATIONBASE_H
#define CPBK2NAVIGATIONBASE_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2ContactNavigation.h>
#include <MVPbkContactStoreObserver.h>

//  FORWARD DECLARATIONS
class MPbk2ContactNavigationLoader;
class MPbk2NavigationObserver;

//  CLASS DECLARATION

/**
 * Phonebook 2 navigator base.
 * Common base class for all navigation strategies.
 * Listens contact stores for store events and drives navigation loader
 * and navigation observer accordingly.
 */
class CPbk2NavigationBase : public CBase,
                            public MPbk2ContactNavigation,
                            private MVPbkContactStoreObserver
    {
    protected: // Construction and destruction

        /**
         * Constructor.
         *
         * @param aObserver             Observer.
         * @param aNavigationLoader     Navigation interface for
         *                              loading contacts.
         * @param aStoreList            List of stores this
         *                              navigator observes.
         */
        CPbk2NavigationBase(
                MPbk2NavigationObserver& aObserver,
                MPbk2ContactNavigationLoader& aNavigationLoader,
                TArray<MVPbkContactStore*> aStoreList );

        /**
         * Destructor.
         */
        ~CPbk2NavigationBase();

    public: // Interface

        /**
         * Returns the contact navigation loader.
         *
         * @return The contact navigation loader.
         */
        inline MPbk2ContactNavigationLoader& NavigationLoader() const;

        /**
         * Returns the scroll view or NULL if not set. Ownership
         * is not given.
         *
         * @return The scroll view or NULL if not set. Ownership is not
         *         given.
         */
        inline MVPbkContactViewBase* ScrollView() const;

    protected: // Base class interface

        /**
         * Second phase base constructor.
         */
        void BaseConstructL();

    protected: // From MPbk2ContactNavigation
        void SetScrollViewL(
                MVPbkContactViewBase* aScrollView,
                TBool aOwnershipTransfered );
        MVPbkContactLink* CurrentContactLC() const;

    protected: // From MVPbkContactViewObserver
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

    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

    private: // Implementation
        void HandleContactRemovedFromViewL(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void CloseStores();
        void RemoveViewObserver();
        void StartIdleCallBackL();
        static TInt IdleNotifierCallBack(
                TAny* aSelf );
        void HandleIdleNotify();

    protected: // Data
        /// Ref: Navigation observer
        MPbk2NavigationObserver& iNavigationObserver;

    private: // Data
        /// Ref: The contact loader.
        MPbk2ContactNavigationLoader& iNavigationLoader;
        /// Ref: List of used stores
        TArray<MVPbkContactStore*> iStoreList;
        /// Own: For asynchronously notifying observers
        CIdle* iIdleNotifier;
        /// Ref: This can be NULL if not set by SetScrollViewL.
        MVPbkContactViewBase* iScrollView;
        /// Own: Owned view. This can be NULL.
        MVPbkContactViewBase* iOwnedView;
        class CDeletedContactInfo;
        /// Own: knows the right contact after the current one is deleted.
        CDeletedContactInfo* iDeletedContactInfo;
    };

/**
 * A helper class to keep current contact also when the contact is deleted.
 */
NONSHARABLE_CLASS( CPbk2NavigationBase::CDeletedContactInfo ) : public CBase
    {
    public: // Construction and destruction

        /**
         * @param aNavigationBase The parent of the contact information
         */
        CDeletedContactInfo( CPbk2NavigationBase& aNavigationBase );

        /**
         * Destructor
         */
        ~CDeletedContactInfo();

    public: // Interface

        /**
         * Saves the removed contact information.
         *
         * @param aView         This must be the scroll view of the parent.
         * @param aIndex        The index of the removed contact.
         * @param aContactLink  The identifier of the removed contact.
         */
        void HandleContactRemovedFromViewL(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );

        /**
         * Saves deleted contact information.
         *
         * @param aContactStore     The store whose contact was deleted
         * @param aStoreEvent       In case of delete event this contains
         *                          the deleted contact link.
         */
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aStoreEvent );

        /**
         * Returns ETrue if contact is deleted from the store and removed
         * from the view.
         *
         * @return  ETrue if contact is deleted from the store and removed
         *          from the view.
         */
        TBool DeletedAndRemoved() const;

        /**
         * Returns the contact that is focused after the current one
         * is deleted and removed.
         *
         * @return  The contact identifier of the contact that is focused
         *          after the current contact is deleted.
         */
        MVPbkContactLink* FocusedContactOrNullLC();

    private: // Data
        /// Ref: The parent of this class
        CPbk2NavigationBase& iNavigationBase;
        /// Own: An identifier of the deleted contact from the store
        MVPbkContactLink* iDeletedContact;
        /// Own: An identifier of the removed contact from the view
        MVPbkContactLink* iRemovedContact;
        /// Own: An index of the removed contact
        TInt iRemovedContactIndex;
    };

// INLINE FUNCTIONS

// --------------------------------------------------------------------------
// CPbk2NavigationBase::NavigationLoader
// --------------------------------------------------------------------------
//
inline MPbk2ContactNavigationLoader&
        CPbk2NavigationBase::NavigationLoader() const
    {
    return iNavigationLoader;
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::ScrollView
// --------------------------------------------------------------------------
//
inline MVPbkContactViewBase* CPbk2NavigationBase::ScrollView() const
    {
    return iScrollView;
    }

#endif // CPBK2NAVIGATIONBASE_H

// End of File
