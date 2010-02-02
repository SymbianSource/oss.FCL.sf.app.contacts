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
*     Group members list View controller (CAknView).
*
*/


#ifndef __CPbkGroupMembersListAppView_H__
#define __CPbkGroupMembersListAppView_H__

//  INCLUDES
#include "CPbkContactListAppView.h" // Application views super class
#include <cntviewbase.h>			// MContactViewObserver
#include <MPbkContactViewListControlObserver.h>

//  FORWARD DECLARATIONS
template<class ControlType> class CPbkControlContainer;
class CPbkViewState;
class CAknNavigationDecorator;
class CContactGroupView;


//  CLASS DECLARATION

/**
 * Phonebook "Group members List View" application view class. 
 */
class CPbkGroupMembersListAppView :
        public CPbkContactListAppView, 
        private MContactViewObserver,
        private MPbkContactViewListControlObserver

    {
    public:  // Constructors and destructor
        /**
         * Creates and returns a new object of this class.
         */
        static CPbkGroupMembersListAppView* NewL();

        /**
         * Destructor.
         */
        ~CPbkGroupMembersListAppView();

    private:  // Command handlers (use "Cmd" prefix)
        void CmdBackL();
        void CmdOpenMeViewsL();
        void CmdRemoveFromFolderL();
        void CmdFetchMembersL();
        void CmdContextMenuL();

    public:  // from CPbkAppView
        CPbkViewState* GetViewStateLC() const;

    private:  // from CPbkContactListAppView
        CPbkContactViewListControl& Control();

    private:  // from CPbkAppView
        TBool HandleCommandKeyL(const TKeyEvent& aKeyEvent, TEventCode aType);

    private:  // from CAknView
        void HandleCommandL(TInt aCommand);
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
	    TUid Id() const;
	    void HandleStatusPaneSizeChange();
	    void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
			const TDesC8& aCustomMessage);
	    void DoDeactivate();

    private: // from CPbkContactListAppView
        void DeleteFocusedEntryL();

    private: // from MContactViewObserver
        void HandleContactViewEvent(const CContactViewBase& aView,
			const TContactViewEvent& aEvent);

    private:  // from MPbkContactViewListControlObserver
        void HandleContactViewListControlEventL
            (CPbkContactViewListControl& aControl,
            const TPbkContactViewListControlEvent& aEvent);

    private:  // functions
        CPbkGroupMembersListAppView();
        void ConstructL();
        void StatusPaneUpdateL(TContactItemId aGroupId);
        void LoadGroupMembersViewL(TContactItemId aGroupId);
        void ConstructNaviIndicatorsL();
        void OptionsMenuFilteringL(CEikMenuPane& aMenuPane, TUint aFlags);
        void SendContactMenuFilteringL(CEikMenuPane& aMenuPane, TUint aFlags);
        void ContextMenuFilteringL(CEikMenuPane& aMenuPane, TUint aFlags);
        void NavigateGroupL(TInt aDir);

        class TIsViewReady
            {
            public: // interface
				/**
				 * Constructor.
				 */
                TIsViewReady();

				/**
				 * Returns ETrue if views are ready.
				 */
                TBool AreViewsReady() const;

				/**
				 * Sets all groups views ready.
				 */
                void SetAllGroupsViewReady();

				/**
				 * Sets control ready.
				 */
                void SetControlReady();

				/**
				 * Flags enumeration.
				 */
                enum 
                    {
                    EReset = 0x0000,
                    EAllGroupsReady = 0x0001,
                    EControlReady = 0x0002
                    };
            private: // data members
				/// Own: the flags
                TUint iReadyViewFlags;
            };

    private:  // data
        /// This view's control container type
        typedef CPbkControlContainer<CPbkContactViewListControl> CContainer;
        /// Own: this view's control container
        CContainer* iContainer;
        /// Own: Group id that was being viewed
        TContactItemId iGroupId;
        /// Own: all groups view
        CContactViewBase* iAllGroupsView;
        /// Own: Group members view
        CContactGroupView* iGroupMembersView;
        /// Own: focused contact item
        TContactItemId iFocusedContact;
        /// Own: navigation decorator
        CAknNavigationDecorator* iNaviDeco;
        /// Own: View asyncronous loading ready
        TIsViewReady iViewsLoadingReady;
    };

#endif // __CPbkGroupMembersListAppView_H__

// End of File
