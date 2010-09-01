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
#include <pathinfo.h>

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// TPbk2MultiDriveAvailable::ExtraDriveAvailable
// --------------------------------------------------------------------------
//
TBool TPbk2MultiDriveAvailable::MultiDriveAvailable( RFs& aFs )
    {
    TBool extraDriveAvailable( EFalse );
    TInt drive( KErrNone );
    TInt err = aFs.CharToDrive( PathInfo::MemoryCardRootPath()[0], drive );
    if ( err == KErrNone )
        {
        TVolumeInfo volumeInfo;
        extraDriveAvailable =  ( aFs.Volume( volumeInfo, drive ) == KErrNone );
        }
    return extraDriveAvailable;
    }

// End of file
