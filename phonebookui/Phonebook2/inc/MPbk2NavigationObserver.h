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
* Description:  Phonebook 2 navigation observer interface.
*
*/


#ifndef MPBK2NAVIGATIONOBSERVER_H
#define MPBK2NAVIGATIONOBSERVER_H

// INCLUDES
#include <w32std.h>

// CLASS DECLARATION

/**
 * Phonebook 2 navigation observer interface.
 */
class MPbk2NavigationObserver
    {
    public: // Types

        /**
         * Navigation event types.
         */
        enum TEventType
            {
            /// Contact was deleted
            EContactDeleted = 1
            };

    public: // Interface

        /**
         * Event handling.
         *
         * @param aEventType    Event type.
         */
        virtual void HandleNavigationEvent(
                const TEventType& aEventType ) = 0;

    protected: // Disabled functions
        virtual ~MPbk2NavigationObserver()
            {}
    };

#endif // MPBK2NAVIGATIONOBSERVER_H

// End of File
