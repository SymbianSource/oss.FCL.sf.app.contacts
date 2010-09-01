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
* Description:  Flash File System (FFS) check helper class methods.
*
*/


// INCLUDE FILES
#include "CPbk2DriveSpaceCheck.h"
#include <pathinfo.h>
#include <sysutil.h>

const TInt KDiskSpaceNeedNotknown = 0;

// ================= MEMBER FUNCTIONS =======================

inline CPbk2DriveSpaceCheck::CPbk2DriveSpaceCheck( RFs& aFs ) :
    iFs( aFs ),
    iDrive( KErrNotFound ),
    iStoreIsInDrive( ETrue )
    {
    }

EXPORT_C CPbk2DriveSpaceCheck* CPbk2DriveSpaceCheck::NewL( RFs& aFs )
    {
    CPbk2DriveSpaceCheck* self = new (ELeave) CPbk2DriveSpaceCheck( aFs );
    return self;
    }
    

CPbk2DriveSpaceCheck::~CPbk2DriveSpaceCheck()
    {
    }

EXPORT_C void CPbk2DriveSpaceCheck::DriveSpaceCheckL()
    {
    TBool criticalLevel( EFalse );
    if ( iDrive == KErrNotFound && iStoreIsInDrive )
        {
        criticalLevel = SysUtil::FFSSpaceBelowCriticalLevelL( &iFs );
        }
    else if ( iDrive != KErrNotFound && iStoreIsInDrive )
        {
        criticalLevel = SysUtil::DiskSpaceBelowCriticalLevelL( 
            &iFs, KDiskSpaceNeedNotknown, iDrive );
        }
        
    if ( criticalLevel )
        {
        User::Leave( KErrDiskFull );
        }
    }

EXPORT_C void CPbk2DriveSpaceCheck::SetStore( const TVPbkContactStoreUriPtr aUriPtr )
    {
    iStoreIsInDrive = EFalse;
    iDrive = KErrNotFound;
    
    const TPtrC driveLetter = 
        aUriPtr.Component( TVPbkContactStoreUriPtr::EContactStoreUriStoreDrive );
    if ( driveLetter.Length() > 0 )
        {
        iStoreIsInDrive = ETrue;
        TDriveUnit driveUnit( driveLetter );
        iDrive = driveUnit;
        }
    }

EXPORT_C void CPbk2DriveSpaceCheck::SetStore( const TDesC& aFilePath )
    {
    iStoreIsInDrive = EFalse;
    iDrive = KErrNotFound;
    
    if( aFilePath.Length() )
        {
        //Extract the first char of the path -> its the drive name       
        const TPtrC driveLetter = aFilePath.Mid(0,1);
        
        if ( driveLetter.Length() > 0 )
            {
            iStoreIsInDrive = ETrue;
            TDriveUnit driveUnit( driveLetter );
            iDrive = driveUnit;
            }
        }
    }

//  End of File  
