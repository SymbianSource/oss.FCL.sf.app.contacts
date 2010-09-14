/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Contact model class for MyCard
 *
 */

#include <TPbk2StoreContactAnalyzer.h>
#include <pbk2uicontrols.rsg>
#include <Pbk2PresentationUtils.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactField.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2IconArray.h>
#include <MPbk2FieldProperty.h>
#include <MPbk2ClipListBoxText.h>
#include <f32file.h>

#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldBinaryData.h>
#include <MVPbkContactFieldData.h>
#include <VPbkUtils.h>
#include <vpbkeng.rsg>

#include <aknlists.h>
#include <avkon.rsg>

#include "spbcontactdatamodelprivate.h"
#include "spbcontactdatamodelrow.h"

/// CONSTANTS
namespace {

/// Granularity of the row array
const TInt KRowArrayGranularity = 4;

/// Initial buffer size for temporary text buffers
const TInt KBufferSize = 256;

/// Disallowed characters in column text
_LIT( KCharsToReplace, "\t" );

/// Replacement character for invalid or graphical column characters
_LIT( KReplacementChars, " " );
const TText KReplacedChars = ' ';

/// Content colum index
const TInt KContentColumnIndex = 2;

/// Column  separator
const TText KColumnSeparator = '\t';

};

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::NewL
// ---------------------------------------------------------------------------
//
CSpbContactDataModelPrivate* CSpbContactDataModelPrivate::NewL( 
    CVPbkContactManager& aCntManager,
    CCoeEnv& aCoeEnv,
    TInt aFieldTypeSelectorRes,
    MPbk2ClipListBoxText* aClip )
    {
    CSpbContactDataModelPrivate* self = new (ELeave) CSpbContactDataModelPrivate(
        aCntManager, aCoeEnv, aFieldTypeSelectorRes, aClip );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::~CSpbContactDataModelPrivate
// ---------------------------------------------------------------------------
//
CSpbContactDataModelPrivate::~CSpbContactDataModelPrivate()
    {
    iRows.ResetAndDestroy();
    delete iLineBuf;
    delete iColumnBuf;
    iListIndxToPresentationIndx.Reset();
    delete iImageBuffer;
    delete iImageFileName;
    delete iLastName;
    delete iFirstName;
    delete iContactLink;
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::CSpbContactDataModelPrivate
// ---------------------------------------------------------------------------
//
CSpbContactDataModelPrivate::CSpbContactDataModelPrivate(
    CVPbkContactManager& aCntManager,
    CCoeEnv& aCoeEnv,
    TInt aFieldTypeSelectorRes,
    MPbk2ClipListBoxText* aClip ) :        
        iCoeEnv(aCoeEnv),
        iCntManager(aCntManager),
        iClip(aClip),
        iRows(KRowArrayGranularity),
        iFieldTypeRes(aFieldTypeSelectorRes)
    {
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::ConstructL
// ---------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::ConstructL()
    {
    iLineBuf = HBufC::NewL(KBufferSize);
    iColumnBuf = HBufC::NewL(KBufferSize);
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::MdcaCount
// ---------------------------------------------------------------------------
//
TInt CSpbContactDataModelPrivate::MdcaCount() const
    {
    return iRows.Count();
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::MdcaPoint
// ---------------------------------------------------------------------------
//
TPtrC CSpbContactDataModelPrivate::MdcaPoint(TInt aIndex) const
    {
    TPtr rowText( iLineBuf->Des() );
    rowText.Zero();

    const CSpbContactDataModelRow& row = *iRows[aIndex];
    const TInt columnCount = row.ColumnCount();

    for( TInt index = 0; index < columnCount; ++index )
        {
        TPtr columnText( iColumnBuf->Des() );
        columnText.Copy( row.At(index) );

        // Clip the column if required
        if( index == KContentColumnIndex && 
            row.IsClipRequired() &&
            iClip )
            {
            iClip->ClipFromBeginning( columnText, aIndex, index );
            }
 
        // Append the column and separator to the formatted row
        rowText.Append( columnText );
        rowText.Append( KColumnSeparator );
        }

    return rowText;
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::PresentationFieldIndex
// ---------------------------------------------------------------------------
//
TInt CSpbContactDataModelPrivate::PresentationFieldIndex( TInt aListIndex )
    {
    return iListIndxToPresentationIndx[ aListIndex ];
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::ListIndex
// ---------------------------------------------------------------------------
//
TInt CSpbContactDataModelPrivate::ListIndex( TInt aPresentationIndex )
    {
    return iListIndxToPresentationIndx.Find( aPresentationIndex );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::ExternalizeL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::ExternalizeL( RWriteStream& aStream ) const
    {
    // write rows
    const TInt rowCount = iRows.Count();
    aStream.WriteInt32L( rowCount );
    for( TInt i = 0; i < rowCount; ++i )
        {
        iRows[i]->ExternalizeL( aStream );
        }
    
    // write field indexes
    const TInt indxCount = iListIndxToPresentationIndx.Count();
    aStream.WriteInt32L( indxCount );
    for( TInt i = 0; i < indxCount; ++i )
        {
        aStream.WriteInt32L( iListIndxToPresentationIndx[i] );
        }
    
    // write other data
    ExternalizeDataL( aStream, Text( CSpbContactDataModel::ETextFirstName ) );
    ExternalizeDataL( aStream, Text( CSpbContactDataModel::ETextLastName ) );
    ExternalizeDataL( aStream, Text( CSpbContactDataModel::ETextImageFileName ) );
    ExternalizeDataL( aStream, Data( CSpbContactDataModel::EDataImageContent ) );
    
    if( iContactLink )
        {
        aStream.WriteInt8L( ETrue );
        HBufC8* link = iContactLink->PackLC();
        aStream.WriteL( *link );
        CleanupStack::PopAndDestroy( link );
        }
    else
        {
        aStream.WriteInt8L( EFalse );
        }
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::InternalizeL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::InternalizeL( RReadStream& aStream )
    {
    Reset();
    
    // read rows
    const TInt count = aStream.ReadInt32L();
    for( TInt i = 0; i < count; ++i )
        {
        CSpbContactDataModelRow* row = CSpbContactDataModelRow::NewL();
        CleanupStack::PushL( row );
        row->InternalizeL( aStream );
        iRows.AppendL( row );
        CleanupStack::Pop( row );
        ExpandBuffersL( row );
        }    

    // read field indexes
    const TInt indxCount = aStream.ReadInt32L();
    for( TInt i = 0; i < indxCount; ++i )
        {
        iListIndxToPresentationIndx.AppendL( aStream.ReadInt32L() );
        }

    // read other data
    InternalizeDataL( aStream, iFirstName );
    InternalizeDataL( aStream, iLastName );
    InternalizeDataL( aStream, iImageFileName );
    InternalizeDataL( aStream, iImageBuffer );

    // read contact link if provided
    if( aStream.ReadInt8L() )
        {
        CVPbkContactLinkArray* links = 
            CVPbkContactLinkArray::NewLC( aStream, iCntManager.ContactStoresL() );
        if( links->Count() )
            {
            // only first link used
            delete iContactLink;
            iContactLink = NULL;
            iContactLink = links->At( 0 ).CloneLC();
            CleanupStack::Pop(); // link
            }
        CleanupStack::PopAndDestroy( links );
        }
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::SetDataL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::SetDataL( 
    const CPbk2PresentationContact& aContact,
    const CPbk2IconArray* aIconArray )
    {
    Reset();

    // save link
    MVPbkContactLink* link = aContact.CreateLinkLC();
    delete iContactLink;
    iContactLink = link;
    CleanupStack::Pop(); // link
    
    const CPbk2PresentationContactFieldCollection& fields = 
        aContact.PresentationFields();
    const TInt fieldCount = fields.FieldCount();
    const TInt count1 = fields.ParentStoreContact().Fields().FieldCount();
    for (TInt index = 0; index < fieldCount; index++)
        {
        CPbk2PresentationContactField& field = fields.At(index);
        const MVPbkContactFieldData& fieldData = field.FieldData();

        // Get master field type list and match field's type against it
        const MVPbkFieldType* fieldType = VPbkUtils::MatchFieldType(
                iCntManager.FieldTypes(), field );
        TInt typeRes = fieldType->FieldTypeResId();
        
        // Handle special field types
        switch( typeRes )
            {
            case R_VPBK_FIELD_TYPE_THUMBNAILPIC:
                {
                delete iImageBuffer;
                iImageBuffer = NULL;
                iImageBuffer = MVPbkContactFieldBinaryData::Cast(fieldData).BinaryData().AllocL();
                break;
                }
            case R_VPBK_FIELD_TYPE_CALLEROBJIMG:
                {
                delete iImageFileName;
                iImageFileName = NULL;
                iImageFileName = MVPbkContactFieldTextData::Cast(fieldData).Text().AllocL();
                break;
                }
            case R_VPBK_FIELD_TYPE_FIRSTNAME:
                {        
                delete iFirstName;
                iFirstName = NULL;
                iFirstName = MVPbkContactFieldTextData::Cast(fieldData).Text().AllocL();
                break;
                }
            case R_VPBK_FIELD_TYPE_LASTNAME:
                { 
                delete iLastName;
                iLastName = NULL;
                iLastName = MVPbkContactFieldTextData::Cast(fieldData).Text().AllocL();
                break;
                }       
            }

        // Handle details list fields
        if( field.IsEditable() && fieldType && !IsHiddenField(fieldType) )
            {
            HBufC* label = GetLabelLC( field );

            if( label->Length() )
                {
                CSpbContactDataModelRow* row =
                        CSpbContactDataModelRow::NewL();
                CleanupStack::PushL(row);

                // Add icon
                TBuf<20> buffer; // 20 should be enough for icon index
                const TPbk2IconId& icon = field.FieldProperty().IconId();
                if( aIconArray )
                    {
                    TInt iconIndex = aIconArray->FindIcon( icon );
                    if( iconIndex != KErrNotFound)
                        {
                        buffer.AppendNum(iconIndex);
                        }
                    }
                row->SetIcon( icon );
                row->AppendColumnL(buffer);

                // add label.
                row->AppendColumnL(*label);

                // Add current index to Presentation Contact index array 
                iListIndxToPresentationIndx.AppendL( index );

                // add field content.
                switch (fieldData.DataType() )
                    {
                    case EVPbkFieldStorageTypeText:
                        {
                        HandleTextTypeFieldL( *fieldType, fieldData, row);
                        break;
                        }
                    case EVPbkFieldStorageTypeDateTime:
                        {
                        HandleDateTimeTypeFieldL(fieldData, row);
                        break;
                        }
                    case EVPbkFieldStorageTypeUri:
                        {
                        HandleUriTypeFieldL( *fieldType, fieldData, row);
                        break;
                        }
                    }

                // Expand row formatting buffer if required
                ExpandBuffersL(row);

                // Add the row if data ok to show to user.
                iRows.AppendL(row);
                CleanupStack::Pop(row);
                }

            CleanupStack::PopAndDestroy(label);
            }
        } // for
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::ContactLink
// --------------------------------------------------------------------------
//
MVPbkContactLink* CSpbContactDataModelPrivate::ContactLink()
    {
    return iContactLink;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::IsEmpty
// --------------------------------------------------------------------------
//
TBool CSpbContactDataModelPrivate::IsEmpty()
    {
    return !iImageBuffer && !iImageFileName && 
        !iFirstName && !iLastName && iRows.Count() == 0; 
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::IsHiddenField
// --------------------------------------------------------------------------
//
TBool CSpbContactDataModelPrivate::IsHiddenField(const MVPbkFieldType* aFieldType)
    {
    TInt resId = aFieldType->FieldTypeResId();
    return ( resId == R_VPBK_FIELD_TYPE_SYNCCLASS );
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::IsFieldTypeL
// ---------------------------------------------------------------------------
//
TBool CSpbContactDataModelPrivate::IsFieldTypeL(
    const MVPbkFieldType& aFieldType, TInt aSelectorResId )
    {
    TPbk2StoreContactAnalyzer analyzer( iCntManager, NULL );
    return analyzer.IsFieldTypeIncludedL( aFieldType, aSelectorResId );
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::HandleTextTypeFieldL
// ---------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::HandleTextTypeFieldL( 
    const MVPbkFieldType& aFieldType,
    const MVPbkContactFieldData& aFieldData, 
    CSpbContactDataModelRow* aRow )
    {
    // Check if clipping is required
    if( IsFieldTypeL( aFieldType, iFieldTypeRes ) )
        {
        aRow->SetClipRequired( ETrue );
        }
    
    TPtrC fieldText = MVPbkContactFieldTextData::Cast(aFieldData).Text();
    TInt resId = aFieldType.FieldTypeResId();
    HBufC* defaultText = NULL;
    if( resId == R_VPBK_FIELD_TYPE_CALLEROBJIMG )
        {
        RFs& fs = iCoeEnv.FsSession(); 
        TEntry entry;
        if ( fs.Entry( fieldText, entry) == KErrNone )
           {
           // get image name
           TParse parse;
           parse.Set(fieldText, NULL, NULL);
           fieldText.Set(parse.Name());
          
           }
        else
            {
            // use default image name
            defaultText = iCoeEnv.AllocReadResourceLC( R_QTN_PHOB_FIELD_THUMBNAIL );
            fieldText.Set( defaultText->Des() );
            }
        }
    TPtr columnBuf( ExpandColumnBufferL( fieldText.Length() ) );
    columnBuf.Zero();

    // replace listbox separator characters.
    Pbk2PresentationUtils::AppendAndReplaceChars( columnBuf, fieldText,
            KCharsToReplace, KReplacementChars );
    if( defaultText )
        {
        CleanupStack::PopAndDestroy( defaultText );
        }
    // Replace characters that can not be displayed correctly.
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters( columnBuf, KReplacedChars );

    aRow->AppendColumnL( columnBuf );
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::HandleDateTimeTypeFieldL
// ---------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::HandleDateTimeTypeFieldL(
    const MVPbkContactFieldData& aFieldData, CSpbContactDataModelRow* aRow)
    {
    const MVPbkContactFieldDateTimeData& date =
            MVPbkContactFieldDateTimeData::Cast(aFieldData);

    HBufC* dateFormat = 
        iCoeEnv.AllocReadResourceLC( R_QTN_DATE_USUAL_WITH_ZERO );

    TLocale locale;
    TBuf<64> dateBuffer;
    TTime time( date.DateTime() );
    time.FormatL( dateBuffer, *dateFormat, locale );
    CleanupStack::PopAndDestroy( dateFormat );

    aRow->AppendColumnL( dateBuffer );
    }

// ---------------------------------------------------------------------------
// CSpbContactDataModelPrivate::HandleUriTypeFieldL
// ---------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::HandleUriTypeFieldL(
    const MVPbkFieldType& aFieldType,
    const MVPbkContactFieldData& aFieldData, CSpbContactDataModelRow* aRow)
    {
    if ( IsFieldTypeL( aFieldType, iFieldTypeRes ) )
        {
        aRow->SetClipRequired( ETrue );
        }
    
    TPtrC uri = MVPbkContactFieldUriData::Cast( aFieldData ).Text();
    TPtr columnBuf( ExpandColumnBufferL( uri.Length() ) );
    columnBuf.Copy( uri );
    
    // Replace characters that can not be displayed correctly.
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters( columnBuf, KReplacedChars );
    
    aRow->AppendColumnL( columnBuf );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::ExpandBuffersL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::ExpandBuffersL(CSpbContactDataModelRow* aRow)
    {
    // Row formatting buffer
    const TInt rowLength = aRow->TotalLength() + aRow->ColumnCount(); // for separator characters

    if( rowLength > iLineBuf->Des().MaxLength() )
        {
        iLineBuf = iLineBuf->ReAllocL(rowLength);
        }

    ExpandColumnBufferL( aRow->MaxColumnLength() );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::ExpandColumnBufferL
// --------------------------------------------------------------------------
//
TPtr CSpbContactDataModelPrivate::ExpandColumnBufferL(TInt aRequiredLength)
    {
    if( aRequiredLength > iColumnBuf->Des().MaxLength() )
        {
        iColumnBuf = iColumnBuf->ReAllocL(aRequiredLength);
        }

    return iColumnBuf->Des();
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::Reset
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::Reset()
    {
    iRows.ResetAndDestroy();
    iListIndxToPresentationIndx.Reset();
    
    delete iImageBuffer;
    iImageBuffer = NULL;
    delete iImageFileName;
    iImageFileName = NULL;
    delete iLastName;
    iLastName = NULL;
    delete iFirstName;
    iFirstName = NULL;
    delete iContactLink;
    iContactLink = NULL;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::Text
// --------------------------------------------------------------------------
//
TPtrC CSpbContactDataModelPrivate::Text( 
    CSpbContactDataModel::TTextTypes aType ) const
    {
    switch( aType )
        {
        case CSpbContactDataModel::ETextImageFileName:
            {
            return iImageFileName ? *iImageFileName : KNullDesC();
            }
        case CSpbContactDataModel::ETextFirstName:
            {
            return iFirstName ? *iFirstName : KNullDesC();
            }
        case CSpbContactDataModel::ETextLastName:
            {
            return iLastName ? *iLastName : KNullDesC();
            }
        }
    return KNullDesC();    
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::Data
// --------------------------------------------------------------------------
//
TPtrC8 CSpbContactDataModelPrivate::Data( 
    CSpbContactDataModel::TBinaryTypes aType ) const
    {
    switch( aType )
        {
        case CSpbContactDataModel::EDataImageContent:
            {
            return iImageBuffer ? *iImageBuffer : KNullDesC8();
            }
        }
    return KNullDesC8();
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::UpdateIconsL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::UpdateIconsL( const CPbk2IconArray& aIconArray )
    {
    // locate and set icon indexes for new icon array 
    const TInt count = iRows.Count();
    for( TInt i = 0; i < count; ++i )
        {
        // find and set icon indexes according to current icon array
        CSpbContactDataModelRow* row = iRows[i];
        if( row->ColumnCount() )
            {
            TInt index = aIconArray.FindIcon( row->Icon() );
            if( index != KErrNotFound )
                {
                TBuf<20> buf;
                buf.Num( index );
                row->ReplaceColumnL( 0, buf );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::SetClipListBoxText
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::SetClipListBoxText( MPbk2ClipListBoxText* aClip )
    {
    iClip = aClip;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::GetLabelLC
// --------------------------------------------------------------------------
//
HBufC* CSpbContactDataModelPrivate::GetLabelLC(
        const CPbk2PresentationContactField& aField)
    {
    TPtr columnBuf( ExpandColumnBufferL( aField.FieldLabel().Length() ) );
    columnBuf.Zero();

    // replace listbox separator characters.
    Pbk2PresentationUtils::AppendAndReplaceChars( columnBuf,
        aField.FieldLabel(), KCharsToReplace, KReplacementChars );

    // Replace characters that can not be displayed correctly
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters( 
        columnBuf, KReplacedChars );

    HBufC* data = HBufC::NewLC( columnBuf.Length() );
    data->Des().Append( columnBuf );

    return data;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::ExternalizeDataL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::ExternalizeDataL( 
    RWriteStream& aStream, const TDesC8& aData ) const
    {
    const TInt length = aData.Length();
    aStream.WriteInt32L( length );
    if( length )
        {
        aStream.WriteL( aData, length );
        }
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::ExternalizeDataL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::ExternalizeDataL( 
    RWriteStream& aStream, const TDesC16& aData ) const
    {
    const TInt length = aData.Length();
    aStream.WriteInt32L( length );
    if( length )
        {
        aStream.WriteL( aData, length );
        }
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::InternalizeDataL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::InternalizeDataL( RReadStream& aStream, HBufC8*& aData )
    {
    const TInt length = aStream.ReadInt32L();
    HBufC8* buf = NULL;
    if( length )
        {
        buf = HBufC8::NewLC( length );
        TPtr8 ptr( buf->Des() );
        aStream.ReadL( ptr, length );
        CleanupStack::Pop( buf );
        }
    delete aData;
    aData = buf;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelPrivate::InternalizeDataL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelPrivate::InternalizeDataL( RReadStream& aStream, HBufC16*& aData )
    {
    const TInt length = aStream.ReadInt32L();
    HBufC16* buf = NULL;
    if( length )
        {
        buf = HBufC16::NewLC( length );
        TPtr16 ptr( buf->Des() );
        aStream.ReadL( ptr, length );
        CleanupStack::Pop( buf );
        }
    delete aData;
    aData = buf;
    }

// End of File
