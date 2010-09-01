/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 UI extension view interface.
*
*/

#ifndef MPBK2UIEXTENSIONVIEW2_H
#define MPBK2UIEXTENSIONVIEW2_H

// INCLUDES
#include <w32std.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 * This class is an extension to MPbk2UIExtensionView.
 * See documentation of MPbk2UIExtensionView from header
 * MPbk2UIExtensionView.h 
 * 
 * You can access this extension by calling
 * MPbk2UIExtensionView->UIExtensionViewExtension()
 */
class MPbk2UIExtensionView2
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2UIExtensionView2()
            {}

        /**
         * Foreground event handling function intended for overriding by sub classes. 
         * @param aForeground - Indicates the required focus state of the control.
         */
         
        virtual void HandleForegroundEventL(TBool aForeground) = 0;
        
    };

#endif // MPBK2UIEXTENSIONVIEW2_H

// End of File
