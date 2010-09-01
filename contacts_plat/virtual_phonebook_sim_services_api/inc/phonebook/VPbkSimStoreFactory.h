/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A factory for creating a SIM phone
*
*/


#ifndef VPBKSIMSTOREFACTORY_H
#define VPBKSIMSTOREFACTORY_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MVPbkSimPhone;
class MVPbkSimStateInformation;

// CLASS DECLARATION

/**
 * An factory for creating a phone
 *
 * @lib VPbkSimStoreImpl.lib
 */
class VPbkSimStoreFactory
    {
    public: // New functions
        /**
         * Returns the SIM phone
         *
         * @return The phone
         */
        IMPORT_C static MVPbkSimPhone* CreatePhoneL();

        /**
         * Returns a sim state information instance
         *
         * @return A sim state information instance
         */
        IMPORT_C static MVPbkSimStateInformation* GetSimStateInformationL();

    };

#endif      // VPBKSIMSTOREFACTORY_H

// End of File
