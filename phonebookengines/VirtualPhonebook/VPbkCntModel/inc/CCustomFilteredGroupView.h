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
* Description:  Contacts Model store custom filtered contact
*                view implementation. Derives from MVPbkContactViewBase,
*                performs the custom filtering.
*
*/


#ifndef VPBKCNTMODEL_CCUSTOMFILTEREDGROUPVIEW_H
#define VPBKCNTMODEL_CCUSTOMFILTEREDGROUPVIEW_H

// INCLUDES
#include "CViewBase.h"
#include <MVPbkContactSelector.h>

// FORWARD DECLARATIONS
class MVPbkFieldTypeSelector;
class MVPbkContactViewBase;
class MCustomContactViewObserver;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CCustomFilteredNativeContactView;


NONSHARABLE_CLASS( CCustomFilteredGroupView ): public CViewBase,
                                                 public MVPbkContactSelector
    {
    public: // Constructor and destructor

        /**
         * Constructor of this class.
         *
         * @param aContactStore                 Contact store.
         * @param aFieldTypeSelector            Field type selector.
         * @param aCustomContactViewObserver    Observer for the view not yet
         *                                      filtered.
         */         
         CCustomFilteredGroupView(
				CContactStore& aContactStore,
                const MVPbkFieldTypeSelector* aFieldTypeSelector,
                MCustomContactViewObserver& aCustomContactViewObserver );
        /**
         * Second phase constructor of this class. This should be called
         * after C++ constructor.
         *
         * @param aViewObserver         Observer for the
         *                                      filtered view.
         * @param aSortOrder            View sort order.
         * @param aNativeViewBase       The native base view.
         */
         void ConstructL( const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aViewObserver,
                const MVPbkFieldTypeList& aSortOrder,
                CContactViewBase& aNativeViewBase);
                        
        /**
         * Destructor.
         */
        ~CCustomFilteredGroupView();

    public: // Interface

        /**
         * Returns the customically filtered native base view.
         *
         * @return  Filtered native base view.
         */
        CContactViewBase& ContactViewBase();

    private: // From CViewBase
        TVPbkContactViewType Type() const;
        void DoInitializeViewL(
                const CVPbkContactViewDefinition& aViewDefinition,
                RContactViewSortOrder& aViewSortOrder );
        void DoTeardownView();
        TBool DoChangeSortOrderL(
                const CVPbkContactViewDefinition& aViewDefinition,
                RContactViewSortOrder& aSortOrder );

    private: // From MVPbkContactSelector
        TBool IsContactIncluded(
                const MVPbkBaseContact& aContact );

    private: // From MContactViewObserver
        void HandleContactViewEvent(
                const CContactViewBase& aView,
                const TContactViewEvent& aEvent );

    private: // Implementation

        void ConstructL(
                const CVPbkContactViewDefinition& aViewDefinition,
                MVPbkContactViewObserver& aObserver,
                const MVPbkFieldTypeList& aSortOrder );
        TBool IsContactIncludedL(
                const MVPbkBaseContact& aContact );
        TBool IsContactFieldIncludedL( 
        		const CContactIdArray* aGroupMembers,
        		const TInt& aMemberCount,
        		const MVPbkFieldTypeSelector& aFieldTypeSelector );

    private: // Data
        /// Ref: Contact store
        CContactStore& iContactStore;
        /// Ref: Field type selector, the filtering criteria
        const MVPbkFieldTypeSelector* iFieldTypeSelector;
        /// Ref: Custom contact view observer
        MCustomContactViewObserver& iCustomContactViewObserver;
        /// Own: Virtual Phonebook filtered view
        MVPbkContactViewBase* iFilteredView;
        /// Own: Custom filtered native contact view, the view to filter
        CCustomFilteredNativeContactView* iCustomFilteredContactView;
    };

} // namespace VPbkCntModel

#endif // VPBKCNTMODEL_CCUSTOMFILTEREDGROUPVIEW_H

// End of File
