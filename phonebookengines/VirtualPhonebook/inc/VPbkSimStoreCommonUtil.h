/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility functions for for all SIM components
*
*/


#ifndef VPBKSIMSTORECOMMONUTIL_H
#define VPBKSIMSTORECOMMONUTIL_H

#include <e32def.h>

/**
 *  Utility functions for for all SIM components
 *
 *  @lib VPbkSimStoreCommon.lib
 */
class VPbkSimStoreCommonUtil
    {
public:
	/**
	 * Gets phonebook defined maximum phonenumber length
	 * from Central Repository. Doesn't leave with KErrNotFound
	 * if CenRep was not found but returns a default value instead.
	 *
	 * @return the phonebook defined maximum phonenumber length
	 */
    IMPORT_C static  TInt SystemMaxPhoneNumberLengthL();

private:

    VPbkSimStoreCommonUtil();
    };

#endif // VPBKSIMSTORECOMMONUTIL_H
