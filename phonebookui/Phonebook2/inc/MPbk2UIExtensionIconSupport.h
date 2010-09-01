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
* Description:  Phonebook 2 UI extension icon support interface.
*
*/


#ifndef MPBK2UIEXTENSIONICONSUPPORT_H
#define MPBK2UIEXTENSIONICONSUPPORT_H

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class CPbk2IconArray;

// CLASS DECLARATION

/**
 * Phonebook 2 UI extension icon support interface.
 * For adding icons to Phonebook 2 icon arrays.
 */
class MPbk2UIExtensionIconSupport
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2UIExtensionIconSupport()
                {}

        /**
         * Appends icons to the icon array if the extension has defined one.
         *
         * @param aIconArray    The Phonebook 2 icon array
         *                      that can be extended.
         */
        virtual void AppendExtensionIconsL(
                CPbk2IconArray& aIconArray ) = 0;

        /**
         * Refreshes extension icons in the icon array. Refresh is not done
         * if the extension doesn't have icons in the array.
         *
         * @param aIconArray    The Phonebook 2 icon array to be refreshed.
         */
        virtual void RefreshL(
                CPbk2IconArray& aIconArray ) = 0;
    };

#endif // MPBK2UIEXTENSIONICONSUPPORT_H

// End of File
