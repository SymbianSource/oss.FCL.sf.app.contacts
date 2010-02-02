/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 group members view.
*
*/


#include "CPguGroupMembersView.h"

// Phonebook 2
#include "CPguAddMembersCmd.h"
#include "CPguRemoveFromGroupCmd.h"
#include "Pbk2GroupUi.hrh"
#include <Pbk2GroupUIRes.rsg>
#include <CPbk2AppUiBase.h>
#include <Pbk2Commands.rsg>
#include <CPbk2UIExtensionView.h>
#include <CPbk2DocumentBase.h>
#include <CPbk2NamesListControl.h>
#include <CPbk2ViewState.h>
#include <CPbk2ControlContainer.h>
#include <MPbk2ViewActivationTransaction.h>
#include <MPbk2ViewExplorer.h>
#include <MPbk2CommandHandler.h>
#include <CPbk2SortOrderManager.h>
#include <MPbk2ContactNavigation.h>
#include <MPbk2ContactNavigationLoader.h>
#include <Pbk2NavigatorFactory.h>
#include <Pbk2Commands.hrh>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2ViewStateTransformer.h>
#include <csxhelp/phob.hlp.hrh>
#include <Pbk2UID.h>
#include <MPbk2StartupMonitor.h>
#include <MVPbkContactViewObserver.h>
#include <CPbk2ContactUiControlSubstitute.h>
#include <MPbk2StoreConfigurationObserver.h>
#include <MPbk2ControlObserver.h>
#include <MPbk2NavigationObserver.h>
#include <MPbk2MenuCommandObserver.h>
#include <Pbk2CommonUi.rsg>
#include <Pbk2UIControls.rsg>
#include <MPbk2PointerEventInspector.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ContactViewSupplier.h>
#include <CPbk2StoreConfiguration.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <MVPbkViewContact.h>
#include <MVPbkExpandable.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>

// System includes
#include <akntitle.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <eiklbx.h>
#include <eiklbo.h>
#include <touchfeedback.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KFirstItem = 0; // First item index

/**
 * Returns ETrue if shift is depressed in aKeyEvent.
 *
 * @param aKeyEvent     Key event.
 * @return  ETrue if shift is depressed.
 */
inline TBool ShiftDown( const TKeyEvent& aKeyEvent )
    {
    return ( ( aKeyEvent.iModifiers ) &
             ( EModifierShift|EModifierLeftShift|EModifierRightShift )
                ) != 0;
    }

/**
 * Returns ETrue if either Shift or Control key is depressed.
 *
 * @param aKeyEvent Key event.
 * @return  ETrue if shift or control is down.
 */
inline TBool IsShiftOrControlDown( const TKeyEvent& aKeyEvent )
    {
    return ( aKeyEvent.iModifiers &
        ( EModifierShift | EModifierLeftShift | EModifierRightShift |
          EModifierCtrl | EModifierLeftCtrl | EModifierRightCtrl ) ) != 0;
    }

/**
 * Phonebook 2 group members transaction.
 * Encapsulates the information that is needed
 * to switch from group to group viewing.
 */
class CGroupMembersViewTransaction : public CBase
    {
    public: // Construction and destruction

        /**
         * Creates new instance of this class.
         *
         * @return  A new instace of this class.
         */
        static CGroupMembersViewTransaction* NewLC();

        /**
         * Destructor
         */
        ~CGroupMembersViewTransaction();

    public: // Interface

        /**
         * Sets group members view.
         *
         * @param aGroupMembersView     Group members view to set.
         */
        void SetGroupMembersView(
                MVPbkContactViewBase* aGroupMembersView );

        /**
         * Set group's name.
         *
         * @param aGroupName        Group name to set.
         */
        void SetGroupName(
                HBufC* aGroupName );

        /**
         * Sets group link.
         *
         * @param aGroupLink Group link to set.
         */
        void SetGroupLink(
                MVPbkContactLink* aGroupLink );

        /**
         * Returns group link.
         *
         * @return  Group link.
         */
        MVPbkContactLink* GroupLink() const;

        /**
         * Returns group link, ownership is transferred.
         *
         * @return  Group link.
         */
        MVPbkContactLink* GetGroupLink();

        /**
         * Returns group name.
         *
         * @return  Group name.
         */
        HBufC* GetGroupName();

        /**
         * Returns group members view.
         *
         * @return  Group members view.
         */
        MVPbkContactViewBase* GroupMembersView() const;

        /**
         * Returns group members view, ownership is transferred.
         *
         * @return  Group members view.
         */
        MVPbkContactViewBase* GetGroupMembersView();

    private: // Implementation
        CGroupMembersViewTransaction();

    private: // Data
        /// Own: Group members view
        MVPbkContactViewBase* iGroupMembersView;
        /// Own: Group name
        HBufC* iGroupName;
        /// Own: Group link
        MVPbkContactLink* iGroupLink;
    };

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::CGroupMembersViewTransaction
// --------------------------------------------------------------------------
//
CGroupMembersViewTransaction::CGroupMembersViewTransaction()
    {
    }

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::~CGroupMembersViewTransaction
// --------------------------------------------------------------------------
//
CGroupMembersViewTransaction::~CGroupMembersViewTransaction()
    {
    delete iGroupMembersView;
    delete iGroupName;
    delete iGroupLink;
    }

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::NewLC
// --------------------------------------------------------------------------
//
CGroupMembersViewTransaction* CGroupMembersViewTransaction::NewLC()
    {
    CGroupMembersViewTransaction* self =
        new ( ELeave ) CGroupMembersViewTransaction;
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::SetGroupMembersView
// --------------------------------------------------------------------------
//
void CGroupMembersViewTransaction::SetGroupMembersView
        ( MVPbkContactViewBase* aGroupMembersView )
    {
    delete iGroupMembersView;
    iGroupMembersView = aGroupMembersView;
    }

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::SetGroupName
// --------------------------------------------------------------------------
//
void CGroupMembersViewTransaction::SetGroupName
        ( HBufC* aGroupName )
    {
    delete iGroupName;
    iGroupName = aGroupName;
    }

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::SetGroupLink
// --------------------------------------------------------------------------
//
void CGroupMembersViewTransaction::SetGroupLink
        ( MVPbkContactLink* aGroupLink )
    {
    delete iGroupLink;
    iGroupLink = aGroupLink;
    }

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::GroupLink
// --------------------------------------------------------------------------
//
MVPbkContactLink* CGroupMembersViewTransaction::GroupLink() const
    {
    return iGroupLink;
    }

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::GetGroupLink
// --------------------------------------------------------------------------
//
MVPbkContactLink* CGroupMembersViewTransaction::GetGroupLink()
    {
    MVPbkContactLink* groupLink = iGroupLink;
    iGroupLink = NULL;
    return groupLink;
    }

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::GetGroupName
// --------------------------------------------------------------------------
//
HBufC* CGroupMembersViewTransaction::GetGroupName()
    {
    HBufC* groupName = iGroupName;
    iGroupName = NULL;
    return groupName;
    }

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::GroupMembersView
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CGroupMembersViewTransaction::GroupMembersView() const
    {
    return iGroupMembersView;
    }

// --------------------------------------------------------------------------
// CGroupMembersViewTransaction::GetGroupMembersView
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CGroupMembersViewTransaction::GetGroupMembersView()
    {
    MVPbkContactViewBase* groupMembersView = iGroupMembersView;
    iGroupMembersView = NULL;
    return groupMembersView;
    }

} /// namespace

/**
 * Phonebook 2 group members view implementation base class.
 */
class CPguGroupMembersView::CGroupMembersViewImpl :
        public CBase,
        protected MVPbkContactViewObserver
    {
    public: // Interface

        /**
         * Creates control.
         *
         * @param aClientRect   Client's rect.
         */
        virtual void CreateControlL(
                const TRect& aClientRect ) = 0;

        /**
         * Returns the control.
         *
         * @return  Control.
         */
        virtual MPbk2ContactUiControl* Control() = 0;

        /**
         * Return view's state.
         *
         * @return  View state.
         */
        virtual CPbk2ViewState* ViewStateLC() const = 0;

        /**
         * Handles command key
         *
         * @param aKeyEvent     Key event.
         * @param aType         Command type.
         * @return  ETrue if command handled in function, else EFalse.
         */
        virtual TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType ) = 0;

        /**
         * Returns menu filtering flags.
         *
         * @return  The flags.
         */
        virtual TInt GetViewSpecificMenuFilteringFlagsL() const = 0;

        /**
         * Dynamically initialises a menu pane.
         *
         * @param aResourceId   Resource id of the menu.
         * @param aMenuPane     The in-memory representation of
         *                      the menu pane.
         */
        virtual void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane ) = 0;

        /**
         * Restores view's state.
         *
         * @param aState    The state to restore.
         */
        virtual void RestoreControlStateL(
                CPbk2ViewState* aState ) = 0;

        /**
         * Handles command,
         *
         * @param aCommand  Id of the command to respond to.
         */
        virtual void HandleCommandL(
                TInt aCommand ) = 0;

        /**
         * Handles a change in the position or size
         * of the screen area occupied by the status pane.
         */
        virtual void HandleStatusPaneSizeChange() = 0;


        /**
         * Handles listbox events in derived classes
         * Note that this is not derived from MEikListBoxObserver but defined here
         */
        virtual void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType ) = 0;


        /**
         * Handles listbox events in derived classes
         * Note that this is not derived from MEikListBoxObserver but defined here
         */
        virtual void ShowContextMenuL() = 0;



        /**
         * Handles pointer events.
         *
         * @param aPointerEvent     The pointer event.
         */
        virtual void HandlePointerEventL(
                const TPointerEvent& aPointerEvent) = 0;

        /**
         * Updates Cba buttons.
         */
        virtual void UpdateCbasL() = 0;

        /**
         * Handles long tap event.
         *
         * @param aPenEventLocation         Pen location
         * @param aPenEventScreenLocation   Pen screen location.
         */
        virtual void HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation ) = 0;

    public: // From MVPbkContactViewObserver
        void ContactViewUnavailable(
                MVPbkContactViewBase& aView );
        void ContactAddedToView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactRemovedFromView(
                MVPbkContactViewBase& aView,
                TInt aIndex,
                const MVPbkContactLink& aContactLink );
        void ContactViewError(
                MVPbkContactViewBase& aView,
                TInt aError,
                TBool aErrorNotified );

    public: // Base class interface

        /**
         * Destructor.
         */
        virtual ~CGroupMembersViewImpl()
            {}

    protected: // Implementation
        CGroupMembersViewImpl()
            {}
    };


/**
 * Group members view implementation class. This class encapsulates
 * the view implementation when the group members view isnt ready, so
 * all the nesessary information needed to create the group members view
 * isnt ready. This class iniates the all groups view loading and waits
 * for its completion. Up on completion it switches to the next state.
 */
class CPguGroupMembersView::CGroupMembersAllGroupsNotReady :
        public CPguGroupMembersView::CGroupMembersViewImpl
    {
    public: // interface
        /**
         * Creates a new instance of this class.
         *
         * @param aViewParent           Parent view.
         * @param aView                 UI extension view.
         * @param aFocusedGroupLink     Focused groups.
         * @return  A new instance of this class.
         */
        static CGroupMembersAllGroupsNotReady* NewL(
                MPbk2GroupMembersViewActivationCallback& aViewParent,
                CPbk2UIExtensionView& aView,
                const MVPbkContactLink* aFocusedGroupLink );

        /**
         * Destructor.
         */
        ~CGroupMembersAllGroupsNotReady();

    public: // From CGroupMembersViewImpl
        void CreateControlL(
                const TRect& aClientRect );
        MPbk2ContactUiControl* Control();
        CPbk2ViewState* ViewStateLC() const;
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        TInt GetViewSpecificMenuFilteringFlagsL() const;
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        void RestoreControlStateL(
                CPbk2ViewState* aState );
        void HandleCommandL(
                TInt aCommand );
        void HandleStatusPaneSizeChange();

        /**
         * Handles listbox events.
         * Note that this is not derived from MEikListBoxObserver but defined here
         */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

        void ShowContextMenuL();

        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );
        void UpdateCbasL();
        void HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation );

    public: // From MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );

    private: // Implementation
        CGroupMembersAllGroupsNotReady(
                MPbk2GroupMembersViewActivationCallback& aViewParent,
                CPbk2UIExtensionView& aView,
                const MVPbkContactLink* aFocusedGroupLink );
        void ConstructL();

    private: // Data
        /// Ref: Parent view
        MPbk2GroupMembersViewActivationCallback& iViewParent;
        /// Ref: UI extension view
        CPbk2UIExtensionView& iView;
        /// Ref: Focused group link
        const MVPbkContactLink* iFocusedGroupLink;
        /// RefO observed all groups view
        MVPbkContactViewBase* iAllGroupsView;
        /// This view's control container type
        typedef CPbk2ControlContainer<
            CPbk2ContactUiControlSubstitute> CContainer;
        /// Own: This view's control container
        CContainer* iContainer;
        /// Ref: Empty control
        CPbk2ContactUiControlSubstitute* iControl;
        
    };


/**
 * Group members view implementation class. This class encapsulates
 * the situation when all nesessary information to show the group
 * members data is present and it can be shown to the user. The navigation
 * from group to group and user commands are handled in this state.
 */
class CPguGroupMembersView::CGroupMembersAllGroupsReady :
        public CPguGroupMembersView::CGroupMembersViewImpl,
        private MPbk2StoreConfigurationObserver,
        private MPbk2ContactNavigationLoader,
        private MPbk2MenuCommandObserver,
        private MPbk2ControlObserver,
        private MPbk2NavigationObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aViewParent           Parent view.
         * @param aView                 UI extension view.
         * @param aContactManager       Contact manager.
         * @param aContactNameFormatter Name formatter
         * @param aStorePropertyArray   Store property array.
         * @param aCommandHandler       Command handler.
         * @param aSortOrderManager     Sort order manager.
         * @param aFocusedGroupIndex    Focused group index.
         * @return  A new instance of this class
         */
        static CGroupMembersAllGroupsReady* NewL(
                MPbk2GroupMembersViewActivationCallback& aViewParent,
                CPbk2UIExtensionView& aView,
                CVPbkContactManager& aContactManager,
                MPbk2ContactNameFormatter& aContactNameFormatter,
                CPbk2StorePropertyArray& aStorePropertyArray,
                MPbk2CommandHandler& aCommandHandler,
                CPbk2SortOrderManager& aSortOrderManager,
                const TInt aFocusedGroupIndex );

        /**
         * Destructor
         */
        ~CGroupMembersAllGroupsReady();

    public: // From CGroupMembersViewImpl
        void CreateControlL(
                const TRect& aClientRect );
        MPbk2ContactUiControl* Control();
        CPbk2ViewState* ViewStateLC() const;
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        TInt GetViewSpecificMenuFilteringFlagsL() const;
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        void RestoreControlStateL(
                CPbk2ViewState* aState );
        void HandleCommandL(
                TInt aCommand );
        void HandleStatusPaneSizeChange();
        /**
         * Handles listbox events.
         * Note that this is not derived from MEikListBoxObserver but defined here
         */
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
        void ShowContextMenuL();

        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );
        void UpdateCbasL();
        void HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation );

    private: // From MPbk2ControlObserver
        void HandleControlEventL(
                MPbk2ContactUiControl& aControl,
                const TPbk2ControlEvent& aEvent );

    private: // From MPbk2StoreConfigurationObserver
        void ConfigurationChanged();
        void ConfigurationChangedComplete();

    private: // From MPbk2ContactNavigationLoader
        void ChangeContactL(
                const MVPbkContactLink& aContactLink );
        MVPbkContactLink* ContactLinkLC() const;

    private: // From MPbk2MenuCommandObserver
        void PreCommandExecutionL(
                const MPbk2Command& aCommand );
        void PostCommandExecutionL(
                const MPbk2Command& aCommand );

    private: // From MVPbkContactViewObserver
        void ContactViewReady(
                MVPbkContactViewBase& aView );

    private: // From MPbk2NavigationObserver
        void HandleNavigationEvent(
                const TEventType& aEventType );

    private: // Implementation
        CGroupMembersAllGroupsReady(
                MPbk2GroupMembersViewActivationCallback& aViewParent,
                CPbk2UIExtensionView& aView,
                CVPbkContactManager& aContactManager,
                MPbk2ContactNameFormatter& aContactNameFormatter,
                CPbk2StorePropertyArray& aStorePropertyArray,
                MPbk2CommandHandler& aCommandHandler,
                CPbk2SortOrderManager& aSortOrderManager );
        void ConstructL(
                const TInt aFocusedGroupIndex );
        void DoCommitTransActionL(
                MVPbkContactViewBase& aView );
        static void ChangeTitlePaneL(
                HBufC* aTitleText );
        void UpdateListEmptyTextL(
                TInt aListState );

    private: // Data
        /// Ref: Parent view
        MPbk2GroupMembersViewActivationCallback& iViewParent;
        /// Ref: UI extension view
        CPbk2UIExtensionView& iView;
        /// Ref: Contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Contact name formatter
        MPbk2ContactNameFormatter& iContactNameFormatter;
        /// Ref: Store properties
        CPbk2StorePropertyArray& iStoreProperties;
        /// Ref: Command handler
        MPbk2CommandHandler& iCommandHandler;
        /// Ref: Sort order
        CPbk2SortOrderManager& iSortOrderManager;
        /// This view's control container type
        typedef CPbk2ControlContainer<CPbk2NamesListControl> CContainer;
        /// Own: This view's control container
        CContainer* iContainer;
        /// Ref: This view's control
        MPbk2ContactUiControl* iControl;
        /// Ref: Focus movement observer
        MPbk2PointerEventInspector* iPointerEventInspector;
        /// Own: Navigation strategy
        MPbk2ContactNavigation* iNavigation;
        /// Own: Group members view
        MVPbkContactViewBase* iGroupMembersView;
        /// Own: View transition transaction
        CGroupMembersViewTransaction* iViewTransitionTransaction;
        /// Own: List of stores used
        RPointerArray<MVPbkContactStore> iStoreList;
        /// Own: ETrue, if any selection modifier key is depressed
        TBool iSelectionModifierUsed;
        /// Own: Stylus popup menu state indicator
        TBool iStylusPopupMenuLaunched;
        // Ref: Store management ptr
        CPbk2StoreConfiguration* iStoreConf;
    };

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersViewImpl:::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersViewImpl::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    TRAP_IGNORE( UpdateCbasL() );
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersViewImpl:::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersViewImpl::ContactAddedToView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    TRAP_IGNORE( UpdateCbasL() );
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersViewImpl:::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersViewImpl::ContactRemovedFromView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    TRAP_IGNORE( UpdateCbasL() );
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersViewImpl:::ContactViewError
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersViewImpl::ContactViewError
        ( MVPbkContactViewBase& /*aView*/, TInt /*aError*/,
          TBool /*aErrorNotified*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::CGroupMembersAllGrou
// --------------------------------------------------------------------------
//
CPguGroupMembersView::CGroupMembersAllGroupsNotReady::
    CGroupMembersAllGroupsNotReady
            ( MPbk2GroupMembersViewActivationCallback& aViewParent,
              CPbk2UIExtensionView& aView,
              const MVPbkContactLink* aFocusedGroupLink ) :
                iViewParent(aViewParent),
                iView(aView),
                iFocusedGroupLink(aFocusedGroupLink)
    {
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::~CGroupMembersAllGro
// --------------------------------------------------------------------------
//
CPguGroupMembersView::CGroupMembersAllGroupsNotReady::
        ~CGroupMembersAllGroupsNotReady()
    {
    iAllGroupsView->RemoveObserver( *this );

    // Remove control from stack
    if (iContainer)
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::ConstructL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::ConstructL()
    {
    // The all groups view isnt ready, observe it then expand
    Phonebook2::Pbk2AppUi()->ApplicationServices().
        ViewSupplier().AllGroupsViewL()->AddObserverL( *this );

    iViewParent.SetFocusedContact( iFocusedGroupLink->CloneLC() );
    CleanupStack::Pop();
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::NewL
// --------------------------------------------------------------------------
//
CPguGroupMembersView::CGroupMembersAllGroupsNotReady*
    CPguGroupMembersView::CGroupMembersAllGroupsNotReady::NewL
        ( MPbk2GroupMembersViewActivationCallback& aViewParent,
          CPbk2UIExtensionView& aView,
          const MVPbkContactLink* aFocusedGroupLink )
    {
    CGroupMembersAllGroupsNotReady* self =
        new ( ELeave ) CGroupMembersAllGroupsNotReady
            ( aViewParent, aView, aFocusedGroupLink );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::CreateControlL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::CreateControlL
        ( const TRect& aClientRect )
    {
    // Create the container and control
    CContainer* container = CContainer::NewLC( &iView, &iView, iView );
    container->SetHelpContext(
            TCoeHelpContext(iView.ApplicationUid(),
            KPHOB_HLP_GROUP_MEMBER_LIST));

    // create empty control
    CPbk2ContactUiControlSubstitute* control =
            CPbk2ContactUiControlSubstitute::NewL();

    // Takes ownership
    container->SetControl(control, aClientRect);

    control->SetContainerWindowL(*container);
    HBufC* emptyText = StringLoader::LoadLC( R_QTN_PHOB_NO_ENTRIES_IN_GRP );
    control->SetTextL( *emptyText );
    CleanupStack::PopAndDestroy( emptyText );

    CCoeEnv::Static()->AppUi()->AddToStackL(iView, container);
    CleanupStack::Pop(container);
    iContainer = container;
    iControl = control;

    iContainer->ActivateL();
    iContainer->Control()->MakeVisible(ETrue);

    // set CBA
    iView.Cba()->SetCommandSetL(R_AVKON_SOFTKEYS_BACK);
    iView.Cba()->DrawDeferred();
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::Control
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPguGroupMembersView::
        CGroupMembersAllGroupsNotReady::Control()
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::ContactViewReady
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::ContactViewReady
        ( MVPbkContactViewBase& aView )
    {
    // If we have a new view, lets change it
    if (&aView == iAllGroupsView)
        {
        TInt groupIndex = KErrNotFound;
        TRAPD(leaveCode,
            {
            groupIndex = iAllGroupsView->IndexOfLinkL(*iFocusedGroupLink);
            // switch to ready state
            iViewParent.TransformViewActivationStateToReadyL(groupIndex);
            });
        if (leaveCode != KErrNone)
            {
            // notify user
            CCoeEnv::Static()->HandleError(leaveCode);
            }
        }
    else
        {
        // When ContactViewReady() is called, the program needs to check whether 
        // the namelist control in the container is visible or not. 
        // If visible, call DrawDeferred() to refresh the container, 
        // and if not, the container needs not to be refreshed to avoid flicker on the background.
        if ( iContainer->Control() && iContainer->Control()->IsVisible() )
            {
            iContainer->DrawDeferred();
            }
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::ViewStateLC
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPguGroupMembersView::
        CGroupMembersAllGroupsNotReady::ViewStateLC() const
    {
    CPbk2ViewState* state = CPbk2ViewState::NewLC();
    MVPbkContactLink* parentContactLink =
        iViewParent.FocusedContact()->CloneLC();
    state->SetParentContact( parentContactLink );
    CleanupStack::Pop(); // parentContactLink
    return state;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandleCommandKeyL
        ( const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
    {
    // No handling in this state
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandleStatusPaneSize
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::
        HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if(iContainer)
        {
        iContainer->SetRect(iView.ClientRect());
        }
    }


// ----------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandleListBoxEventL(CEikListBox* /*aListBox*/ ,TListBoxEvent /*aEventType*/ )
    {
    }



// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandlePointerEventL
        ( const TPointerEvent& /*aPointerEvent*/ )
    {
    }


// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::ShowContextMenuL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::ShowContextMenuL()
    {
    }
   



// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::UpdateCbasL()
    {
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandleLongTapEventL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandleLongTapEventL
        ( const TPoint& /*aPenEventLocation*/,
          const TPoint& /*aPenEventScreenLocation*/ )
    {
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::GetViewSpecificMenu
// --------------------------------------------------------------------------
//
TInt CPguGroupMembersView::CGroupMembersAllGroupsNotReady::
        GetViewSpecificMenuFilteringFlagsL() const
    {
    return iControl->GetMenuFilteringFlagsL();
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::DynInitMenuPaneL
        ( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::
        RestoreControlStateL( CPbk2ViewState* /*aState*/ )
    {
    // Nothing to restore
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandleCommandL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsNotReady::HandleCommandL
        ( TInt /*aCommand*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::CGroupMembersAllGroups
// --------------------------------------------------------------------------
//
CPguGroupMembersView::CGroupMembersAllGroupsReady::CGroupMembersAllGroupsReady
        ( MPbk2GroupMembersViewActivationCallback& aViewParent,
          CPbk2UIExtensionView& aView,
          CVPbkContactManager& aContactManager,
          MPbk2ContactNameFormatter& aContactNameFormatter,
          CPbk2StorePropertyArray& aStorePropertyArray,
          MPbk2CommandHandler& aCommandHandler,
          CPbk2SortOrderManager& aSortOrderManager ) :
            iViewParent(aViewParent),
            iView(aView),
            iContactManager(aContactManager),
            iContactNameFormatter(aContactNameFormatter),
            iStoreProperties(aStorePropertyArray),
            iCommandHandler(aCommandHandler),
            iSortOrderManager(aSortOrderManager)
    {
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::~CGroupMembersAllGroups
// --------------------------------------------------------------------------
//
CPguGroupMembersView::CGroupMembersAllGroupsReady::
        ~CGroupMembersAllGroupsReady()
    {    
    iCommandHandler.RemoveMenuCommandObserver( *this );

    if ( iStoreConf )
        {
        iStoreConf->RemoveObserver( *this );
        }

    delete iViewTransitionTransaction;

    // remove control from stack
    if (iContainer)
        {
        iContainer->Control()->HideThumbnail();
        iContainer->Control()->MakeVisible(EFalse);
        CCoeEnv::Static()->AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        }

    // Stop observing group members view
    if (iGroupMembersView)
        {
        iGroupMembersView->RemoveObserver( *this );
        }
    delete iGroupMembersView;
    delete iNavigation;
    iStoreList.Reset();
    iStoreList.Close();
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::ConstructL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::ConstructL
        ( const TInt aFocusedGroupIndex )
    {
    // Add this view to observe command events
    iCommandHandler.AddMenuCommandObserver(*this);

    iStoreConf = &( Phonebook2::Pbk2AppUi()->ApplicationServices().
    	StoreConfiguration() );
    iStoreConf->AddObserverL( *this );

    // Expand the group members view from the all groups view
    const MVPbkViewContact& group =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            ViewSupplier().AllGroupsViewL()->ContactAtL
                ( aFocusedGroupIndex );
    iGroupMembersView = group.Expandable()->ExpandLC(*this,
                iSortOrderManager.SortOrder());
    CleanupStack::Pop(); // ExpandLC

    // create a store list that contains all the stores
    MVPbkContactStoreList& storeList = iContactManager.ContactStoresL();
    for (TInt i = 0; i < storeList.Count(); ++i)
        {
        MVPbkContactStore& store = storeList.At(i);
        iStoreList.Append(&store);
        }

    // Create a navigation object which can navigate all the available groups
    iNavigation = Pbk2NavigatorFactory::CreateContactNavigatorL
        ( iView.Id(), *this, *this, iStoreList.Array(),
          R_QTN_PHOB_NAVI_POS_INDICATOR_GROUP_MEMBERS_VIEW );

    // Navigation view remains the same
    iNavigation->SetScrollViewL
        ( Phonebook2::Pbk2AppUi()->ApplicationServices().
          ViewSupplier().AllGroupsViewL(), EFalse );

    // Set the group title to the title pane
    HBufC* titleText = iContactNameFormatter.GetContactTitleL(group.Fields(),
            MPbk2ContactNameFormatter::EPreserveLeadingSpaces );
    CleanupStack::PushL( titleText );
    ChangeTitlePaneL(titleText);
    CleanupStack::Pop( titleText );
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::NewL
// --------------------------------------------------------------------------
//
CPguGroupMembersView::CGroupMembersAllGroupsReady*
    CPguGroupMembersView::CGroupMembersAllGroupsReady::NewL
            ( MPbk2GroupMembersViewActivationCallback& aViewParent,
              CPbk2UIExtensionView& aView,
              CVPbkContactManager& aContactManager,
              MPbk2ContactNameFormatter& aContactNameFormatter,
              CPbk2StorePropertyArray& aStorePropertyArray,
              MPbk2CommandHandler& aCommandHandler,
              CPbk2SortOrderManager& aSortOrderManager,
              const TInt aFocusedGroupIndex )
    {
    CGroupMembersAllGroupsReady* self =
        new ( ELeave ) CGroupMembersAllGroupsReady
            ( aViewParent, aView, aContactManager,
              aContactNameFormatter, aStorePropertyArray, aCommandHandler,
              aSortOrderManager );
    CleanupStack::PushL (self );
    self->ConstructL( aFocusedGroupIndex );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::CreateControlL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::CreateControlL
        ( const TRect& aClientRect )
    {
    // Create the container and control
    CContainer* container = CContainer::NewLC( &iView, &iView, iView );
    container->SetHelpContext(
            TCoeHelpContext(iView.ApplicationUid(),
            KPHOB_HLP_GROUP_MEMBER_LIST));

    CPbk2NamesListControl* control = CPbk2NamesListControl::NewL(
            R_PBK2_GROUPMEMBERS_CONTROL,
            container, iContactManager, *iGroupMembersView,
            iContactNameFormatter, iStoreProperties);

    // Takes ownership
    container->SetControl(control, aClientRect);
    control->SetTextL( KNullDesC );
    control->AddObserverL(*this);

    iNavigation->UpdateNaviIndicatorsL(*iViewParent.FocusedContact());
    //Set the current group link to Nameslist control
    control->SetCurrentGroupLinkL(iViewParent.FocusedContact());

    CCoeEnv::Static()->AppUi()->AddToStackL(iView, container);
    CleanupStack::Pop(container);
    iContainer = container;
    iControl = control;
    iPointerEventInspector = control;

    iContainer->ActivateL();
    iContainer->Control()->MakeVisible( ETrue );
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::Control
// --------------------------------------------------------------------------
//
MPbk2ContactUiControl* CPguGroupMembersView::CGroupMembersAllGroupsReady::
        Control()
    {
    return iControl;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleControlEventL
        ( MPbk2ContactUiControl& /*aControl*/,
          const TPbk2ControlEvent& aEvent )
    {
    switch(aEvent.iEventType)
        {
        case TPbk2ControlEvent::EReady:
            {
            UpdateCbasL();

            // Restore control state
            iViewParent.RestoreStateL();
            if (iControl)
                {
                iControl->ShowThumbnail();
                }
            break;
            }
        case TPbk2ControlEvent::EControlLeavesSelectionMode: // FALLTHROUGH
        case TPbk2ControlEvent::EItemRemoved:                // FALLTHROUGH
        case TPbk2ControlEvent::EContactSelected:            // FALLTHROUGH
        case TPbk2ControlEvent::EContactUnselected:          // FALLTHROUGH
        case TPbk2ControlEvent::EContactUnselectedAll:       
            {
            UpdateCbasL();
            break;
            }
        case TPbk2ControlEvent::EControlStateChanged:
            {
            UpdateCbasL();
            UpdateListEmptyTextL( aEvent.iInt );
            iContainer->Control()->DrawDeferred();
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::ConfigurationChanged
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::ConfigurationChanged()
    {
    // Update navigations scroll view
    TRAPD( error,
        iNavigation->SetScrollViewL
            ( Phonebook2::Pbk2AppUi()->ApplicationServices().
              ViewSupplier().AllGroupsViewL(), EFalse ) );
    if ( error != KErrNone )
        {
        CCoeEnv::Static()->HandleError( error );
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::ConfigurationChangedCo
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::
        ConfigurationChangedComplete()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::ChangeContactL
// Called through MPbk2ContactNavigationCallback to change the
// group that is viewed.
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::ChangeContactL
        ( const MVPbkContactLink& aContactLink )
    {
    if (iViewTransitionTransaction)
        {
        // remove observer from view
        iViewTransitionTransaction->GroupMembersView()->RemoveObserver( *this );

        // destroy tranaction
        delete iViewTransitionTransaction;
        iViewTransitionTransaction = NULL;
        }

    CGroupMembersViewTransaction* transaction =
        CGroupMembersViewTransaction::NewLC();
    // create a copy of the group link
    MVPbkContactLink* groupLink = aContactLink.CloneLC();

    // Get the index of the group to view from the all groups view
    const TInt groupIndex = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ViewSupplier().AllGroupsViewL()->IndexOfLinkL( *groupLink );
    if (groupIndex != KErrNotFound)
        {
        // Get the group at position from the all groups view
        const MVPbkViewContact& group = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ViewSupplier().AllGroupsViewL()->ContactAtL( groupIndex );

        // retrieve group name, which is later shown on the title pane
        HBufC* groupName = iContactNameFormatter.GetContactTitleL(group.Fields(),
                MPbk2ContactNameFormatter::EPreserveLeadingSpaces);
        CleanupStack::PushL(groupName);

        // initiate the next group members view loading
        MVPbkContactViewBase* groupMembersView = group.Expandable()->ExpandLC(*this,
                iSortOrderManager.SortOrder());

        // the group members view loading has initiated, transfer ownership of these
        // items to the transaction. Transaction will be committed when the view
        // reaches the ready state
        transaction->SetGroupMembersView(groupMembersView);
        CleanupStack::Pop(); // ExpandLC
        transaction->SetGroupName(groupName);
        CleanupStack::Pop(groupName);
        transaction->SetGroupLink(groupLink);
        CleanupStack::Pop(); // CloneLC

        // now assign the transaction and wait for the view to become ready
        iViewTransitionTransaction = transaction;
        CleanupStack::Pop(transaction);
        }
    else
        {
        // the all groups view isnt ready, so contact change isnt possible
        // cleanup
        CleanupStack::PopAndDestroy(2, transaction); // groupLink, transaction
        User::Leave(KErrNotReady);
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::ViewStateLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CPguGroupMembersView::CGroupMembersAllGroupsReady::
        ContactLinkLC() const
    {
    return iViewParent.FocusedContact()->CloneLC();
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::ViewStateLC
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPguGroupMembersView::CGroupMembersAllGroupsReady::
        ViewStateLC() const
    {
    CPbk2ViewState* state = iControl->ControlStateL();
    CleanupStack::PushL( state );
    MVPbkContactLink* parentContactLink =
        iViewParent.FocusedContact()->CloneLC();
    state->SetParentContact( parentContactLink );
    CleanupStack::Pop(); // parentContactLink
    return state;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleCommandKeyL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool itemSpecEnabled = 
        Phonebook2::Pbk2AppUi()->ActiveView()->MenuBar()->ItemSpecificCommandsEnabled();
    TBool result = iNavigation->HandleCommandKeyL( aKeyEvent, aType );

    if(!result && aType == EEventKey)
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyBackspace:
                {
                if ( (itemSpecEnabled && (iControl->NumberOfContacts() > 0) )
                        || iControl->ContactsMarked() )
                    {
                    if ( iControl->FindTextL() == KNullDesC )
                        {
                        HandleCommandL( EPbk2CmdRemoveFromGroup );
                        result = ETrue;
                        }
                    }
                break;
                }
            case EKeyEnter: // FALLTHROUGH
            case EKeyOK:
                {
                if ( !ShiftDown(aKeyEvent) ) // pure OK or ENTER key
					{	
                    if ( iControl->ContactsMarked() && itemSpecEnabled )
                    	{
                    	iView.LaunchPopupMenuL(
                    		R_PHONEBOOK2_GROUPMEMBERS_CONTEXT_MENUBAR);
						result = ETrue;
						}
					else if ( iControl->NumberOfContacts() == 0 )
						{
						result = ETrue;
						}
                    }
                break;
                }

            case EKeyPhoneSend:
                {
                // Use the EPbk2CmdCall id to identify that
                // call is being launched with send key
                if ( itemSpecEnabled && (iControl->NumberOfContacts() > 0) &&
                     !iControl->ContactsMarked() )
                    {
                    // If there is a dialog display on the top of screen, don't make call.
                    if ( !(CEikonEnv::Static()->EikAppUi()->IsDisplayingDialog()) )
                        {
                        // Use the EPbk2CmdCall id to identify that
                        // call is being launched with send key
                        HandleCommandL( EPbk2CmdCall );
                        }
                    result = ETrue;
                    }
                break;
                }

            case EKeyPoC:
                {
                // Use the EPbk2CmdPoC id to identify that
                // call is being launched with PoC key
                HandleCommandL( EPbk2CmdPoC );
                result = ETrue;
                break;
                }

            default:
                {
                break;
                }
            }
        }

    // Update selection modifier key state
    iSelectionModifierUsed = IsShiftOrControlDown( aKeyEvent ) ||
        ( (aType == EEventKeyDown) && (aKeyEvent.iCode == EStdKeyHash) );

    return result;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::GetViewSpecificMenuFi
// --------------------------------------------------------------------------
//
TInt CPguGroupMembersView::CGroupMembersAllGroupsReady::
        GetViewSpecificMenuFilteringFlagsL() const
    {
    return iControl->GetMenuFilteringFlagsL();
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    // Ask the control do control specific filtering
    // (for example call HandleMarkableListDynInitMenuPane if needed)
    iControl->DynInitMenuPaneL(aResourceId, aMenuPane);
    
    switch ( aResourceId )
        {
        case R_PHONEBOOK2_OPEN_ME_MENU:
            {
            // Weed out commands not meant to be used with empty list
            // Weed out commands not meant to be used with marked items
            if ( iControl->NumberOfContacts() == 0 ||
                    iControl->ContactsMarked() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdOpenCca, ETrue );
                }
            break;
            }

        case R_PHONEBOOK2_GROUPMEMBERS_MENU:
            {
            // Weed out commands not meant to be used with empty list
            if ( iControl->NumberOfContacts() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdRemoveFromGroup, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdBelongsToGroups, ETrue );
                }

            if ( iControl->ContactsMarked() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdBelongsToGroups, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdAddMembers, ETrue );
                aMenuPane->SetItemSpecific( EPbk2CmdRemoveFromGroup, EFalse );
                }
            // Weed out commands not meant to be if names list is empty
            if ( Phonebook2::Pbk2AppUi()->ApplicationServices().
                    ViewSupplier().AllContactsViewL()->ContactCountL() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdAddMembers, ETrue );
                }
            break;
            }

        case R_PHONEBOOK2_BCARD_SEND_MENU:
            {
            // Weed out commands not meant to be used with empty list
            if ( iControl->NumberOfContacts() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdSend, ETrue );
                }
            if ( iControl->ContactsMarked() )
                {
                aMenuPane->SetItemSpecific( EPbk2CmdSend, EFalse );
                }
            break;
            }

        case R_PHONEBOOK2_GROUPMEMBERS_CONTEXT_MENU:
            {
            // Weed out commands not meant to be used with empty list
            if ( iControl->NumberOfContacts() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdRemoveFromGroup, ETrue );
                }

            // Weed out commands not meant to be used with marked items
            if ( iControl->ContactsMarked() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdAddMembers, ETrue );
                }

            // Weed out commands not meant to be if names list is empty
            if ( Phonebook2::Pbk2AppUi()->ApplicationServices().
                    ViewSupplier().AllContactsViewL()->ContactCountL() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdAddMembers, ETrue );
                }
            break;
            }

        default:
            {
            
            // Phonebook 2 menu filtering happens in Commands
            iCommandHandler.DynInitMenuPaneL(aResourceId,
                    aMenuPane, iView, *iControl );
            }
      
        };
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::ContactViewReady
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::ContactViewReady
        ( MVPbkContactViewBase& aView )
    {
    // If we have a new view, lets change it
    if ((&aView != iGroupMembersView) &&
        (&aView == iViewTransitionTransaction->GroupMembersView()))
        {
        TRAP_IGNORE(DoCommitTransActionL(aView));
        }
    else
        {
        // When ContactViewReady() is called, the program needs to check whether 
        // the namelist control in the container is visible or not. 
        // If visible, call DrawDeferred() to refresh the container, 
        // and if not, the container needs not to be refreshed to avoid flicker on the background.
        if ( iContainer->Control() && iContainer->Control()->IsVisible() )
            {
            iContainer->DrawDeferred();
            }
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleNavigationEvent
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleNavigationEvent
        ( const TEventType& aEventType )
    {
    if (aEventType == MPbk2NavigationObserver::EContactDeleted)
        {
        // Return to the previous view
        TRAPD( err,
            {
            CPbk2ViewState* state = ViewStateLC();
            if ( iViewParent.FocusedContact() )
                {
                // change focused contact to current focused contact
                state->SetFocusedContact(
                    iViewParent.FocusedContact()->CloneLC() );
                CleanupStack::Pop(); // currentGroup
                }
            Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->
                ActivatePreviousViewL( state );
            CleanupStack::PopAndDestroy(state);
            } ); // TRAPD

        if ( err != KErrNone )
            {
            CCoeEnv::Static()->HandleError( err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::DoCommitTransActionL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::DoCommitTransActionL
        ( MVPbkContactViewBase& aView )
    {
    MVPbkContactLink* groupLink = iViewTransitionTransaction->GroupLink();
    HBufC* titleText = iViewTransitionTransaction->GetGroupName();
    CleanupStack::PushL( titleText );

    // Update navi indicators in the navi pane
    iNavigation->UpdateNaviIndicatorsL( *groupLink );

    // Change title pane text
    ChangeTitlePaneL(titleText);
    CleanupStack::Pop(titleText);

    // Reset old view before changing
    iContainer->Control()->Reset();

    // Set control's active view next
    iContainer->Control()->SetViewL( aView );

    // Trash old view
    if (iGroupMembersView)
        {
        iGroupMembersView->RemoveObserver( *this );
        delete iGroupMembersView;
        iGroupMembersView = NULL;
        }

    // Set correct focused index (First item)
    iContainer->Control()->SetFocusedContactIndexL(KFirstItem);

    //Set the current group link to Nameslist control
   iContainer->Control()->SetCurrentGroupLinkL(groupLink);
    // The new base view is already being observed by this class
    // (observer set in ExpandLC call in ChangeContactL method)

    // commit the transaction
    MVPbkContactViewBase* groupMembersView =
        iViewTransitionTransaction->GetGroupMembersView();
    groupLink = iViewTransitionTransaction->GetGroupLink();
    iGroupMembersView = groupMembersView;
    iViewParent.SetFocusedContact(groupLink);

    // delete the transaction
    delete iViewTransitionTransaction;
    iViewTransitionTransaction = NULL;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::ChangeTitlePaneL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::ChangeTitlePaneL
        ( HBufC* aTitleText )
    {
    CEikStatusPane* statusPane =
        CEikonEnv::Static()->AppUiFactory()->StatusPane();

    if (statusPane && statusPane->PaneCapabilities(
            TUid::Uid(EEikStatusPaneUidTitle)).IsPresent())
        {
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>(statusPane->
                            ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
        titlePane->SetText(aTitleText); // Takes ownership of aTitle
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::UpdateListEmptyTextL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::UpdateListEmptyTextL
        ( TInt aListState )
    {
    HBufC* text = NULL;

    switch ( aListState )
        {
        case CPbk2NamesListControl::EStateEmpty:
            {
            text = StringLoader::LoadLC( R_QTN_PHOB_NO_ENTRIES_IN_GRP );
            break;
            }
        case CPbk2NamesListControl::EStateFiltered:
            {
            text = StringLoader::LoadLC( R_PBK2_FIND_NO_MATCHES );
            break;
            }
        default:
            {
            text = KNullDesC().AllocLC();
            break;
            }
        }

    if ( text )
        {
        iControl->SetTextL( *text );
        CleanupStack::PopAndDestroy(); // text
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::UpdateCbasL()
    {
    if ( iControl )
        {
        if ( iControl->NumberOfContacts() > 0
            && !iControl->ContactsMarked() )
            {
            // Set middle softkey as Open.
            iView.Cba()->SetCommandSetL(R_PGU_SOFTKEYS_OPTIONS_BACK_OPEN);
            iView.Cba()->DrawDeferred();
            }
        else
            {
            // If names list is empty, MSK should be empty
            if ( Phonebook2::Pbk2AppUi()->ApplicationServices().
                    ViewSupplier().AllContactsViewL()->ContactCountL() == 0 )
                {
                // Set middle softkey as Context menu.
                iView.Cba()->SetCommandSetL(
                    R_PBK2_SOFTKEYS_OPTIONS_BACK_EMPTY);
                }
            else
                {
                // Set middle softkey as Context menu.
                iView.Cba()->SetCommandSetL(
                    R_PBK2_SOFTKEYS_OPTIONS_BACK_CONTEXT);
                }
            iView.Cba()->DrawDeferred();
            // Change context menu when marked items
            iView.MenuBar()->SetContextMenuTitleResourceId
                ( R_PHONEBOOK2_GROUPMEMBERS_CONTEXT_MENUBAR );
            }
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::RestoreControlStateL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::RestoreControlStateL
        ( CPbk2ViewState* aState )
    {
    if ( iControl )
        {
        iControl->RestoreControlStateL(aState);
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleCommandL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleCommandL
        ( TInt aCommand )
    { 
        switch(aCommand)
            {
            case EPbk2CmdRemoveFromGroup:
                {
                if ( iControl && ( iControl->FocusedContactIndex() != KErrNotFound ) )
                    {
                    CPguRemoveFromGroupCmd* cmd =
                            CPguRemoveFromGroupCmd::NewLC(
                                *iViewParent.FocusedContact(),
                                *iControl);
                    iCommandHandler.AddAndExecuteCommandL(cmd);
                    
                    CleanupStack::Pop(cmd);  // command handler takes the ownership
                    }
                break;
                }
    
            case EPbk2CmdAddMembers:
                {
                CPguAddMembersCmd* cmd =
                        CPguAddMembersCmd::NewLC(*iViewParent.FocusedContact(),
                            *iControl);
                iCommandHandler.AddAndExecuteCommandL(cmd);
                CleanupStack::Pop(cmd); // command handler takes the ownership
                break;
                }
    
            default:
                {
                // Do nothing
                break;
                }
            }
        
    if ( iControl && !iCommandHandler.HandleCommandL( aCommand, *iControl,
        &iView ) )
            {
            iControl->ProcessCommandL( aCommand );
            Phonebook2::Pbk2AppUi()->HandleCommandL( aCommand );
            }
        
    UpdateCbasL();
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleStatusPaneSizeCh
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::
    HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if( iContainer )
        {
        iContainer->SetRect(iView.ClientRect());
        }
    }


// ----------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleListBoxEventL(CEikListBox* /*aListBox*/,TListBoxEvent aEventType)
    { 

    if( AknLayoutUtils::PenEnabled() )  
        {
        switch ( aEventType )
            {
            case EEventItemDoubleClicked:
            case EEventItemSingleClicked:
                {

                ShowContextMenuL();
                break;
                }
            case EEventItemClicked: //Happens after focus changed
            break;
            case EEventEnterKeyPressed:
                {
                if (iControl->NumberOfContacts() > 0 &&
                    !iControl->ContactsMarked())
                    {
                    // Select key is mapped to "Open Contact" command
                    HandleCommandL( EPbk2CmdOpenCca );
                    }
                else
                    {
                    iView.LaunchPopupMenuL(
                            R_PHONEBOOK2_GROUPMEMBERS_CONTEXT_MENUBAR);
                    }
                break;
                }
            default:
               break;
            }
        }


    }




// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::HandlePointerEventL
        ( const TPointerEvent& aPointerEvent )
    {


    if ( AknLayoutUtils::PenEnabled() )
        {
        if ( iPointerEventInspector->FocusableItemPointed() && iContainer && !(iControl->ContactsMarked()))
            {
            iContainer->LongTapDetectorL().PointerEventL( aPointerEvent );
            }
            


        if ( iControl->NumberOfContacts()==0) 
            {       
            if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                {
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if ( feedback )
                    {
                    feedback->InstantFeedback( ETouchFeedbackBasic );
                    }
                }
            else if  ( aPointerEvent.iType == TPointerEvent::EButton1Up )       
                {
                ShowContextMenuL();
                }
            }

        }

    }
    
    
// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::ShowContextMenuL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::ShowContextMenuL()
    {
    if ( iStylusPopupMenuLaunched )
        {
        // Absorb EButton1Up event if we already launched the
        // stylus popup menu
        iStylusPopupMenuLaunched = EFalse;
        }
    else if ( !iPointerEventInspector->SearchFieldPointed() &&
              !iSelectionModifierUsed )
        {
        if ( iControl->NumberOfContacts() > 0 && 
                iPointerEventInspector->FocusedItemPointed() )
            {
            if ( iControl->ContactsMarked() )
                {
                iView.LaunchPopupMenuL(
                    R_PHONEBOOK2_GROUPMEMBERS_CONTEXT_MENUBAR );
                }
            else
                {
                // Open contact
                HandleCommandL(EPbk2CmdOpenCca);
                }
            }
        }
    }
   
    
    

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleLongTapEventL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::HandleLongTapEventL
        ( const TPoint& /*aPenEventLocation*/,
          const TPoint& aPenEventScreenLocation )
    {
    if( iControl->ContactsMarked() )
        {
        //if there are marked contacts, context menu should not open
        return;
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::PreCommandExecutionL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::PreCommandExecutionL
        ( const MPbk2Command& /*aCommand*/ )
    {    
    if (iContainer && iContainer->Control())
        {
        iContainer->Control()->HideThumbnail();
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CGroupMembersAllGroupsReady::PostCommandExecutionL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::CGroupMembersAllGroupsReady::PostCommandExecutionL
        ( const MPbk2Command& /*aCommand*/ )
    {    
    if ( iContainer && iContainer->Control() )
        {
        iContainer->Control()->ShowThumbnail();
        }

    UpdateCbasL();
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::CPguGroupMembersView
// --------------------------------------------------------------------------
//
CPguGroupMembersView::CPguGroupMembersView( CPbk2UIExtensionView& aView ) :
            iView( aView )
    {
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::~CPguGroupMembersView
// --------------------------------------------------------------------------
//
CPguGroupMembersView::~CPguGroupMembersView()
    {
    delete iFocusedGroup;
    delete iControlState;
    delete iViewImpl;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::NewL
// --------------------------------------------------------------------------
//
CPguGroupMembersView* CPguGroupMembersView::NewL
        ( CPbk2UIExtensionView& aView )
    {
    CPguGroupMembersView* self =
        new(ELeave) CPguGroupMembersView( aView );
    return self;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::HandleStatusPaneSizeChange
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::HandleStatusPaneSizeChange()
    {
    if ( iViewImpl )
        {
        iViewImpl->HandleStatusPaneSizeChange();
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::ViewStateLC
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPguGroupMembersView::ViewStateLC() const
    {
    CPbk2ViewState* ret = NULL;
    if ( iViewImpl )
        {
        ret = iViewImpl->ViewStateLC();
        }
    else
        {
        CleanupStack::PushL( ret ); // LC function semantics
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPguGroupMembersView::HandleCommandKeyL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    // Pass the key event to the strategy
    return iViewImpl->HandleCommandKeyL(aKeyEvent, aType);
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::GetViewSpecificMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPguGroupMembersView::GetViewSpecificMenuFilteringFlagsL() const
    {
    return iViewImpl->GetViewSpecificMenuFilteringFlagsL();
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::DoActivateL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::DoActivateL
        ( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
          const TDesC8& aCustomMessage )
    {
    UpdateViewStateL(aCustomMessageId, aCustomMessage);

    MPbk2ApplicationServices& appServices =
        Phonebook2::Pbk2AppUi()->ApplicationServices();

    // Focus setting cannot be done here as the group view might not be ready
    // when using view launch

    MVPbkContactViewBase* allGroupsView =
        appServices.ViewSupplier().AllGroupsViewL();

    // get the focused group from the view parameters
    const MVPbkContactLink* focusedGroupLink = iControlState->FocusedContact();
    // find the group's index from the group view
    TInt groupIndex = allGroupsView->IndexOfLinkL(*focusedGroupLink);
    if (groupIndex == KErrNotFound)
        {
        // if the parameter is correct check whether the focused group is set
        groupIndex = allGroupsView->IndexOfLinkL(*iFocusedGroup);
        }
    else
        {
        delete iFocusedGroup;
        iFocusedGroup = iControlState->TakeFocusedContact();
        }

    MPbk2ViewActivationTransaction* viewActivationTransaction = NULL;
    if (groupIndex == KErrNotFound)
        {
        // if the group view isnt ready, thus the index cannot be retrieved,
        // do a alternative view and refresh later, when the view is ready
        viewActivationTransaction =
            Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->HandleViewActivationLC
                ( iView.Id(), aPrevViewId, NULL, NULL,
                  Phonebook2::EUpdateNaviPane | Phonebook2::EUpdateTitlePane |
                  Phonebook2::EUpdateContextPane );

        iViewImpl = CPguGroupMembersView::CGroupMembersAllGroupsNotReady::NewL
            ( *this, iView, focusedGroupLink );
        }
    else
        {
        // standard phonebook view activation
        const MVPbkViewContact& contact = allGroupsView->ContactAtL(groupIndex);
        HBufC* title = appServices.NameFormatter().GetContactTitleL
                ( contact.Fields(),
                  MPbk2ContactNameFormatter::EPreserveLeadingSpaces );
        CleanupStack::PushL( title );

        viewActivationTransaction =
            Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->HandleViewActivationLC
                ( iView.Id(), aPrevViewId, title, NULL,
                  Phonebook2::EUpdateNaviPane | Phonebook2::EUpdateTitlePane |
                  Phonebook2::EUpdateContextPane );



        iViewImpl = CGroupMembersAllGroupsReady::NewL
            ( *this, iView, appServices.ContactManager(),
                appServices.NameFormatter(), appServices.StoreProperties(),
                *appServices.CommandHandlerL(), appServices.SortOrderManager(),
                groupIndex );
        }

    // notifies startup monitor of view activation
    if (Phonebook2::Pbk2AppUi()->Pbk2StartupMonitor())
        {
        Phonebook2::Pbk2AppUi()->Pbk2StartupMonitor()->
            NotifyViewActivationL( iView.Id() );
        }

    iViewImpl->CreateControlL(iView.ClientRect());
    
    CPbk2NamesListControl* nlctrl=static_cast <CPbk2NamesListControl*> (iViewImpl->Control());
    CCoeControl* ctrl=nlctrl->ComponentControl(0);
    CEikListBox* listbox=static_cast <CEikListBox*> (ctrl);
    listbox->SetListBoxObserver( this ); 

    iPreviousViewId = aPrevViewId;

    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy(); // viewActivationTransaction
    if (groupIndex != KErrNotFound)
        {
        CleanupStack::PopAndDestroy(); // title
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::DoDeactivate()
    {
    // Trash the old states
    delete iControlState;
    iControlState = NULL;

    // Store current state, safe to ignore. There's no real harm,
    // if theres no stored state when activating this view again
    TRAP_IGNORE( StoreStateL() );

    delete iViewImpl;
    iViewImpl = NULL;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:       // FALLTHROUGH
        case EPbk2CmdOpenPreviousView:
            {
            ReturnToPreviousViewL();
            break;
            }
        default:
            {
            if ( iViewImpl )
                {
                // route handle command to view implementation
                iViewImpl->HandleCommandL(aCommand);
                }
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::DynInitMenuPaneL(
        TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( iViewImpl )
        {
        // Ask the control do control specific filtering
        // (for example call HandleMarkableListDynInitMenuPane if needed)
        iViewImpl->DynInitMenuPaneL(aResourceId, aMenuPane);
        }
    }

// ----------------------------------------------------------------------------
// CPguGroupMembersView::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CPguGroupMembersView::HandleListBoxEventL(CEikListBox* aListBox,TListBoxEvent aEventType)
    {
    
    
    
    if( AknLayoutUtils::PenEnabled() )  
        {
        switch ( aEventType )
            {
            case EEventItemDoubleClicked:
            case EEventItemSingleClicked:
		    case EEventEnterKeyPressed:
                {

                if ( iViewImpl )
                    {
                    // route the events to the view implementing the application
                    // view currently
                    iViewImpl->HandleListBoxEventL(aListBox,aEventType);
                    }
                break;
                }
            case EEventItemClicked: //Happens after focus changed
            break;
            default:
               break;
            }
        }
     
    }



// --------------------------------------------------------------------------
// CPguGroupMembersView::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::HandlePointerEventL(
        const TPointerEvent& aPointerEvent)
    {
    if ( iViewImpl )
        {
        // route the pointer events to the view implementing the application
        // view currently
        iViewImpl->HandlePointerEventL(aPointerEvent);
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::HandleLongTapEventL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::HandleLongTapEventL(
        const TPoint& aPenEventLocation,
        const TPoint& aPenEventScreenLocation )
    {
    if ( iViewImpl )
        {
        // route the long tap event to the view implementing the application
        // view currently
        iViewImpl->HandleLongTapEventL(
            aPenEventLocation, aPenEventScreenLocation );
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::TransformViewActivationStateToReady
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::TransformViewActivationStateToReadyL(
        const TInt aGroupIndex)
    {
    if ( aGroupIndex == KErrNotFound )
        {
        ReturnToPreviousViewL();
        }
    else
        {
        // Pops the control from the control stack
        delete iViewImpl;
        iViewImpl = NULL;

        // Switch view control states
        MPbk2ApplicationServices& appServices =
            Phonebook2::Pbk2AppUi()->ApplicationServices();
        iViewImpl = CPguGroupMembersView::CGroupMembersAllGroupsReady::NewL
            ( *this, iView, appServices.ContactManager(),
              appServices.NameFormatter(), appServices.StoreProperties(),
              *appServices.CommandHandlerL(), appServices.SortOrderManager(),
              aGroupIndex );

        // Creates and activates the control
        iViewImpl->CreateControlL(iView.ClientRect());
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::FocusedContact
// --------------------------------------------------------------------------
//
MVPbkContactLink* CPguGroupMembersView::FocusedContact() const
    {
    return iFocusedGroup;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::SetFocusedContact
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::SetFocusedContact(MVPbkContactLink* aGroupLink)
    {
    delete iFocusedGroup;
    iFocusedGroup = aGroupLink;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::StoreStateL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::StoreStateL()
    {
    // retrieves the views state from the implementation
    CPbk2ViewState* state = ViewStateLC();
    CleanupStack::Pop(state);
    if ( state )
        {
        // and stores it as member data
        delete iControlState;
        iControlState = state;
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::RestoreStateL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::RestoreStateL()
    {
    iViewImpl->RestoreControlStateL(iControlState);
    delete iControlState;
    iControlState = NULL;
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::UpdateViewStateL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::UpdateViewStateL(
        const TUid aCustomMessageId,
        const TDesC8& aCustomMessage)
    {
    CPbk2ViewState* viewState = NULL;
    // Read the desired UI state from aCustomMessage
    if (aCustomMessageId == TUid::Uid(KPbkViewStateUid))
        {
        // Handle lecagy view state message
        CPbk2ViewStateTransformer* transformer =
            CPbk2ViewStateTransformer::NewLC
                ( Phonebook2::Pbk2AppUi()->ApplicationServices().
                    ContactManager() );
        viewState = transformer->TransformLegacyViewStateToPbk2ViewStateLC
            ( aCustomMessage );
        CleanupStack::Pop(); // viewState
        CleanupStack::PopAndDestroy(transformer);
        }
    else if (aCustomMessageId == CPbk2ViewState::Uid())
        {
        // Read the desired UI state from aCustomMessage
        viewState = CPbk2ViewState::NewL(aCustomMessage);
        }
    else
        {
        User::Leave(KErrNotSupported);
        }

    if ( iControlState )
        {
        // Merge parameter view state with the stored state
        if ( viewState->FocusedContact() &&
             viewState->FocusedContact() != iControlState->FocusedContact() )
            {
            iControlState->SetFocusedContact(
                            viewState->TakeFocusedContact() );
            iControlState->SetFocusedFieldIndex( KErrNotFound );
            iControlState->SetTopFieldIndex( KErrNotFound );
            }
        if ( viewState->FocusedFieldIndex() >= 0 )
            {
            iControlState->SetFocusedFieldIndex(
                    viewState->FocusedFieldIndex() );
            }
        if ( viewState->TopFieldIndex() >= 0 )
            {
            iControlState->SetTopFieldIndex(
                    viewState->TopFieldIndex() );
            }
        if ( viewState->ParentContact() &&
             viewState->ParentContact() != iControlState->ParentContact() )
            {
            iControlState->SetParentContact(
                viewState->TakeParentContact() );
            }

        // Input state overwrites the saved state. When the view is deactivated
        // and then again activated marks are not set if the input state
        /// doesn't say otherwise.
        iControlState->SetMarkedContacts( viewState->TakeMarkedContacts() );

        // Delete parameter view state
        delete viewState;
        }
    else
        {
        // No stored state, use the parameter supplied one
        iControlState = viewState;
        }
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::ReturnToPreviousViewL
// --------------------------------------------------------------------------
//
void CPguGroupMembersView::ReturnToPreviousViewL() const
    {
    CPbk2ViewState* state = ViewStateLC();
    if ( iFocusedGroup )
        {
        // Change focused contact to current focused contact
        state->SetFocusedContact( iFocusedGroup->CloneLC() );
        CleanupStack::Pop(); // currentGroup
        }
    Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->
        ActivatePreviousViewL( state );
    CleanupStack::PopAndDestroy(state);
    }

// End of File
