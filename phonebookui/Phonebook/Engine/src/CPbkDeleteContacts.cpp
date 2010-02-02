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
*		Multiple contact deletion process
*
*/

// INCLUDE FILES
#include "CPbkDeleteContacts.h"
#include <cntdef.h>
#include <cntdb.h>
#include <sysutil.h>
#include "CPbkContactEngine.h"
#include <PbkDebug.h>
#include "PbkCompressConfig.h"


// LOCAL CONSTANTS
const TInt KB = 1024;


// ================= MEMBER FUNCTIONS =======================

inline TBool CPbkDeleteContacts::MoreContactsToDelete() const
    {
    return (iContacts && iContacts->Count() > 0);
    }

inline void CPbkDeleteContacts::Cancel()
    {
    delete iContacts;
    iContacts = NULL;
    }

inline TInt64 CPbkDeleteContacts::FreeSpaceOnDbDrive() const
    {
    TVolumeInfo volInfo;
    volInfo.iFree = 0;
    iEngine.FsSession().Volume(volInfo,iDbDrive);
    return volInfo.iFree;
    }

inline CPbkDeleteContacts::CPbkDeleteContacts
        (CPbkContactEngine& aEngine, RSharedDataClient& aSharedDataClient) :
    iEngine(aEngine),
    iFreeSpaceRequiredToDelete(8*KB),// 8 kB is an approximation of needed space
    iSharedDataClient(aSharedDataClient)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContacts::CPbkDeleteContacts(0x%x)"),
        this);
    }

inline void CPbkDeleteContacts::ConstructL(const CContactIdArray& aContacts)
    {
    iTotalContactsToDelete = aContacts.Count();
    iContacts = CContactIdArray::NewL(&aContacts);
    iContacts->ReverseOrder();    
    TDriveUnit dbDrive;
    iEngine.Database().DatabaseDrive(dbDrive);
    iDbDrive = dbDrive;
    }

EXPORT_C CPbkDeleteContacts* CPbkDeleteContacts::NewLC
        (CPbkContactEngine& /*aEngine*/,
        const CContactIdArray& /*aContacts*/)
    {
    User::Leave(KErrNotSupported);
    return NULL;
    }

CPbkDeleteContacts* CPbkDeleteContacts::NewLC
        (CPbkContactEngine& aEngine,
        const CContactIdArray& aContacts,
        RSharedDataClient& aSharedDataClient)
    {
    CPbkDeleteContacts* self = new(ELeave) CPbkDeleteContacts(aEngine, aSharedDataClient);
    CleanupStack::PushL(self);
    self->ConstructL(aContacts);
    return self;
    }


CPbkDeleteContacts::~CPbkDeleteContacts()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContacts::~CPbkDeleteContacts(0x%x)"),
        this);    
    delete iContacts;
    }

TInt CPbkDeleteContacts::DeletedCount() const
    {
    return iDeletedCount;
    }

void CPbkDeleteContacts::StepL()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContacts::StepL()"),
        this);
    CContactDatabase& db = iEngine.Database();

    CompressIfRequired();
    if (MoreContactsToDelete())
        {
        const TInt sizeBefore = db.FileSize();
        
        iSharedDataClient.RequestFreeDiskSpaceLC(iFreeSpaceRequiredToDelete);
        const TInt index = iContacts->Count() - 1;
        const TContactItemId id = (*iContacts)[index];
        iContacts->Remove(index);

        db.DeleteContactL(id);
        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("    deleted contact %d"), id);
        ++iDeletedCount;
        CleanupStack::PopAndDestroy();  // RequestFreeDiskSpaceLC

        // Calculate how much database grew rounded to next kB
        TInt sizeDiff = db.FileSize() - sizeBefore + KB;
        sizeDiff -= sizeDiff % KB;
        if (sizeDiff > iFreeSpaceRequiredToDelete)
            {
            // Update maximum size required for deletion
            iFreeSpaceRequiredToDelete = sizeDiff;
            }
        }
    }

TInt CPbkDeleteContacts::TotalNumberOfSteps()
    {
    return iTotalContactsToDelete;
    }

TBool CPbkDeleteContacts::IsProcessDone() const
    {
    return !MoreContactsToDelete();
    }

void CPbkDeleteContacts::ProcessFinished()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContacts::ProcessFinished()"),
        this);

    // Final compress
    CompressIfRequired();
    Cancel();
    }

TInt CPbkDeleteContacts::HandleStepError(TInt aError)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkDeleteContacts::HandleStepError(%d)"), aError);

    TInt result = aError;
    switch (aError)
        {
        case KErrNotFound:  // FALLTHROUGH
        case KErrInUse:     // FALLTHROUGH
            {
			// Ignore these errors
			// KErrNotFound means that somebody got the contact first
			// KErrInUse means that the contact is open
            result = KErrNone;
            break;
            }

        default:  // Something more serious happened -> give up
            {
            Cancel();
            break;
            }
        }

    return result;
    }

void CPbkDeleteContacts::ProcessCanceled()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkDeleteContacts::ProcessCanceled()"),
        this);

    Cancel();
    }

void CPbkDeleteContacts::CompressIfRequired()
    {
    // Cancel any async compress first
    iEngine.CancelCompress();
    CContactDatabase& db = iEngine.Database();
    // Compress synchronously always if compression is required by the DB
    // or file system free space is below what is needed for deletion
    if (PbkCompressConfig::CompressRequired(db) ||
        FreeSpaceOnDbDrive() < iFreeSpaceRequiredToDelete)
        {
        TRAPD(result, db.CompactL());
        PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("CPbkDeleteContacts::CompressIfRequired(): compacted database, result=%d"), result);
        }
    }


//  End of File
