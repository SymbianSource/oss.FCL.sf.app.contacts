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
* Description:  Phonebook 2 sort order manager implementation.
*
*/


#ifndef CPBK2SORTORDERMANAGERIMPL_H
#define CPBK2SORTORDERMANAGERIMPL_H

// INCLUDE FILES
#include <CPbk2SortOrderManager.h>
#include <MVPbkContactViewObserver.h>
#include <babitflags.h>
#include "pbk2nameordercenrep.h"

// FORWARD DECLARATIONS
class CVPbkSortOrder;
class RResourceFile;
class CRepository;

/// Function pointer type definition
typedef void ( MPbk2SortOrderObserver::*Pbk2SortOrderObserverEvent )();


// CLASS DECLARATION

/**
 * An internal observer inferface for getting sort order
 * changed events from CenRep.
 */
class MPbk2CenRepSortOrderObserver
    {
    public: // Interface

        /**
         * Called when CenRep notifies about changed sort order.
         */
        virtual void CenRepSortOrderChangedL() = 0;

        /**
         * Called if CenRepSortOrderChangedL leaves.
         *
         * @param aError    Error code.
         */
        virtual void CenRepSortOrderChangeError(
                TInt aError ) = 0;

    protected: // Disabled functions
        ~MPbk2CenRepSortOrderObserver()
            {}
    };

/**
 * An internal observer inferface for getting separator
 * changed events from CenRep.
 */
class MPbk2CenRepSeparatorObserver
    {
    public: // Interface

        /**
         * Called when CenRep notifies about changed separator.
         */
        virtual void CenRepSeparatorChangedL() = 0;

        /**
         * Called if CenRepSeparatorChangedL leaves.
         *
         * @param aError    Error code.
         */
        virtual void CenRepSeparatorChangeError(
                TInt aError ) = 0;

    protected: // Disabled functions
        ~MPbk2CenRepSeparatorObserver()
            {}
    };

/**
 * Phonebook 2 sort order manager implementation for contact view.
 */
NONSHARABLE_CLASS(CPbk2SortOrderManagerImpl) : public CBase,
            public MVPbkContactViewObserver,
            private MPbk2CenRepSortOrderObserver,
            private MPbk2CenRepSeparatorObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aMasterFieldTypeList      Field type list.
         * @param aFs                       A file system session
         *                                  reference for resource file
         *                                  handling. If NULL then
         *                                  an own session is created.
         * @return  A new instance of this class.
         */
        static CPbk2SortOrderManagerImpl* NewL(
                const MVPbkFieldTypeList& aMasterFieldTypeList,
                RFs* aFs = NULL );

        /**
         * Destructor.
         */
        ~CPbk2SortOrderManagerImpl();

    public: // Interface

        /**
         * Sets the contact view that this manager manages.
         *
         * @param aContactView  The contact view to manage.
         */
        void SetContactViewL(
                MVPbkContactViewBase& aContactView );

        /**
         * Adds an observer.
         *
         * @param aObserver     The observer to add.
         */
        void AddObserverL(
                MPbk2SortOrderObserver& aObserver );

        /**
         * Removes an observer.
         *
         * @param aObserver     The observer to remove.
         */
        void RemoveObserver(
                MPbk2SortOrderObserver& aObserver );

        /**
         * Sets the name displaying order for the managed contact view.
         *
         * @param aNameDisplayOrder     New name display order for
         *                              the managed contact view.
         * @param aSeparator            Custom separator to be used between
         *                              lastname and firstname if it exists.
         *                              If KNullDesC is given, space is used
         *                              as separator.
         */
        void SetNameDisplayOrderL(
                CPbk2SortOrderManager::TPbk2NameDisplayOrder aNameDisplayOrder,
                const TDesC& aSeparator = KNullDesC );

        /**
         * Returns the current name display order.
         *
         * @return  Current name display order.
         */
        CPbk2SortOrderManager::TPbk2NameDisplayOrder NameDisplayOrder() const;

        /**
         * Returns the current sort order.
         *
         * @return  Current sort order.
         */
        const MVPbkFieldTypeList& SortOrder() const;

        /**
         * Returns the language specific default separator.
         * This can not be changed run time.
         *
         * @return the language specific default separator.
         */
        const TDesC& DefaultSeparator() const;

        /**
         * Returns the current separator. This the separator that
         * can be changed run time and saved to Central Repository.
         *
         * @return A current separator.
         */
        const TDesC& CurrentSeparator() const;

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

    private: // From MPbk2CenRepSortOrderObserver
        void CenRepSortOrderChangedL();
        void CenRepSortOrderChangeError(
                TInt aError );

    private: // From MPbk2CenRepSeparatorObserver
        void CenRepSeparatorChangedL();
        void CenRepSeparatorChangeError(
                TInt aError );

    private: // Implementation
        CPbk2SortOrderManagerImpl(
                const MVPbkFieldTypeList& aMasterFieldTypeList );
        void ConstructL( RFs* aFs );
        void SendEventToObservers(
                Pbk2SortOrderObserverEvent aEvent );
        TInt FindObserver(
                MPbk2SortOrderObserver& aObserver );
        CVPbkSortOrder* CreateSortOrderL(
                CPbk2SortOrderManager::TPbk2NameDisplayOrder aNameDisplayOrder,
                RResourceFile* aResFile );                
        CVPbkSortOrder* CreateSortOrderLC(
                CPbk2SortOrderManager::TPbk2NameDisplayOrder aNameDisplayOrder,
                RResourceFile* aResFile );
        TPbk2NameOrderInCenRep PersistentNameDisplayOrder() const;
        TInt SetPersistentNameDisplayOrder(
                TPbk2NameOrderInCenRep aNameDisplayOrder );
        TPbk2NameOrderInCenRep DefaultNameDisplayOrderConfigurationL(
                RResourceFile& aResFile ) const;
        CPbk2SortOrderManager::TPbk2NameDisplayOrder ConvertNameDisplayOrder(
                TPbk2NameOrderInCenRep aCenRepOrder ) const;
        TPbk2NameOrderInCenRep ConvertNameDisplayOrder(
                CPbk2SortOrderManager::TPbk2NameDisplayOrder aDisplayOrderOrder ) const;
        HBufC* PersistentSeparatorL() const;
        TBool IsDefaultSeparatorConfiguredL(
                RResourceFile& aResFile );
        HBufC* DefaultSeparatorConfigurationL(
                RResourceFile& aResFile ) const;
        TBool SetSeparatorL(
                const TDesC& aSeparator );
        TInt SetPersistentSeparator(
                const TDesC& aSeparator ) const;
        void NotifyPsEngineAboutSortOrderChangeL() const;

    private: // Data
        /// Ref: Contact view whose sort order this manager manages
        MVPbkContactViewBase* iContactView;
        /// Ref: Master field type list of vpbk
        const MVPbkFieldTypeList& iMasterFieldTypeList;
        /// Own: Current sort order
        CVPbkSortOrder* iSortOrder;
        /// Own: Array of observers
        RPointerArray<MPbk2SortOrderObserver> iObservers;
        /// Own: Custom separator for lastname and firstname
        /// saved in central repository
        HBufC* iSeparator;
        /// Own: The language specific default separator
        HBufC* iDefaultSeparator;
        /// Own: Central Repository connection
        CRepository* iSortOrderSettings;
        /// Own: A Central Repository monitor for sort order
        class CSortOrderMonitor;
        CSortOrderMonitor* iSortOrderMonitor;
        /// Own: A Central Repository monitor for separator character
        class CSeparatorMonitor;
        CSeparatorMonitor* iSeparatorMonitor;
        /// Own: The default language specific name order that doesn't
        ///      change.
        TPbk2NameOrderInCenRep iDefaultNameOrder;
        //Flag to indicate Feature manager initilization
        TBool iFeatureManagerInitilized;
    };

/**
 * An internal class that listens sort order key in CenRep
 */
NONSHARABLE_CLASS(CPbk2SortOrderManagerImpl::CSortOrderMonitor)
        :   public CActive
    {
    public: // Construction and destruction

        /**
         * Constructor.
         *
         * @param aRepository   Central repository reference.
         * @param aObserver     Observer.
         */
        CSortOrderMonitor(
                CRepository& aRepository,
                MPbk2CenRepSortOrderObserver& aObserver );

        /**
         * Destructor.
         */
        ~CSortOrderMonitor();

    public: // Interface

        /**
         * Activates the monitor.
         */
        void ActivateL();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // Data
        /// Ref: An open session for listening to sort order changes
        CRepository& iRepository;
        /// Ref: An observer that is called when key changes
        MPbk2CenRepSortOrderObserver& iObserver;
    };

/**
 * An internal class that listens sort order key in CenRep
 */
NONSHARABLE_CLASS(CPbk2SortOrderManagerImpl::CSeparatorMonitor)
        :   public CActive
    {
    public: // Construction and destruction

        /**
         * Constructor.
         *
         * @param aRepository   Central repository reference.
         * @param aObserver     Observer.
         */
        CSeparatorMonitor(
                CRepository& aRepository,
                MPbk2CenRepSeparatorObserver& aObserver );

        /**
         * Destructor.
         */
        ~CSeparatorMonitor();

    public: // Interface

        /**
         * Activates the monitor.
         */
        void ActivateL();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // Data
        /// Ref: An open session for listening to sort order changes
        CRepository& iRepository;
        /// Ref: An observer that is called when key changes
        MPbk2CenRepSeparatorObserver& iObserver;
    };

#endif // CPBK2SORTORDERMANAGERIMPL_H

// End of File
