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
* Description:   Utility class for splitting find string into separate words
*
*/


// INCLUDES
#include "CPbk2FindStringSplitter.h"
#include <MPbk2ContactNameFormatter.h>
#include <badesca.h>


//----------------------------------------------------------------------------
// CPbk2FindStringSplitter::CPbk2FindStringSplitter
//----------------------------------------------------------------------------
//
inline CPbk2FindStringSplitter::CPbk2FindStringSplitter(
    MPbk2ContactNameFormatter& aNameFormatter ):
    iNameFormatter( aNameFormatter )
    {
    }


//----------------------------------------------------------------------------
// CPbk2FindStringSplitter::~CPbk2FindStringSplitter
//----------------------------------------------------------------------------
//
CPbk2FindStringSplitter::~CPbk2FindStringSplitter()
    {
    }


//----------------------------------------------------------------------------
// CPbk2FindStringSplitter::NewL
//----------------------------------------------------------------------------
//
CPbk2FindStringSplitter* CPbk2FindStringSplitter::NewL(
        MPbk2ContactNameFormatter& aNameFormatter )
    {
    CPbk2FindStringSplitter* self = new ( ELeave ) CPbk2FindStringSplitter(
        aNameFormatter );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


//----------------------------------------------------------------------------
// CPbk2FindStringSplitter::ConstructL
//----------------------------------------------------------------------------
//
void CPbk2FindStringSplitter::ConstructL()
    {
    }
    
//----------------------------------------------------------------------------
// CPbk2FindStringSplitter::SplitTextIntoArrayL
//----------------------------------------------------------------------------
//
MDesCArray* CPbk2FindStringSplitter::SplitTextIntoArrayL( const TDesC& aText )
    {
    const TInt KGranularity = 2; 
    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( KGranularity );
    const TInt textLength = aText.Length();
    for ( TInt beg = 0; beg < textLength; ++beg )
        {
        // Skip separators before next word
        if ( !iNameFormatter.IsFindSeparatorChar( aText[beg] ) )
            {
            // Scan the end of the word
            TInt end = beg;
            for (; end < textLength &&
                   !iNameFormatter.IsFindSeparatorChar( aText[end] );
                ++end )
                {
                }
            const TInt len = end - beg;
            // Append found word to the array
            array->AppendL( aText.Mid( beg,len ) );
            // Scan for next word
            beg = end;
            }
        }
    return array;
    }
