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
* Description:  This is MMC version of TPbk2ExtraDriveAvailable
*                Checks is there MMC in phone
*
*/


#include "tpbk2multidriveavailable.h"
#include <driveinfo.h>


const TInt PhoneMemoryOnly = 1;

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// TPbk2ExtraDriveAvailable::ExtraDriveAvailable
// --------------------------------------------------------------------------
//
TBool TPbk2MultiDriveAvailable::MultiDriveAvailable( RFs& aFs )
    {
    DriveInfo::TDriveArray driveArray;
    TInt err = DriveInfo::GetUserVisibleDrives( 
        aFs, driveArray );
    TInt foundDrives(0);
    if ( err == KErrNone )
        {
        TVolumeInfo volumeInfo;
        TInt driveCount( driveArray.Count() );
        for ( TInt i(0); i < driveCount && foundDrives <= PhoneMemoryOnly; ++i )
            {
            if ( aFs.Volume( volumeInfo, driveArray[i] ) == KErrNone )
                {
                ++foundDrives;
                }
            }
        }
    return foundDrives > PhoneMemoryOnly;
    }

// End of file
