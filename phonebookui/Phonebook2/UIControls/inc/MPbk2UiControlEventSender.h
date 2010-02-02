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
* Description:  Phonebook 2 contact UI control event sender interface.
*
*/

#ifndef MPBK2UICONTROLEVENTSENDER_H
#define MPBK2UICONTROLEVENTSENDER_H

// FORWARD DECLARATIONS
class TPbk2ControlEvent;

// CLASS DECLARATION

/**
 * Phonebook 2 contact UI control event sender interface.
 */
class MPbk2UiControlEventSender
    {
    public: // Interface

        /**
         * Sends event to observers.
         *
         * @param aEvent    The event to send.
         * @return ETrue if Ready-event has been sent, EFalse otherwise.
         */
        virtual TBool SendEventToObserversL(
                const TPbk2ControlEvent& aEvent ) = 0;

    protected: // Protected functions
        virtual ~MPbk2UiControlEventSender()
                {}
    };

#endif // MPBK2UICONTROLEVENTSENDER_H

// End of File
