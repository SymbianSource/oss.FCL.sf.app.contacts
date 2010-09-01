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
* Description:  Phonebook 2 view supplier.
*
*/


#ifndef CPBK2CONTACTVIEWSUPPLIER_H
#define CPBK2CONTACTVIEWSUPPLIER_H

// INCLUDES
#include <e32base.h>
#include <MPbk2ContactViewSupplier.h>
#include <MPbk2ContactViewSupplier2.h>
#include <MVPbkContactViewObserver.h>
#include <MPbk2StoreConfigurationObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactViewBase;
class CVPbkContactManager;
class CPbk2SortOrderManager;
class CPbk2StoreConfiguration;
class CPbk2StorePropertyArray;
class MVPbkFieldTypeList;
class CVPbkSortOrderAcquirer;
class CPbk2NonTopContactSelector;

// CLASS DECLARATION

/**
 * Phonebook 2 view supplier.
 * Responsible for owning and delivering shared contact views.
 */
class CPbk2ContactViewSupplier : public CBase,
                                 public MPbk2ContactViewSupplier,
                                 public MPbk2ContactViewSupplier2,
                                 private MVPbkContactViewObserver,
                                 private MPbk2StoreConfigurationObserver
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager               Virtual Phonebook
         *                                      contact manager.
         * @param aSortOrderManager             Sort order manager.
         * @param aStoreConfiguration           Store configuration.
         * @param aStoreProperties              Store properties.
         * @param aActOnConfigurationEvents     Indicates whether to
         *                                      reconstruct the views when
         *                                      configuration changes.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ContactViewSupplier* NewL(
                CVPbkContactManager& aContactManager,
                CPbk2SortOrderManager& aSortOrderManager,
                CPbk2StoreConfiguration& aStoreConfiguration,
                CPbk2StorePropertyArray& aStoreProperties,
                TBool aActOnConfigurationEvents );

        /**
         * Destructor.
         */
        ~CPbk2ContactViewSupplier();

    public: // From MPbk2ContactViewSupplier
        MVPbkContactViewBase* AllContactsViewL();
        MVPbkContactViewBase* AllGroupsViewL();
        TAny* MPbk2ContactViewSupplierExtension( TUid aExtensionUid );
        
    public: // From MPbk2ContactViewSupplier2
        MVPbkContactViewBase* TopContactsViewL();

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

    private: // From MPbk2StoreConfigurationObserver
        void ConfigurationChanged();
        void ConfigurationChangedComplete();

    private: // Implementation
        CPbk2ContactViewSupplier(
                CVPbkContactManager& aContactManager,
                CPbk2SortOrderManager& aSortOrderManager,
                CPbk2StoreConfiguration& aStoreConfiguration,
                CPbk2StorePropertyArray& aStoreProperties );
        void ConstructL(
                TBool aActOnConfigurationEvents );
        void HandleConfigurationChangedL();
        MVPbkContactViewBase* CreateAllContactsViewL();
        MVPbkContactViewBase* CreateAllGroupsViewL();

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Sort order manager
        CPbk2SortOrderManager& iSortOrderManager;
        /// Ref: Store configuration
        CPbk2StoreConfiguration& iStoreConfiguration;
        /// Ref: Store properties
        CPbk2StorePropertyArray& iStoreProperties;
        /// Own: The shared all contacts view object
        MVPbkContactViewBase* iAllContactsView;
        /// Own: The shared all groups view object
        MVPbkContactViewBase* iAllGroupsView;
        /// Own: Old contacts view
        MVPbkContactViewBase* iOldContactsView;
        /// Own: Old groups view
        MVPbkContactViewBase* iOldGroupsView;
        /// Own: Group sort order acquirer which owns the group sort order
        CVPbkSortOrderAcquirer* iSortOrderAcquirer;

        CPbk2NonTopContactSelector* iNonTopContactSelector;
    };

#endif // CPBK2CONTACTVIEWSUPPLIER_H

// End of File
