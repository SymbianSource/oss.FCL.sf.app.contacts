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
* Description:  Sorted composite contact view.
*
*/


#ifndef CVPBKSORTEDCOMPOSITECONTACTVIEW_H
#define CVPBKSORTEDCOMPOSITECONTACTVIEW_H

// INCLUDE FILES
#include "CVPbkCompositeContactView.h"
#include "MVPbkContactViewSortPolicy.h"

// FORWARD DECLARATIONS
class MVPbkContactViewSortPolicy;
class RFs;

/**
 * Virtual Phonebook sorted composite contact view.
 */
NONSHARABLE_CLASS(CVPbkSortedCompositeContactView) : 
        public CVPbkCompositeContactView
    {
    public:
        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer.
         * @param aSortOrder    Sort order.
         * @param aFieldTypes   Field types to be used in sorting.
         * @param aFs           File system handle
         * @return  A new instance of this class.
         */
        static CVPbkSortedCompositeContactView* NewLC(
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder, 
                const MVPbkFieldTypeList& aFieldTypes,
                RFs& aFs);

        /**
         * Destructor.
         */
        ~CVPbkSortedCompositeContactView();
                
    private: // From MVPbkContactViewFiltering
        MVPbkContactViewBase* CreateFilteredViewLC(
            MVPbkContactViewObserver& aObserver,
            const MDesCArray& aFindWords,
            const MVPbkContactBookmarkCollection* aAlwaysIncludedContacts );

    private: // From CVPbkCompositeContactView
        void DoBuildContactMappingL();
        TInt DoHandleContactAdditionL(
                TInt aSubViewIndex,
                TInt aIndex );
        void ChangeSortOrderL(
                const MVPbkFieldTypeList& aSortOrder );
                
    private: // Implementation
        CVPbkSortedCompositeContactView(
                const MVPbkFieldTypeList& aFieldTypes, RFs& aFs);
        void ConstructL(const MVPbkFieldTypeList& aSortOrder);
        TInt FindInsertionPos(const TContactMapping& aMapping) const;
        TInt CompareMappings(const TContactMapping& aLhs, 
                const TContactMapping& aRhs) const;        

    private: // Data
        /// Ref: Field types used in sorting
        const MVPbkFieldTypeList& iFieldTypes;
        /// Own: Sort policy
        MVPbkContactViewSortPolicy* iSortPolicy;
        /// Ref: File system handle
        RFs& iFs;
    };

#endif // CVPBKSORTEDCOMPOSITECONTACTVIEW_H

// End of File
