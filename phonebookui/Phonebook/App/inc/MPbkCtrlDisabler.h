/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Disables current view controller
*
*/


#ifndef MPBKCTRLDISABLER_H
#define MPBKCTRLDISABLER_H


//  INCLUDES
#include <cntdef.h>

//  FORWARD DECLARATIONS


//  CLASS DECLARATION

/**
 * Contact Info View navigation callback interface. 
 */
class MPbkCtrlDisabler
    {
    public: // interface
        /**
         * Disables/enables current view controller
         * @aDisable 
         * 
         */
        virtual void DisableController() = 0;

    };

#endif // MPBKCTRLDISABLER_H
      
// End of File
