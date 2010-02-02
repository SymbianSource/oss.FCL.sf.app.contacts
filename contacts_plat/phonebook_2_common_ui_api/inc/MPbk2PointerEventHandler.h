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
* Description:  Phonebook 2 command pointer event handler interface.
*
*/


#ifndef MPBK2POINTEREVENTHANDLER_H
#define MPBK2POINTEREVENTHANDLER_H

// INCLUDE FILES
#include <e32std.h>

// FORWARD DECLARATIONS
class TPoint;
class TPointerEvent;

// CLASS DECLARATION

/**
 * Phonebook 2 pointer event handler interface.
 * Allows classes not derived from CCoeControl to also handle pointer
 * events. Used for distributing pointer events from control container
 * to application view and to application UI.
 */
class MPbk2PointerEventHandler
    {
    public: // Interface

        /**
         * Handles pointer events.
         *
         * @param aPointerEvent     Pointer event.
         */
        virtual void Pbk2ProcessPointerEventL(
                const TPointerEvent& aPointerEvent ) = 0;

        /**
         * Handles long tap pointer events.
         *
         * @param aPenEventLocation        Long tap event location relative
         *                                 to parent control.
         * @param aPenEventScreenLocation  Long tap event location relative
         *                                 to screen.
         * @see MAknLongTapDetectorCallBack
         */
        virtual void Pbk2HandleLongTapEventL(
                const TPoint& aPenEventLocation,
                const TPoint& aPenEventScreenLocation ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* PointerEventHandlerExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Disabled functions
        virtual ~MPbk2PointerEventHandler()
                {}
    };

#endif // MPBK2POINTEREVENTHANDLER_H

// End of File
