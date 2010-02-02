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
 * Description:  Listbox model row for MyCard details list
 *
 */

#include "ccappmycardlistboxrow.h"

// ======== MEMBER FUNCTIONS ========

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxRow::CCCAppMyCardListBoxRow
// --------------------------------------------------------------------------
//
inline CCCAppMyCardListBoxRow::CCCAppMyCardListBoxRow()
    {
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxRow::~CCCAppMyCardListBoxRow
// --------------------------------------------------------------------------
//
CCCAppMyCardListBoxRow::~CCCAppMyCardListBoxRow()
    {
    iColumns.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxRow::NewL
// --------------------------------------------------------------------------
//
CCCAppMyCardListBoxRow* CCCAppMyCardListBoxRow::NewL()
    {
    return new (ELeave) CCCAppMyCardListBoxRow;
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxRow::ColumnCount
// --------------------------------------------------------------------------
//
TInt CCCAppMyCardListBoxRow::ColumnCount() const
    {
    return iColumns.Count();
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxRow::At
// --------------------------------------------------------------------------
//
TPtrC CCCAppMyCardListBoxRow::At(TInt aColumnIndex) const
    {
    return *iColumns[aColumnIndex];
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxRow::AppendColumnL
// --------------------------------------------------------------------------
//
void CCCAppMyCardListBoxRow::AppendColumnL(const TDesC& aColumnText)
    {
    HBufC* buf = aColumnText.AllocLC();
    iColumns.AppendL( buf );
    CleanupStack::Pop( buf );
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxRow::IsClipRequired
// --------------------------------------------------------------------------
//
TBool CCCAppMyCardListBoxRow::IsClipRequired() const
    {
    return iClipBeginning;
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxRow::SetClipRequired
// --------------------------------------------------------------------------
//
void CCCAppMyCardListBoxRow::SetClipRequired(TBool aFlag)
    {
    iClipBeginning = aFlag;
    }

// --------------------------------------------------------------------------
// CCCAppMyCardListBoxRow::TotalLength
// --------------------------------------------------------------------------
//
TInt CCCAppMyCardListBoxRow::TotalLength() const
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
// CCCAppMyCardListBoxRow::MaxColumnLength
// --------------------------------------------------------------------------
//
TInt CCCAppMyCardListBoxRow::MaxColumnLength() const
    {
    TInt result = 0;
    const TInt count = iColumns.Count();
    for (TInt i = 0; i < count; ++i)
        {
        result = Max(result, At(i).Length());
        }
    return result;
    }

// End of File
