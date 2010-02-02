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
* Description:  Check Flash File system disk space
*
*/



#ifndef C_CVPBKFFSCHECK_H
#define C_CVPBKFFSCHECK_H

//  INCLUDES
#include <e32base.h>    // CBase

namespace VPbkEngUtils {

/**
 *   Flash File System (FFS) check helper class.
 *
 *  @lib VPbkEngUtils.dll
 */
class CVPbkFFSCheck : public CBase
    {

public:

    IMPORT_C static CVPbkFFSCheck* NewL( RFs& aFs );

    IMPORT_C ~CVPbkFFSCheck();

    /**
     * Checks critical FFS level.
     *
     * @param aBytesToWrite     how many bytes will be written to FFS. 
     *                          Default is unknown (=0). Leaves with KErrDiskFull
     *                          if not enough space.
     */
     IMPORT_C void FFSClCheckL(TInt aBytesToWrite=0);

private:

    CVPbkFFSCheck( RFs& aFs );

private: // data

    /**
     * Open file system session
     */
    RFs& iFs;

    };

} // namespace VPbkEngUtils
#endif // C_CVPBKFFSCHECK_H
