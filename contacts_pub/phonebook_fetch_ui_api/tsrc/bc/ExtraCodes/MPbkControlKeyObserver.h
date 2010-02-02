/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*      Phonebook UI control key event observer.
*
*/








#ifndef __MPbkControlKeyObserver_H__
#define __MPbkControlKeyObserver_H__

//  INCLUDES
#include <coedef.h> // TKeyResponse
#include <w32std.h> // TKeyEvent, TEventCode

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook UI control key event observer interface.
 */
class MPbkControlKeyObserver
    {
    public:  // Interface
        /**
         * Called from OfferKeyEventL of the observed control.
         *
         * @param   aKeyEvent   see CCoeControl::OfferKeyEventL.
         * @param   aType       see CCoeControl::OfferKeyEventL.
         * @return see CCoeControl::OfferKeyEventL.
         * @see CCoeControl::OfferKeyEventL. If returns EKeyWasConsumed the 
         *      control should not forward the key event.
         */
        virtual TKeyResponse PbkControlKeyEventL
            (const TKeyEvent& aKeyEvent,TEventCode aType) = 0;
    };


#endif // __MPbkControlKeyObserver_H__

// End of File
