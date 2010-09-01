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
*		Phonebook command key event handler interface.
*		Allows classes not derived from CCoeControl to also handle key events.
*		Used for distributing key events from control container to app view and
*		app ui.
*
*/



#ifndef __MPbkKeyEventHandler_H__
#define __MPbkKeyEventHandler_H__

// CLASS DECLARATION

/**
 * Phonebook command key event handler interface.
 * Allows classes not derived from CCoeControl to also handle key events.
 * Used for distributing key events from control container to app view and
 * app ui.
 */
class MPbkKeyEventHandler
    {
    public:  // Interface
        /**
         * Key event handler.
         *
         * @param aKeyEvent key event as in CCoeControl::OfferKeyEventL.
         * @param aType key event type as in CCoeControl::OfferKeyEventL.
         * @return ETrue if the key event was consumed, EFalse otherwise.
         *
         * @see CPbkControlContainer::OfferKeyEventL
         * @see CCoeControl::OfferKeyEventL
         */
        virtual TBool PbkProcessKeyEventL(const TKeyEvent& aKeyEvent,
			TEventCode aType) = 0;
    };


#endif  // __MPbkKeyEventHandler_H__
            

// End of File
