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
*     Abstract application launching interface.
*
*/


#ifndef __MPbkApplicationLauncher_H__
#define __MPbkApplicationLauncher_H__

/**
 * Abstract application launching interface.
 */
class MPbkApplicationLauncher
    {
	public: // Interface
        virtual ~MPbkApplicationLauncher() {}
        
        /**
         * Launches the application with given UID.
         */
	    virtual void LaunchApplicationL(TUid aAppUid) =0;
	};

#endif // MPbkApplicationLauncher

// End of File
