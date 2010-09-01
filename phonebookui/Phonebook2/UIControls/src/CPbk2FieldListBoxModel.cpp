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
#include "CPbk2FieldListBoxModel.h"
#include <StringLoader.h>

// Phonebook 2
#include "MPbk2ClipListBoxText.h"
#include "MPbk2FieldAnalyzer.h"
#include "CPbk2FieldFormatter.h"
#include "MPbk2ContactFieldDynamicProperties.h"
#include "CPbk2ContactFieldDynamicProperties.h"
#include <CPbk2IconArray.h>
#include <Pbk2PresentationUtils.h>
#include <CPbk2PresentationContactField.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <MPbk2FieldPropertyArray.h>
#include <MPbk2FieldProperty.h>
#include <TPbk2StoreContactAnalyzer.h>
#include <Pbk2UIControls.rsg>
#include "Pbk2AddressTools.h"

// Virtual Phonebook
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactFieldData.h>
#include <VPbkUtils.h>
#include <VPbkEng.rsg>
#include <MVPbkStoreContact.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KCharsToReplace, "\t" );
_LIT( KReplacementChars, " " );
const TInt KMaxFormattedTIntLength( 15 );
const TInt KInitialBufferLength( 128 );
const TInt KGranularity( 8 );

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
inline CPbk2FieldListBoxModel::CPbk2FieldListBoxModel( TParams& aParams,
    const TArray<CPbk2PresentationContactFieldCollection*>*
        aFieldCollectionArray,
    const TArray<TPresenceIconPosInIconArray>* aPresenceIconsPos ):
        iRows( KGranularity ),
        iParams( aParams ),
        iFieldCollectionArray( aFieldCollectionArray ),
        iPresenceIconsPos( aPresenceIconsPos )
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
    iDuplicatesArray.Reset();
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::NewL
// --------------------------------------------------------------------------
//
CPbk2FieldListBoxModel* CPbk2FieldListBoxModel::NewL
    ( TParams& aParams,
      const TArray<CPbk2PresentationContactFieldCollection*>*
        aFieldCollectionArray,
      const TArray<TPresenceIconPosInIconArray>* aPresenceIconsPos )
    {
    CPbk2FieldListBoxModel* self = new ( ELeave )
        CPbk2FieldListBoxModel( aParams, aFieldCollectionArray,
            aPresenceIconsPos );
    CleanupStack::PushL( self );
    self->ConstructL( aParams );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2FieldListBoxModel::ConstructL
        ( TParams& aParams )
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
    // Format all the fields and add lines to iTextArray
    const TInt fieldCount = iParams.iFields.FieldCount();
    for ( TInt i=0; i < fieldCount; ++i )
        {
        FormatFieldL( iParams.iFields.At(i), iParams );
        }
        
    // Do the same for xSP contacts, if there are any
    if ( iFieldCollectionArray != NULL && iFieldCollectionArray->Count() > 0 )
        {
        TInt contactsCount = iFieldCollectionArray->Count();
        for ( TInt j = 0; j < contactsCount; j++ )
            {
            TInt xspFieldCount = iFieldCollectionArray->operator[](j)->
                FieldCount();
            for ( TInt k = 0; k < xspFieldCount; k++ )
                {
                FormatFieldL( iFieldCollectionArray->operator[](j)->At(k),
                    iParams );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::GetModelIndex
// --------------------------------------------------------------------------
//
TInt CPbk2FieldListBoxModel::GetModelIndex( TInt aListboxIndex )
    {
    TInt retIndex = aListboxIndex;
    TInt count = iDuplicatesArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iDuplicatesArray[i] < aListboxIndex )
            {
            retIndex++;
            }
        }
    return retIndex;
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
void CPbk2FieldListBoxModel::FormatFieldL
        ( const CPbk2PresentationContactField& aField, TParams& aParams )
    {
    // Figure out whether the field should be shown
    const MPbk2FieldAnalyzer* fieldAnalyzer = aParams.iFieldAnalyzer;
    const MPbk2ContactFieldDynamicProperties* dynamicProps
        = iDynamicProperties;

    TBool showField = ETrue;
    if ( fieldAnalyzer && fieldAnalyzer->IsHiddenField( aField ) ||
         dynamicProps && dynamicProps->IsHiddenField( aField ) )
        {
        showField = EFalse;
        }
    
    if (iParams.iCommMethod == VPbkFieldTypeSelectorFactory::EVOIPCallSelector && 
            showField && fieldAnalyzer )
        {
        const TInt voipFlag = fieldAnalyzer->VoipSupportL( iParams.iFields );
        TInt resId = aField.BestMatchingFieldType()->FieldTypeResId();
        if ( !fieldAnalyzer->IsFieldVoipCapable( resId, voipFlag ) )
            {
            showField = EFalse;
            }        
        }
    
    if ( showField )
        {
        // Get master field type list and match field's type against it
        const MVPbkFieldTypeList* masterFieldTypeList =
            &iParams.iContactManager.FieldTypes();

        const MVPbkFieldType* fieldType =
            VPbkUtils::MatchFieldType( *masterFieldTypeList, aField );

        TPbk2StoreContactAnalyzer analyzer( iParams.iContactManager, NULL );
        if ( analyzer.IsFieldTypeIncludedL(
        		aField, R_PHONEBOOK2_ADDRESS_SELECTOR ) )
            {
            FormatAddressFieldL( aField, aParams );
            }
        else
        	{
            CPbk2FieldListBoxRow* row = CPbk2FieldListBoxRow::NewL();
            CleanupStack::PushL(row);
            TPtr columnBuf(iColumnBuf->Des());
            AppendIconsToBeginningL(aField, *fieldType, *row, aParams);
            AppendFieldLabelL(aField, *row, columnBuf);
            AppendFieldContentL(aField, *fieldType, *row, columnBuf);
            AppendIconsToEndL(aField, *row, aParams);

            // Expand row formatting buffer if required
            ExpandBuffersL(*row);

            // Add the row 
            // If the content of field is equal to other rows and need to be duplicated,
		        // duplicate it
            if ( aParams.iFieldContentNeedToBeDuplicated &&
		                DuplicatesExist( columnBuf ) ) 
                { 
			          CleanupStack::PopAndDestroy(row);    
			          iDuplicatesArray.AppendL( iRows.Count() - 1 );  // position of 
                                                                // the duplicate
                }    
            else    
                { 
        	      iRows.AppendL(row);    
        	      CleanupStack::Pop(row);                                                                                     
                } 
        	}
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::FormatAddressFieldL
// --------------------------------------------------------------------------
//
void CPbk2FieldListBoxModel::FormatAddressFieldL
        ( const CPbk2PresentationContactField& aField, TParams& aParams )
    {
    CPbk2FieldListBoxRow* row = CPbk2FieldListBoxRow::NewL();
    CleanupStack::PushL(row);

    TPtr columnBuf(iColumnBuf->Des());
    AppendAddressIconsToBeginningL(aField, *row, aParams);
    AppendAddressFieldLabelL(aField, *row, columnBuf);
    TBool duplicates = DuplicatesAddressExist( columnBuf, row->ColumnCount() - 1 );
    AppendAddressFieldContentL(aField, *row, columnBuf);

    // Expand row formatting buffer if required
    ExpandBuffersL(*row);

    // Add the row
    if ( !duplicates )
        {
        iRows.AppendL(row);
        CleanupStack::Pop(row);
        }
    else
        {
        CleanupStack::PopAndDestroy(row);
        iDuplicatesArray.AppendL( iRows.Count() - 1 ); // position of 
                                                       // the duplicate
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
    
    if ( analyzer.IsFieldTypeIncludedL( aField, R_PHONEBOOK2_PHONENUMBER_SELECTOR )
            || analyzer.IsFieldTypeIncludedL ( aField, R_PHONEBOOK2_EMAIL_SELECTOR )
            || analyzer.IsFieldTypeIncludedL( aField, R_PHONEBOOK2_SIP_SELECTOR )
            || analyzer.IsFieldTypeIncludedL( aField, R_PHONEBOOK2_IMPP_SELECTOR ) )
        {
        iTextClipper->ClipFromBeginning( aColumnText, aIndex, aColumnIndex );
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
// CPbk2FieldListBoxModel::AppendAddressFieldLabelL
// Appends field label. Removes any listbox separator characters.
// --------------------------------------------------------------------------
//
inline void CPbk2FieldListBoxModel::AppendAddressFieldLabelL
        ( const CPbk2PresentationContactField& aField,
        CPbk2FieldListBoxRow& aRow, TPtr& aColumnBuf )
    {
    aColumnBuf.Set( ExpandColumnBufferL( aField.FieldLabel().Length() ) );
    aColumnBuf.Zero();
    TBool address = EFalse;

    const TInt countProperties =
        aField.FieldProperty().FieldType().VersitProperties().Count();
    TArray<TVPbkFieldVersitProperty> props =
        aField.FieldProperty().FieldType().VersitProperties();
    for ( TInt i = 0; i < countProperties; ++i )
        {
        if ( props[ i ].Name() == EVPbkVersitNameADR
        		&& props[ i ].Parameters().Contains( EVPbkVersitParamHOME ) )
        	{
        	HBufC* label =
        	    StringLoader::LoadLC( R_QTN_PHOB_HEADER_ADDRESS_HOME );
        	Pbk2PresentationUtils::AppendAndReplaceChars
        	    ( aColumnBuf, *label,
        	    KCharsToReplace, KReplacementChars );
        	CleanupStack::PopAndDestroy( label );
        	address = ETrue;
        	}
        else if ( props[ i ].Name() == EVPbkVersitNameADR
        		&& props[ i ].Parameters().Contains( EVPbkVersitParamWORK ) )
        	{
        	HBufC* label =
        	    StringLoader::LoadLC( R_QTN_PHOB_HEADER_ADDRESS_WORK );
        	Pbk2PresentationUtils::AppendAndReplaceChars
        	    ( aColumnBuf, *label,
        	    KCharsToReplace, KReplacementChars );
        	CleanupStack::PopAndDestroy( label );
        	address = ETrue;
        	}
        else if ( props[ i ].Name() == EVPbkVersitNameADR
        		&& !( props[ i ].Parameters().Contains( EVPbkVersitParamHOME ) )
        		&& !( props[ i ].Parameters().Contains( EVPbkVersitParamWORK ) ) )
        	{
        	HBufC* label = StringLoader::LoadLC( R_QTN_PHOB_HEADER_ADDRESS );
        	Pbk2PresentationUtils::AppendAndReplaceChars
        	    ( aColumnBuf, *label,
        	    KCharsToReplace, KReplacementChars );
        	CleanupStack::PopAndDestroy( label );
        	address = ETrue;
        	}
        }

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
// CPbk2FieldListBoxModel::AppendAddressFieldContentL
// Appends field content.
// --------------------------------------------------------------------------
//
inline void CPbk2FieldListBoxModel::AppendAddressFieldContentL
        ( const CPbk2PresentationContactField& aField,
        CPbk2FieldListBoxRow& aRow, TPtr& aColumnBuf )
    {
    TPbk2FieldGroupId address = EPbk2FieldGroupIdNone;
    const TInt countFieldProps =
        aField.FieldProperty().FieldType().VersitProperties().Count();
    TArray<TVPbkFieldVersitProperty> fieldProps =
        aField.FieldProperty().FieldType().VersitProperties();
    for ( TInt i = 0; i < countFieldProps; ++i )
    	{
    	if ( fieldProps[ i ].Name() == EVPbkVersitNameADR )
    		{
    		if ( fieldProps[ i ].Parameters().Contains( EVPbkVersitParamHOME ) )
    			{
    			address = EPbk2FieldGroupIdHomeAddress;
    			}
    		else if ( fieldProps[ i ].Parameters().Contains( EVPbkVersitParamWORK ) )
    			{
    			address = EPbk2FieldGroupIdCompanyAddress;
    			}
    		else if ( !( fieldProps[ i ].Parameters().Contains( EVPbkVersitParamHOME ) )
    				&& !( fieldProps[ i ].Parameters().Contains( EVPbkVersitParamWORK ) ) )
    			{
    			address = EPbk2FieldGroupIdPostalAddress;
    			}
    		}
    	}
    if ( address )
    	{
    	RBuf text;
    	Pbk2AddressTools::GetAddressShortPreviewLC(
    		static_cast<MVPbkStoreContact&>(
    			aField.ParentContact() ), address, text );
    	HBufC* buffer = HBufC::NewLC( text.Length() );
    	TPtr textFormatted ( buffer->Des() );
    	Pbk2PresentationUtils::AppendWithNewlineTranslationL
    	    ( textFormatted, text );
    	aColumnBuf.Set( ExpandColumnBufferL( textFormatted.Length() ) );
    	aColumnBuf.Zero();
    	Pbk2PresentationUtils::AppendAndReplaceChars
    	    ( aColumnBuf, textFormatted, KCharsToReplace, KReplacementChars );
    	CleanupStack::PopAndDestroy( buffer );
    	CleanupStack::PopAndDestroy( &text );	// text
    	// Replace characters that can not be displayed correctly
    	Pbk2PresentationUtils::ReplaceNonGraphicCharacters
    	    (aColumnBuf, ' ');
    	aRow.AppendColumnL( aColumnBuf );
    	}
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::AppendIconsToBeginningL
// Adds icons to the beginning.
// --------------------------------------------------------------------------
//
inline void CPbk2FieldListBoxModel::AppendIconsToBeginningL
        ( const CPbk2PresentationContactField& aField,
          const MVPbkFieldType& aFieldType,
          CPbk2FieldListBoxRow& aRow, TParams& aParams )
    {
    // Format icon index
    TBuf<KMaxFormattedTIntLength> iconText; 
    
    // appened presence icon for xSP field
    TInt iconIndex = KErrNotFound;
    if ( iPresenceIconsPos != NULL && 
         aFieldType.FieldTypeResId() == R_VPBK_FIELD_TYPE_IMPP )
        {
        TPtrC fieldText( iFieldFormatter->FormatFieldContentL
            ( aField, aFieldType ) );
        TInt count = iPresenceIconsPos->Count();
        for ( TInt i = 0; i < count && iconIndex == KErrNotFound; i++ )
            {
				if ( !(fieldText.CompareF( iPresenceIconsPos->operator[](i).
                          iServiceName ) ))
                {
                iconIndex = aParams.iIconArray.FindIcon(
                     iPresenceIconsPos->operator[](i).iIconId );
                }
            }
        }
    
    if ( iconIndex == KErrNotFound )
        {
        iconIndex = aParams.iIconArray.FindIcon(
            aField.FieldProperty().IconId() );
        }
    if ( iconIndex == KErrNotFound )
        {
        iconIndex = aParams.iIconArray.FindIcon( iParams.iDefaultIconId );
        }

    iconText.Num( iconIndex );
    aRow.AppendColumnL( iconText );
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::AppendAddressIconsToBeginningL
// Adds icons to the beginning.
// --------------------------------------------------------------------------
//
inline void CPbk2FieldListBoxModel::AppendAddressIconsToBeginningL
        ( const CPbk2PresentationContactField& aField,
          CPbk2FieldListBoxRow& aRow, TParams& aParams )
    {
    // Format icon index
    TBuf<KMaxFormattedTIntLength> iconText; 
    TInt iconIndex = KErrNotFound;
    
    if ( IsGeoFieldForAddressL( aField ) )
        {
        iconIndex = aParams.iIconArray.FindIcon(
            TPbk2AppIconId( EPbk2qgn_prop_locev_map ) );
        }
    else
    	{
        iconIndex = aParams.iIconArray.FindIcon(
    	    TPbk2AppIconId( EPbk2qgn_prop_pb_no_valid_lm ) );
    	}

    iconText.Num( iconIndex );
    aRow.AppendColumnL( iconText );
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::IsGeoFieldForAddressL
// Checks if address is validated.
// --------------------------------------------------------------------------
//
inline TBool CPbk2FieldListBoxModel::IsGeoFieldForAddressL
        ( const CPbk2PresentationContactField& aField )
    {
    TBool result = EFalse;
    TInt startIndex = 0;

    TPbk2StoreContactAnalyzer analyzer( iParams.iContactManager, NULL );
    if ( analyzer.IsFieldTypeIncludedL(
        	aField, R_PHONEBOOK2_HOME_ADDRESS_SELECTOR ) )
    	{
    	if ( analyzer.HasFieldL( R_PHONEBOOK2_HOME_GEO_SELECTOR,
    			startIndex,
    			&iParams.iFields.ParentStoreContact() ) != KErrNotFound )
    		{
    		result = ETrue;
    		}
    	}
    else if ( analyzer.IsFieldTypeIncludedL(
        	aField, R_PHONEBOOK2_WORK_ADDRESS_SELECTOR ) )
    	{
    	if ( analyzer.HasFieldL( R_PHONEBOOK2_WORK_GEO_SELECTOR,
    			startIndex,
    	    	&iParams.iFields.ParentStoreContact() ) != KErrNotFound )
    	    {
    	    result = ETrue;
    	    }
    	}
    else if ( analyzer.IsFieldTypeIncludedL(
        	aField, R_PHONEBOOK2_GENERAL_ADDRESS_SELECTOR ) )
    	{
    	if ( analyzer.HasFieldL( R_PHONEBOOK2_GENERAL_GEO_SELECTOR,
    			startIndex,
    	    	&iParams.iFields.ParentStoreContact() ) != KErrNotFound )
    	    {
    	    result = ETrue;
    	    }
    	}

    return result;
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
        // Check and add speed dial icon
        if ( aParams.iFieldAnalyzer->HasSpeedDialL( aField.StoreField() ) )
            {
            TBuf<KMaxFormattedTIntLength> iconText;
            iconText.Num( aParams.iIconArray.
                FindIcon( TPbk2AppIconId( EPbk2qgn_indi_qdial_add ) ) );
            aRow.AppendColumnL( iconText );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::DuplicatesExist
// Checks if row with the same content already exists.
// --------------------------------------------------------------------------
//
inline TBool CPbk2FieldListBoxModel::DuplicatesExist(
    TPtrC aContent )
    {
    TBool ret = EFalse;
    TInt rowsCount = iRows.Count();
    for (TInt i = 0; i < rowsCount && !ret; i++ )
        {            
        if ( iRows[i]->At( EContentColumn ).CompareF( aContent ) == 0 )
            {
            ret = ETrue;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2FieldListBoxModel::DuplicatesAddressExist
// Checks if row with the same content already exists.
// --------------------------------------------------------------------------
//
inline TBool CPbk2FieldListBoxModel::DuplicatesAddressExist(
    TPtrC aContent, TInt aIndex )
    {
    TBool ret = EFalse;
    TInt rowsCount = iRows.Count();
    for( TInt i = 0; i < rowsCount && !ret; i++ )
        {
        if ( aIndex < iRows[i]->ColumnCount() )
            {
            if ( iRows[i]->At( aIndex ).CompareF( aContent ) == 0 )
                {
                ret = ETrue;
                }
            }
        }
    
    return ret;
    }

//  End of File
