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
* Description:  Phonebook 2 contact view builder.
*
*/


#ifndef CPBK2CONTACTVIEWBUILDER_H
#define CPBK2CONTACTVIEWBUILDER_H

// INCLUDE FILES
#include <e32base.h>
#include <Pbk2ContactView.hrh>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class CVPbkContactViewDefinition;
class MVPbkContactViewBase;
class CPbk2StorePropertyArray;
class CVPbkContactStoreUriArray;
class MVPbkContactViewObserver;
class MVPbkFieldTypeList;
class CVPbkFieldTypeSelector;
class MVPbkContactSelector;

// CLASS DECLARATION

/**
 * Phonebook 2 contact view builder.
 */
class CPbk2ContactViewBuilder : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @param aStoreProperties  Store property array.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ContactViewBuilder* NewLC(
                CVPbkContactManager& aContactManager,
                CPbk2StorePropertyArray& aStoreProperties );

        /**
         * Destructor.
         */
        ~CPbk2ContactViewBuilder();

    public: // Interface

        /**
         * Creates a contact view for the stores specified in the
         * given array.
         *
         * @param aUriArray     Array of URIs of stores to include
         *                      in the view.
         * @param aObserver     Observer for the view.
         * @param aSortOrder    Sort order for the view.
         * @param aFilter       Field type selector used to filter the view.
         * @param aFlags		TVPbkContactViewFlag flags for view modifying
         * @return  The contact view.
         */
        IMPORT_C MVPbkContactViewBase* CreateContactViewForStoresLC(
                const CVPbkContactStoreUriArray& aUriArray,
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder,
                CVPbkFieldTypeSelector* aFilter,
                TUint32 aFlags ) const;

        /**
         * Creates a TOP-NON-TOP contact view for the stores specified in the
         * given array.
         *
         * @param aUriArray     Array of URIs of stores to include
         *                      in the view.
         * @param aObserver     Observer for the view.
         * @param aSortOrder    Sort order for the view.
         * @param aFilter       Field type selector used to filter the view.
         * @param aContactSelector A contact selector.
         * @return  The contact view.
         */
        IMPORT_C MVPbkContactViewBase* CreateTopNonTopContactViewForStoresLC(
                const CVPbkContactStoreUriArray& aUriArray,
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder,
                CVPbkFieldTypeSelector* aFilter,
                TUint32 aFlags,
                MVPbkContactSelector& aContactSelector ) const;

        /**
         * Creates a group view for the stores specified in the
         * given array.
         *
         * @param aUriArray     Array of URIs of stores to include
         *                      in the view.
         * @param aObserver     Observer for the view.
         * @param aSortOrder    Sort order for the view.
         * @param aFilter       Field type selector used to filter the view.
         * @param aFlags		TVPbkContactViewFlag flags for view modifying
         * @return  The contact view.
         */
        IMPORT_C MVPbkContactViewBase* CreateGroupViewForStoresLC(
                const CVPbkContactStoreUriArray& aUriArray,
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder,
                CVPbkFieldTypeSelector* aFilter,
                TUint32 aFlags ) const;

    private: // Implementation
        CPbk2ContactViewBuilder(
                CVPbkContactManager& aContactManager,
                CPbk2StorePropertyArray& aStoreProperties );
        CVPbkContactViewDefinition* BuildMainViewStructureLC(
                CVPbkFieldTypeSelector* aFilter,
                TUint32 aFlags ) const;
        MVPbkContactViewBase* CreateViewForStoresLC(
                TPbk2ContactViewType aViewType,
                const CVPbkContactStoreUriArray& aUriArray,
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder,
                CVPbkFieldTypeSelector* aFilter,
                TUint32 aFlags,
                TBool aTopContacts = EFalse  ) const;
        static TBool IncludeThisViewInTopContactMode(
                TPbk2ContactViewType aViewType );
        void SetTopRelatedFilteringL(
                CVPbkContactViewDefinition& aNewSubViewDef,
                TPbk2ContactViewType aViewType,
                CVPbkFieldTypeSelector* aFilter ) const;

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Store property array
        CPbk2StorePropertyArray& iStoreProperties;
        mutable MVPbkContactSelector* iContactSelector; // Does not own
    };

#endif // CPBK2CONTACTVIEWBUILDER_H

// End of File
