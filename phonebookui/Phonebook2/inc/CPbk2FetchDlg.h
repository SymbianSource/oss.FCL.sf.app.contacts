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
* Description:  Phonebook 2 fetch dialog.
*
*/


#ifndef CPBK2FETCHDLG_H
#define CPBK2FETCHDLG_H

//  INCLUDES
#include <AknDialog.h>
#include "MPbk2FetchDlg.h"
#include "MPbk2FetchResultsObserver.h"
#include <MPbk2DialogEliminator.h>
#include <babitflags.h>

//  FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class CAknNavigationDecorator;
class MPbk2FetchDlgPages;
class MVPbkContactOperationBase;
class MPbk2ContactUiControl;
class MVPbkViewContact;
class MPbk2ExitCallback;
class CPbk2FetchResults;
class CPbk2NamesListControl;

// CLASS DECLARATIONS

/**
 * Phonebook 2 fetch dialog.
 * Responsible for:
 * - presenting a fetch dialog for the user
 * - managing dialog pages
 * - handling select/deselect of contacts
 */
class CPbk2FetchDlg : public CAknDialog,
                      public MPbk2FetchDlg,
                      public MPbk2DialogEliminator,
                      private MPbk2FetchResultsObserver
    {
    public: // Data structures

        /**
         * Fetch parameter flags.
         */
        enum TFetchFlags
            {
            /// Multiple entry fetch
            EFetchMultiple  = 0x0001,
            /// Single entry fetch
            EFetchSingle    = 0x0002,
            /// Call item fetch
            EFetchCallItem  = 0x0004
            };

        /**
         * Exit parameter flags
         */
       enum TExitState
            {
            EExitOrdered,
            EExitApproved,
            EOkToExitWithoutHandlingIt
            };

        /**
         * Fetch parameters struct.
         * Parameters specify the fetch return value.
         */
        class TParams
            {
            public: // Interface

                /**
                 * Constructor.
                 */
                IMPORT_C TParams();

                /**
                 * Returns a cleanup item which will release all resources
                 * held by this object in case a leave occurs.
                 */
                IMPORT_C operator TCleanupItem();

            private: // Implementation
                static void Cleanup(
                        TAny* aPtr );

            private: // Unimplemented
                /// Unimplemented assignment operator
                TParams& operator=(
                        const TParams& );

            public: // Input parameters
                /// Own: Fetch flags, defaults to EFetchSingle
                /// @see TFetchFlags
                TUint iFlags;
                /// Own: PBK2_FETCH_DIALOG resource id
                /// Defaults to r_pbk2_single_entry_fetch_dlg
                TInt iResId;
                /// Ref: Pre-marked entries
                MVPbkContactLinkArray* iMarkedEntries;
                /// Ref: The contact view shown in the names list page
                MVPbkContactViewBase* iNamesListView;
                /// Ref: The contact view shown in the groups list page
                MVPbkContactViewBase* iGroupsListView;
                /// Own: Optional fetch softkey resource id
                TInt iCbaId;
                /// Own: Optional navi pane resource id
                TInt iNaviPaneId;
                /// Ref: Exit callback
                MPbk2ExitCallback* iExitCallback;
                /// Own: SK1 is invisible till user selects defined amount of entries
                /// default is 1 if not defined by user
                TInt iMinSelection;
            };

    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aParams   The fetch parameters.
         * @param aObserver Fetch observer.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2FetchDlg* NewL(
                TParams aParams,
                MPbk2FetchDlgObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2FetchDlg();

    public: // Interface

        /**
         * Marks the given contact as selected.
         *
         * @param aContactLink  Link to a contact to select.
         */
        IMPORT_C void AcceptDelayedFetchL(
                const TDesC8& aContactLink );

    public: // From CEikDialog
        IMPORT_C TInt ExecuteLD();

    public: // From MPbk2DialogEliminator
        void RequestExitL(
                TInt aCommandId );
        void ForceExit();
        void ResetWhenDestroyed(
                MPbk2DialogEliminator** aSelfPtr );

    private: // From CEikDialog
        SEikControlInfo CreateCustomControlL(
                TInt aControlType );
        void PreLayoutDynInitL();
        TBool OkToExitL(
                TInt aButtonId );
        void PageChangedL(
                TInt aPageId );
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void HandleResourceChange(
                TInt aType );
        void LineChangedL(
                TInt aControlId );

    private: // From MPbk2FetchDlg
        CCoeControl* FetchDlgControl(
                TInt aCtrlId ) const;
        MVPbkContactViewBase& FetchDlgViewL(
                TInt aControlId ) const;
        void SetFetchDlgViewL(
                TInt aControlId,
                MVPbkContactViewBase& aView );
        MVPbkContactLinkArray& FetchDlgSelection();
        void FetchDlgHandleError(
                TInt aError );
        TRect FetchDlgClientRect() const;
        void FetchDlgPageChangedL(
                MPbk2FetchDlgPage& aPage );
        MPbk2FetchDlgObserver& FetchDlgObserver() const;
        void SelectContactL(
                const MVPbkContactLink& aLink,
                TBool aSelected );

    private: // From MPbk2ControlObserver
        void HandleControlEventL(
                MPbk2ContactUiControl& aControl,
                const TPbk2ControlEvent& aEvent );

    private: // From MPbk2FetchResultsObserver
        void ContactSelected(
                const MVPbkContactLink& aLink,
                TBool aSelected );
        void ContactSelectionFailed();

    private: // Implementation
        CPbk2FetchDlg(
                TParams aParams,
                MPbk2FetchDlgObserver& aObserver );
        void ConstructL();
        void SetCbaCommandSetL(
                TInt aResourceId );
        void SetupStatusPaneL();
        void UpdateCbasL();
        void UpdateMultiSelectionMSKL(
                TBool aContactSelected );
        void RemoveCommandFromMSK();
        void SetMiddleSoftKeyLabelL(
                const TInt aResourceId,
                const TInt aCommandId );
        static TInt TryAcceptSelectionL(
                TAny* aSelf );
        TBool CheckIsOkToExitL(
                TInt aButtonId );
        TBool Canceled(
                TInt aButtonId );
        void SkinTabsL();
        static TInt DelaySkinning(
                TAny* aFetchDlg );
        static TInt RestoreSelections(
                TAny* aFetchDlg );
        void RestoreSelectionsL();
        TBool OkToExitApplicationL(
                TInt aCommandId );
        void ExitApplication(
                TInt aCommandId );

    private: // Types and data structures
        /// Resource data, see PBK2_FETCHENTRY_DIALOG.
        struct TResData
            {
            /**
             * Reads the data from resources.
             *
             * @param aReader   Resource reader pointed
             *                  to PBK2_FETCHENTRY_DIALOG resource.
             */
            void ReadFromResource(
                    TResourceReader& aReader );

            /// Own: Dialog resource id
            TInt iDialogId;
            /// Own: CBA resource id
            TInt iCbaId;
            /// Own: Empty CBA resource id
            TInt iEmptyCbaId;
            /// Own: Navi pane resource id
            TInt iNaviPaneId;
            };

    private: // Data
        /// Own: Parameters
        TParams iParams;
        /// Own: Resource data
        TResData iResData;
        /// Own: Navi pane control
        CAknNavigationDecorator* iNaviDecorator;
        /// Ref: Set to NULL when this dialog is destroyed
        MPbk2DialogEliminator** iSelfPtr;
        /// Ref: Set to ETrue when the execution enters destructor
        TBool* iDestroyedPtr;
        /// Own: Dialog pages
        MPbk2FetchDlgPages* iPages;
        /// Own: Current CBA command set
        TInt iCbaCommandSet;
        /// Ref: Observer
        MPbk2FetchDlgObserver& iObserver;
        /// Own: Exit handling
        TBitFlags8 iExitRecord;
        /// Own: Dialog accepter
        CAsyncCallBack* iDialogAccepter;
        /// Own: Delay for tab skinning
        CIdle* iTabSkinDelay;
        /// Own: Fetch results
        CPbk2FetchResults* iResults;
        /// Own: Link to focused contact
        MVPbkContactLink* iFocusedContactLink;
        /// Own: Delay for restoring selections
        CIdle* iSelectionRestorer;
        /// Own: Set to EFalse if MSK is not enabled in the current layout
        TBool iMSKEnabled;
        /// Not own: Names list control
        CPbk2NamesListControl* iNamesListControl;
        /// Not own: Group list control
        CPbk2NamesListControl* iGroupListControl;
    };

#endif // CPBK2FETCHDLG_H

// End of File
