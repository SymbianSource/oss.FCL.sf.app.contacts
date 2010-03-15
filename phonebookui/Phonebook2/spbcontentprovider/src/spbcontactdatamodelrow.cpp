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
 * Description:  Listbox model row contact model
 *
 */

#include "spbcontactdatamodelrow.h"
#include <s32strm.h>

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::CSpbContactDataModelRow
// --------------------------------------------------------------------------
//
inline CSpbContactDataModelRow::CSpbContactDataModelRow()
    {
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::~CSpbContactDataModelRow
// --------------------------------------------------------------------------
//
CSpbContactDataModelRow::~CSpbContactDataModelRow()
    {
    iColumns.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::NewL
// --------------------------------------------------------------------------
//
CSpbContactDataModelRow* CSpbContactDataModelRow::NewL()
    {
    return new (ELeave) CSpbContactDataModelRow;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::ColumnCount
// --------------------------------------------------------------------------
//
TInt CSpbContactDataModelRow::ColumnCount() const
    {
    return iColumns.Count();
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::At
// --------------------------------------------------------------------------
//
TPtrC CSpbContactDataModelRow::At(TInt aColumnIndex) const
    {
    return *iColumns[aColumnIndex];
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::AppendColumnL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelRow::AppendColumnL(const TDesC& aColumnText)
    {
    HBufC* buf = aColumnText.AllocLC();
    iColumns.AppendL( buf );
    CleanupStack::Pop( buf );
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::ReplaceColumnL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelRow::ReplaceColumnL( 
    TInt aIndex, const TDesC& aColumnText )
    {
    HBufC* buf = aColumnText.AllocL();
    HBufC*& old = iColumns[aIndex];
    delete old;
    old = buf;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::IsClipRequired
// --------------------------------------------------------------------------
//
TBool CSpbContactDataModelRow::IsClipRequired() const
    {
    return iClipBeginning;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::SetClipRequired
// --------------------------------------------------------------------------
//
void CSpbContactDataModelRow::SetClipRequired(TBool aFlag)
    {
    iClipBeginning = aFlag;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::Icon
// --------------------------------------------------------------------------
//
const TPbk2IconId& CSpbContactDataModelRow::Icon()
    {
    return iIcon;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::SetIcon
// --------------------------------------------------------------------------
//
void CSpbContactDataModelRow::SetIcon( const TPbk2IconId& aIcon )
    {
    iIcon = aIcon;
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::TotalLength
// --------------------------------------------------------------------------
//
TInt CSpbContactDataModelRow::TotalLength() const
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
// CSpbContactDataModelRow::MaxColumnLength
// --------------------------------------------------------------------------
//
TInt CSpbContactDataModelRow::MaxColumnLength() const
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
// CSpbContactDataModelRow::ExternalizeL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelRow::ExternalizeL( RWriteStream& aStream ) const
    {
    const TInt count = iColumns.Count();
    aStream.WriteInt8L( iClipBeginning );
    aStream.WriteL( (TUint8*)(&iIcon), sizeof( TPbk2IconId ) );

    aStream.WriteInt32L( count );
    for (TInt i = 0; i < count; ++i)
        {
        HBufC* text = iColumns[i]; 
        aStream.WriteInt32L( text->Length() );
        aStream.WriteL( *text );
        }
    }

// --------------------------------------------------------------------------
// CSpbContactDataModelRow::InternalizeL
// --------------------------------------------------------------------------
//
void CSpbContactDataModelRow::InternalizeL( RReadStream& aStream )
    {
    iColumns.ResetAndDestroy();

    iClipBeginning = aStream.ReadInt8L();
    aStream.ReadL( (TUint8*)(&iIcon), sizeof( TPbk2IconId ) );
    
    const TInt count = aStream.ReadInt32L();
    for (TInt i = 0; i < count; ++i)
        {
        const TInt length = aStream.ReadInt32L();
        HBufC* buffer = HBufC::NewLC( length );
        TPtr column( buffer->Des() );
        aStream.ReadL( column, length );
        iColumns.AppendL( buffer );
        CleanupStack::Pop( buffer );
        }
    }

// End of File
