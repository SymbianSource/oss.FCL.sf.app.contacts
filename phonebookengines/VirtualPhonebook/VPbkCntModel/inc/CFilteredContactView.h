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


#ifndef VPBKCNTMODEL_CFILTEREDCONTACTVIEW_H
#define VPBKCNTMODEL_CFILTEREDCONTACTVIEW_H

// INCLUDES
#include "CViewBase.h"
#include <MVPbkContactViewObserver.h>

// FORWARD DECLARATIONS
class CVPbkFieldTypeSelector;
class CContactFilteredView;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CCustomFilteredContactView;


/**
 * Virtual phonebook filtered contact view. This class filters contacts
 * based on field types. 
 */
NONSHARABLE_CLASS( CFilteredContactView ): 
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
        static CFilteredContactView* NewLC(
                const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver,
                CContactStore& aParentStore, 
                const MVPbkFieldTypeList& aSortOrder,
                RFs& aFs );
        /**
         * Destructor.
         */
        ~CFilteredContactView();

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
        CFilteredContactView(
                CContactStore& aParentStore,
                RFs& aFs );
        void ConstructL(
                const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder );
        void ConstructBaseViewsL(
                const CVPbkContactViewDefinition& aViewDefinition,
                MContactViewObserver& aObserver,
                RContactViewSortOrder& aViewSortOrder );

    private: // Data
        /// Own: All contacts view
        CContactViewBase* iBaseView;
        /// Ref: All contacts view if shared.
        CContactNamedRemoteView* iRemoteView;
        /// Own: Also same as CViewBase::iView, the filtered view
        CContactFilteredView* iFilteredView;
        /// Own: Custom filtered contact view
        CCustomFilteredContactView* iCustomFilteredView;
        /// Own: Contacts Model contact view filter
        TUint iNativeFilter;
        /// Own: Custom filtering needed indicator
        TBool iCustomFilteringNeeded;
        /// Own: The contact view filter
        CVPbkFieldTypeSelector* iFilter;
        /// Ref: Native filtered view observer
        MContactViewObserver* iNativeObserver;
        /// Ref: File server session
        RFs& iFs;
    };

} // namespace VPbkCntModel

#endif // VPBKCNTMODEL_CFILTEREDCONTACTVIEW_H

// End of File
