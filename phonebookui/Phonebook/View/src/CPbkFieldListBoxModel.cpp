/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides methods for phonebook list box model class.
*
*/


// INCLUDE FILES
#include "CPbkFieldListBoxModel.h"  // this class

#include "MPbkClipListBoxText.h"    // interface for clipping text
#include "MPbkFieldAnalyzer.h"      // interface for analyzing field contents
#include <CPbkIconArray.h>
#include "CPbkFieldFormatter.h"
#include "CPbkInputAbsorber.h"

#include <CPbkContactItem.h>    // CPbkFieldArray
#include <CPbkFieldInfo.h>      // CPbkFieldInfo
#include <PbkEngUtils.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ConstructL,
    EPanicPostCond_ConstructL
    };

// ================= LOCAL FUNCTIONS =======================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkFieldListBoxModel");
    User::Panic(KPanicText, aReason);
    }
#endif

const TInt KMaxFormattedTIntLength = 15;
const TInt KInitialBufferLength = 128;

}

// ================= MEMBER FUNCTIONS =======================

// CPbkFieldListBoxRow ////////////////////////////////////////////////////////

inline CPbkFieldListBoxRow::CPbkFieldListBoxRow()
    {
    }

CPbkFieldListBoxRow* CPbkFieldListBoxRow::NewL()
    {
    CPbkFieldListBoxRow* self = new(ELeave) CPbkFieldListBoxRow;
    return self;
    }

CPbkFieldListBoxRow::~CPbkFieldListBoxRow()
    {
    iColumns.ResetAndDestroy();
    }

TInt CPbkFieldListBoxRow::ColumnCount() const
    {
    return iColumns.Count();
    }

TPtrC CPbkFieldListBoxRow::At(TInt aColumnIndex) const
    {
    return *iColumns[aColumnIndex];
    }

void CPbkFieldListBoxRow::AppendColumnL(const TDesC& aColumnText)
    {
    HBufC* buf = aColumnText.AllocLC();
    User::LeaveIfError(iColumns.Append(buf));
    CleanupStack::Pop(buf);
    }

TInt CPbkFieldListBoxRow::TotalLength() const
    {
    TInt result = 0;
    const TInt count = iColumns.Count();
    for (TInt i = 0; i < count; ++i)
        {
        result += At(i).Length();
        }
    return result;
    }

TInt CPbkFieldListBoxRow::MaxColumnLength() const
    {
    TInt result = 0;
    const TInt count = iColumns.Count();
    for (TInt i = 0; i < count; ++i)
        {
        result = Max(result, At(i).Length());
        }
    return result;
    }


// CPbkFieldListBoxModel //////////////////////////////////////////////////////

CPbkFieldListBoxModel::TParams::TParams
        (const CPbkFieldArray& aFields,
        const TDesC& aTimeFormat,
        const CPbkIconArray& aIconArray) :
    iFields(aFields),
    iTimeFormat(aTimeFormat),
    iIconArray(aIconArray),
    iDefaultIconId(EPbkNullIconId),
    iFieldAnalyzer(NULL)
    {
    }

inline CPbkFieldListBoxModel::CPbkFieldListBoxModel() :
    iRows(8)
    {
    // CBase::operator new() will reset member data
    __ASSERT_DEBUG(!iFields, Panic(EPanicPostCond_Constructor));
    }

CPbkFieldListBoxModel* CPbkFieldListBoxModel::NewL
        (TParams& aParams)
    {
    CPbkFieldListBoxModel* self = new(ELeave) CPbkFieldListBoxModel;
    CleanupStack::PushL(self);
    self->iFields = &aParams.iFields;
    self->ConstructL(aParams);
    CleanupStack::Pop(self);
    return self;
    }

CPbkFieldListBoxModel::~CPbkFieldListBoxModel()
    {
    iRows.ResetAndDestroy();
    delete iColumnBuf;
    delete iLineBuf;
    delete iFieldFormatter;
    delete iAbsorber;
    }

const CPbkFieldArray& CPbkFieldListBoxModel::Array() const
    {
    return *iFields;
    }

void CPbkFieldListBoxModel::AppendRowL
        (CPbkFieldListBoxRow* aLine)
    {
    ExpandBuffersL(*aLine);
    iRows.AppendL(aLine);
    }

void CPbkFieldListBoxModel::SetClipper
        (MPbkClipListBoxText& aTextClipper)
    {
    this->iTextClipper = &aTextClipper;
    }

TInt CPbkFieldListBoxModel::MdcaCount() const
    {
    return iRows.Count();
    }

TPtrC16 CPbkFieldListBoxModel::MdcaPoint
        (TInt aIndex) const
    {
    const TText KSeparator = '\t';
    TPtr rowText(iLineBuf->Des());
    rowText.Zero();

    const CPbkFieldListBoxRow& row = *iRows[aIndex];
    const TInt columnCount = row.ColumnCount();
    for (TInt columnIndex = 0; columnIndex < columnCount; ++columnIndex)
        {
        TPtr columnText(iColumnBuf->Des());
        columnText.Copy(row.At(columnIndex));

        // Clip the column if required
        if (iTextClipper &&
            columnIndex == EContentColumn &&
            aIndex < iFields->Count())
            {
            const TPbkContactItemField& field = (*iFields)[aIndex];
            // Clip text from beginning if field is numeric field
            // or e-mail field
            if ((field.FieldInfo().NumericField()) ||
                (field.FieldInfo().FieldId() == EPbkFieldIdEmailAddress))
                {
                iTextClipper->ClipFromBeginning(columnText, aIndex, columnIndex);
                }
            }

        // Append the column and separator to the formatted row
        rowText.Append(columnText);
        rowText.Append(KSeparator);
        }

    return rowText;
    }

void CPbkFieldListBoxModel::FormatFieldL
        (const TPbkContactItemField& aField,
        TParams& aParams)
    {
    CPbkFieldListBoxRow* row = CPbkFieldListBoxRow::NewL();
    CleanupStack::PushL(row);

    // Format icon index
    TPbkIconId iconId = aParams.iDefaultIconId;
    if (iconId < 0)
        {
        iconId = aField.IconId();
        }
    TBuf<KMaxFormattedTIntLength> iconText;
    TInt iconIndex = aParams.iIconArray.FindIcon(iconId);
    if (iconIndex < 0)
        {
        // If icon is not found by Id fall back to empty icon
        iconIndex = aParams.iIconArray.FindIcon(EPbkqgn_prop_nrtyp_empty);
        }
    iconText.Num(iconIndex);
    row->AppendColumnL(iconText);

    _LIT(KCharsToReplace, "\t");
    _LIT(KReplacementChars, " ");

    // Append field label. Remove any listbox separator characters.
    TPtr columnBuf(ExpandColumnBufferL(aField.Label().Length()));
    columnBuf.Zero();
    PbkEngUtils::AppendAndReplaceChars
        (columnBuf, aField.Label(), KCharsToReplace, KReplacementChars);
    // replace characters that can not be displayed corrctly
    PbkEngUtils::ReplaceNonGraphicCharacters(columnBuf, ' ');
    row->AppendColumnL(columnBuf);

    // Append field content
    TPtrC fieldText(iFieldFormatter->FormatFieldContentL(aField));
    columnBuf.Set(ExpandColumnBufferL(fieldText.Length()));
    columnBuf.Zero();
    PbkEngUtils::AppendAndReplaceChars
        (columnBuf, fieldText, KCharsToReplace, KReplacementChars);
    // replace characters that can not be displayed corrctly
    PbkEngUtils::ReplaceNonGraphicCharacters(columnBuf, ' ');
    row->AppendColumnL(columnBuf);

    // Add additional icons to the end
    if (aParams.iFieldAnalyzer)
        {
        // Check and add speed dial icon
        if (aParams.iFieldAnalyzer->HasSpeedDialL(aField))
            {
            iconText.Num(aParams.iIconArray.FindIcon(EPbkqgn_indi_qdial_add));
            row->AppendColumnL(iconText);
            }

        // Check and add voice tag icon
        if (aParams.iFieldAnalyzer->HasVoiceTagL(aField))
            {
            iconText.Num(aParams.iIconArray.FindIcon(EPbkqgn_indi_voice_add));
            row->AppendColumnL(iconText);
            }
        }

    // Expand row formatting buffer if required
    ExpandBuffersL(*row);

    // Add the row
    iRows.AppendL(row);
    CleanupStack::Pop(row);
    }

void CPbkFieldListBoxModel::ConstructL
        (TParams& aParams)
    {
    __ASSERT_DEBUG(iFields, Panic(EPanicPreCond_ConstructL));

    // Install input absorber to ignore any activity while list model
    // is contructed (Voice tag query does active scheduler wait)
    iAbsorber = CPbkInputAbsorber::NewL();

    iFieldFormatter = CPbkFieldFormatter::NewL();
    iFieldFormatter->SetTimeFormatL(aParams.iTimeFormat);
    iLineBuf = HBufC::NewL(KInitialBufferLength);
    iColumnBuf = HBufC::NewL(KInitialBufferLength);

    // Format all the fields and add lines to iTextArray
    const TInt fieldCount = iFields->Count();
    for (TInt i=0; i < fieldCount; ++i)
        {
        FormatFieldL((*iFields)[i], aParams);
        }

    // Remove input absorber
    delete iAbsorber;
    iAbsorber = NULL;

    __ASSERT_DEBUG(iFields, Panic(EPanicPostCond_ConstructL));
    }

void CPbkFieldListBoxModel::ExpandBuffersL
        (const CPbkFieldListBoxRow& aRow)
    {
    // Row formatting buffer
    const TInt rowLength = aRow.TotalLength()
        + aRow.ColumnCount();  // for separator characters
    if (rowLength > iLineBuf->Des().MaxLength())
        {
        iLineBuf = iLineBuf->ReAllocL(rowLength);
        }

    ExpandColumnBufferL(aRow.MaxColumnLength());
    }

TPtr CPbkFieldListBoxModel::ExpandColumnBufferL
        (TInt aRequiredLength)
    {
    if (aRequiredLength > iColumnBuf->Des().MaxLength())
        {
        iColumnBuf = iColumnBuf->ReAllocL(aRequiredLength);
        }
    return (iColumnBuf->Des());
    }

//  End of File
