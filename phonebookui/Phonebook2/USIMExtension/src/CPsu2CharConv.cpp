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
* Description:  A class for converts characters into SMS 7 bit format and
*                decides if the data is too long.
*
*/


#include "CPsu2CharConv.h"

// System includes

// Debug headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KMaxSizeOfConvertedChar = 5;
const TInt KReductionSize = 3;
const TInt KOneCharacter = 1;
const TInt KTwoBytes = 2;
_LIT( KAtSign, "@" );
const TInt KLineSeparator( 0x2028 );
const TInt KParagraphSeparator( 0x2029 );

} // namespace

// -----------------------------------------------------------------------------
// CPsu2CharConv::CPsu2CharConv
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsu2CharConv::CPsu2CharConv( RFs& aFs, const TUint aCharacterSet ) :
    iFs( aFs ),
    iCharacterSetIdentifier( aCharacterSet )
    {
    }

// -----------------------------------------------------------------------------
// CPsu2CharConv::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsu2CharConv::ConstructL()
    {
    iConverter = CCnvCharacterSetConverter::NewL();
    iConverter->PrepareToConvertToOrFromL(
        iCharacterSetIdentifier,
        iFs );
    }

void CPsu2CharConv::SetDownGradeLf( CCnvCharacterSetConverter::
        TDowngradeForExoticLineTerminatingCharacters aDowngrade )
    {
    iConverter->SetDowngradeForExoticLineTerminatingCharacters( aDowngrade );
    }

// -----------------------------------------------------------------------------
// CPsu2CharConv::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsu2CharConv* CPsu2CharConv::NewL( RFs& aFs, const TUint aChacterSet )
    {
    CPsu2CharConv* self = NewLC( aFs, aChacterSet );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPsu2CharConv::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsu2CharConv* CPsu2CharConv::NewLC( RFs& aFs, const TUint aChacterSet )
    {
    CPsu2CharConv* self = new( ELeave ) CPsu2CharConv( aFs, aChacterSet );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// ~CPsu2CharConv
// -----------------------------------------------------------------------------
//
CPsu2CharConv::~CPsu2CharConv()
    {
    delete iConverter;
    }

// -----------------------------------------------------------------------------
// CPsu2CharConv::CheckFieldValidityL
// -----------------------------------------------------------------------------
//
TPtrC CPsu2CharConv::CheckFieldValidityL(
        const TDesC& aData, 
        TInt aMaxLength,
        TPsu2FieldValidity& aValidity )
    {
    TPtrC result( aData );
    aValidity = EOk;

    TBuf8<KMaxSizeOfConvertedChar> convBuf;
    TInt unConv = 0;
    TInt simLength = 0;
    TInt maxUiLength = 0;
    TInt uiLength = aData.Length();
    TBool hasConvertedChar = EFalse;

    // Make the conversion one character at a time
    for ( TInt i = 0; i < uiLength; ++i )
        {
        convBuf.Zero();
        TPtrC ch( aData.Mid( i, KOneCharacter ) );
        User::LeaveIfError( 
            iConverter->ConvertFromUnicode( convBuf, ch, unConv ) );

        if ( unConv > 0 )
            {
            // If the character can't be converted it's unicode,
            // in that case it's assumed that all characters take two bytes as
            // in UCS2 coding scheme 1
            simLength = uiLength * KTwoBytes;
            maxUiLength = aMaxLength / KTwoBytes;
            }
        else
            {
            // Character was converted. If the converted character
            // differs from original, it requires more space from SIM.
            // Make the reduction only once, since it appears not matter
            // how many funny characters there are.
            if ( !hasConvertedChar )
                {
                TBuf16<KMaxSizeOfConvertedChar> convBuf16;
                convBuf16.Copy( convBuf );
                if ( ch.Compare( convBuf16 ) != 0 && ch.Compare( KAtSign ) != 0 )
                    {
                    // SIM max.length is decreased once by 3 chars, if special
                    // char is other than @. This kind of behaviour seems to
                    // be in all SIMs with different name field sizes.
                    aMaxLength -= KReductionSize;
                    hasConvertedChar = ETrue;
                    }
                }
            simLength += convBuf.Length();
            maxUiLength = i;
            }

        if ( simLength > aMaxLength )
            {
            // If length is too long no need to continue checking
            aValidity = ETooLong;
            result.Set( aData.Left( maxUiLength ) );
            break;
            }
        }    

    return result;
    }

// -----------------------------------------------------------------------------
// CPsu2CharConv::CheckFieldLengthL
// -----------------------------------------------------------------------------
//
TPtrC CPsu2CharConv::CheckFieldLengthL(
        const TDesC& aData, 
        TInt aMaxLength,
        TInt& aUnconvertedCount )
    {
    TPtrC result( aData );
    HBufC8* convertedData = HBufC8::NewLC( aData.Length() * KMaxSizeOfConvertedChar );
    TPtr8 convertedDataPtr = convertedData->Des();

    // Start with full length data and remove one character on each iteration
    for( TInt i( aData.Length() ); i > 0; i-- )
        {
        TPtrC convertableData( aData.Left( i ) );
        convertedDataPtr.Zero();

        User::LeaveIfError(
                iConverter->ConvertFromUnicode(
                        convertedDataPtr, convertableData, aUnconvertedCount ) );
        TInt convertedLength = convertedDataPtr.Length();

        // Check if any extra space needs to be reserved (there is some
        // differences in special character conversions between Symbian
        // character converter and SAT)
        ReviseLength( convertableData, convertedLength );

        // Check if the conversion will fit to the space available
        if( convertedLength <= aMaxLength )
            {
            PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
                ("CPsu2CharConv::CheckFieldLengthL passed, max len: %i"), i );
            // Maximum allowed data length found, no need to continue
            result.Set( convertableData );
            break;
            }
        }

    CleanupStack::PopAndDestroy( convertedData );
    return result;
    }

// -----------------------------------------------------------------------------
// CPsu2CharConv::ReviseMaxLength
// Checks some special cases to need if extra space needs to be reserved for
// the data when saving to (U)SIM.
// -----------------------------------------------------------------------------
//
void CPsu2CharConv::ReviseLength(
        const TDesC& aData, TInt& aConvertedLength )
    {
    // Check if the data contains unicode line/paragraph separator(s).
    // If it does it will require some extra space when saved to (U)SIM.
    for( TInt i(0); i < aData.Length(); i++ )
        {
        if( aData[i] == KLineSeparator || aData[i] == KParagraphSeparator )
            {
            PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
                ("CPsu2CharConv::CheckFieldLengthL separator found" ) );
            const TInt KParagraphSeparatorExtraSpace(3);
            aConvertedLength += KParagraphSeparatorExtraSpace;
            // Search no more, reserve more space only once
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPsu2CharConv::IsUnicodeL
// 
// -----------------------------------------------------------------------------
//
TBool CPsu2CharConv::IsUnicodeL( const TDesC& aText )
    {
    TBool isUnicode( EFalse );
    TInt unconvertedCount(0);
    HBufC8* convertedText = HBufC8::NewLC( aText.Length() );
    TPtr8 convertedTextPtr = convertedText->Des(); 

    iConverter->ConvertFromUnicode( convertedTextPtr, aText, unconvertedCount );

    // If any characters were not converted or if the converted text is longer
    // than the original the text must be unicode
    if( unconvertedCount > 0 ||
        aText.Length() < convertedText->Length() )
        {
        isUnicode = ETrue;
        }

    CleanupStack::PopAndDestroy( convertedText );
    return isUnicode;
    }

// End of File
