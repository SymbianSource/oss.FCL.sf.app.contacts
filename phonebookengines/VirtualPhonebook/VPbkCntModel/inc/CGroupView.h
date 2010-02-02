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
* Description:  Virtual Phonebook group view
*
*/
 

#ifndef CGROUPVIEW_H
#define CGROUPVIEW_H

// INCLUDES
#include "CViewBase.h"

namespace VPbkCntModel {

/**
 * Virtual phonebook group view
 */
NONSHARABLE_CLASS( CGroupView ): public CViewBase
    {
    public:  // Constructor and destructor
        /**
         * Creates new instance of this class
         * @param aObserver Observer that gets notifications
         * @param aParentStore Store where to get the groups
         * @param aSortOrder Order of the results in the view
         * @return A new instance of this class
         */
        static CGroupView* NewLC(
                const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver,
                CContactStore& aParentStore, 
                const MVPbkFieldTypeList& aSortOrder);
        ~CGroupView();

    private: // From CViewBase
        TVPbkContactViewType Type() const;
        void DoInitializeViewL(
                const CVPbkContactViewDefinition& aViewDefinition,
                RContactViewSortOrder& aViewSortOrder );
        void DoTeardownView();
        TBool DoChangeSortOrderL(
                const CVPbkContactViewDefinition& aViewDefinition,
                RContactViewSortOrder& aSortOrder );

    private: // Implementation
        CGroupView(CContactStore& aParentStore);
        void ConstructL(const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver, 
                const MVPbkFieldTypeList& aSortOrder);

    private: // Data
        /// Ref: Reference to CViewBase::iView
        CContactNamedRemoteView* iRemoteView;
    };

} // namespace VPbkCntModel

#endif // CGROUPVIEW_H

// End of file
