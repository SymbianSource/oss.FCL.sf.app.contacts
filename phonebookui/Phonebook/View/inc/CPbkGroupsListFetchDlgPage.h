/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Phonebook fetch dialog page: Groups List.
*
*/


#ifndef __CPbkGroupsListFetchDlgPage_H__
#define __CPbkGroupsListFetchDlgPage_H__

//  INCLUDES
#include <e32base.h>        // CBase
#include "MPbkFetchDlgPage.h"
#include <MPbkContactViewListControlObserver.h>
#include <cntviewbase.h>    // MContactViewObserver
#include <MPbkContactSelector.h>
#include "MPbkFetchDlgSelection.h"

//  FORWARD DECLARATIONS
class CPbkContactViewListControl;
class CPbkContactSubView;
class CPbkContactIdSet;

// CLASS DECLARATION

/**
 * Phonebook fetch dialog page: Groups List.
 */
NONSHARABLE_CLASS(CPbkGroupsListFetchDlgPage) : 
        public CBase, 
        public MPbkFetchDlgPage,
        public MPbkFetchDlgSelection,
        private MPbkContactViewListControlObserver,
        private MContactViewObserver,
        private MPbkContactSelector
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aParentDlg parent dialog
         */
        static CPbkGroupsListFetchDlgPage* NewL(MPbkFetchDlg& aParentDlg);

        /**
         * Destructor.
         */
        ~CPbkGroupsListFetchDlgPage();

    public:  // from MPbkFetchDlgPage
        TInt FetchDlgPageId() const;
        TBool DlgPageReady() const;
        void ActivateFetchDlgPageL();
        void DeactivateFetchDlgPage();
        TContactItemId FocusedContactIdL() const;
        TBool IsFetchDlgPageEmpty() const;
        void SetMPbkFetchDlgSelection(MPbkFetchDlgSelection* aAccepter); 
        TBool ItemsMarked() const;
        void LayoutContents();
        const CContactIdArray& MarkedItemsL() const;
        TBool ProcessSoftkeyMarkCommandL(TInt aCommandId);
        CPbkContactViewListControl& Control();
        
    public:  // from MPbkFetchDlgSelection
    	TBool ContactSelectionAcceptedL
    		(TContactItemId aGroupId, TInt aCurrentSelectedCount) const;        

    private:  // from MPbkContactViewListControlObserver
        void HandleContactViewListControlEventL
            (CPbkContactViewListControl& aControl,
            const TPbkContactViewListControlEvent& aEvent);

    private:  // from MContactViewObserver
	    void HandleContactViewEvent
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);

    private: // from MPbkContactSelector    
        TBool IsContactIncluded(TContactItemId aId);

    private:  // Implementation
        CPbkGroupsListFetchDlgPage(MPbkFetchDlg& aFetchDlg);
        void ConstructL();
        void SetupControlL();
        void HandleGroupSelectionL(TContactItemId aGroupId, TBool aSelected);
        void HandleContactViewEventL
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);
        void HandleContactViewEventL(const TContactViewEvent& aEvent);
        void HandleGroupsViewEventL(const TContactViewEvent& aEvent);

    private:  // Data
        /// Ref: Owner dialog of this page
        MPbkFetchDlg& iParentDlg;
        /// Ref: groups list control owned by iParentDlg
        CPbkContactViewListControl* iControl;
        /// Ref: names list contact view
        CContactViewBase& iContactView;
        /// Ref: all groups contact view
        CContactViewBase* iAllGroupsView;
        /// Ref: filtered groups view
        CPbkContactSubView* iGroupSubView;
        /// Own: initialisation state flags
        TUint iStateFlags;
        /// Own: set of included groups
        CPbkContactIdSet* iGroupsToInclude;
        /// Ref: selection accepter
        MPbkFetchDlgSelection* iFetchSelection;
        /// Own: id array of contacts from selected groups
        CContactIdArray* iGroupItemsArray;
    };

#endif // __CPbkGroupsListFetchDlgPage_H__
            
// End of File
