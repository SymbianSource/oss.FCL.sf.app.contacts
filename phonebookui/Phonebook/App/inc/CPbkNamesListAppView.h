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
*     Names List View controller (CAknView).
*
*/


#ifndef __CPbkNamesListAppView_H__
#define __CPbkNamesListAppView_H__

//  INCLUDES
#include "CPbkContactListAppView.h" // Superclass
#include <MPbkContactViewListControlObserver.h>
#include <MPbkMenuCommandObserver.h>
#include <cntdef.h>  // TContactItemId
#include <cntviewbase.h>
#include "MPbkCtrlDisabler.h"

//  FORWARD DECLARATIONS
template<class ControlType> class CPbkControlContainer;
class MPbkViewExtension;
class CPbkExtGlobals;
class CPbkContactItem;
class CPbkFFSCheck;
class RSharedDataClient;
class CPbkDbRecoveryUi;

//  CLASS DECLARATION

/**
 * Phonebook "Names List" application view class.
 * This class implements the UI logic of the Phonebook names list view.
 */
class   CPbkNamesListAppView :
        public CPbkContactListAppView,
        private MPbkContactViewListControlObserver,
        private MContactViewObserver,
        private MPbkMenuCommandObserver,
        private MPbkCtrlDisabler
    {
    public:  // interface
        /**
         * Creates and returns a new object of this class.
         */
        static CPbkNamesListAppView* NewL();

        /**
         * Destructor.
         */
        ~CPbkNamesListAppView();

    private:  // Command handlers (use "Cmd" prefix)
        void CmdCreateNewL();
        void CmdDeleteMeL();
        void CmdEditMeL();
        void CmdAddToGroupL();
        void CmdDuplicateMeL();
        void CmdContextMenuL();
		void CmdLaunchSimPdL();
		void CmdLaunchSdnL();
		void CmdOpenSettingsL();
		void CmdGoToURLL();
		void CmdOpenOneTouchL();
        void CmdPhonebookInfoL();

    private:  // from CPbkContactListAppView
        CPbkContactViewListControl& Control();
        void DeleteFocusedEntryL();

    private:  // from CPbkAppView
        CPbkViewState* GetViewStateLC() const;

    private:  // from CAknView
        void HandleCommandL(TInt aCommand);
	    TUid Id() const;
	    void HandleStatusPaneSizeChange();
	    void DoActivateL(const TVwsViewId& aPrevViewId,
			TUid aCustomMessageId,const TDesC8& aCustomMessage);
	    void DoDeactivate();
	    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
	    void HandleForegroundEventL(TBool aForeground);

    private:  // from MPbkContactViewListControlObserver
        void HandleContactViewListControlEventL
            (CPbkContactViewListControl& aControl,
            const TPbkContactViewListControlEvent& aEvent);

    private:  // from MContactViewObserver
    	void HandleContactViewEvent
            (const CContactViewBase& aView,const TContactViewEvent& aEvent);
    
    private: // from MPbkMenuCommandObserver
        void PreCommandExecutionL();
        void PostCommandExecutionL(TPbkCommandStatus aStatus);

    private: // from MPbkCtrlDisabler
        void DisableController();

    private:  // Implementation
        CPbkNamesListAppView();
        void ConstructL();
        void DoEditL(TContactItemId aContactId);
        TBool DoDeleteContactQueryL(TContactItemId aContactId);
        void DoDeleteContactL(TContactItemId aContactId);
        TBool DoDeleteMarkedContactsQueryL
			(const CContactIdArray& aMarkedItems); 
        void DoDeleteMarkedContactsL(const CContactIdArray& aMarkedItems);
        void RestoreControlStateL();
        void HandleRemoveEvent(const TContactViewEvent& aEvent);
        void HandleRemoveFocusedEventL(TInt aIndex);
        void NamesListPaneMenuFilteringL(CEikMenuPane& aMenuPane,
			TUint aFlags);
        void PhonebookInfoPaneMenuFilteringL(CEikMenuPane& aMenuPane,
			TUint aFlags);
        void ContextMarkedItemsPaneMenuFilteringL(CEikMenuPane& aMenuPane,
			TUint aFlags);
		void SettingsPaneMenuFilteringL(CEikMenuPane& aMenuPane,
			TUint aFlags);
		void DoOpenContactEditorDlgL(CPbkContactViewListControl& aControl,
			CPbkContactItem& aNewContact, TBool aIsNewContact,
			TInt aFieldToFocus=-1, TBool aEdited=EFalse);
		void DoOpenSettingsDlgL();
        void DoShowPhonebookInfoL();
        TBool NoOpenOneTouch() const;
        void FFSCheckL();
        void RequestFreeDiskSpaceLC();
        void DbCompactL();

    private:  // data
        /// This view's control container type
        typedef CPbkControlContainer<CPbkContactViewListControl> CContainer;
        /// Own: this view's control container
        CContainer* iContainer;
        /// Own: contact to focus when it appears in control
        TContactItemId iContactToFocus;
        /// Own: control state passed as a parameter
        CPbkViewState* iParamState;
        /// Own: stored control state
        CPbkViewState* iControlState;
        /// Ref: all contacts view
        CContactViewBase* iAllContactsView;
        /// Own: for extension menu filtering and command handling
        MPbkViewExtension* iViewExtension;
        /// Own: for accessing extension factory
        CPbkExtGlobals* iExtGlobal;
        /// Own: Flash File System check helper
        CPbkFFSCheck* iPbkFFSCheck;
        /// Own: shared data client
        RSharedDataClient* iSharedDataClient;       
        /// Own: DB recovery UI
        CPbkDbRecoveryUi* iDbRecoveryUi;
    };


#endif // __CPbkNamesListAppView_H__
            

// End of File
