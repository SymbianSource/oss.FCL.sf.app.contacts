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
*     Fetch dialog for Phonebook.
*
*/


#ifndef __CPbkFetchDlg_H__
#define __CPbkFetchDlg_H__

//  INCLUDES
#include <AknDialog.h>       // CEikDialog
#include "MPbkFetchDlg.h"
#include <eikcmobs.h>
#include <cntdef.h>
#include <MPbkContactViewListControlObserver.h>


//  FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CPbkContactEngine;
class CPbkContactIdSet;
class MPbkFetchDlgAccept;
class CPbkContextPaneIcon;
class MPbkFetchDlgPages;
class MPbkFetchKeyCallback;
class CPbkExtGlobals;
class MPbkFetchDlgSelection;


// CLASS DECLARATION

/**
 * Generic Phonebook fetch dialog -- do not use directly!
 * Use CPbkSingleEntryFetch for Single Entry Fetch or
 * CPbkMultipleEntryFetch for Multiple entry Fetch.
 */
NONSHARABLE_CLASS(CPbkFetchDlg) : 
        public CAknDialog, 
        private MPbkFetchDlg,
        private MPbkContactViewListControlObserver
    {
    public: // data structures
        /**
         * Fetch parameter flags.
         */
        enum TFetchFlags 
            { 
            FETCH_FOCUSED = 0x0001, 
            FETCH_MARKED  = 0x0002
            };

        /**
         * Fetch paramters struct. 
         * Parameters specify the fetch return value.
         */
        class TParams
            {
            public:  // Input parameters
                /**
                 * Fetch flags, see TFetchFlags.
                 * Defaults to FETCH_FOCUSED.
                 */
                TUint iFlags;

                /**
                 * PBK_FETCHENTRY_DIALOG resource id.
                 * Defaults to Single entry fetch.
                 */
                TInt iResId;

                /**
                 * MANDATORY: Contacts to display.
                 */
                CContactViewBase* iContactView;

                /**
                 * Optional fetch key event callback. 
                 */
                MPbkFetchKeyCallback* iKeyCallback;

                /**
                 * Optional fetch accept callback.
                 */
                MPbkFetchDlgAccept* iAcceptCallback;
                
                /**
                 * Optional fetch selection callback.
                 */
                MPbkFetchDlgSelection* iFetchSelection;

                /**
                 * Optional Fetch Softkey resource ID
                 */
                TInt iCbaId;

            public:  // Output parameters
                /**
                 * Returns the focused entry if iFlags has FETCH_FOCUSED set.
                 */
                TContactItemId iFocusedEntry;

                /**
                 * Returns the marked entries if iFlags has FETCH_MARKED set. 
                 * Caller must delete iMarkedEntries when no longer needed.
                 */
                CContactIdArray* iMarkedEntries;

            public:  // Interface
                /**
                 * C++ Constructor. Inits this object for single entry fetch.
                 */
                TParams();

                /**
                 * Returns a cleanup item which will release all resources 
                 * held by this object in case a leave occurs.
                 */
                operator TCleanupItem();

                // Compiler-generated destructor is ok for this class

            private:  // Implementation
                static void Cleanup(TAny* aPtr);

            private:  // Unimplemented
                /// Unimplemented copy constructor
                TParams(const TParams&);
                /// Unimplemented assignment operator
                TParams& operator=(const TParams&);
            };

    public:  // Constructors and destructor
        /**
         * Creates a fetch dialog with specified parameters.
         *
         * @param aParams   Fetch parameters, see TParams.
         * @param aEngine   Phonebook engine.
         * @return a new instace of this class.
         */
        static CPbkFetchDlg* NewL(TParams& aParams, CPbkContactEngine& aEngine);

        /**
         * Sets aSelfPtr to NULL when this dialog is destroyed.
         * @precond !aSelfPtr || *aSelfPtr==this
         */
        void ResetWhenDestroyed(CPbkFetchDlg** aSelfPtr);

        /**
         * Destructor.
         */ 
        ~CPbkFetchDlg();

    public:  // Interface
        /**
         * Executes and deletes (also if a leave occurs) this dialog.
         *
         * @return !=0 if dialog was accepted, 0 if canceled.
         */
        TInt ExecuteLD();

    private:  // from CEikDialog
	    SEikControlInfo CreateCustomControlL(TInt aControlType);
        void PreLayoutDynInitL();
        TBool OkToExitL(TInt aButtonId);
        void PageChangedL(TInt aPageId);
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
		void HandleResourceChange(TInt aType);
		void LineChangedL(TInt aControlId);
    public:   // MEikCommandObserver    
        void ProcessCommandL(TInt aCommandId);

    private:  // from MPbkFetchDlg
        CCoeControl* FetchDlgControl(TInt aCtrlId) const;
        CContactViewBase& FetchDlgNamesView() const;
        CPbkContactIdSet& FetchDlgSelection();
        void FetchDlgHandleError(TInt aError);
        TRect FetchDlgClientRect() const;
        CPbkContactEngine& PbkEngine();
        void FetchDlgPageChangedL(MPbkFetchDlgPage& aPage);
        void UpdateMSKL( TInt aIndex );

    private: // from MPbkContactViewListControlObserver
        void HandleContactViewListControlEventL(
                CPbkContactViewListControl& aControl,
                const TPbkContactViewListControlEvent& aEvent);

    private: // Implementation
        CPbkFetchDlg(TParams& aParams, CPbkContactEngine& aEngine);
        void ConstructL();
        void SetCbaCommandSetL(TInt aResourceId);
        void SetupStatusPaneL();
        void UpdateCbasL();
        static TInt TryAcceptSelectionL( TAny* aSelf );
        void SkinTabsL();
        static TInt DelaySkinning(TAny* aFetchDlg);

    private:  // types
        /**
         * Resource data, see PBK_FETCHENTRY_DIALOG.
         */
        struct TResData
            {
            TInt iDialogId;
            TInt iCbaId;
            TInt iEmptyCbaId;
            TInt iNaviPaneId;
            void ReadFromResource(TResourceReader& aReader);
            };

    private:  // data
        /// Ref: paramters
        TParams& iParams;
        /// Ref: Contact engine
        CPbkContactEngine& iEngine;
        /// Own: Resource data
        TResData iResData;
        /// Own: Navi pane control
        CAknNavigationDecorator* iNaviDecorator;
        /// Own: Selected contacts array
        CPbkContactIdSet* iSelectedIdSet;
        /// Ref: is the dialog canceled
        TBool* iCanceledPtr;
        /// Ref: Set to NULL when this dialog is destroyed.
        CPbkFetchDlg** iSelfPtr;
        /// Own: Phonebooks application icon
        CPbkContextPaneIcon* iContextPaneIcon;
        /// Own: dialog pages
        MPbkFetchDlgPages* iPages;
        /// Ref: currently active page
        MPbkFetchDlgPage* iCurrentPage;
        /// Own: current CBA command set
        TInt iCbaCommandSet;
        /// Own: handle to UI extension factory
        CPbkExtGlobals* iExtGlobals;
        /// Own: Resource id of the caller's status pane
        TInt iPreviousStatusPaneLayout;
        /// Own: Asynchronous dialog accepter callback
        CAsyncCallBack* iDialogAccepter;
        /// Own: Delay for tab skinning
        CIdle* iTabSkinDelay;
    };

#endif // __CPbkFetchDlg_H__

// End of File
