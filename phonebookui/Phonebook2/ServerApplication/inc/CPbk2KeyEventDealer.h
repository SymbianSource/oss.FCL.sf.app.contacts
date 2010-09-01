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
* Description:  Phonebook 2 key event dealer.
*
*/


#ifndef CPBK2KEYEVENTDEALER_H
#define CPBK2KEYEVENTDEALER_H

// INCLUDES
#include <coecntrl.h>

// FORWARD DECLARATIONS
class MPbk2KeyEventHandler;

// CLASS DECLARATION

/**
 * Phonebook 2 key event dealer.
 * Responsible for listening key events and dealing them
 * to a key event handler. Listens the key events with
 * higher priority than standard dialogs.
 */
class CPbk2KeyEventDealer : public CCoeControl
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aKeyEventHandler      Handler for the key events.
         * @return  A new instance of this class.
         */
        static CPbk2KeyEventDealer* NewL(
                MPbk2KeyEventHandler& aKeyEventHandler );

        /**
         * Destructor.
         */
        ~CPbk2KeyEventDealer();

    private: // From CCoeControl
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // Implementation
        CPbk2KeyEventDealer(
                MPbk2KeyEventHandler& aKeyEventHandler );
        void ConstructL();

    private: // Data
        /// Ref: Key event handler
        MPbk2KeyEventHandler& iKeyEventHandler;
    };

#endif // CPBK2KEYEVENTDEALER_H

// End of File
