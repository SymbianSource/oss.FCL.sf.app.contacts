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
* Description:  Contacts Model store contact view implementation.
*
*/


#ifndef VPBKCNTMODEL_CFILTEREDGROUPVIEW_H
#define VPBKCNTMODEL_CFILTEREDGROUPVIEW_H

// INCLUDES
#include "CViewBase.h"
#include <MVPbkContactViewObserver.h>

// FORWARD DECLARATIONS
class CVPbkFieldTypeSelector;
class CContactFilteredView;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CCustomFilteredGroupView;


/**
 * Virtual phonebook filtered groups view. This class filters contacts
 * based on field types. 
 */
NONSHARABLE_CLASS( CFilteredGroupView ): 
        public CViewBase,
        public MVPbkContactViewObserver
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer.
         * @param aParentStore  Parent contact store.
         * @param aSortOrder    Contact view sort order.
         * @param aFilter       Contact view filter
         * @param aFs           File server session.
         * @return  A new instance of this class.
         */
        static CFilteredGroupView* NewLC(
                const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver,
                CContactStore& aParentStore, 
                const MVPbkFieldTypeList& aSortOrder,
                RFs& aFs );
        /**
         * Destructor.
         */
        ~CFilteredGroupView();

    private: // From CViewBase
        TVPbkContactViewType Type() const;
        void DoInitializeViewL(
                const CVPbkContactViewDefinition& aViewDefinition,
                RContactViewSortOrder& aViewSortOrder );
        void DoTeardownView();
        TBool DoChangeSortOrderL(
                const CVPbkContactViewDefinition& aViewDefinition,
                RContactViewSortOrder& aSortOrder );

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
        CFilteredGroupView(
                CContactStore& aParentStore );
        void ConstructL(
                const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder,
                RFs& aFs );
        void ConstructBaseViewsL(
                const CVPbkContactViewDefinition& aViewDefinition,
                MContactViewObserver& aObserver,
                RContactViewSortOrder& aViewSortOrder );

    private: // Data
        /// Own: All groups view
        CContactViewBase* iBaseView;
        /// Ref: All groups view if shared
        CContactNamedRemoteView* iRemoteView;
        /// Own: Custom filtered contact view
        CCustomFilteredGroupView* iCustomFilteredView;
        /// Own: The contact view filter
        CVPbkFieldTypeSelector* iFilter;
        /// Ref: Native filtered view observer
        MContactViewObserver* iNativeObserver;
    };

} // namespace VPbkCntModel

#endif // VPBKCNTMODEL_CFILTEREDGROUPVIEW_H

// End of File
