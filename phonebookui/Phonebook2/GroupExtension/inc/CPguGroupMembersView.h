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


#ifndef CPGUGROUPMEMBERSVIEW_H
#define CPGUGROUPMEMBERSVIEW_H

// INCLUDES
#include <e32base.h>
#include <vwsdef.h>
#include <MPbk2UIExtensionView.h>
#include <eiklbo.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionView;
class CPbk2NamesListControl;
class CPbk2ContactNavigation;
template<class ControlType> class CPbk2ControlContainer;
class MPbk2ContactUiControl;
class CGroupMembersViewActivation;
class MVPbkContactLink;

// CLASS DECLARATIONS

/**
 * Phonebook group members view activation state switcher interface.
 */
class MPbk2GroupMembersViewActivationCallback
    {
    public: // Interface

        /**
         * Transforms view's activation state to ready.
         *
         * @param aGroupIndex   Index of group.
         */
        virtual void TransformViewActivationStateToReadyL(
                const TInt aGroupIndex ) = 0;

        /**
         * Restores previous state.
         */
        virtual void RestoreStateL() = 0;

        /**
         * Returns focused contact.
         *
         * @return  Link to the focused contact.
         */
        virtual MVPbkContactLink* FocusedContact() const = 0;

        /**
         * Sets focused contact.
         *
         * @param aGroupLink    Focused group link.
         */
        virtual void SetFocusedContact(
                MVPbkContactLink* aGroupLink ) = 0;

    protected: // Disabled fuctions
        ~MPbk2GroupMembersViewActivationCallback()
                {}
    };

/**
 * Phonebook 2 group members view.
 */
class CPguGroupMembersView : public CBase,
                             public MPbk2UIExtensionView,
                             public MEikListBoxObserver,
                             private MPbk2GroupMembersViewActivationCallback
                             

    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aView             The extension view.
         * @return  A new instance of this class.
         */
        static CPguGroupMembersView* NewL(
                CPbk2UIExtensionView& aView );

        /**
         * Destructor.
         */
        ~CPguGroupMembersView();

    public: // From MPbk2UIExtensionView
        void HandleStatusPaneSizeChange();
        CPbk2ViewState* ViewStateLC() const;
        TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        TInt GetViewSpecificMenuFilteringFlagsL() const;
        void DoActivateL(
                const TVwsViewId& aPrevViewId,
                TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        void DoDeactivate();
        void HandleCommandL(
                TInt aCommand );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );
        void HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation );


    protected: 
        // from MEikListBoxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

    private: // From MPbk2GroupMembersViewActivationCallback
        void TransformViewActivationStateToReadyL(
                const TInt aGroupIndex );
        void RestoreStateL() const;
        MVPbkContactLink* FocusedContact() const;
        void SetFocusedContact(
                MVPbkContactLink* aGroupLink );

    private: // Implementation
        CPguGroupMembersView(
                CPbk2UIExtensionView& aView );
        void StoreStateL();
        void RestoreStateL();
        void UpdateViewStateL(
                const TUid aCustomMessageId,
                const TDesC8& aCustomMessage );
        class CGroupMembersViewImpl;
        class CGroupMembersAllGroupsNotReady;
        class CGroupMembersAllGroupsReady;
        void ReturnToPreviousViewL() const;
        void ShowContextMenuL();


    private: // Data
        /// Ref: View that contains this extension view
        CPbk2UIExtensionView& iView;
        /// Own: Stored control state
        CPbk2ViewState* iControlState;
        /// Own: Focused group
        MVPbkContactLink* iFocusedGroup;
        /// Own: The view which this view was activated from
        TVwsViewId iPreviousViewId;
        /// Own: View activation state machine
        CGroupMembersViewImpl* iViewImpl;
    };

#endif // CPGUGROUPMEMBERSVIEW_H

// End of File
