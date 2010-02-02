/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook group members view
*
*/
 

#ifndef CGROUPMEMBERSVIEW_H
#define CGROUPMEMBERSVIEW_H

// INCLUDES
#include "CViewBase.h"

#include <MVPbkContactViewObserver.h>

namespace VPbkCntModel {


/**
 * Virtual phonebook group members view
 */
NONSHARABLE_CLASS( CGroupMembersView ): 
        public CViewBase,
        public MVPbkContactViewObserver
    {
    public:  // Constructor and destructor
        /**
         * Creates new instance of this class
         * @param aParentStore Store where to get the group members
         * @param aGroupId Identification of the group
         * @param aObserver Observer that gets notifications
         * @param aSortOrder Order of the results in the view
         * @return A new instance of this class
         */
        static CGroupMembersView* NewLC(
                CContactStore& aParentStore,
                TContactItemId aGroupId,
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder);
        ~CGroupMembersView();

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
        void ContactViewReady(MVPbkContactViewBase&) 
            {}
        void ContactViewUnavailable(MVPbkContactViewBase&)
            {}
        void ContactAddedToView(MVPbkContactViewBase&, TInt, const MVPbkContactLink&)
            {}
        void ContactRemovedFromView(MVPbkContactViewBase&, TInt, const MVPbkContactLink&)
            {}
        void ContactViewError(MVPbkContactViewBase&, TInt, TBool)
            {}

    private: // Implementation
        CGroupMembersView(
                CContactStore& aParentStore,
                TContactItemId aGroupId);
        void ConstructL(MVPbkContactViewObserver& aObserver, const MVPbkFieldTypeList& aSortOrder);

    private: // Data
        /// Own: contact base view
        CViewBase* iBaseView;
        /// Ref: group identification
        TContactItemId iGroupId;
    };

} // namespace VPbkCntModel

#endif // CGROUPMEMBERSVIEW_H

// End of file
