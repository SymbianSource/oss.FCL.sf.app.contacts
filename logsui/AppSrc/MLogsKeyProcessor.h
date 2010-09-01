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
*     Interface which allows subscribers to consume key event or pass the event
*     to other view class.
*       
*
*/


#ifndef MLogsKeyProcessor_H
#define MLogsKeyProcessor_H

// FORWARD DECLARATIONS
class MAknTabObserver;

// ===========================================================================
// MLogsKeyProcessor
// ===========================================================================

/**
*	An interface a CCoeControl class can implement to forward the keyevents before
*	the actual control gets the key events. This is useful in situations where 
*	the control consumes keys which you need.
*/
class MLogsKeyProcessor
    {
    public:
    /**
     * process the keyevent befor it reaches the control
     * @param aKeyEvent the event fired
     * @param aType the type of event
     * @return ETrue if key event was consumed, otherwise EFalse
     */
    virtual TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType ) = 0;

	/**
     * returns the pointer to MAknTabObserver interface
     * @return pointer to the MAknTabObserver interface
     */
    virtual MAknTabObserver* TabObserver() = 0;
    
    /**
     * Called when the current focused listbox item is tapped 
     * or after a double tap.       
     *         
     * @param aIndex to the current focused listbox item 
     */ 
    //virtual void ProcessPointerEventL(TInt aIndex) = 0;
    
    /**
     * Called when the current focused listbox focus changed (by tapping)
     */ 
    // virtual void FocusChangedL() = 0;
    };

            
#endif

// End of File
