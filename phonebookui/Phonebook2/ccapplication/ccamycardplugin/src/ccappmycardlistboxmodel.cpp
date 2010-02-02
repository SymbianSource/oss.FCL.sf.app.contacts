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
 * Description:  Listbox model for MyCard details list
 *
 */

#include <TPbk2StoreContactAnalyzer.h>
#include <Pbk2UIControls.rsg>
#include <Pbk2PresentationUtils.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactField.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2IconArray.h>
#include <MPbk2FieldProperty.h>

#include <CVPbkContactManager.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldData.h>
#include <VPbkUtils.h>
#include <VPbkEng.rsg>

#include <aknlists.h>
#include <avkon.rsg>

#include "ccappmycardcommon.h"
#include "ccappmycardlistboxmodel.h"
#include "ccappmycardlistboxrow.h"
#include <ccappmycardpluginrsc.rsg>


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


// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::NewL
// ---------------------------------------------------------------------------
//
CCCAppMyCardListBoxModel* CCCAppMyCardListBoxModel::NewL(CCCAppMyCard& aMyCard,
        CCoeEnv& aCoeEnv, CEikListBox& aListBox, CPbk2IconArray& aIconArray)
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardListBoxModel::NewL()") );

    CCCAppMyCardListBoxModel* self = new (ELeave) CCCAppMyCardListBoxModel(
            aMyCard, aCoeEnv, aListBox, aIconArray);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardListBoxModel::NewL()") );
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::~CCCAppMyCardListBoxModel
// ---------------------------------------------------------------------------
//
CCCAppMyCardListBoxModel::~CCCAppMyCardListBoxModel()
    {
    CCA_DP(KMyCardLogFile,
            CCA_L("->CCCAppMyCardListBoxModel::~CCCAppMyCardListBoxModel()") );

    iRows.ResetAndDestroy();
    delete iLineBuf;
    delete iColumnBuf;
    iInxToPresentationIdx.Close();

    CCA_DP(KMyCardLogFile,
            CCA_L("<-CCCAppMyCardListBoxModel::~CCCAppMyCardListBoxModel()") );
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::CCCAppMyCardListBoxModel
// ---------------------------------------------------------------------------
//
CCCAppMyCardListBoxModel::CCCAppMyCardListBoxModel(
    CCCAppMyCard& aMyCard,
    CCoeEnv& aCoeEnv,
    CEikListBox& aListBox,
    CPbk2IconArray& aIconArray ) :
        iCoeEnv(aCoeEnv),
        iMyCard(aMyCard),
        iListBox(aListBox),
        iIconArray(aIconArray),
        iRows(KRowArrayGranularity)
    {
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardListBoxModel::ConstructL()
    {
    iLineBuf = HBufC::NewL(KBufferSize);
    iColumnBuf = HBufC::NewL(KBufferSize);

    iMyCard.AddObserverL(this);
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::MyCardEventL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardListBoxModel::MyCardEventL(TEvent aEvent)
    {
    if( aEvent == MMyCardObserver::EEventContactLoaded)
        {
        CCA_DP( KMyCardLogFile,
            CCA_L("->CCCAppMyCardListBoxModel::MyCardEventL EEventContactLoaded") );

        iPresentationContact = &iMyCard.PresentationContactL();
        AddDataL();
        }
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::MdcaCount
// ---------------------------------------------------------------------------
//
TInt CCCAppMyCardListBoxModel::MdcaCount() const
    {
    return iRows.Count();
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::MdcaPoint
// ---------------------------------------------------------------------------
//
TPtrC CCCAppMyCardListBoxModel::MdcaPoint(TInt aIndex) const
    {
    TPtr rowText( iLineBuf->Des() );
    rowText.Zero();

    const CCCAppMyCardListBoxRow& row = *iRows[aIndex];
    const TInt columnCount = row.ColumnCount();
    TInt fieldCount = iPresentationContact->PresentationFields().FieldCount();

    for( TInt index = 0; index < columnCount; ++index )
        {
        TPtr columnText( iColumnBuf->Des() );
        columnText.Copy( row.At(index) );

        // Clip the column if required
        if( index == KContentColumnIndex && 
            row.IsClipRequired() && 
            aIndex < fieldCount )
            {
            ClipFromBeginning( columnText, aIndex, index );
            }
 
        // Append the column and separator to the formatted row
        rowText.Append( columnText );
        rowText.Append( KColumnSeparator );
        }

    return rowText;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::FieldIndex
// ---------------------------------------------------------------------------
//
TInt CCCAppMyCardListBoxModel::FieldIndex( TInt aIndex )
    {
    TInt* index = iInxToPresentationIdx.Find( aIndex );

    if( index && *index >= 0 )
        {
        return iPresentationContact->PresentationFields().StoreIndexOfField( *index );
        }
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::AddDataL
// --------------------------------------------------------------------------
//
void CCCAppMyCardListBoxModel::AddDataL()
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardListBoxModel::AddDataL()") );

    iRows.ResetAndDestroy();
    iInxToPresentationIdx.Close();

    TInt listIdx = -1;

    TInt fieldCount = iPresentationContact->PresentationFields().FieldCount();
    for (TInt index = 0; index < fieldCount; index++)
        {
        CPbk2PresentationContactField& field =
                iPresentationContact->PresentationFields().At(index);

        // Get master field type list and match field's type against it
        const MVPbkFieldType* fieldType = VPbkUtils::MatchFieldType(
                iMyCard.ContactManager().FieldTypes(), field );

        if( field.IsEditable() && fieldType && !IsHiddenField(fieldType) )
            {
            HBufC* label = GetLabelLC( field );

            if( label->Length() )
                {
                CCCAppMyCardListBoxRow* row =
                        CCCAppMyCardListBoxRow::NewL();
                CleanupStack::PushL(row);

                const MVPbkContactFieldData& fieldData = field.FieldData();

                // Add icon
                TBuf<20> buffer; // 20 should be enough for icon index
                TInt iconIndex = iIconArray.FindIcon(
                        field.FieldProperty().IconId() );
                if( iconIndex != KErrNotFound)
                    {
                    buffer.AppendNum(iconIndex);
                    }
                row->AppendColumnL(buffer);

                // add label.
                row->AppendColumnL(*label);

                // Add current index to Presentation Contact index array 
                // to estimate the text is needed to clip or not.
                iInxToPresentationIdx.InsertL(++listIdx, index);

                // add field content.
                switch (fieldData.DataType() )
                    {
                    case EVPbkFieldStorageTypeText:
                        {
                        HandleTextTypeFieldL(index, fieldData, row);
                        break;
                        }
                    case EVPbkFieldStorageTypeDateTime:
                        {
                        HandleDateTimeTypeFieldL(fieldData, row);
                        break;
                        }
                    case EVPbkFieldStorageTypeUri:
                        {
                        HandleUriTypeFieldL(index, fieldData, row);
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

    iListBox.HandleItemAdditionL();

    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardListBoxModel::AddDataL()") );
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::IsHiddenField
// --------------------------------------------------------------------------
//
TBool CCCAppMyCardListBoxModel::IsHiddenField(const MVPbkFieldType* aFieldType)
    {
    TInt resId = aFieldType->FieldTypeResId();
    return ( resId == R_VPBK_FIELD_TYPE_SYNCCLASS ||
             resId == R_VPBK_FIELD_TYPE_CALLEROBJIMG );
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::FieldAtLC
// ---------------------------------------------------------------------------
//
MVPbkBaseContactField* CCCAppMyCardListBoxModel::FieldAtLC(TInt aIndex)
    {
    TInt *presentationIdx = iInxToPresentationIdx.Find(aIndex);

    if( !presentationIdx)
        {
        return NULL;
        }

    TInt index = iPresentationContact->PresentationFields().StoreIndexOfField(
            *presentationIdx);

    if( index != KErrNotFound)
        {
        // Use FieldAtLC to avoid the unvalidity of the field after new
        // FieldAt call.
        return iMyCard.StoreContact().Fields().FieldAtLC(index);
        }

    return NULL;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::IsFieldTypeL
// ---------------------------------------------------------------------------
//
TBool CCCAppMyCardListBoxModel::IsFieldTypeL(
    TInt aIndex, TInt aSelectorResId )
    {
    TBool ret = EFalse;
    MVPbkBaseContactField* field = FieldAtLC(aIndex);

    if( field )
        {
        TPbk2StoreContactAnalyzer analyzer( iMyCard.ContactManager(), NULL );
        ret = analyzer.IsFieldTypeIncludedL( *field, aSelectorResId );
        CleanupStack::PopAndDestroy(field);
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::HandleTextTypeFieldL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardListBoxModel::HandleTextTypeFieldL( 
    TInt aIndex,
    const MVPbkContactFieldData& aFieldData, 
    CCCAppMyCardListBoxRow* aRow )
    {
    const TDesC& fieldText = MVPbkContactFieldTextData::Cast(aFieldData).Text();

    // Check if clipping is required
    if( IsFieldTypeL( aIndex, R_MYCARD_CLIP_FIELD_SELECTOR ) )
        {
        aRow->SetClipRequired( ETrue );
        }
    
    TPtr columnBuf( iColumnBuf->Des() );
    columnBuf.Set( ExpandColumnBufferL( fieldText.Length() ) );
    columnBuf.Zero();

    // replace listbox separator characters.
    Pbk2PresentationUtils::AppendAndReplaceChars( columnBuf, fieldText,
            KCharsToReplace, KReplacementChars );

    // Replace characters that can not be displayed correctly.
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters( columnBuf, KReplacedChars );

    aRow->AppendColumnL( columnBuf );
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::HandleDateTimeTypeFieldL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardListBoxModel::HandleDateTimeTypeFieldL(
        const MVPbkContactFieldData& aFieldData, CCCAppMyCardListBoxRow* aRow)
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
// CCCAppMyCardListBoxModel::HandleUriTypeFieldL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardListBoxModel::HandleUriTypeFieldL(TInt /*aIndex*/,
        const MVPbkContactFieldData& aFieldData, CCCAppMyCardListBoxRow* aRow)
    {
    const MVPbkContactFieldUriData& uri = 
        MVPbkContactFieldUriData::Cast( aFieldData );

    HBufC* tempBuf = uri.Text().AllocLC();
    TPtr text = tempBuf->Des();
    // Replace characters that can not be displayed correctly.
    Pbk2PresentationUtils::ReplaceNonGraphicCharacters( text, KReplacedChars );
    
    aRow->AppendColumnL( text );
    CleanupStack::PopAndDestroy( tempBuf );
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::ClipFromBeginning
// --------------------------------------------------------------------------
//
TBool CCCAppMyCardListBoxModel::ClipFromBeginning(TDes& aBuffer,
        TInt aItemIndex, TInt aSubCellNumber) const
    {
    // TODO: This could be better. Model should not care about the 
    // type of the listbox.
    CAknFormDoubleGraphicStyleListBox* listbox =
        static_cast<CAknFormDoubleGraphicStyleListBox*> (&iListBox);

    return AknTextUtils::ClipToFit( aBuffer, AknTextUtils::EClipFromBeginning,
        listbox, aItemIndex, aSubCellNumber );
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::ExpandBuffersL
// --------------------------------------------------------------------------
//
void CCCAppMyCardListBoxModel::ExpandBuffersL(CCCAppMyCardListBoxRow* aRow)
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
// CCCAppMyCardListBoxModel::ExpandColumnBufferL
// --------------------------------------------------------------------------
//
TPtr CCCAppMyCardListBoxModel::ExpandColumnBufferL(TInt aRequiredLength)
    {
    if( aRequiredLength > iColumnBuf->Des().MaxLength() )
        {
        iColumnBuf = iColumnBuf->ReAllocL(aRequiredLength);
        }

    return iColumnBuf->Des();
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxModel::GetLabelLC
// --------------------------------------------------------------------------
//
HBufC* CCCAppMyCardListBoxModel::GetLabelLC(
        const CPbk2PresentationContactField& aField)
    {
    TPtr columnBuf( iColumnBuf->Des() );
    columnBuf.Set( ExpandColumnBufferL(aField.FieldLabel().Length() ) );
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

// End of File
