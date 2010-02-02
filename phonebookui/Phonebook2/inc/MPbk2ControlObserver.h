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
* Description:  Phonebook 2 control observer interface.
*
*/


#ifndef MPBK2CONTROLOBSERVER_H
#define MPBK2CONTROLOBSERVER_H

//  FORWARD DECLARATIONS
class MPbk2ContactUiControl;

// CLASS DECLARATION

/**
 * Phonebook 2 control observer event type.
 */
class TPbk2ControlEvent
    {
    public: // Types

        /**
         * Event types.
         */
        enum TEventType
            {
            /// Control unavailable
            EUnavailable,
            /// Control ready
            /// @see MVPbkContactViewObserver::ContactViewReady
            EReady,
            /// Item added to the control
            /// @see MVPbkContactViewObserver::ContactAddedToView
            EItemAdded,
            /// Item removed from the control
            /// @see MVPbkContactViewObserver::ContactRemovedFromView
            EItemRemoved,
            /// Sent when contact selection status changes because of
            /// the user actions. Selection changes caused programmically
            /// by control API calls are not reported.
            EContactSelected,
            /// Sent when contact selection status changes because of
            /// the user actions. Selection changes caused programmically
            /// by control API calls are not reported.
            EContactUnselected,
            /// Sent when contact selection status changes because of
            /// the user actions. Selection changes caused programmically
            /// by control API calls are not reported.
            EContactUnselectedAll,
            /// Sent when the control enters busy state
            EEnterBusy,
            /// Sent when the control exits busy state
            EExitBusy,
            /// Sent when the control's content is changed
            /// because of the find filtering
            EContactSetChanged,
            /// Sent when user taps a contact item in a list. This might be
            /// the only event sent if selection accepter prevents the focused
            /// contact from being selected e.g. in a multi-selection fetch.
            EContactTapped,
            /// Sent when user double-taps a contact item in a list. In this
            /// case, EContactTapped is not sent.
            EContactDoubleTapped,
            /// Sent when the control's logical state changes
            EControlStateChanged,
            /// Sent when listbox enters selection mode if the control 
            /// supports selection. Selection mode is enabled by 
            /// long pressing shift, ctrl or hash keys (when hash key
            /// marking is enabled).
            EControlEntersSelectionMode,
            /// Sent when listbox leaves the selection mode if the control 
            /// supports selection.
            EControlLeavesSelectionMode,
            /// Sent when focus changed in listbox.
            EControlFocusChanged,
            /// Sent when enter was pressed in listbox.
            EControlEnterKeyPressed
            };

    public: // Constructor

        /**
         * Constructor.
         *
         * @param aEventType    Event type.
         */
        inline TPbk2ControlEvent(
                TEventType aEventType ) :
                    iEventType( aEventType ),
                    iInt( KErrNotFound )
            {
            }

    public: // Data
        /// Own: Event type
        TEventType iEventType;
        /// Own: Index
        TInt iInt;
    };


/**
 * Phonebook 2 control observer interface.
 */
class MPbk2ControlObserver
    {
    public:  // Interface

        /**
         * Handles Phonebook 2 control event.
         *
         * @param aControl  The control.
         * @param aEvent    The event.
         */
        virtual void HandleControlEventL(
                MPbk2ContactUiControl& aControl,
                const TPbk2ControlEvent& aEvent ) = 0;

    protected: // Protected functions
        /**
         * Destructor.
         */
        virtual ~MPbk2ControlObserver()
                {}
    };

#endif // MPBK2CONTROLOBSERVER_H

// End of File
