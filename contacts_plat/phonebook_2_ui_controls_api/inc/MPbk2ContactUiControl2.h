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
* Description:  Phonebook 2 contact UI control interface.
*
*/


#ifndef MPBK2CONTACTUICONTROL2_H
#define MPBK2CONTACTUICONTROL2_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLATIONS

class MPbk2UiControlCmdItem;


// CLASS DECLARATION

/**
 * This class is an extension to MPbk2ContactUiControl.
 * See documentation of MVPbkStoreContact from header MPbk2ContactUiControl.h
 */
class MPbk2ContactUiControl2
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ContactUiControl2()
            {}

        /**
         * Number of command items.
         */
        virtual TInt CommandItemCount() const = 0;

        /**
         * Gives a certain command item.
         * @see CommandItemCount()
         * @param aIndex The index of the command item.
         * @return The command item, as const.
         */
        virtual const MPbk2UiControlCmdItem& CommandItemAt( TInt aIndex ) const = 0;

        /**
         * Gives a focused command item, or NULL if no command item is
         * currently focused.
         * @return The focused command item or NULL if none is focused.
         */
        virtual const MPbk2UiControlCmdItem* FocusedCommandItem() const = 0;

        /**
         * Deletes a command item from the list.
         * @param aIndex The command item that is to be deleted.
         */
        virtual void DeleteCommandItemL( TInt aIndex ) = 0;

        /**
         * Adds a command item to the list if not already added.
         * Transfers ownership of aCommand to the called object.
         *
         * @param aCommand The command item that is to be added
         * @param aIndex Index to which the command item that is to be added
         *        must not be greater than CommandItemCount()
         */
        virtual void AddCommandItemL(MPbk2UiControlCmdItem* aCommand, TInt aIndex) = 0;

    };

#endif // MPBK2CONTACTUICONTROL2_H

// End of File
