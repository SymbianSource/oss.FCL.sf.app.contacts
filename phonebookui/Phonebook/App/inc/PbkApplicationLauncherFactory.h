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
*     Application launcher factory.
*
*/


#ifndef __PbkApplicationLauncherFactory_H__
#define __PbkApplicationLauncherFactory_H__

#include <e32base.h>

class MPbkApplicationLauncher;

/**
 * Application launcher factory.
 */
class PbkApplicationLauncherFactory
    {
	public: // Interface
	    static MPbkApplicationLauncher* CreateLC();

    private: // Hidden constructor
        PbkApplicationLauncherFactory();
	};

#endif // PbkApplicationLauncherFactory

// End of File
