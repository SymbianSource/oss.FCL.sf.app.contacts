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


#include <sysutil.h>
#include "CVPbkDiskSpaceCheck.h"

namespace VPbkEngUtils {

// ======== MEMBER FUNCTIONS ========

CVPbkDiskSpaceCheck::CVPbkDiskSpaceCheck( RFs& aFs, TInt aDrive ) :
    iFs ( aFs ),
    iDrive ( aDrive )
    {
    }

CVPbkDiskSpaceCheck::~CVPbkDiskSpaceCheck()
    {
    }

EXPORT_C CVPbkDiskSpaceCheck* CVPbkDiskSpaceCheck::NewL( RFs& aFs, TInt aDrive )
    {
    CVPbkDiskSpaceCheck* self = new( ELeave ) CVPbkDiskSpaceCheck( aFs, aDrive );
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkFFSCheck::FFSClCheckL
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkDiskSpaceCheck::DiskSpaceCheckL(TInt aBytesToWrite/*=0*/)
    {
    if (SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, aBytesToWrite, iDrive) )
        {
        // Show not enough memory note
        User::Leave(KErrDiskFull);
        }
    }
    
} // namespace VPbkEngUtils


