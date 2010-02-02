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
*     Phonebook fetch dialog page: Names List.
*
*/


#ifndef __CPbkNamesListFetchDlgPage_H__
#define __CPbkNamesListFetchDlgPage_H__


//  INCLUDES
#include <e32base.h>        // CBase
#include "MPbkFetchDlgPage.h"
#include <MPbkContactViewListControlObserver.h>
#include "MPbkFetchDlgSelection.h"
#include <cntviewbase.h>    // MContactViewObserver

//  FORWARD DECLARATIONS
class CPbkContactViewListControl;

// CLASS DECLARATION

/**
 * Phonebook fetch dialog page: Names List.
 */
NONSHARABLE_CLASS(CPbkNamesListFetchDlgPage) : 
        public CBase, 
        public MPbkFetchDlgPage,
        public MPbkFetchDlgSelection,
        private MPbkContactViewListControlObserver,
        private MContactViewObserver
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aParentDlg parent dialog
         */
        static CPbkNamesListFetchDlgPage* NewL(MPbkFetchDlg& aParentDlg);

        /**
         * Destructor
         */
        ~CPbkNamesListFetchDlgPage();

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
            (const CContactViewBase& aView,const TContactViewEvent& aEvent);

    private:  // Implementation
        CPbkNamesListFetchDlgPage(MPbkFetchDlg& aFetchDlg);
        void ConstructL();
        void HandleContactSelectionL
            (TContactItemId aContactId, TBool aSelected);

    private:  // Data
        /// Ref: Owner dialog of this page
        MPbkFetchDlg& iParentDlg;
        /// Ref: names list control owned by iParentDlg
        CPbkContactViewListControl* iControl;
        /// Ref: names list contact view
        CContactViewBase& iContactView;
        /// Ref: selection accepter
        MPbkFetchDlgSelection* iFetchSelection;
    };

#endif // __CPbkNamesListFetchDlgPage_H__
            
// End of File
