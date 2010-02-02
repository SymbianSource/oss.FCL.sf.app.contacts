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
*     Contact Info View group members navigation stategy interface.
*
*/


#ifndef __MPbkContactInfoNavigationStrategy_H__
#define __MPbkContactInfoNavigationStrategy_H__


//  INCLUDES
#include <w32std.h>

//  FORWARD DECLARATIONS
class CPbkContactItem;


//  CLASS DECLARATION

/**
 * Contact Info View group members navigation stategy interface. 
 */
class MPbkContactInfoNavigationStrategy
    {
    public:  // Destructor
        /**
         * Destructor.
         */
        virtual ~MPbkContactInfoNavigationStrategy() { }

    public: // interface
        /**
         * Handle Key event.
         */
        virtual TBool HandleCommandKeyL(const TKeyEvent& aKeyEvent, TEventCode aType) = 0;

    };

#endif // __MPbkContactInfoNavigationStrategy_H__
      
// End of File
