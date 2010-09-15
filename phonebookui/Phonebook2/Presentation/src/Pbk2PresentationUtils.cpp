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
*
*/


// INCLUDE FILES
#include "Pbk2PresentationUtils.h"

// From system
#include <featmgr.h>
#include <fbs.h>

/// Namespace for local PresentationUtils definitions
namespace __RVCT_UNS_Pbk2PresentationUtils {

// LOCAL CONSTANTS
enum TPanicCode
    {
    EPanicPreCond_CopyAndReplaceChars = 1
    };

// default presentation resource file
_LIT( KPbk2PresentationResFile, "Pbk2Presentation.rsc" );
// Chinese presentation resource file 
_LIT( KPbk2PresentationResFileChinese, "Pbk2PresentationChinese.rsc" );
// UnSync Chinese presentation resource file 
_LIT( KPbk2PresentationResFileChineseUnSync, "Pbk2PresentationChineseUnSync.rsc" );

// ================= LOCAL FUNCTIONS =======================
#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "Pbk2PresentationUtils");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

// Zero Width characters
const TUint KZWSCharacter = 0x200B;
const TUint KZWNJCharacter = 0x200C;
const TUint KZWJCharacter = 0x200D;

// ================= LOCAL FUNCTIONS =======================

/** 
 * Check if the given char is a zero width character: 
 * Zero Width Space, Zero Width Non-Joiner
 * or Zero Width Joiner character
 * @param aChar Given character
 * @return ETrue if aChar is zero width character.
 *         EFalse if aChar is not specified zero width character.
 */
inline TBool ZWSCharacter( const TChar aChar )
    {
    const TChar zwsChar( KZWSCharacter );
    const TChar zwnjChar( KZWNJCharacter );
    const TChar zwjChar( KZWJCharacter );
    return ( aChar == zwsChar ) || ( aChar == zwnjChar ) ||
           ( aChar == zwjChar );
    }
      
} // __RVCT_UNS_Pbk2PresentationUtils

using namespace __RVCT_UNS_Pbk2PresentationUtils;


// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// Pbk2PresentationUtils::TrimAllAppend
// --------------------------------------------------------------------------
//
TInt Pbk2PresentationUtils::TrimAllAppend(const TDesC& aText, TDes& aDest)
    {
    const TInt oldDestLength = aDest.Length();

    // 1. Find first non space character
    const TInt textLength = aText.Length();
    TInt firstNonSpaceChar = 0;
    while(firstNonSpaceChar < textLength
        && TChar(aText[firstNonSpaceChar]).IsSpace())
        {
        ++firstNonSpaceChar;
        }

    // 2. Append the source text to destination,
    // not including the leading spaces
    aDest.Append(aText.Mid(firstNonSpaceChar));
    // 3. Trim the new part of the destination to remove
    // trailing and middle spaces
    TPtr ptr = aDest.MidTPtr(oldDestLength);
    ptr.TrimAll();
    aDest.SetLength(oldDestLength);
    aDest.Append(ptr);

    return aDest.Length() - oldDestLength;
    }

// --------------------------------------------------------------------------
// Pbk2PresentationUtils::TrimRightAppend
// --------------------------------------------------------------------------
//
EXPORT_C TInt Pbk2PresentationUtils::TrimRightAppend(
        const TDesC& aText, TDes& aDest)
    {
    // Create modifiable descriptor pointer that points to the non-used space
    // at the end of the destination buffer
    TPtr temp(
            (TUint16*) aDest.MidTPtr( aDest.Length() ).Ptr(),
            aDest.MaxLength() - aDest.Length() );
    temp.Append( aText );
    temp.TrimRight();
    // Update the destination descriptor length to match the actual contents
    aDest.SetLength( aDest.Length() + temp.Length() );
    return temp.Length();
    }
    
// --------------------------------------------------------------------------
// Pbk2PresentationUtils::AppendAndReplaceChars
// --------------------------------------------------------------------------
//
EXPORT_C TInt Pbk2PresentationUtils::AppendAndReplaceChars
        (TDes& aDest, const TDesC& aSrc,
        const TDesC& aCharsToReplace, const TDesC& aReplaceChars)
    {
    // Check that aDest has enough capacity
    __ASSERT_DEBUG((aDest.MaxLength()-aDest.Length()) >= aSrc.Length(),
        Panic(EPanicPreCond_CopyAndReplaceChars));
    // Check that a replacament is provided for all characters
    __ASSERT_DEBUG(aCharsToReplace.Length() == aReplaceChars.Length(),
        Panic(EPanicPreCond_CopyAndReplaceChars));

    TInt count, i;
    const TInt sourceLenght = aSrc.Length();
    for (count=0, i=0; i < sourceLenght; ++i)
        {
        TText ch = aSrc[i];
        const TInt replacePos = aCharsToReplace.Locate(aSrc[i]);
        if (replacePos != KErrNotFound)
            {
            ++count;
            ch = aReplaceChars[replacePos];
            }
        aDest.Append(ch);
        }

    return count;
    }
    
// --------------------------------------------------------------------------
// Pbk2PresentationUtils::ReplaceNonGraphicCharacters
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2PresentationUtils::ReplaceNonGraphicCharacters
        (TDes& aText, TText aChar)
    {
    const TInt len = aText.Length();
    for ( TInt i=0; i < len; ++i )
        {
        if ( !TChar(aText[i]).IsGraph() &&
             !ZWSCharacter( aText[i] ) )
            {
            // If non-graphic char is specified in ZWSCharacter,
            // it will not be replaced. Otherwise replace non-graphic
            // character with aChar.
                aText[i] = aChar;
            }
        }
    }

// --------------------------------------------------------------------------
// Pbk2PresentationUtils::AppendWithNewlineTranslationL
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2PresentationUtils::AppendWithNewlineTranslationL
        ( TDes& aBuffer, const TDesC& aText )
    {
    const TUint16 KCarriageReturn = 0x000d;
    const TUint16 KLineFeed = 0x000a;
    const TUint16 KParagraphSeparator = 0x2029;
    
    const TInt length( aText.Length() );
    const TInt max( aBuffer.MaxLength() );
    
    for ( register TInt i = 0;
          (i < length) && (aBuffer.Length() < max); ++i )
        {
        register TUint16 ch = aText[i];
        if ( ch == KCarriageReturn )
            {
            ch = KParagraphSeparator;
            // Skip linefeeds that follow carriage return
            if ( (i < length-1) && (aText[i+1] == KLineFeed) )
                {
                ++i;
                }
            }
        else if ( ch == KLineFeed )
            {
            ch = KParagraphSeparator;
            }
        aBuffer.Append( ch );
        }
    }
    
// --------------------------------------------------------------------------
// Pbk2PresentationUtils::PresentationResourceFile
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& Pbk2PresentationUtils::PresentationResourceFile()
    {
    if (FeatureManager::FeatureSupported(KFeatureIdChinese))
        {
       if ( FeatureManager::FeatureSupported(KFeatureIdFfTdUnSyncabPbfieldsInd) )
            {
            // The resource which can't be synchronized with a server.
            return KPbk2PresentationResFileChineseUnSync;
            }
        else
            {
            return KPbk2PresentationResFileChinese;
            }
        }
    return KPbk2PresentationResFile;
    }    


// --------------------------------------------------------------------------
// Pbk2PresentationImageUtils::CropImageL
// --------------------------------------------------------------------------
//
EXPORT_C void Pbk2PresentationImageUtils::CropImageL( 
        CFbsBitmap& aBitmap, 
        TCroppingMode aCroppingMode, 
        const TSize& aTargetSize  )
    {
    const TSize sourceSize( aBitmap.SizeInPixels() );
    // crop the image only if the width is bigger than height
    TBool landscape( ETrue );
    if( sourceSize.iHeight >= sourceSize.iWidth )
        {
        // portrait image
        landscape = EFalse;
        }
    // take the shorter side
    const TInt sideSizeH( 
            landscape ? sourceSize.iHeight : sourceSize.iWidth );
    const TInt targetSizeW( 
            landscape ? aTargetSize.iWidth : aTargetSize.iHeight );
    TInt sideSizeW( sideSizeH );   
    if( EOptimizedCropping == aCroppingMode &&
        sideSizeW < targetSizeW )
        {
        sideSizeW = targetSizeW;
        if( sideSizeW >= 
                ( landscape ? sourceSize.iWidth : sourceSize.iHeight ) )
            {
            return; // no cropping
            }
        }
    
    // set target size
    const TSize targetSize( landscape? 
            TSize(sideSizeW, sideSizeH ): TSize( sideSizeH, sideSizeW ) );

    // crop from both sides
    const TPoint targetPoint( ( sourceSize.iWidth - targetSize.iWidth ) / 2,
                              ( sourceSize.iHeight - targetSize.iHeight ) / 2 );
    const TRect targetRect( targetPoint, targetSize );
    
    // create new bitmap
    CFbsBitmap* target = new( ELeave ) CFbsBitmap;
    CleanupStack::PushL( target );
    const TDisplayMode displayMode( aBitmap.DisplayMode() );
    User::LeaveIfError( target->Create( targetSize, displayMode ) );
    
    // get scanline
    HBufC8* scanLine = HBufC8::NewLC( aBitmap.ScanLineLength
        ( targetSize.iWidth, displayMode ) );
    TPtr8 scanLinePtr = scanLine->Des();
    
    TPoint startPoint( targetRect.iTl.iX, targetRect.iTl.iY );
    TInt targetY = 0;
    for (; startPoint.iY < targetRect.iBr.iY; ++startPoint.iY )
        {
        aBitmap.GetScanLine( 
                scanLinePtr, startPoint, targetSize.iWidth, displayMode );
        target->SetScanLine( scanLinePtr, targetY++ );
        }

    aBitmap.Reset();
    User::LeaveIfError( aBitmap.Duplicate( target->Handle() ) );
    CleanupStack::PopAndDestroy( 2, target ); // scanLine, target    
    }    

// End of File
