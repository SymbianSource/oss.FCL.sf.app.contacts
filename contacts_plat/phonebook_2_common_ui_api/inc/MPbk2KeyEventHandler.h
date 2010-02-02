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
* Description:  Phonebook 2 command key event handler interface.
*
*/


#ifndef MPBK2KEYEVENTHANDLER_H
#define MPBK2KEYEVENTHANDLER_H

// INCLUDE FILES
#include <w32std.h>

// CLASS DECLARATION

/**
 * Phonebook 2 command key event handler interface.
 * Allows classes not derived from CCoeControl to also handle key
 * events. Used for distributing key events from control container
 * to application view and to application UI.
 */
class MPbk2KeyEventHandler
    {
    public: // Interface

        /**
         * Handles key events.
         *
         * @param aKeyEvent     Key event as in CCoeControl::OfferKeyEventL.
         * @param aType         Key event type as in
         *                      CCoeControl::OfferKeyEventL.
         * @return  ETrue if the key event was consumed, EFalse otherwise.
         * @see CCoeControl::OfferKeyEventL
         */
        virtual TBool Pbk2ProcessKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* KeyEventHandlerExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Disabled functions
        virtual ~MPbk2KeyEventHandler()
                {}
    };

#endif // MPBK2KEYEVENTHANDLER_H

// End of File
