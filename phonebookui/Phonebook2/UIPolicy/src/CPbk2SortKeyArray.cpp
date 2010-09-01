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
* Description:  Sort key array
*
*/


#include "CPbk2SortKeyArray.h"

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanic_SetText_OOB = 1,
    EPanic_SortKeyAt_OOB
    };

void Panic( TPanicCode aPanic )
    {
    _LIT( KPanicCat, "CPbk2SortKeyArray" );
    User::Panic( KPanicCat, aPanic );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2SortKeyArray::CPbk2SortKeyArray
// --------------------------------------------------------------------------
//
inline CPbk2SortKeyArray::CPbk2SortKeyArray()
    {
    }

// --------------------------------------------------------------------------
// CPbk2SortKeyArray::~CPbk2SortKeyArray
// --------------------------------------------------------------------------
//
CPbk2SortKeyArray::~CPbk2SortKeyArray()
    {
    iKeyTypes.Close();
    iTexts.Close();
    }

// --------------------------------------------------------------------------
// CPbk2SortKeyArray::NewL
// --------------------------------------------------------------------------
//
CPbk2SortKeyArray* CPbk2SortKeyArray::NewL()
    {
    return new ( ELeave ) CPbk2SortKeyArray;
    }

// --------------------------------------------------------------------------
// CPbk2SortKeyArray::AppendL
// --------------------------------------------------------------------------
//
void CPbk2SortKeyArray::AppendL( const TSortKey& aKey )
    {
    User::LeaveIfError( iKeyTypes.Append( aKey.Type() ) );
    TInt err = iTexts.Append( aKey.Text() );
    if ( err != KErrNone )
        {
        // If appending to iTexts was not successful remove also the
        // item in iKeyTypes to keep arrays in sync.
        iKeyTypes.Remove( iKeyTypes.Count() - 1 );
        User::Leave( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SortKeyArray::Reset
// --------------------------------------------------------------------------
//
void CPbk2SortKeyArray::Reset()
    {
    iKeyTypes.Reset();
    iTexts.Reset();
    }

// --------------------------------------------------------------------------
// CPbk2SortKeyArray::SetText
// --------------------------------------------------------------------------
//
void CPbk2SortKeyArray::SetText( const TDesC& aText, TInt aIndex )
    {
    __ASSERT_DEBUG( iTexts.Count() > aIndex,
        Panic( EPanic_SetText_OOB ) );

    iTexts[aIndex].Set( aText );
    }

// --------------------------------------------------------------------------
// CPbk2SortKeyArray::SortKeyCount
// --------------------------------------------------------------------------
//
TInt CPbk2SortKeyArray::SortKeyCount() const
    {
    return iKeyTypes.Count();
    }

// --------------------------------------------------------------------------
// CPbk2SortKeyArray::SortKeyAt
// --------------------------------------------------------------------------
//
TSortKey CPbk2SortKeyArray::SortKeyAt( TInt aIndex ) const
    {
    __ASSERT_DEBUG( iTexts.Count() > aIndex && iKeyTypes.Count() > aIndex,
        Panic( EPanic_SortKeyAt_OOB ) );

    return TSortKey( iTexts[aIndex], iKeyTypes[aIndex] );
    }

// End of File
