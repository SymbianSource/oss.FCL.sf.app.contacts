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
* Description: 
*           Phonebook 2 DRM manager.
*
*/


// INCLUDE FILES
#include "CPbk2DrmManager.h"

// Phonebook 2
#include <pbk2commonui.rsg>

// System includes
#include <featmgr.h>
#include <DRMCommon.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <apgcli.h>                     // RApaLsSession
#include <musicplayerdomaincrkeys.h>    // KCRUidMusicPlayerFeatures
#include <centralrepository.h>
#include <DRMHelper.h>
#include <coemain.h>

#ifdef RD_DRM_COMMON_INTERFACE_FOR_OMA_AND_WMDRM
#include <drmutility.h>
#include <drmagents.h>
#endif

/// Unnamed namespace for local definitions
namespace {

_LIT(KSpaceTxt, " ");

} /// namespace

// --------------------------------------------------------------------------
// CPbk2DrmManager::CPbk2DrmManager
// --------------------------------------------------------------------------
//
CPbk2DrmManager::CPbk2DrmManager() :
        iDrmEnabled( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPbk2DrmManager::~CPbk2DrmManager
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2DrmManager::~CPbk2DrmManager()
    {
    if ( iDrmClient )
        {
        iDrmClient->Disconnect();
        }
    delete iDrmClient;
    delete iDrmHelper;
    delete iMusicPlayerFeatures;
    
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPbk2DrmManager::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2DrmManager* CPbk2DrmManager::NewL()
    {
    CPbk2DrmManager* self = new ( ELeave ) CPbk2DrmManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2DrmManager::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2DrmManager::ConstructL()
    {
    // Sets up TLS, must be done before FeatureManager is used
    FeatureManager::InitializeLibL();

    iDrmHelper = CDRMHelper::NewL();
    iDrmClient = DRMCommon::NewL();
    if ( iDrmClient->Connect() == DRMCommon::EOk )
        {
        // If unable to connect, DRM protection is always on
        iDrmEnabled = ETrue;
        }
    
    iMusicPlayerFeatures = CRepository::NewLC( KCRUidMusicPlayerFeatures );
    CleanupStack::Pop(); // iMusicPlayerFeatures
    }

#ifndef RD_DRM_COMMON_INTERFACE_FOR_OMA_AND_WMDRM

// Some magic constants
static const TInt KMinContentLength( 16 );
_LIT8( KContentProtectionType, "DRM" );
_LIT8( KASFHeaderObject, "75B22630668E11CFA6D900AA0062CE6C" );


// -----------------------------------------------------------------------------
// FormatGUID
// -----------------------------------------------------------------------------
//
LOCAL_C void FormatGUID( TDes8& aGUID )
    {
    TBuf8<16> copyGUID( aGUID );
    TInt i;
    for( i = 0; i < 4; i++ )
        {
        copyGUID[i] = aGUID[3-i];
        }
    for( i = 4; i < 6; i++ )
        {
        copyGUID[i] = aGUID[9 - i];
        }
    for( i = 6; i < 8; i++ )
        {
        copyGUID[i] = aGUID[13 - i];
        }
    for( i = 8; i < 16 ; i++ )
        {
        copyGUID[i] = aGUID[i];
        }
    aGUID.Delete( 0, 32 );
    for( i = 0; i <16; i++ )
        {
        aGUID.AppendNumFixedWidthUC( copyGUID[i], EHex, 2 );
        }
    }

// -----------------------------------------------------------------------------
// ConvertToInt64
// -----------------------------------------------------------------------------
//
LOCAL_C TInt64 ConvertToInt64( TDesC8& aDes )
    {
    TInt64 num = 0;
    TInt i;
    for( i = 7 ; i >= 0; i-- )
        {
        num <<= 8;
        num |= aDes[i];
        }
    return num;
    }


// -----------------------------------------------------------------------------
// IsProtectedWmDrmL
// returns ETrue, if file is protected WMDRM file
//         EFalse if file is not protected WMDRM file
// Leaves with KErrUnderflow if file has too little data to decide
//         whether WmDrm or not
//         may also leave with other system wide error code
// -----------------------------------------------------------------------------
//
LOCAL_C TBool IsProtectedWmDrmL( RFile& aFileHandle )
    {
    TInt r( KErrNone );
    HBufC8* buffer( NULL );
    TInt pos( 0 );
    RFile file;
    TBuf8< 32 > header;

    TInt64 headerSize( 0 );
    TBool isProtectedWmDrm( EFalse );
    TPtr8 headerPtr( NULL, 0 );

    // Leave if given handle is invalid
    if( !aFileHandle.SubSessionHandle() )
        {
        User::Leave( KErrBadHandle );
        }

    file.Duplicate( aFileHandle );
    CleanupClosePushL( file );

    User::LeaveIfError( file.Seek( ESeekStart, pos ) );

    // Check if the file is an ASF file
    // TODO: Check on runtime wether WM DRM is supporeted or not
    //       Take Implementation from DRM::CDrmUtility::GetDrmInfoL,
    //       which is not yet available

    User::LeaveIfError( file.Read( 0, header, KMinContentLength ) );
    if( header.Length() < KMinContentLength )
        {
        User::Leave( KErrUnderflow );
        }

    FormatGUID( header );

    if( header == KASFHeaderObject )
        {
        // It's ASF, check still whether it's WM DRM protected or not
        file.Read( header,8 );
        headerSize = ConvertToInt64( header );
        if( headerSize <= 30 )
            {
            User::Leave( KErrUnderflow );
            }
        if ( headerSize > KMaxTInt16 )
            {
            User::Leave( KErrOverflow );
            }
        buffer = HBufC8::NewLC( headerSize );

        headerPtr.Set( buffer->Des() );
        User::LeaveIfError( file.Read( headerPtr, headerSize - 24 ) );

        r = headerPtr.Find( KContentProtectionType );
        if ( KErrNotFound != r )
            {
            isProtectedWmDrm = ETrue;
            }
        CleanupStack::PopAndDestroy( buffer ); // buffer
        }
        CleanupStack::PopAndDestroy(); // file

    return isProtectedWmDrm;
    }

#endif // RD_DRM_COMMON_INTERFACE_FOR_OMA_AND_WMDRM

// -----------------------------------------------------------------------------
// CPbk2DrmManager::IsFileWMDRMProtectedL
// -----------------------------------------------------------------------------
//
TBool CPbk2DrmManager::IsFileWMDRMProtectedL( const TDesC& aFileName,
											TBool& aIsProtected  ) const
	{
	TBool res = EFalse;
	RFs& fsSession( CCoeEnv::Static()->FsSession() );
	RFile hFile;

	TInt err = hFile.Open( fsSession, aFileName, 
						EFileRead | EFileStream | EFileShareReadersOnly );
	if( err == KErrInUse )
		{
		err = hFile.Open( fsSession, aFileName, 
						EFileRead | EFileStream | EFileShareAny );
		}
	if( err != KErrNone )
		{
		User::Leave( err );
		}
	CleanupClosePushL( hFile );

#ifdef RD_DRM_COMMON_INTERFACE_FOR_OMA_AND_WMDRM
	TPtrC agent( KNullDesC );
	DRM::CDrmUtility* drmUtil( DRM::CDrmUtility::NewLC() );
	drmUtil->GetAgentL( hFile, agent );
	if( agent.Compare( DRM::KDrmWMAgentName ) == 0 )
		{
		res = ETrue;
		}
	else
		{
		res = iDrmClient->IsProtectedFile( aFileName, aIsProtected );	
		}	
	CleanupStack::PopAndDestroy( drmUtil );
#else
	res = IsProtectedWmDrmL( hFile );
	aIsProtected = EFalse;
#endif

	CleanupStack::PopAndDestroy( &hFile );
	return res;
	}

// --------------------------------------------------------------------------
// CPbk2DrmManager::IsProtectedFile
// --------------------------------------------------------------------------
//

EXPORT_C TInt CPbk2DrmManager::IsProtectedFile(
        const TDesC& aFileName,
        TBool& aIsProtected )
    {
    aIsProtected = ETrue;
        
    const TBool drmSupported =
        ( FeatureManager::FeatureSupported( KFeatureIdDrm )||
          FeatureManager::FeatureSupported( KFeatureIdDrmFull ) );

    TInt drmErrorCode( KErrNone );
    if ( !drmSupported )
        {
        // No DRM support, file retrieval always ok
        aIsProtected = EFalse;
        }
    else if ( iDrmEnabled )
        {
        // Check is the file protected
		drmErrorCode = IsFileWMDRMProtectedL( aFileName,aIsProtected );
        }
    return drmErrorCode;
    }

// --------------------------------------------------------------------------
// CPbk2DrmManager::IsRingingToneForbidden
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2DrmManager::IsRingingToneForbidden( 
        const TDesC& aFileName,
        TBool& aIsProtected )
    {
    aIsProtected = ETrue;
    TInt error( KErrNone );
    const TBool drmSupported =
        ( FeatureManager::FeatureSupported( KFeatureIdDrm ) ||
          FeatureManager::FeatureSupported( KFeatureIdDrmFull ) );

    if ( !drmSupported )
        {
        // No DRM support, file retrieval always ok
        aIsProtected = EFalse;
        }
    else
        {
        // DRM is enabled. File must be checked
        error = IsProtectedFile( aFileName, aIsProtected );
        if ( error == KErrNone )
            {
            if ( aIsProtected )
                {
                TRAP( error, CheckProtectedFileL( aFileName, aIsProtected ) );
                }
            else
                {
                TRAP( error, CheckUnprotectedFileL( aFileName, aIsProtected ) );
                }
            }
         else
         	{
         	error = 0;
         	TRAP_IGNORE( ShowErrorNoteL( R_QTN_PHOB_PROTECTED_TONE ) );
         	}           
        }
    return error;       
    }

// --------------------------------------------------------------------------
// CPbk2DrmManager::IsThumbnailForbidden
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2DrmManager::IsThumbnailForbidden( 
        const TDesC& aFileName,
        TBool& aIsProtected )
    {
    aIsProtected = ETrue;
    TInt error = IsProtectedFile( aFileName, aIsProtected );
    
    // Thumbnails with any DRM protection are not allowed
    if ( error == KErrNone && aIsProtected )
        {
        TRAP_IGNORE( ShowErrorNoteL( R_PBK2_QTN_DRM_NOT_ALLOWED ) );
        }
    
    return error;
    }

// --------------------------------------------------------------------------
// CPbk2DrmManager::ShowErrorNoteL
// --------------------------------------------------------------------------
//
void CPbk2DrmManager::ShowErrorNoteL( TInt aResource )
    {
    CEikonEnv* env = CEikonEnv::Static();
    HBufC* noteText = StringLoader::LoadLC( aResource, env );

    // DRM notifications are shown as waiting information notes
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *noteText );

    CleanupStack::PopAndDestroy( noteText ); //noteText
    }

// --------------------------------------------------------------------------
// CPbk2DrmManager::IsDrmRequiredForPlaybackL
// --------------------------------------------------------------------------
//
TBool CPbk2DrmManager::IsDrmRequiredForPlaybackL()
    {
    // It is possible to disable support for non-DRM protected rich audio
    // formats    
    TInt value( 0 );
    User::LeaveIfError(
        iMusicPlayerFeatures->Get( KRequireDRMInPlayback, value ) );    
    return static_cast<TBool>( value );
    }

// --------------------------------------------------------------------------
// CPbk2DrmManager::IsBlockedMimeTypeL
// --------------------------------------------------------------------------
//
TBool CPbk2DrmManager::IsBlockedMimeTypeL( const TDesC& aMimeType )
    {
    TBool ret = EFalse;    
    TBuf<1> tmp; // Magic: 1 char to get length of actual value
    TInt realLen = 0;
    TInt err = iMusicPlayerFeatures->Get(
        KPlaybackRestrictedMimeTypes, tmp, realLen );
    if ( err == KErrOverflow )
        {
        // Prepare list of blocked MIME types
        HBufC* listBuf = HBufC::NewLC( realLen + KSpaceTxt().Length() );
        TPtr list = listBuf->Des();
        User::LeaveIfError(
            iMusicPlayerFeatures->Get
                ( KPlaybackRestrictedMimeTypes, list ) );
        list.Append( KSpaceTxt );

        // Prepare buffer for aMimeType
        HBufC* mimeBuf = HBufC::NewLC( aMimeType.Length() +
                                       KSpaceTxt().Length() );
        TPtr mime = mimeBuf->Des();
        mime = aMimeType;
        // FindF() would find "audio/3gpp" in "audio/3gpp2" without
        // the added space
        mime.Append( KSpaceTxt );

        // If result is not KErrNotFound, this MIME-type is indeed blocked
        ret = ( list.FindF( mime ) >= 0 );

        CleanupStack::PopAndDestroy(2); // listBuf, mimeBuf
        }
    else
        {
        User::LeaveIfError( err );
        }    
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2DrmManager::CheckProtectedFileL
// --------------------------------------------------------------------------
//
void CPbk2DrmManager::CheckProtectedFileL( 
        const TDesC& aFileName,
        TBool& aProtected )
    {
    aProtected = ETrue;
    TBool automatedOk(EFalse);

    // Check that file can be set as automated content.
    TInt err = iDrmHelper->CanSetAutomated( aFileName, automatedOk );
    if ( err==KErrNone )
        {
        if ( !automatedOk )
            {
            ShowErrorNoteL(R_QTN_PHOB_DRM_PREV_RIGHTS_SET);
            }
        else
            {
            DRMCommon::TContentProtection protection;
            HBufC8* mimeType = NULL; // ignored
            HBufC8* contentUri = NULL;
            TUint dataLength; // ignored
            err = iDrmClient->GetFileInfo( 
                    aFileName, protection, mimeType, contentUri, dataLength );
            delete mimeType;
            mimeType = NULL;
            if( err )
                {
                delete contentUri;
                User::Leave( err );
                }

            CDRMRights* rights = NULL;
            err = iDrmClient->GetActiveRights( 
                    *contentUri, DRMCommon::EPlay, rights );

            switch ( err )
                {
                case CDRMRights::EFullRights: // Fall through
                case CDRMRights::ERestrictedRights:
                    {
                    CDRMRights::TRestriction restriction; // ignore
                    CDRMRights::TExpiration expiration;
                    TUint32 constType;

                    // Ignore return value
                    rights->GetRightsInfo( 
                        DRMCommon::EPlay, restriction, expiration, constType );

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
                                aProtected = EFalse;
                                break;
                                }
                            case CDRMRights::EFutureRights:
                                {
                                iDrmHelper->HandleErrorL( 
                                    DRMCommon::ENoRights, aFileName );
                                break;
                                }
                            case CDRMRights::EExpiredRights:
                                {
                                iDrmHelper->HandleErrorL( 
                                    DRMCommon::ERightsExpired, aFileName );
                                break;
                                }
                            default:
                                {
                                // Do nothing
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
                    // Do nothing
                    break;
                    }
                }

            delete rights;
            delete contentUri;
            }
        }
    else
        {
        // Locked file case is handled separately.
        if ( err==KErrInUse )
            {
            CCoeEnv::Static()->HandleError( KErrInUse );
            }
        else
            {
            // Leave if DrmHelper not handle the error
            User::LeaveIfError( iDrmHelper->HandleErrorL(err,aFileName) );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2DrmManager::CheckUnprotectedFileL
// --------------------------------------------------------------------------
//
void CPbk2DrmManager::CheckUnprotectedFileL( 
        const TDesC& aFileName, 
        TBool& aProtected )
    {
    aProtected = ETrue;    
    RApaLsSession apaLs;
    User::LeaveIfError( apaLs.Connect() );
    TUid appUidNotUsed = KNullUid;
    TDataType dataType;
    TInt err = apaLs.AppForDocument( aFileName, appUidNotUsed, dataType );
    apaLs.Close();
    User::LeaveIfError( err );

    // Check if this unprotected MIME type should be blocked
    if ( IsDrmRequiredForPlaybackL() &&
         IsBlockedMimeTypeL( dataType.Des() ) )
        {
        ShowErrorNoteL( R_QTN_PHOB_UNPROTECTED_TONE );
        }
    else
        {
        aProtected = EFalse;
        }
    }

//  End of File
