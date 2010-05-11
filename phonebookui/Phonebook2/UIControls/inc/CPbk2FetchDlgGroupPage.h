/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 fetch dialog group page.
*
*/


#ifndef CPBK2FETCHDLGGROUPPAGE_H
#define CPBK2FETCHDLGGROUPPAGE_H

// INCLUDES
#include <e32base.h>
#include "MPbk2FetchDlgPage.h"
#include "MPbk2ControlObserver.h"

// FORWARD DECLARATIONS
class CPbk2NamesListControl;
class MPbk2FetchDlg;
class MVPbkContactViewBase;
class CPbk2FetchDlgGroupMarker;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * Phonebook 2 fetch dialog group page.
 * Responsible for presenting and managing a single fetch page.
 */
NONSHARABLE_CLASS(CPbk2FetchDlgGroupPage) : public CBase,
                                            public MPbk2FetchDlgPage,
                                            private MPbk2ControlObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aParentDlg        Parent dialog.
         * @param aControlId        Id of the control.
         * @param aContactManager   Virtual Phonebook contact manager.
         */
        static CPbk2FetchDlgGroupPage* NewL(
                MPbk2FetchDlg& aParentDlg,
                TInt aControlId,
                CVPbkContactManager& aContactManager );

        /**
         * Destructor
         */
        ~CPbk2FetchDlgGroupPage();

    public: // From MPbk2FetchDlgPage
        TInt FetchDlgPageId() const;
        void ActivateFetchDlgPageL();
        void DeactivateFetchDlgPage();
        const MVPbkBaseContact* FocusedContactL() const;
        MPbk2ContactUiControl& Control() const;
        MVPbkContactViewBase& View() const;
        void SetViewL(
                MVPbkContactViewBase& aView );
        TBool DlgPageReady() const;
        TBool DlgPageEmpty() const;
        void SelectContactL(
                const MVPbkContactLink& aContactLink,
                TBool aSelect );
        void HandleResourceChange(
                TInt aType );

    private: // From MPbk2ControlObserver
        void HandleControlEventL(
                MPbk2ContactUiControl& aControl,
                const TPbk2ControlEvent& aEvent );


    private: // Implementation
        CPbk2FetchDlgGroupPage(
                MPbk2FetchDlg& aFetchDlg,
                TInt aControlId,
                CVPbkContactManager& aContactManager );
        void ConstructL();
        void HandleContactSelectionL(
                const MVPbkBaseContact* aContact,
                TInt aIndex,
                TBool aSelected );
        void SetPagesReadyStateL(
                TBool aReadyState );
        void UpdateListEmptyTextL(
                TInt aListState );
        HBufC* ListEmptyTextLC(
                TInt aListState );

    protected: // Data
        /// Ref: Parent dialog of this page
        MPbk2FetchDlg& iParentDlg;
        /// Ref: Names list control
        CPbk2NamesListControl* iControl;
        /// Ref: Contact view
        MVPbkContactViewBase* iContactView;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Own: Control id
        TInt iControlId;
        /// Own: Flag indicating is the control ready
        TBool iIsReady;
        /// Own: Marker
        CPbk2FetchDlgGroupMarker* iMarker;
    };

#endif // CPBK2FETCHDLGGROUPPAGE_H

// End of File