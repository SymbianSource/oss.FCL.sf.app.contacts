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
*           Phonebook DRM policy managing class
*
*/


// INCLUDE FILES
#include <CPbkDrmManager.h>
#include <PbkConfig.hrh>

#include <featmgr.h>
#include <DRMCommon.h>
#include <bldvariant.hrh>
#include <PbkView.rsg>
#include <eikenv.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <apgcli.h>                     // RApaLsSession
#include <MusicPlayerInternalCRKeys.h>  // KCRUidMusicPlayerFeatures
#include <centralrepository.h>          // CRepository
#include <DRMHelper.h>                  // CDRMHelper

namespace {
    
// CONSTANTS

// Blocked mimetypes when DRM is enforced. Keep the last space in list.
// List comes from 107-14385: VFKK: Updates to non-DRM audio blocking requirements
_LIT( KBlockedMimeTypes, "audio/amr-wb audio/mp4 audio/3gpp audio/3gpp2 audio/aac audio/mpeg audio/x-wav audio/wav audio/x-ms-wma ");

} // namespace

// ================= MEMBER FUNCTIONS =======================

inline CPbkDrmManager::CPbkDrmManager() :
    iDrmEnabled(EFalse)
    {
    }

inline void CPbkDrmManager::ConstructL()
    {
    // Sets up TLS, must be done before FeatureManager is used.
    FeatureManager::InitializeLibL();

    iDrmHelper = CDRMHelper::NewL();
    iDrmClient = DRMCommon::NewL();
    if (iDrmClient->Connect() == DRMCommon::EOk)
        {
        // if unable to connect, DRM protection is always on
        iDrmEnabled = ETrue;
        }
    }

EXPORT_C CPbkDrmManager* CPbkDrmManager::NewL()
    {
    CPbkDrmManager* self = new(ELeave) CPbkDrmManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CPbkDrmManager::~CPbkDrmManager()
    {
    if (iDrmClient)
        {
        iDrmClient->Disconnect();
        }
    delete iDrmClient;

    delete iDrmHelper;
    iDrmHelper = NULL;

    FeatureManager::UnInitializeLib();
    }

EXPORT_C TBool CPbkDrmManager::IsProtectedFile
        (const TDesC& aFileName)
    {
    TBool result( ETrue );
    const TBool drmSupported =
        (FeatureManager::FeatureSupported(KFeatureIdDrm)
        || FeatureManager::FeatureSupported(KFeatureIdDrmFull));

    if (!drmSupported)
        {
        // No DRM support, file retrieval always ok
        result = EFalse;
        }
    else if (iDrmEnabled && drmSupported)
        {
        TBool protection( ETrue );
        // Check is the file protected
        TInt drmErrorCode = iDrmClient->IsProtectedFile(aFileName, protection);

        switch (drmErrorCode)
            {
            case KErrInUse:
                {
                // File is in use. Assume that file is protected.
                // Otherwise there is a threat that protected file 
                // is handled as unprotected.
                result = ETrue;                                
                break;
                }

            case DRMCommon::EOk:    // FALLTHROUGH
            default:
                {
                result = protection;
                break;
                }
            }
        }
    return result;
    }

EXPORT_C TBool CPbkDrmManager::IsRingingToneProtectedL( const TDesC& aFileName )
    {
    TInt result( KErrGeneral );
    const TBool drmSupported =
        (FeatureManager::FeatureSupported(KFeatureIdDrm)
        || FeatureManager::FeatureSupported(KFeatureIdDrmFull));

    if (!drmSupported)
        {
        // No DRM support, file retrieval always ok
        result = KErrNone;
        }
    else
        {
        // DRM is enabled. File must be checked
        if (IsProtectedFile(aFileName))
            {
            result = CheckProtectedFileL(aFileName);
            }
        else
            {
            result = CheckUnprotectedFileL(aFileName);
            }
        }
    if (result == KErrNone)
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

void CPbkDrmManager::ShowErrorNoteL( TInt aResource )
    {
    CEikonEnv* env = CEikonEnv::Static();
    HBufC* noteText = StringLoader::LoadLC( aResource, env );

    CAknErrorNote* note =
        new(ELeave)CAknErrorNote( R_AKN_INFORMATION_NOTE_WAIT );
    note->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy( noteText ); //noteText
    }


EXPORT_C TBool CPbkDrmManager::IsThumbnailProtectedL( const TDesC& aFileName )
    {
    // Thumbnails with any DRM protection are not allowed
    if (IsProtectedFile( aFileName ) )
        {
        ShowErrorNoteL( R_QTN_DRM_NOT_ALLOWED );
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


EXPORT_C TBool CPbkDrmManager::HasFileRestrictedRights( const TDesC& aFileName )
    {
    TInt result( KErrGeneral );
    DRMCommon::TContentProtection protection;
    HBufC8* mimeType = NULL; // ignored
    HBufC8* contentUri = NULL;
    TUint dataLength; // ignored

    const TBool drmSupported =
        (FeatureManager::FeatureSupported(KFeatureIdDrm)
        || FeatureManager::FeatureSupported(KFeatureIdDrmFull));

    if (!drmSupported)
        {
        // No DRM support, file retrieval always ok
        result = KErrNone;
        }
    else
        {
        // DRM is enabled. File must be checked
        TInt err = iDrmClient->GetFileInfo( aFileName, protection, mimeType,
                                            contentUri, dataLength );
        delete mimeType;
        if( err )
            {
            // Some error occured. Treat the file as protected
            result = err;
            }
        else if ( protection == DRMCommon::ENoDCFFile )
            {
            result = KErrNone;
            }
        else
            {
            CDRMRights* rights = NULL;
            err = iDrmClient->GetActiveRights( *contentUri, DRMCommon::EPlay, rights );

            if ( err == CDRMRights::EFullRights )
                {
                result = KErrNone;
                }

            if ( rights )
                {
                delete rights;
                }
            }
        }

    if ( contentUri )
        {
        delete contentUri;
        }

    if ( result == KErrNone )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }


TBool CPbkDrmManager::IsDrmRequiredForPlaybackL()
    {
    // It is possible to disable support for non-DRM protected rich audio
    // formats.
    CRepository* musicPlayerFeatures =
        CRepository::NewLC(KCRUidMusicPlayerFeatures);
    TInt value=0;
    User::LeaveIfError(
        musicPlayerFeatures->Get(KRequireDRMInPlayback, value));
    CleanupStack::PopAndDestroy(musicPlayerFeatures);
    return static_cast<TBool>(value);
    }


 TBool CPbkDrmManager::IsBlockedMimeTypeL( const TDesC& aMimeType)
    {
    // Add space to aMimeType for searching (FindF() would find "audio/3gpp" in
    // "audio/3gpp2" without the added space).
    TBool isBlocked(EFalse);
    if( aMimeType.Length() > 0 )
        {
        HBufC* mimeBuf = HBufC::NewLC(aMimeType.Length() + 1);
        mimeBuf->Des().Copy(aMimeType);
        mimeBuf->Des().Append(' ');
        isBlocked = (KBlockedMimeTypes().FindF(*mimeBuf) != KErrNotFound);
        CleanupStack::PopAndDestroy(mimeBuf);
        }
    return isBlocked;
    }


TInt CPbkDrmManager::CheckProtectedFileL( const TDesC& aFileName )
    {
    TBool automatedOk(EFalse);
    
    // Check that file can be set as automated content.
    TInt err = iDrmHelper->CanSetAutomated(aFileName,
            automatedOk);        
    if (err==KErrNone && !automatedOk)
        {
        ShowErrorNoteL(R_QTN_PHOB_DRM_PREV_RIGHTS_SET);     
        return KErrGeneral;        
        }                        
    if (err!=KErrNone)
        {
        // Locked file case is handled separately.
        if (err==KErrInUse)
            {
            CCoeEnv::Static()->HandleError(KErrInUse);
            }            
        else
            {
            iDrmHelper->HandleErrorL(err,aFileName);
            }            
        return KErrGeneral;
        }
    
    DRMCommon::TContentProtection protection;
    HBufC8* mimeType = NULL; // ignored
    HBufC8* contentUri = NULL;
    TUint dataLength; // ignored
    err = iDrmClient->GetFileInfo( aFileName, protection, mimeType,
                                        contentUri, dataLength );
    delete mimeType;
    if( err )
        {
        delete contentUri;
        User::Leave( err );
        }

    TInt result( KErrGeneral );
    CDRMRights* rights = NULL;
    err = iDrmClient->GetActiveRights( *contentUri, DRMCommon::EPlay, rights );

    switch ( err )
        {
        case CDRMRights::EFullRights: // Fall through
        case CDRMRights::ERestrictedRights:
            {
            CDRMRights::TRestriction restriction; // ignore
            CDRMRights::TExpiration expiration;
            TUint32 constType;

            rights->GetRightsInfo( DRMCommon::EPlay, restriction, // ignore return
                               expiration, constType );       // value


            // This is CFM protected file, ringingtone is set to "no"
            if ( ( rights->GetPermission().iInfoBits & ENoRingingTone )
                // This is normal DRM protected tone
                || ( constType & ( CDRMRights::ECountBased |
                                   CDRMRights::ETimeIsAccumulatedTime ) ) )
                {
                ShowErrorNoteL( R_QTN_PHOB_DRM_PREV_RIGHTS_SET );
                }
            else
                {
                switch ( expiration )
                    {
                    case CDRMRights::EValidRights:
                        {
                        result=KErrNone;
                        break;
                        }

                    case CDRMRights::EFutureRights:
                        {
                        iDrmHelper->HandleErrorL( DRMCommon::ENoRights, aFileName );
                        break;
                        }

                    case CDRMRights::EExpiredRights:
                        {
                        iDrmHelper->HandleErrorL( DRMCommon::ERightsExpired, aFileName );
                        break;
                        }

                    default:
                        {
                        break;
                        }
                    }
                }
            break;
            }

        case CDRMRights::EPreviewRights:
            {
            ShowErrorNoteL( R_QTN_PHOB_DRM_PREV_RIGHTS_SET );
            break;
            }

        case DRMCommon::ENoRights:
            {
            iDrmHelper->HandleErrorL( DRMCommon::ENoRights, aFileName );
            break;
            }

        default:
            {
            break;
            }

        }

    delete rights;
    delete contentUri;
    return result;
    }


TInt CPbkDrmManager::CheckUnprotectedFileL(const TDesC& aFileName)
    {
    TInt result = KErrNone;
    RApaLsSession apaLs;
    User::LeaveIfError( apaLs.Connect() );
    TUid appUidNotUsed = KNullUid;
    TDataType dataType;
    TInt err = apaLs.AppForDocument( aFileName, appUidNotUsed, dataType );
    apaLs.Close();
    User::LeaveIfError( err );
    // Check if this unprotected MIME type should be blocked
    if (IsDrmRequiredForPlaybackL() &&
        IsBlockedMimeTypeL(dataType.Des()))
        {
        result = KErrPermissionDenied;
        ShowErrorNoteL( R_QTN_PHOB_UNPROTECTED_TONE );
        }
    return result;
    }

//  End of File
