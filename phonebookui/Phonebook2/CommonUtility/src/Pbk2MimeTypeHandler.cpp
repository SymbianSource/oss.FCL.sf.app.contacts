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
* Description:  Phonebook 2 MIME type handler
*
*/


// Phonebook 2
#include <Pbk2MimeTypeHandler.h>

// System includes
#include <e32base.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KMimeTypeImage, "IMAGE" );
_LIT( KMimeTypeAudio, "AUDIO" );
_LIT( KMimeTypeVideo, "VIDEO" );
_LIT( KMimeTypeRingingTone, "APPLICATION/VND.NOKIA.RINGING-TONE" );
_LIT( KMimeTypeRealMedia, "APPLICATION/VND.RN-REALMEDIA" );
_LIT( KMimeTypeRealMediaX, "APPLICATION/X-PN-REALMEDIA" );
_LIT( KMimeTypeStreamDescriptor, "APPLICATION/SDP" );
_LIT( KMimeTypeFlash, "APPLICATION/X-SHOCKWAVE-FLASH" );

_LIT8( KMimeTypeImage8, "IMAGE" );
_LIT8( KMimeTypeAudio8, "AUDIO" );
_LIT8( KMimeTypeVideo8, "VIDEO" );
_LIT8( KMimeTypeRingingTone8, "APPLICATION/VND.NOKIA.RINGING-TONE" );
_LIT8( KMimeTypeRealMedia8, "APPLICATION/VND.RN-REALMEDIA" );
_LIT8( KMimeTypeRealMediaX8, "APPLICATION/X-PN-REALMEDIA" );
_LIT8( KMimeTypeStreamDescriptor8, "APPLICATION/SDP" );
_LIT8( KMimeTypeFlash8, "APPLICATION/X-SHOCKWAVE-FLASH" );

} /// namespace

// --------------------------------------------------------------------------
// Pbk2MimeTypeHandler::MapMimeTypeL
// --------------------------------------------------------------------------
//
EXPORT_C TInt Pbk2MimeTypeHandler::MapMimeTypeL
        ( const TDesC8& aMimeTypeString )
    {
    _LIT8( KSeparator, "/" );

    // Check for a type separator in the string
    TInt pos = aMimeTypeString.Find(KSeparator);
    // Leave if no separator was found.. the MIME
    // standard requires it
    User::LeaveIfError(pos);

    // Construct the compare string
    TPtrC8 extractString(aMimeTypeString.Left(pos));
    HBufC8* compareBuf = extractString.AllocLC();
    TPtr8 compareString = compareBuf->Des();
    compareString.UpperCase();

    // Perform the comparison
    TInt ret = EMimeTypeNotSupported;
    // MIME type case:  IMAGE/*
    if (compareString.CompareF(KMimeTypeImage8) == 0)
        {
        ret = EMimeTypeImage;
        }
    // MIME type case:  AUDIO/*
    else if (compareString.CompareF(KMimeTypeAudio8) == 0)
        {
        ret = EMimeTypeAudio;
        }
    // MIME type case:  VIDEO/*
    else if (compareString.CompareF(KMimeTypeVideo8) == 0)
        {
        ret = EMimeTypeVideo;
        }
    // MIME type case: APPLICATION/VND.NOKIA.RINGING-TONE
    else if (aMimeTypeString.CompareF(KMimeTypeRingingTone8) == 0)
        {
        ret = EMimeTypeAudio;
        }
    // MIME type case: APPLICATION/VND.RN-REALMEDIA
    else if (aMimeTypeString.CompareF(KMimeTypeRealMedia8) == 0)
        {
        ret = EMimeTypeVideo;
        }
    // MIME type case: APPLICATION/X-PN-REALMEDIA
    else if (aMimeTypeString.CompareF(KMimeTypeRealMediaX8) == 0)
        {
        ret = EMimeTypeVideo;
        }
    // MIME type case: APPLICATION/SDP
    else if (aMimeTypeString.CompareF(KMimeTypeStreamDescriptor8) == 0)
        {
        ret = EMimeTypeVideo;
        }
    // MIME type case: APPLICATION/X-SHOCKWAVE-FLASH
    else if (aMimeTypeString.CompareF(KMimeTypeFlash8) == 0)
        {
        // Return value should be EMimeTypeVideo, but flash videos
        // aren't supported yet. Thus, we return default value EMimeTypeNotSupported
        }

    CleanupStack::PopAndDestroy( compareBuf );

    return ret;
    }

// --------------------------------------------------------------------------
// Pbk2MimeTypeHandler::MapMimeTypeL
// --------------------------------------------------------------------------
//
EXPORT_C TInt Pbk2MimeTypeHandler::MapMimeTypeL
        ( const TDesC& aMimeTypeString )
    {
    _LIT( KSeparator, "/" );

    // Check for a type separator in the string
    TInt pos = aMimeTypeString.Find(KSeparator);
    // Leave if no separator was found.. the MIME
    // standard requires it
    User::LeaveIfError(pos);

    // Construct the compare string
    TPtrC extractString(aMimeTypeString.Left(pos));
    HBufC* compareBuf = extractString.AllocLC();
    TPtr compareString = compareBuf->Des();
    compareString.UpperCase();

    // Perform the comparison
    TInt ret = EMimeTypeNotSupported;
    // MIME type case:  IMAGE/*
    if (compareString.CompareF(KMimeTypeImage) == 0)
        {
        ret = EMimeTypeImage;
        }
    // MIME type case:  AUDIO/*
    else if (compareString.CompareF(KMimeTypeAudio) == 0)
        {
        ret = EMimeTypeAudio;
        }
    // MIME type case:  VIDEO/*
    else if (compareString.CompareF(KMimeTypeVideo) == 0)
        {
        ret = EMimeTypeVideo;
        }
    // MIME type case: APPLICATION/VND.NOKIA.RINGING-TONE
    else if (aMimeTypeString.CompareF(KMimeTypeRingingTone) == 0)
        {
        ret = EMimeTypeAudio;
        }
    // MIME type case: APPLICATION/VND.RN-REALMEDIA
    else if (aMimeTypeString.CompareF(KMimeTypeRealMedia) == 0)
        {
        ret = EMimeTypeVideo;
        }
    // MIME type case: APPLICATION/X-PN-REALMEDIA
    else if (aMimeTypeString.CompareF(KMimeTypeRealMediaX) == 0)
        {
        ret = EMimeTypeVideo;
        }
    // MIME type case: APPLICATION/SDP
    else if (aMimeTypeString.CompareF(KMimeTypeStreamDescriptor) == 0)
        {
        ret = EMimeTypeVideo;
        }
    // MIME type case: APPLICATION/X-SHOCKWAVE-FLASH
    else if (aMimeTypeString.CompareF(KMimeTypeFlash) == 0)
        {
        // Return value should be EMimeTypeVideo, but flash videos
        // aren't supported yet. Thus, we return default value EMimeTypeNotSupported
        }

    CleanupStack::PopAndDestroy( compareBuf );

    return ret;
    }

// End of File
