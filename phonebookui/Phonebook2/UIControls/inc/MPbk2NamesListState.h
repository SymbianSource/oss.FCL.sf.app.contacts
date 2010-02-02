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
* Description:  Phonebook 2 names list state interface.
*
*/


#ifndef MPBK2NAMESLISTSTATE_H
#define MPBK2NAMESLISTSTATE_H

// INCLUDES
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactUiControl2.h>
#include <MPbk2PointerEventInspector.h>
#include "cpbk2filteredviewstack.h"
#include <w32std.h>
#include <coedef.h>
#include <coecobs.h>

// FORWARD DECLARATIONS
class CCoeControl;
class CWindowGc;

// CLASS DECLARATION

/**
 * Interface for names list control states.
 */
class MPbk2NamesListState :
        public MPbk2ContactUiControl,
        public MPbk2ContactUiControl2,
        public MPbk2PointerEventInspector,
        public MPbk2FilteredViewStackObserver
    {
    public: // Interface
        /**
         * Destructor.
         */
        virtual ~MPbk2NamesListState() { }

        /**
         * Performs the operations required when the state is
         * being activated.
         */
        virtual void ActivateStateL() = 0;

        /**
         * Performs the operations required when the state is
         * being deactivated.
         */
        virtual void DeactivateState() = 0;

        /**
         * Returns the number of component controls of the state.
         *
         * @see CCoeControl::CountComponentControls()
         * @return  Number of component controls.
         */
        virtual TInt CountComponentControls() const = 0;

        /**
         * Returns the control at given index.
         *
         * @see CCoeControl::ComponentControl()
         * @param aIndex    The index to inspect.
         * @return  Control at given index.
         */
        virtual CCoeControl* ComponentControl( TInt aIndex ) const = 0;

        /**
         * Handles size changed events.
         *
         * @see CCoeControl::SizeChanged()
         */
        virtual void SizeChanged() = 0;

        /**
         * Makes the components visible/invisible.
         *
         * @param aVisible  ETrue if components are to be shown,
         *                  EFalse otherwise.
         */
        virtual void MakeComponentsVisible( TBool aVisible ) = 0;

        /**
         * @see CCoeControl::ActivateL
         */
        virtual void ActivateL() = 0;

        /**
         * @see CCoeControl::Draw
         */
        virtual void Draw( const TRect& aRect, CWindowGc& aGc ) const = 0;

        /**
         * @see CCoeControl::OfferKeyEventL
         */
        virtual TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                TEventCode aType ) = 0;

        /**
         * @see CCoeControl::HandlePointerEventL
         */
        virtual void HandlePointerEventL(
                const TPointerEvent& aPointerEvent ) = 0;

        /**
         * Returns the type of the state.
         *
         * @see TPbk2NamesListState
         * @return  A state enumeration.
         */
        virtual TInt NamesListState() const = 0;

        /**
         * Handles contact view event.
         *
         * @see TPbk2StateUpdateEvent
         * @param aEvent    Indicates what event took place.
         * @param aIndex    Index of the view contact, where the
         *                  event took place.
         */
        virtual void HandleContactViewEventL( TInt aEvent, TInt aIndex ) = 0;

        /**
         * Returns the CCoeControl instance.
         *
         * @return  CCoeControl instance of this control.
         */
        virtual CCoeControl& CoeControl() = 0;

        /**
         * For forwarding control events to state. States
         * shouldn't be MCoeControlObservers themselves.
         *
         * @see MCoeControlObserver::HandleControlEventL
         * @param aParam used to pass special parameters.
         */
        virtual void HandleControlEventL( CCoeControl* aControl,
            MCoeControlObserver::TCoeEvent aEventType, TInt aParam ) = 0;

        /**
         * Return the top contact in the names list or NULL
         *
         * @return The top contact in the names list or NULL
         */
        virtual const MVPbkViewContact* TopContactL() = 0;

        /**
         * Implements handling of command item addition & removal
         * in state objects. The state object needs to inform
         * listbox accordingly & take actions on focus etc.
         *
         * @param aEvent: Refer to TPbk2StateUpdateEvent
         * @param aCmdIndex: The command item index to be added/removed
         */           
        virtual void HandleCommandEventL(TInt aEvent, TInt aCmdIndex) = 0;

    public: // Types
        /// The state update events we are interested in.
        enum TPbk2StateUpdateEvent
            {
            /// Item added
            EItemAdded = 0,
            /// Item removed
            EItemRemoved,
            /// Sort order changed
            ESortOrderChanged,
            // Called when contact view is updated. 
            EContactViewUpdated
            };
    };

#endif // MPBK2NAMESLISTSTATE_H

// End of File
