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
* Description:  Ordered composite contact view.
*
*/


#ifndef CVPBKORDEREDCOMPOSITECONTACTVIEW_H
#define CVPBKORDEREDCOMPOSITECONTACTVIEW_H

// INCLUDE FILES
#include "CVPbkCompositeContactView.h"

/**
 * Virtual Phonebook Ordered composite contact view.
 */
NONSHARABLE_CLASS(CVPbkOrderedCompositeContactView)
		: public CVPbkCompositeContactView
    {
    public:
        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer.
         * @param aSortOrder    Sort order.
         * @return A new instance of this class.
         */
        static CVPbkOrderedCompositeContactView* NewLC(
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder );
    
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

    private: // Implementation
        CVPbkOrderedCompositeContactView();
        void ConstructL(
                const MVPbkFieldTypeList& aSortOrder );
        static TInt CompareOrderedMappings(
                const TContactMapping& aLhs,
                const TContactMapping& aRhs );
    };

#endif // CVPBKORDEREDCOMPOSITECONTACTVIEW_H

// End of File
