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
* Description:  Phonebook 2 icon information.
*
*/


#include "CPbk2IconInfo.h"

// System includes
#include <barsread.h>
#include <fbs.h>

// --------------------------------------------------------------------------
// CPbk2IconInfo::CPbk2IconInfo
// --------------------------------------------------------------------------
//
CPbk2IconInfo::CPbk2IconInfo()
    {
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::CPbk2IconInfo
// --------------------------------------------------------------------------
//
CPbk2IconInfo::CPbk2IconInfo(
        TPbk2IconId aPbk2IconId,
        TInt aIconId,
        TInt aMaskId,
        TPtrC aMbmFileName,
        TAknsItemID aSkinId,
        TAknsItemID aColorId,
        TInt aColorIndex,
        TInt aDefaultColorIndex ) :
    iPbk2IconId ( aPbk2IconId ),
    iIconId ( aIconId ),
    iMaskId ( aMaskId ),
    iMbmFileName ( aMbmFileName ),
    iSkinId ( aSkinId ),
    iColorId ( aColorId ),
    iColorIndex ( aColorIndex ),
    iDefaultColorIndex( aDefaultColorIndex )
    {
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::CPbk2IconInfo
// --------------------------------------------------------------------------
//
CPbk2IconInfo::CPbk2IconInfo(
    TPbk2IconId aPbk2IconId,
    const CFbsBitmap* aBitmap, 
    const CFbsBitmap* aMask,
    const TSize aSize ) :
        iPbk2IconId ( aPbk2IconId ), 
        iExternalBitmap ( aBitmap ), 
        iExternalMask ( aMask ), 
        iExternalBitmaps( ETrue ), 
        iExternalIconSize( aSize)
    {
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::~CPbk2IconInfo
// --------------------------------------------------------------------------
//
CPbk2IconInfo::~CPbk2IconInfo()
    {
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::NewLC
// --------------------------------------------------------------------------
//
CPbk2IconInfo* CPbk2IconInfo::NewLC( 
        TResourceReader& aReader,
        CDesCArray& aMbmFileNames )
    {
    CPbk2IconInfo* self = new( ELeave ) CPbk2IconInfo;
    CleanupStack::PushL( self );
    self->ConstructL( aReader, aMbmFileNames );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconInfo* CPbk2IconInfo::NewLC(
        TPbk2IconId aPbk2IconId,
        TInt aIconId,
        TInt aMaskId,
        TPtrC aMbmFileName,
        TAknsItemID aSkinId,
        TAknsItemID aColorId,
        TInt aColorIndex,
        TInt aDefaultColorIndex )
    {
    CPbk2IconInfo* self = new( ELeave ) CPbk2IconInfo(
            aPbk2IconId, aIconId, aMaskId, aMbmFileName,
            aSkinId, aColorId, aColorIndex, aDefaultColorIndex );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2IconInfo* CPbk2IconInfo::NewLC( 
        TPbk2IconId aPbk2IconId,
        const CFbsBitmap* aBitmap, 
        const CFbsBitmap* aMask,
        const TSize aSize )
    {
    CPbk2IconInfo* self = new( ELeave ) CPbk2IconInfo(
            aPbk2IconId, aBitmap, aMask, aSize );
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2IconInfo::ConstructL( 
        TResourceReader& aReader,
        CDesCArray& aMbmFileNames )
    {
    aReader.ReadInt8();  // read version number

    iPbk2IconId = TPbk2IconId(aReader);

    HBufC* mbmFileName = aReader.ReadHBufCL();
    TPtr ptr( mbmFileName->Des() );
    TInt pos = KErrNotFound;
    // Find returns less than zero if not found
    if ( aMbmFileNames.Find( ptr, pos ) != 0 )
        {
        CleanupStack::PushL( mbmFileName );
        aMbmFileNames.AppendL( ptr );
        CleanupStack::Pop( mbmFileName );
        const TInt lastIndex = aMbmFileNames.Count() - 1;
        iMbmFileName.Set( aMbmFileNames.MdcaPoint( lastIndex ) );
        }
    else
        {
        iMbmFileName.Set( aMbmFileNames.MdcaPoint( pos ) );
        }
    delete mbmFileName;
    mbmFileName = NULL;

    iIconId = aReader.ReadInt32();
    iMaskId = aReader.ReadInt32();
    const TInt skinIdMajor = aReader.ReadInt32();
    const TInt skinIdMinor = aReader.ReadInt32();
    iSkinId.Set(skinIdMajor, skinIdMinor);
    const TInt colorIdMajor = aReader.ReadInt32();
    const TInt colorIdMinor = aReader.ReadInt32();
    iColorId.Set(colorIdMajor, colorIdMinor);
    iColorIndex = aReader.ReadInt32();
    iDefaultColorIndex = aReader.ReadInt32();
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::Pbk2IconId
// --------------------------------------------------------------------------
//
const TPbk2IconId& CPbk2IconInfo::Pbk2IconId() const
    {
    return iPbk2IconId;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::MbmFileName
// --------------------------------------------------------------------------
//
const TDesC& CPbk2IconInfo::MbmFileName() const
    {
    return iMbmFileName;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::IconId
// --------------------------------------------------------------------------
//
TInt CPbk2IconInfo::IconId() const
    {
    return iIconId;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::MaskId
// --------------------------------------------------------------------------
//
TInt CPbk2IconInfo::MaskId() const
    {
    return iMaskId;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::SkinId
// --------------------------------------------------------------------------
//
TAknsItemID CPbk2IconInfo::SkinId() const
    {
    return iSkinId;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::SkinId
// --------------------------------------------------------------------------
//
TAknsItemID CPbk2IconInfo::ColorId() const
    {
    return iColorId;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::ColorIndex
// --------------------------------------------------------------------------
//
TInt CPbk2IconInfo::ColorIndex() const
    {
    return iColorIndex;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::DefaultColorIndex
// --------------------------------------------------------------------------
//
TInt CPbk2IconInfo::DefaultColorIndex() const
    {
    return iColorIndex;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::ExternalBitmap
// --------------------------------------------------------------------------
//
const CFbsBitmap* CPbk2IconInfo::ExternalBitmap() const
    {
    return iExternalBitmap;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::Bitmap
// --------------------------------------------------------------------------
//
const CFbsBitmap* CPbk2IconInfo::ExternalMask() const
    {
    return iExternalMask;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::IsIconExternal
// --------------------------------------------------------------------------
//
TBool CPbk2IconInfo::IsIconExternal() const
    {
    return iExternalBitmaps;
    }

// --------------------------------------------------------------------------
// CPbk2IconInfo::ExternalIconSize
// --------------------------------------------------------------------------
//
TSize CPbk2IconInfo::ExternalIconSize() const
    {
    return iExternalIconSize;
    }

// End of File
