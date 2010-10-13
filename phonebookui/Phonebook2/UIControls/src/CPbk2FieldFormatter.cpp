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
* Description:  Phonebook 2 field formatter.
*
*/

#include <CPbk2FieldFormatter.h>

// Phonebook 2
#include <Pbk2PhoneNumberFormatterFactory.h>
#include <MPbk2PhoneNumberFormatter.h>
#include <Pbk2PresentationUtils.h>
#include <Pbk2UIControls.rsg>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkContactFieldData.h>
#include <MPbk2FieldProperty.h>
#include <MPbk2FieldPropertyArray.h>
#include <VPbkFieldType.hrh>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldUriData.h>
#include <CVPbkFieldTypeSelector.h>
#include <VPbkSyncConstants.h>


// System includes
#include <AknUtils.h>
#include <barsread.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KBufferLength = 128;

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicPostCond_ReallocBufferL = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2FieldFormatter");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2FieldFormatter::CPbk2FieldFormatter
// --------------------------------------------------------------------------
//
inline CPbk2FieldFormatter::CPbk2FieldFormatter
        ( const MPbk2FieldPropertyArray& aFieldProperties,
          const MVPbkFieldTypeList& aFieldTypeList ):
            iFieldProperties( aFieldProperties ),
            iFieldTypeList( aFieldTypeList )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::~CPbk2FieldFormatter
// --------------------------------------------------------------------------
//
CPbk2FieldFormatter::~CPbk2FieldFormatter()
    {
    delete iTimeFormat;
    delete iNumberFormatter;
    delete iBuffer;
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2FieldFormatter::ConstructL()
    {
    iBuffer = HBufC::NewL( KBufferLength );
    iNumberFormatter =
        Pbk2PhoneNumberFormatterFactory::CreatePhoneNumberFormatterL
            ( KBufferLength );
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2FieldFormatter* CPbk2FieldFormatter::NewL
        ( const MPbk2FieldPropertyArray& aFieldProperties,
          const MVPbkFieldTypeList& aFieldTypeList )
    {
    CPbk2FieldFormatter* self =
        new ( ELeave ) CPbk2FieldFormatter
            ( aFieldProperties, aFieldTypeList );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::FormatFieldContentL
// --------------------------------------------------------------------------
//
EXPORT_C TPtrC CPbk2FieldFormatter::FormatFieldContentL
        ( const MVPbkBaseContactField& aField,
          const MVPbkFieldType& aFieldType )
    {
    TPtr text( iBuffer->Des() );

    switch ( aField.FieldData().DataType() )
        {
        case EVPbkFieldStorageTypeText:
            {
            const MVPbkContactFieldTextData* fieldData =
                    &MVPbkContactFieldTextData::Cast( aField.FieldData() );
            TPtrC fieldText( fieldData->Text() );

            if( MatchesFieldTypeL
                    ( aFieldType, R_PHONEBOOK2_RINGTONE_SELECTOR ) ||
                MatchesFieldTypeL
                    ( aFieldType, R_PHONEBOOK2_IMAGE_SELECTOR ) )
                {
                FormatFileNameContentL( fieldText,text );
                }
            else if ( MatchesFieldTypeL
                    ( aFieldType, R_PHONEBOOK2_DTMF_SELECTOR ) )
                {
                FormatDtmfContentL( fieldText,text );
                }
            else if ( MatchesFieldTypeL
                    ( aFieldType, R_PHONEBOOK2_PHONENUMBER_SELECTOR ) )
                {
                FormatPhoneNumberContentL( fieldText, text );
                }
            else if ( MatchesFieldTypeL
                    ( aFieldType, R_PHONEBOOK2_SYNCRONIZATION_SELECTOR ) )
                {
                FormatSyncContentL( fieldText, text );
                }
            else if ( MatchesFieldTypeL
                    ( aFieldType, R_PHONEBOOK2_ADDRESS_SELECTOR ) )
                {
                FormatAddressContentL( fieldText, text );
                }
            else
                {
                // Default formatting
                FormatTextContentL( fieldText, text, aFieldType );
                }
            break;
            }

        case EVPbkFieldStorageTypeDateTime:
            {
            const MVPbkContactFieldDateTimeData* fieldData =
                    &MVPbkContactFieldDateTimeData::Cast
                        ( aField.FieldData() );
            FormatDateTimeContentL( *fieldData, text );
            break;
            }
        case EVPbkFieldStorageTypeUri:
            {
            const MVPbkContactFieldUriData* fieldData =
                    &MVPbkContactFieldUriData::Cast
                        ( aField.FieldData() );
            TPtrC fieldText( fieldData->Text() );
            FormatTextContentL( fieldText, text, aFieldType );
            break;
            }
        default:
            {
            text.Zero();
            break;
            }
        }

    return text;
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::SetTimeFormatL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2FieldFormatter::SetTimeFormatL( const TDesC& aTimeFormat )
    {
    HBufC* timeFormat = aTimeFormat.AllocL();
    delete iTimeFormat;
    iTimeFormat = timeFormat;
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::ReallocBufferL
// --------------------------------------------------------------------------
//
TPtr CPbk2FieldFormatter::ReallocBufferL( TInt aNewSize )
    {
    const TInt currMaxLength = iBuffer->Des().MaxLength();
    if (aNewSize > currMaxLength)
        {
        iBuffer = iBuffer->ReAllocL(aNewSize);
        }

    // PostCond:
    __ASSERT_DEBUG(iBuffer->Des().MaxLength() >= aNewSize,
        Panic(EPanicPostCond_ReallocBufferL));

    return iBuffer->Des();
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::MatchesFieldTypeL
// --------------------------------------------------------------------------
//
TBool CPbk2FieldFormatter::MatchesFieldTypeL
        ( const MVPbkFieldType& aFieldType, TInt aResourceId ) const
    {
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, aResourceId );

    CVPbkFieldTypeSelector* selector = CVPbkFieldTypeSelector::NewL
        ( reader, iFieldTypeList );

    // Check if the field type is the one needed
    TBool ret = selector->IsFieldTypeIncluded(aFieldType);
    CleanupStack::PopAndDestroy(); // resource buffer
    delete selector;
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::FormatPhoneNumberContentL
// --------------------------------------------------------------------------
//
void CPbk2FieldFormatter::FormatPhoneNumberContentL
        ( TPtrC& aRawContent, TPtr& aFormattedContent )
    {
    iNumberFormatter->SetMaxBufferLengthL( aRawContent.Length() );

    TPtrC formattedPhoneNumber
        ( iNumberFormatter->FormatPhoneNumberForDisplay( aRawContent ) );
    aFormattedContent.Set( ReallocBufferL( formattedPhoneNumber.Length() ) );
    aFormattedContent.Copy( formattedPhoneNumber );
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::FormatDtmfContentL
// --------------------------------------------------------------------------
//
void CPbk2FieldFormatter::FormatDtmfContentL
        ( TPtrC& aRawContent, TPtr& aFormattedContent )
    {
    HBufC* numberBuffer = HBufC::NewLC( aRawContent.Length() );

    TPtr number = numberBuffer->Des();
    number.Copy( aRawContent );
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( number );

    aFormattedContent.Set( ReallocBufferL( number.Length() ) );
    aFormattedContent.Copy( number );

    CleanupStack::PopAndDestroy( numberBuffer );
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::FormatSyncContentL
// --------------------------------------------------------------------------
//
void CPbk2FieldFormatter::FormatSyncContentL
        ( TPtrC& aRawContent, TPtr& aFormattedContent )
    {
    if ( !aRawContent.CompareF( KVPbkContactSyncPublic ) )
        {
        HBufC* textBuffer = CCoeEnv::Static()->AllocReadResourceLC
            ( R_QTN_CALE_CONFIDENT_PUBLIC );

        TPtr localizedSyncText = textBuffer->Des();
        aFormattedContent.Set( ReallocBufferL
            ( localizedSyncText.Length() ) );
        aFormattedContent.Copy( localizedSyncText );

        CleanupStack::PopAndDestroy( textBuffer );
        }
    else if ( !aRawContent.CompareF( KVPbkContactSyncNoSync ) )
        {
        HBufC* textBuffer = CCoeEnv::Static()->AllocReadResourceLC
            ( R_QTN_CALE_CONFIDENT_NONE );

        TPtr localizedSyncText = textBuffer->Des();
        aFormattedContent.Set( ReallocBufferL
            ( localizedSyncText.Length() ) );
        aFormattedContent.Copy( localizedSyncText );

        CleanupStack::PopAndDestroy( textBuffer );
        }
    else
        {
        // Otherwise sync setting is the default value - private
        HBufC* textBuffer = CCoeEnv::Static()->AllocReadResourceLC
            ( R_QTN_CALE_CONFIDENT_PRIVATE );

        TPtr localizedSyncText = textBuffer->Des();
        aFormattedContent.Set( ReallocBufferL
            ( localizedSyncText.Length() ) );
        aFormattedContent.Copy( localizedSyncText );

        CleanupStack::PopAndDestroy( textBuffer );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::FormatFileNameContentL
// --------------------------------------------------------------------------
//
void CPbk2FieldFormatter::FormatFileNameContentL
        ( TPtrC& aRawContent, TPtr& aFormattedContent )
    {
    aFormattedContent.Set( ReallocBufferL ( KMaxFileName ) );
    aFormattedContent.Copy( aRawContent );
    TParsePtr fileNameParser( aFormattedContent );
    TPtrC baseName( fileNameParser.Name() );
    aFormattedContent.Copy( baseName );
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::FormatTextContentL
// --------------------------------------------------------------------------
//
void CPbk2FieldFormatter::FormatTextContentL
        ( TPtrC& aRawContent, TPtr& aFormattedContent,
          const MVPbkFieldType& aFieldType )
    {
    aFormattedContent.Set( ReallocBufferL( aRawContent.Length() ) );
    aFormattedContent.Zero();
    Pbk2PresentationUtils::AppendWithNewlineTranslationL
        ( aFormattedContent, aRawContent );

    // Check is there need to display the digits in the text with
    // foreign characters. If the field is of numeric type,
    // language specific conversion is needed.
    const MPbk2FieldProperty* property =
        iFieldProperties.FindProperty( aFieldType );

    if ( property &&
         property->EditMode() == EPbk2FieldEditModeNumeric )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion
            ( aFormattedContent );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::FormatAddressContentL
// --------------------------------------------------------------------------
//
void CPbk2FieldFormatter::FormatAddressContentL
        ( TPtrC& aRawContent, TPtr& aFormattedContent )
    {
    aFormattedContent.Set( ReallocBufferL( aRawContent.Length() ) );
    aFormattedContent.Zero();
    Pbk2PresentationUtils::AppendWithNewlineTranslationL
        ( aFormattedContent, aRawContent );
    }

// --------------------------------------------------------------------------
// CPbk2FieldFormatter::FormatDateTimeContentL
// --------------------------------------------------------------------------
//
void CPbk2FieldFormatter::FormatDateTimeContentL
        ( const MVPbkContactFieldDateTimeData& aRawContent,
          TPtr& aFormattedContent )
    {
    TInt error = KErrNone;
    do
        {
        TRAPD( error, aRawContent.DateTime().FormatL
            ( aFormattedContent, *iTimeFormat ) );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion
            ( aFormattedContent );

        if ( error == KErrOverflow )
            {
            // Allocate bigger buffer for formatting text
            aFormattedContent.Set( ReallocBufferL
                ( 2 * aFormattedContent.MaxLength() ) );
            }
        else
            {
            // Rethrow other errors
            User::LeaveIfError( error );
            }
        } while ( error == KErrOverflow );
    }

// End of File
