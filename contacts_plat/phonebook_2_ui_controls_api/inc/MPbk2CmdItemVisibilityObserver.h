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
* Description:  
*
*/


#ifndef  MPBK2CMDITEMVISIBILITYOBSERVER_H_
#define  MPBK2CMDITEMVISIBILITYOBSERVER_H_

/**
 * Observer for command item. Clients of MPbk2UiControlCmdItem
 * can implement this class to get event notifications on the change of visibility
 * of a command item.
 */
class MPbk2CmdItemVisibilityObserver
    {
    public:

        /**
         * Informs the observer that visibility has changed.
         * @param aVisible The new visibility.
         */
        virtual void CmdItemVisibilityChanged( TInt aCmdItemId, TBool aVisible ) = 0;
    };

#endif MPBK2CMDITEMVISIBILITYOBSERVER_H_
