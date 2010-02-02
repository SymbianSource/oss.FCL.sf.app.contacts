/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Implements assign ringing tone provider.
*
*/


// INCLUDE FILES
#include "CPbkAiwCmdAssignProvider.h"

#include <AiwMenu.h>
#include <AiwCommon.h>
#include <AiwCommon.hrh>
#include <barsread.h>
#include <PbkAiwProvidersRes.rsg>
#include <Phonebook.rsg>
#include <CPbkContactEngine.h>
#include <AknNoteWrappers.h>
#include <CPbkMultipleEntryFetchDlg.h>
#include "CPbkAssignRingingToneCmd.h"
#include "CPbkAssignImageCmd.h"
#include "PbkDataCaging.hrh"
#include "PbkAiwProviders.hrh"
#include "PbkProviderUtils.h"
#include <CPbkDrmManager.h>

#include <stringloader.h>
#include <centralrepository.h>
#include <ProfileEngineDomainCRKeys.h> //KProEngRingingToneMaxSize
#include <pbkdebug.h>


/// Unnamed namespace for local definitions
namespace {

_LIT(KViewResFileName, "PbkView.rsc");

const TInt Kkilo = 1024;

}

// ================= MEMBER FUNCTIONS =======================

CPbkAiwCmdAssignProvider* CPbkAiwCmdAssignProvider::NewL()
    {
    CPbkAiwCmdAssignProvider* self =
        new(ELeave) CPbkAiwCmdAssignProvider;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkAiwCmdAssignProvider::CPbkAiwCmdAssignProvider()
        : iViewResourceFile(*CCoeEnv::Static())
    {
    }


void CPbkAiwCmdAssignProvider::ConstructL()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkAiwCmdAssignProvider::ConstructL start"));

    BaseConstructL();

    _LIT(KSeparator, "\\");

    {
    TFileName fileName;
    fileName.Copy(KPbkRomFileDrive);
    fileName.Append(KDC_RESOURCE_FILES_DIR);
    fileName.Append(KSeparator);
    fileName.Append(KViewResFileName);
    iViewResourceFile.OpenL(fileName);
    } // TFileName goes out of scope

    iDrmManager = CPbkDrmManager::NewL();
    GetMaxToneFileSizeL( iToneFileSizeLimitKB );

    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkAiwCmdAssignProvider::ConstructL end"));
    }


CPbkAiwCmdAssignProvider::~CPbkAiwCmdAssignProvider()
    {
    delete iDrmManager;
    iViewResourceFile.Close();
    }

void CPbkAiwCmdAssignProvider::HandleServiceCmdL
        (const TInt& aCmdId, const CAiwGenericParamList& aInParamList,
         CAiwGenericParamList& /*aOutParamList*/, TUint /*aCmdOptions*/,
         const MAiwNotifyCallback* aCallback)
    {
    if ((aCmdId == EPbkAiwCmdAssignThumbnail) ||
        (aCmdId == EPbkAiwCmdAssignRingingTone) ||
        (aCmdId == EPbkAiwCmdAssignCallImage))
        {
        // Retrieve filename and file MIME type from AIW param list
        TPtrC fileName = PbkProviderUtils::GetAiwParamAsDescriptor
            (aInParamList, EGenericParamFile);

        TPtrC mimeTypeString = PbkProviderUtils::GetAiwParamAsDescriptor
            (aInParamList, EGenericParamMIMEType);

        // Leave, if there were not given all the required parameters
        if ((fileName == KNullDesC) || (mimeTypeString == KNullDesC))
            {
            User::Leave(KErrArgument);
            }

        VerifyMimeTypeL(aCmdId, mimeTypeString);

        TInt rtSizeLimitError = KErrNone;

        // Ringing tone size limit check returns KErrNone or KErrTooBig
        if (aCmdId == EPbkAiwCmdAssignRingingTone)
            {
            rtSizeLimitError = CheckFileSizeLimit( fileName );
            }

        if ( rtSizeLimitError == KErrNone )
            {
            // Perform the operation
            CmdAssignL(aCmdId, fileName, aInParamList, aCallback);
            }
        }
    }


void CPbkAiwCmdAssignProvider::InitializeMenuPaneL
        (CAiwMenuPane& aMenuPane, TInt aIndex,
         TInt /*aCascadeId*/, const CAiwGenericParamList& aInParamList)
    {
    // Get MIME type string from parameters
    TPtrC mimeTypeString = PbkProviderUtils::GetAiwParamAsDescriptor
        (aInParamList, EGenericParamMIMEType);

    TPtrC fileNameString = PbkProviderUtils::GetAiwParamAsDescriptor
        (aInParamList, EGenericParamFile);

    if ((mimeTypeString.Length() > 0) && (fileNameString.Length() > 0))
        {
        // Map the MIME type
        TInt mimeType = PbkProviderUtils::MapMimeTypeL(mimeTypeString);

        // Create the appropriate menu based on received MIME type
        if (mimeType != EPbkMimeTypeNotSupported)
            {
            TInt menuResId;
            if (mimeType == EPbkMimeTypeImage)
                {
                menuResId = R_AIW_ASSIGN_IMAGE_MENU;
                }
            else
                {
                menuResId = R_AIW_ASSIGN_TONE_MENU;
                }
            TResourceReader reader;
            CCoeEnv::Static()->CreateResourceReaderLC(reader, menuResId);
            aMenuPane.AddMenuItemsL(reader, KAiwCmdAssign, aIndex);
            CleanupStack::PopAndDestroy(); // reader
            }
        }
    }


void CPbkAiwCmdAssignProvider::HandleMenuCmdL
        (TInt aMenuCmdId, const CAiwGenericParamList& aInParamList,
         CAiwGenericParamList& aOutParamList, TUint aCmdOptions,
         const MAiwNotifyCallback* aCallback)
    {
    // Route to HandleServiceCmdL
    HandleServiceCmdL(aMenuCmdId, aInParamList,
        aOutParamList, aCmdOptions, aCallback);
    }


void CPbkAiwCmdAssignProvider::CmdAssignL
        (TInt aMenuCmdId, const TDesC& aFileName,
        const CAiwGenericParamList& aInParamList,
        const MAiwNotifyCallback* aCallback)
    {
    if ( aMenuCmdId == EPbkAiwCmdAssignRingingTone &&
         iDrmManager->IsRingingToneProtectedL( aFileName ) )
        {
        return;
        }
    else if ( aMenuCmdId == EPbkAiwCmdAssignThumbnail &&
         iDrmManager->IsThumbnailProtectedL( aFileName ) )
        {
        return;
        }

    SaveStatusPaneL();

    CContactViewBase& allContactsView = iEngine->AllContactsView();

    CPbkMultipleEntryFetchDlg::TParams params;
    params.iContactView = &allContactsView;

    // Cope with the design problems of AIW framework
    MAiwNotifyCallback* nonConstCallback =
        const_cast<MAiwNotifyCallback*> (aCallback);

    CPbkMultipleEntryFetchDlg* fetchDlg = CPbkMultipleEntryFetchDlg::NewL
        (params, *iEngine);
    const TInt res = fetchDlg->ExecuteLD();
    if (res)
        {
        CleanupStack::PushL(params);

        switch (aMenuCmdId)
            {
            case EPbkAiwCmdAssignThumbnail:
                {
                CPbkAssignImageCmd* cmd =
                    CPbkAssignImageCmd::NewL(
                        aFileName,
                        params.iMarkedEntries,
                        EPbkFieldIdThumbnailImage,
                        *iEngine,
                        aInParamList,
                        nonConstCallback);
                AddAndExecuteCommandL(cmd);
                CleanupStack::Pop(); // params, ownership was taken above
                break;
                }

            case EPbkAiwCmdAssignRingingTone:
                {
                CPbkAssignRingingToneCmd* cmd =
                    CPbkAssignRingingToneCmd::NewL(
                        aFileName,
                        params.iMarkedEntries,
                        *iEngine,
                        aInParamList,
                        nonConstCallback);
                AddAndExecuteCommandL(cmd);
                CleanupStack::Pop(); // params, ownership was taken above
                break;
                }

            default:
                {
                // Unsupported MIME type somehow slipped through,
                // ignore it
                CleanupStack::PopAndDestroy(); // params
                break;
                }
            }
        }
    else
        {
        if (aCallback)
            {
            // Cope with the design errors of AIW framework, it declares
            // the event param list is optional, but still takes it as
            //a reference
            CAiwGenericParamList* eventParamList = NULL;
            nonConstCallback->HandleNotifyL(
                KAiwCmdAssign,
                KAiwEventCanceled,
                *eventParamList,
                aInParamList);
            }
        }

    RestoreStatusPaneL();
    }

void CPbkAiwCmdAssignProvider::VerifyMimeTypeL
        (TInt aMenuCmdId, const TDesC& aMimeTypeString)
    {
    // Map the MIME type
    TInt mimeType = PbkProviderUtils::MapMimeTypeL(aMimeTypeString);

    // Verify MIME type
    if (mimeType == EPbkMimeTypeImage)
        {
        if (aMenuCmdId == EPbkAiwCmdAssignRingingTone)
            {
            User::Leave(KErrArgument);
            }
        }
    else if (mimeType == EPbkMimeTypeAudio)
        {
        if (aMenuCmdId != EPbkAiwCmdAssignRingingTone)
            {
            User::Leave(KErrArgument);
            }
        }
    else if (mimeType == EPbkMimeTypeRingingTone)
        {
        if (aMenuCmdId != EPbkAiwCmdAssignRingingTone)
            {
            User::Leave(KErrArgument);
            }
        }

#ifdef RD_VIDEO_AS_RINGING_TONE

    else if (mimeType == EPbkMimeTypeVideo)
        {
        if (aMenuCmdId != EPbkAiwCmdAssignRingingTone)
            {
            User::Leave(KErrArgument);
            }
        }
#endif      //   RD_VIDEO_AS_RINGING_TONE

    else
        {
        User::Leave(KErrArgument);
        }
    }

// -----------------------------------------------------------------------------
// CPbkAiwCmdAssignProvider::CheckFileSizeLimit
// -----------------------------------------------------------------------------
//
TInt CPbkAiwCmdAssignProvider::CheckFileSizeLimit( const TDesC& aFileName )
    {
    TInt error = KErrNone;

    if ( iToneFileSizeLimitKB  )
        {
        if ( CheckToneFileSize( aFileName, iToneFileSizeLimitKB) != KErrNone )
            {
            TRAP_IGNORE(
                ShowSizeErrorNoteL( iToneFileSizeLimitKB ));
            error = KErrTooBig;
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CPbkAiwCmdAssignProvider::GetMaxToneFileSizeL
// -----------------------------------------------------------------------------
//
void CPbkAiwCmdAssignProvider::GetMaxToneFileSizeL( TInt& aMaxSizeKB ) const
    {
    CRepository* cenrep = CRepository::NewL( KCRUidProfileEngine );
    CleanupStack::PushL( cenrep );
    TInt error = cenrep->Get( KProEngRingingToneMaxSize, aMaxSizeKB );
    CleanupStack::PopAndDestroy( cenrep );
    if ( error != KErrNone )
        {
        aMaxSizeKB = 0;
        }
    if ( aMaxSizeKB < 0 )
        {
         aMaxSizeKB = 0;
        }
    }

// -----------------------------------------------------------------------------
// CPbkAiwCmdAssignProvider::CheckToneFileSize
// -----------------------------------------------------------------------------
//
TInt CPbkAiwCmdAssignProvider::CheckToneFileSize( const TDesC& aFile, TInt aSizeLimitKB )
    {
    // Get file size
    TInt size = 0;
    TInt error = KErrNone;
    RFs fs;
    error = fs.Connect();
    if ( !error )
        {
        TEntry entry;
        error = fs.Entry( aFile, entry );
        if ( !error )
            {
            size = entry.iSize;
            }
        fs.Close();
        }

    // Note: if file size can't be determined, the check fails.
    aSizeLimitKB *= Kkilo;
    if ( aSizeLimitKB  &&  size > aSizeLimitKB )
        {
        error = KErrTooBig;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CPbkAiwCmdAssignProvider::ShowSizeErrorNoteL
// -----------------------------------------------------------------------------
//
void CPbkAiwCmdAssignProvider::ShowSizeErrorNoteL( TInt aSizeLimitKB ) const
    {
    HBufC* errorText = StringLoader::LoadLC( R_PROFILE_TEXT_TONE_MAXSIZE_ERROR, aSizeLimitKB );
    CAknInformationNote* note = new( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *errorText );

    CleanupStack::PopAndDestroy( errorText );
    }

// End of File

