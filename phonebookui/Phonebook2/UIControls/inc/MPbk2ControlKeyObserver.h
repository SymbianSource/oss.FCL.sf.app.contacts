/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*      Phonebook 2 UI control key event observer.
*
*/


#ifndef MPBK2CONTROLKEYOBSERVER_H
#define MPBK2CONTROLKEYOBSERVER_H

//  INCLUDES
#include <coedef.h> // TKeyResponse
#include <w32std.h> // TKeyEvent, TEventCode

// CLASS DECLARATION

/**
 * @internal Only Phonebook 2 internal use supported!
 *
 * Phonebook 2 UI control key event observer interface.
 */
class MPbk2ControlKeyObserver
    {
    public:  // Interface
        /**
         * Called from OfferKeyEventL of the observed control.
         *
         * @param aKeyEvent   see CCoeControl::OfferKeyEventL.
         * @param aType       see CCoeControl::OfferKeyEventL.
         * @return see CCoeControl::OfferKeyEventL.
         * @see CCoeControl::OfferKeyEventL. If returns EKeyWasConsumed the 
         *      control should not forward the key event.
         */
        virtual TKeyResponse Pbk2ControlKeyEventL
            (const TKeyEvent& aKeyEvent,TEventCode aType) = 0;
    };

#endif // MPBK2CONTROLKEYOBSERVER_H

// End of File
