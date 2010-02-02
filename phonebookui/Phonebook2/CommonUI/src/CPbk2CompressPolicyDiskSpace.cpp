/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Phonebook 2 compress policy disk space utility.
*
*/


// INCLUDE FILES
#include "CPbk2CompressPolicyDiskSpace.h"

// Phonebook 2
#include "MPbk2CompressPolicyManager.h"

// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2CompressPolicyDiskSpace::CPbk2CompressPolicyDiskSpace
// --------------------------------------------------------------------------
//
inline CPbk2CompressPolicyDiskSpace::CPbk2CompressPolicyDiskSpace
        ( MPbk2CompressPolicyManager& aManager,
        RFs& aFs, TInt64 aThreshold, TInt aDrive ) :
            CActive( CActive::EPriorityIdle ),
            iManager( aManager ), iFs( aFs ),
            iThreshold( aThreshold ), iDrive( aDrive )
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyDiskSpace::~CPbk2CompressPolicyDiskSpace
// --------------------------------------------------------------------------
//
CPbk2CompressPolicyDiskSpace::~CPbk2CompressPolicyDiskSpace()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyDiskSpace::NewL
// --------------------------------------------------------------------------
//
CPbk2CompressPolicyDiskSpace* CPbk2CompressPolicyDiskSpace::NewL
        (MPbk2CompressPolicyManager& aManager,
        RFs& aFs, TInt64 aThreshold, TInt aDrive)
    {
    CPbk2CompressPolicyDiskSpace* self =
        new ( ELeave ) CPbk2CompressPolicyDiskSpace
                ( aManager, aFs, aThreshold, aDrive );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyDiskSpace::Start
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyDiskSpace::Start()
    {
    RequestNotification();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyDiskSpace::Stop
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyDiskSpace::Stop()
    {
    Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyDiskSpace::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyDiskSpace::DoCancel()
    {
    iFs.NotifyDiskSpaceCancel();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyDiskSpace::RunL
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyDiskSpace::RunL()
    {
    PBK2_DEBUG_PRINT(   PBK2_DEBUG_STRING
        (   "CPbk2CompressPolicyDiskSpace::RunL() executed" ) );

    // Leave if NotifyDiskSpace signals error
    User::LeaveIfError(iStatus.Int());

    // Disk space is below CL -> initiate a compress
    iManager.CompressStores();
    // Renew notification request
    RequestNotification();
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyDiskSpace::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2CompressPolicyDiskSpace::RunError( TInt /*aError*/ )
    {
    // Compression is a silent background operation -> ignore all errors
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2CompressPolicyDiskSpace::RequestNotification
// --------------------------------------------------------------------------
//
void CPbk2CompressPolicyDiskSpace::RequestNotification()
    {
    Cancel();
    iFs.NotifyDiskSpace( iThreshold, KDefaultDrive, iStatus );
    SetActive();
    }

//  End of File
