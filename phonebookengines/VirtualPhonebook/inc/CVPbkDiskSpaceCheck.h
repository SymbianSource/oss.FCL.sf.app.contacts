/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Check disk space if it is under Critical Level
*
*/



#ifndef C_CVPBKFFSCHECK_H
#define C_CVPBKFFSCHECK_H

//  INCLUDES
#include <e32base.h>    // CBase

namespace VPbkEngUtils {

/**
 * Disk space check helper class.
 *
 * @lib VPbkEngUtils.dll
 */
class CVPbkDiskSpaceCheck : public CBase
    {

public:

    /**
     * @param aFs               Open file system session
     *                      
     * @param aDrive            number of the drive to be checked.
     *                          (see TDriveNumber of f32file.h)
     */
    IMPORT_C static CVPbkDiskSpaceCheck* NewL( RFs& aFs, TInt aDrive );

    IMPORT_C ~CVPbkDiskSpaceCheck();

    /**
     * Checks critical disk space level. 
     * Will leave with KErrDiskFull if space is below CL
     *
     * @param aBytesToWrite  how many bytes will be written to FFS. 
     *                       Default is unknown (=0). Leaves with KErrDiskFull
     *                       if not enough space.
     */
     IMPORT_C void DiskSpaceCheckL( TInt aBytesToWrite=0 );

private:

    CVPbkDiskSpaceCheck( RFs& aFs, TInt aDrive );

private: // data

    /// Not Owned: Open file system session
    RFs& iFs;
    /// Not Owned: Drive which space is checked
    TInt iDrive;    

    };

} // namespace VPbkEngUtils
#endif // C_CVPBKFFSCHECK_H
