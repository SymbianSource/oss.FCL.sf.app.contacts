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
* Description:  Phonebook 2 field list box model.
*
*/



// INCLUDE FILES
#include "cpbk2fieldlistboxmodel.h"

// Phonebook 2
#include "mpbk2cliplistboxtext.h"
#include "mpbk2fieldanalyzer.h"
#include "cpbk2fieldformatter.h"
#include "mpbk2contactfielddynamicproperties.h"
#include <cpbk2iconarray.h>
#include <pbk2presentationutils.h>
#include <cpbk2presentationcontactfield.h>
#include <cpbk2presentationcontactfieldcollection.h>
#include <mpbk2fieldpropertyarray.h>
#include <mpbk2fieldproperty.h>
#include <tpbk2storecontactanalyzer.h>
#include <pbk2uicontrols.rsg>
#include <tpbk2destructionindicator.h>
#include <pbk2profile.h>
#include "cpbk2contactfielddynamicproperties.h"

// Virtual Phonebook
#include <mvpbkfieldtype.h>
#include <tvpbkfieldversitproperty.h>
#include <mvpbkcontactfielddata.h>
#include <mvpbkcontactstore.h>
#include <cvpbkcontactmanager.h>
#include <mvpbkcontactfielddata.h>
#include <vpbkutils.h>

#include <pbk2debug.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KCharsToReplace, "\t" );
_LIT( KReplacementChars, " " );
const TInt KMaxFormattedTIntLength( 15 );
const TInt KInitialBufferLength( 128 );
const TInt KGranularity( 8 );

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_AppendRowL = 1
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2FieldListBoxModel");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG 

} /// namespace


// --------------------------------------------------------------------------
// CPbk2FieldListBoxRow::CPbk2FieldListBoxRow
// --------------------------------------------------------------------------
//
inline CPbk2FieldListBoxRow::CPbk2FieldListBoxRow()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxRow::~CPbk2FieldListBoxRow
// --------------------------------------------------------------------------
//
CPbk2FieldListBoxRow::~CPbk2FieldListBoxRow()
    {
    iColumns.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxRow::NewL
// --------------------------------------------------------------------------
//
CPbk2FieldListBoxRow* CPbk2FieldListBoxRow::NewL()
    {
    return new( ELeave ) CPbk2FieldListBoxRow;
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxRow::ColumnCount
// --------------------------------------------------------------------------
//
TInt CPbk2FieldListBoxRow::ColumnCount() const
    {
    return iColumns.Count();
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxRow::At
// --------------------------------------------------------------------------
//
TPtrC CPbk2FieldListBoxRow::At( TInt aColumnIndex ) const
    {
    return *iColumns[aColumnIndex];
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxRow::AppendColumnL
// --------------------------------------------------------------------------
//
void CPbk2FieldListBoxRow::AppendColumnL( const TDesC& aColumnText )
    {
    HBufC* buf = aColumnText.AllocLC();
    User::LeaveIfError( iColumns.Append( buf ) );
    CleanupStack::Pop( buf );
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxRow::TotalLength
// --------------------------------------------------------------------------
//
TInt CPbk2FieldListBoxRow::TotalLength() const
    {
    TInt result = 0;
    const TInt count = iColumns.Count();
    for (TInt i = 0; i < count; ++i)
        {
        result += At(i).Length();
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxRow::MaxColumnLength
// --------------------------------------------------------------------------
//
TInt CPbk2FieldListBoxRow::MaxColumnLength() const
    {
    TInt result = 0;
    const TInt count = iColumns.Count();
    for (TInt i = 0; i < count; ++i)
        {
        result = Max(result, At(i).Length());
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::CPbk2FieldListBoxModel
// --------------------------------------------------------------------------
//
inline CPbk2FieldListBoxModel::CPbk2FieldListBoxModel( 
        TParams& aParams ) :
    iRows( KGranularity ), 
    iParams( aParams )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::~CPbk2FieldListBoxModel
// --------------------------------------------------------------------------
//
CPbk2FieldListBoxModel::~CPbk2FieldListBoxModel()
    {
    iRows.ResetAndDestroy();
    delete iColumnBuf;
    delete iLineBuf;
    delete iFieldFormatter;
    delete iDynamicProperties;
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::NewL
// --------------------------------------------------------------------------
//
CPbk2FieldListBoxModel* CPbk2FieldListBoxModel::NewL
        ( TParams& aParams )
    {
    CPbk2FieldListBoxModel* self =
        new ( ELeave ) CPbk2FieldListBoxModel( aParams );
    CleanupStack::PushL( self );
    self->ConstructL( aParams );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2FieldListBoxModel::ConstructL( 
        TParams& aParams )
    {
    iFieldFormatter = CPbk2FieldFormatter::NewL
        ( aParams.iFieldProperties, aParams.iContactManager.FieldTypes() );
    iFieldFormatter->SetTimeFormatL( aParams.iTimeFormat );
    iLineBuf = HBufC::NewL( KInitialBufferLength );
    iColumnBuf = HBufC::NewL( KInitialBufferLength );
    iDynamicProperties = aParams.iDynamicProperties;
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::AppendRowL
// --------------------------------------------------------------------------
//
void CPbk2FieldListBoxModel::AppendRowL( CPbk2FieldListBoxRow* aLine )
    {
    __ASSERT_DEBUG( aLine, Panic( EPanicPreCond_AppendRowL ) );
    ExpandBuffersL( *aLine );
    iRows.AppendL( aLine );
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::SetClipper
// --------------------------------------------------------------------------
//
void CPbk2FieldListBoxModel::SetClipper
        ( MPbk2ClipListBoxText& aTextClipper )
    {
    this->iTextClipper = &aTextClipper;
    }
    
// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::FormatFieldsL
// --------------------------------------------------------------------------
//
void CPbk2FieldListBoxModel::FormatFieldsL()
    {
    // reset rows
    iRows.ResetAndDestroy();

    // Format all the fields and add lines to iTextArray
    const TInt fieldCount = iParams.iFields.FieldCount();
    for ( TInt i=0; i < fieldCount; ++i )
        {
        PBK2_PROFILE_START(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldsLFieldAt);
        const CPbk2PresentationContactField& field = iParams.iFields.At(i);
        PBK2_PROFILE_END(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldsLFieldAt);

        PBK2_PROFILE_START(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldsLFormatField);
        FormatFieldL(field , iParams );
        PBK2_PROFILE_END(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldsLFormatField);
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::MdcaCount
// --------------------------------------------------------------------------
//
TInt CPbk2FieldListBoxModel::MdcaCount() const
    {
    return iRows.Count();
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::MdcaPoint
// --------------------------------------------------------------------------
//
TPtrC16 CPbk2FieldListBoxModel::MdcaPoint( TInt aIndex ) const
    {
    const TText KSeparator = '\t';
    TPtr rowText( iLineBuf->Des() );
    rowText.Zero();

    const CPbk2FieldListBoxRow& row = *iRows[aIndex];
    const TInt columnCount = row.ColumnCount();
    for ( TInt columnIndex = 0; columnIndex < columnCount; ++columnIndex )
        {
        TPtr columnText( iColumnBuf->Des() );
        columnText.Copy( row.At( columnIndex ) );

        // Clip the column if required
        if ( iTextClipper && columnIndex == EContentColumn &&
             aIndex < iParams.iFields.FieldCount() )
            {
            ClipText( iParams.iFields.At( aIndex ), aIndex,
                columnText, columnIndex );
            }

        // Append the column and separator to the formatted row
        rowText.Append( columnText );
        rowText.Append( KSeparator );
        }

    return rowText;
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::FormatFieldL
// --------------------------------------------------------------------------
//
void CPbk2FieldListBoxModel::FormatFieldL( 
        const CPbk2PresentationContactField& aField, 
        TParams& aParams )
    {
    // Figure out whether the field should be shown
    const MPbk2FieldAnalyzer* fieldAnalyzer = aParams.iFieldAnalyzer;
    const MPbk2ContactFieldDynamicProperties* dynamicProps
        = iDynamicProperties; 
                    
    TBool showField = ETrue;
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxModelFormatFieldLShowField);
    if ( fieldAnalyzer && fieldAnalyzer->IsHiddenField( aField ) ||
         dynamicProps && dynamicProps->IsHiddenField( aField ) )
        {
        showField = EFalse;
        }
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxModelFormatFieldLShowField);

    if ( showField )
        {
        PBK2_PROFILE_START(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLCreateFieldListBoxRow);
        CPbk2FieldListBoxRow* row = CPbk2FieldListBoxRow::NewL();
        CleanupStack::PushL(row);
        PBK2_PROFILE_END(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLCreateFieldListBoxRow);
 
        // Get master field type list and match field's type against it
        PBK2_PROFILE_START(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLGetMasterFieldTypeList);
        const MVPbkFieldTypeList* masterFieldTypeList =
            &iParams.iContactManager.FieldTypes();
        PBK2_PROFILE_END(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLGetMasterFieldTypeList);

        PBK2_PROFILE_START(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLMatchFieldType);
        const MVPbkFieldType* fieldType =
            VPbkUtils::MatchFieldType( *masterFieldTypeList, aField );
        PBK2_PROFILE_END(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLMatchFieldType);

        TPtr columnBuf(iColumnBuf->Des());
        PBK2_PROFILE_START(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLAppendIcons);
        AppendIconsToBeginningL(aField, *fieldType, *row, aParams);
        PBK2_PROFILE_END(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLAppendIcons);
        PBK2_PROFILE_START(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLAppendFieldLabel);
        AppendFieldLabelL(aField, *row, columnBuf);
        PBK2_PROFILE_END(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLAppendFieldLabel);
        PBK2_PROFILE_START(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLAppendFieldContent);
        AppendFieldContentL(aField, *fieldType, *row, columnBuf);
        PBK2_PROFILE_END(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLAppendFieldContent);
        PBK2_PROFILE_START(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLAppendEndIcons);
        AppendIconsToEndL(aField, *row, aParams);
        PBK2_PROFILE_END(
            Pbk2Profile::EContactInfoListBoxModelFormatFieldLAppendEndIcons);
                
        // Expand row formatting buffer if required
        PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxModelFormatFieldLExpandBuffer);
        ExpandBuffersL(*row);
        PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxModelFormatFieldLExpandBuffer);

        // Add the row
        PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxModelFormatFieldLAppendRow);
        iRows.AppendL(row);
        PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxModelFormatFieldLAppendRow);
        CleanupStack::Pop(row);
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::ExpandBuffersL
// --------------------------------------------------------------------------
//
void CPbk2FieldListBoxModel::ExpandBuffersL
        ( const CPbk2FieldListBoxRow& aRow )
    {
    // Row formatting buffer
    const TInt rowLength = aRow.TotalLength()
        + aRow.ColumnCount();  // for separator characters

    if (rowLength > iLineBuf->Des().MaxLength())
        {
        iLineBuf = iLineBuf->ReAllocL( rowLength );
        }

    ExpandColumnBufferL( aRow.MaxColumnLength() );
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::ExpandColumnBufferL
// --------------------------------------------------------------------------
//
TPtr CPbk2FieldListBoxModel::ExpandColumnBufferL
        ( TInt aRequiredLength )
    {
    if ( aRequiredLength > iColumnBuf->Des().MaxLength() )
        {
        iColumnBuf = iColumnBuf->ReAllocL( aRequiredLength );
        }
    return ( iColumnBuf->Des() );
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::ClipText
// Clip text from beginning if field is numeric field or e-mail field.
// --------------------------------------------------------------------------
//
inline void CPbk2FieldListBoxModel::ClipText
        ( const CPbk2PresentationContactField& aField, TInt aIndex,
         TPtr& aColumnText, TInt aColumnIndex ) const
    {
    TPbk2StoreContactAnalyzer analyzer( iParams.iContactManager, NULL );
    TBool clip = analyzer.IsFieldTypeIncludedL
        ( aField, R_PHONEBOOK2_PHONENUMBER_SELECTOR );

    if ( !clip )
        {
        clip = analyzer.IsFieldTypeIncludedL
            ( aField, R_PHONEBOOK2_EMAIL_SELECTOR );
        }

    // Clip if required
    if ( clip )
        {
        iTextClipper->ClipFromBeginning
            ( aColumnText, aIndex, aColumnIndex );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::AppendFieldLabelL
// Appends field label. Removes any listbox separator characters.
// --------------------------------------------------------------------------
//
inline void CPbk2FieldListBoxModel::AppendFieldLabelL
        ( const CPbk2PresentationContactField& aField,
        CPbk2FieldListBoxRow& aRow, TPtr& aColumnBuf )
    {
    aColumnBuf.Set( ExpandColumnBufferL( aField.FieldLabel().Length() ) );
    aColumnBuf.Zero();
    Pbk2PresentationUtils::AppendAndReplaceChars
        ( aColumnBuf, aField.FieldLabel(),
        KCharsToReplace, KReplacementChars );
    // Replace characters that can not be displayed correctly
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters
        ( aColumnBuf, ' ' );
    aRow.AppendColumnL( aColumnBuf );
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::AppendFieldContentL
// Appends field content.
// --------------------------------------------------------------------------
//
inline void CPbk2FieldListBoxModel::AppendFieldContentL
        ( const CPbk2PresentationContactField& aField,
        const MVPbkFieldType& aFieldType,
        CPbk2FieldListBoxRow& aRow, TPtr& aColumnBuf )
    {
    TPtrC fieldText( iFieldFormatter->FormatFieldContentL
        ( aField, aFieldType ) );
    aColumnBuf.Set( ExpandColumnBufferL( fieldText.Length() ) );
    aColumnBuf.Zero();
    Pbk2PresentationUtils::AppendAndReplaceChars
        ( aColumnBuf, fieldText, KCharsToReplace, KReplacementChars );
    // Replace characters that can not be displayed correctly
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters
        (aColumnBuf, ' ');
    aRow.AppendColumnL( aColumnBuf );
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::AppendIconsToBeginningL
// Adds icons to the beginning.
// --------------------------------------------------------------------------
//
inline void CPbk2FieldListBoxModel::AppendIconsToBeginningL
        ( const CPbk2PresentationContactField& /*aField*/,
          const MVPbkFieldType& aFieldType,
          CPbk2FieldListBoxRow& aRow, TParams& aParams )
    {
    TBuf<KMaxFormattedTIntLength> iconText;

    // Format icon index
    TInt iconIndex = aParams.iIconArray.FindIcon( iParams.iDefaultIconId );
    const MPbk2FieldProperty* property =
        aParams.iFieldProperties.FindProperty( aFieldType );
    if (property)
        {
        iconIndex = aParams.iIconArray.FindIcon( property->IconId() );
        }

    iconText.Num( iconIndex );
    aRow.AppendColumnL( iconText );
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::AppendIconsToEndL
// Adds additional icons to the end.
// --------------------------------------------------------------------------
//
inline void CPbk2FieldListBoxModel::AppendIconsToEndL
        ( const CPbk2PresentationContactField& aField,
          CPbk2FieldListBoxRow& aRow, TParams& aParams )
    {
    if ( aParams.iFieldAnalyzer )
        {
        PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxModelAppendEndIconSpeedDial);
        TBool hasSpeedDial = 
            aParams.iFieldAnalyzer->HasSpeedDialL( aField.StoreField() );
        PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxModelAppendEndIconSpeedDial);
        PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxModelAppendEndIconSpeedDialIcon);
        // Check and add speed dial icon
        if ( hasSpeedDial  )
            {                        
            TBuf<KMaxFormattedTIntLength> iconText;
            iconText.Num( aParams.iIconArray.
                FindIcon( TPbk2AppIconId( EPbk2qgn_indi_qdial_add ) ) );
            aRow.AppendColumnL( iconText );
            }
        PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxModelAppendEndIconSpeedDialIcon);
            
        // Calls asynchronous VPbk function which has been
        // synchronized there            
        PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxModelAppendEndIconVoiceTag);
        TBool hasVoiceTag = 
            aParams.iFieldAnalyzer->HasVoiceTagL( aField.StoreField() );
        PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxModelAppendEndIconVoiceTag);

        // Check and add voice tag icon
        PBK2_PROFILE_START(Pbk2Profile::EContactInfoListBoxModelAppendEndIconVoiceTagIcon);
        if ( hasVoiceTag )
            {            
            TBuf<KMaxFormattedTIntLength> iconText;
            iconText.Num( aParams.iIconArray.
                FindIcon( TPbk2AppIconId( EPbk2qgn_indi_voice_add ) ) );
            aRow.AppendColumnL( iconText );
            }
        PBK2_PROFILE_END(Pbk2Profile::EContactInfoListBoxModelAppendEndIconVoiceTagIcon);
        }
    }

//  End of File
