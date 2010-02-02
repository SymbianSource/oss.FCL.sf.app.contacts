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


#ifndef VPBKCNTMODEL_CCONTACTVIEW_H
#define VPBKCNTMODEL_CCONTACTVIEW_H

// INCLUDES
#include "CViewBase.h"

namespace VPbkCntModel {

NONSHARABLE_CLASS( CContactView ): public CViewBase
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer.
         * @param aParentStore  Parent contact store.
         * @param aSortOrder    Contact view sort order.
         * @return  A new instance of this class.
         */
        static CContactView* NewLC(
                const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver,
                CContactStore& aParentStore, 
                const MVPbkFieldTypeList& aSortOrder );
        /**
         * Destructor.
         */
        ~CContactView();

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
        CContactView(
                CContactStore& aParentStore );
        void ConstructL(
                const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder );

    private: // Data
        /// Ref: Reference to CViewBase::iView
        CContactNamedRemoteView* iRemoteView;
    };

} // namespace VPbkCntModel

#endif // VPBKCNTMODEL_CCONTACTVIEW_H

// End of File
