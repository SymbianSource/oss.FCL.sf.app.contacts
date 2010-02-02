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


#ifndef __CPbkExeLauncher_H__
#define __CPbkExeLauncher_H__

#include <e32base.h>
#include "MPbkApplicationLauncher.h"

/**
 * Application launcher factory.
 */
class CPbkExeLauncher : public CBase,
                        public MPbkApplicationLauncher
    {
	public: // Interface
	    static MPbkApplicationLauncher* NewLC();

    private: // From MPbkApplicationLauncher
        void LaunchApplicationL(TUid aAppUid);
	};

#endif // CPbkExeLauncher

// End of File
