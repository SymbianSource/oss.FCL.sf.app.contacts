/*
* Copyright (c) 2002, 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Mixin class for Logs timer.
*
*/


#ifndef MLOGSTIMER_H
#define MLOGSTIMER_H

/**
*  Mixin class for Logs timer
*
* @since 3.1
*/
class MLogsTimer
    {

    public: // New functions
        
        /**
        * This function is called after on timeout
        */
        virtual void HandleLogsTimerL( TAny* aPtr ) = 0;
    };

#endif      // MLOGSTIMER_H   
            
// End of File
