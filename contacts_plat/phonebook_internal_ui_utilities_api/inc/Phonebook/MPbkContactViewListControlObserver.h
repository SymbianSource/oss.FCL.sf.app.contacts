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
*    Phonebook contact view list control observer interface.
*
*/


#ifndef __MPbkContactViewListControlObserver_H__
#define __MPbkContactViewListControlObserver_H__

//  INCLUDES
#include <cntdef.h> // TContactItemId

//  FORWARD DECLARATIONS
class CPbkContactViewListControl;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Event type for MPbkContactViewListControlObserver.
 * @see MPbkContactViewListControlObserver
 */
class TPbkContactViewListControlEvent
    {
    public:
        enum TEventType
            {
            /// See TContactViewEvent::EUnavailable
	        EUnavailable,
            /// See TContactViewEvent::EReady
	        EReady,
            /// See TContactViewEvent::EItemAdded
	        EItemAdded,
            /// See TContactViewEvent::EItemRemoved
	        EItemRemoved,
            /**
             * Sent when contact selection status changes because of user 
             * actions. Selection changes caused by CPbkContactViewListControl
             * API calls are not reported.
             */
            EContactSelected,
            /**
             * Sent when contact selection status changes because of user 
             * actions. Selection changes caused by CPbkContactViewListControl
             * API calls are not reported.
             */
            EContactUnselected,
            /// Sent when the control enter busy state
            EEnterBusy,
            /// Sent when the control enter exits busy state
            EExitBusy,
            /// Sent when the control's content is changed because of find filtering
            EContactSetChanged,
            /**
             * Sent when user taps a contact item in a list. This might be
             * the only event sent if selection accepter prevents the focused
             * contact from being selected e.g. in a multi-selection fetch.
             */
            EContactTapped,
            /**
             * Sent when user double-taps a contact item in a list. In this
             * case, EContactTapped is not sent.
             */
            EContactDoubleTapped
            };

		/**
		 * Constructor.
		 * @param aEventType event type
		 */
        inline TPbkContactViewListControlEvent(TEventType aEventType)
            : iEventType(aEventType), iInt(KErrNotFound), iContactId(KNullContactId)
            {
            }
		/// Own: event type
        TEventType iEventType;
		/// Own: index number
        TInt iInt;
		/// Own: contact item id
        TContactItemId iContactId;
    };


/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook contact view list control observer interface.
 *
 * @see TPbkContactViewListControlEvent
 * @see CPbkContactViewListControl
 * @see CPbkContactViewListControl::AddObserverL
 * @see CPbkContactViewListControl::RemoveObserver
 */
class MPbkContactViewListControlObserver
    {
    public:  // Interface
		/**
		 * Handles cotnact view list control event.
		 * @param aControl the control
		 * @param aEvent the event
		 */
        virtual void HandleContactViewListControlEventL
            (CPbkContactViewListControl& aControl,
            const TPbkContactViewListControlEvent& aEvent) = 0;

    protected:  // Interface
        /**
         * Destructor.
         */ 
        virtual ~MPbkContactViewListControlObserver() { }
    };

#endif // __MPbkContactViewListControlObserver_H__

// End of File
