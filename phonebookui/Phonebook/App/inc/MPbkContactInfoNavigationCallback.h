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
*     Contact Info View navigation callback interface.
*
*/


#ifndef __MPbkContactInfoNavigationCallback_H__
#define __MPbkContactInfoNavigationCallback_H__


//  INCLUDES
#include <cntdef.h>

//  FORWARD DECLARATIONS


//  CLASS DECLARATION

/**
 * Contact Info View navigation callback interface. 
 */
class MPbkContactInfoNavigationCallback
    {
    public: // interface
        /**
         * Changed contact to aContactId.
         */
        virtual void ChangeContactL(TContactItemId aContactId) = 0;

        /**
         * Returns a instance of the statuspane or NULL if not present.
         */
        virtual CEikStatusPane* GetStatusPane() const = 0;

        /**
         * Returns the current contact item id.
         */
        virtual TContactItemId GetContactItemId() const = 0;

    protected:  // Destructor
        virtual ~MPbkContactInfoNavigationCallback() { }
    };

#endif // __MPbkContactInfoNavigationCallback_H__
      
// End of File
