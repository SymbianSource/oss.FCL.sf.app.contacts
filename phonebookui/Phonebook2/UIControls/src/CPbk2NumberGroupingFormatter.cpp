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
* Description:  Phonebook 2 number grouping phone number formatter.
*
*/


#include "CPbk2NumberGroupingFormatter.h"

// System includes
#include <NumberGrouping.h>
#include <AknUtils.h>

// Define to force number grouping on for testing purposes
#undef PBK2_FORCE_NUMBERGROUPING_ON

/// Unnamed namespace for local definitions
namespace {

const TInt KGroupingFactor = 2;

}  /// namespace


// --------------------------------------------------------------------------
// CPbk2NumberGroupingFormatter::CPbk2NumberGroupingFormatter
// --------------------------------------------------------------------------
//
CPbk2NumberGroupingFormatter::CPbk2NumberGroupingFormatter()
    {
    }

// --------------------------------------------------------------------------
// CPbk2NumberGroupingFormatter::~CPbk2NumberGroupingFormatter
// --------------------------------------------------------------------------
//
CPbk2NumberGroupingFormatter::~CPbk2NumberGroupingFormatter()
    {
    delete iNumberGrouping;
    delete iFormatterNumberBuffer;
    }

// --------------------------------------------------------------------------
// CPbk2NumberGroupingFormatter::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2NumberGroupingFormatter::ConstructL
        (  TInt aMaxDisplayLength )
    {
    // The grouping object takes care of calculating the size of the
    // grouped number, just pass the length of the non-grouped number
    iNumberGrouping = CPNGNumberGrouping::NewL( aMaxDisplayLength );

    iFormatterNumberBuffer = HBufC::NewL
        ( KGroupingFactor * aMaxDisplayLength );

#ifdef PBK2_FORCE_NUMBERGROUPING_ON
    iNumberGrouping->iForceLanguage = ELangAmerican;
#endif // PBK2_FORCE_NUMBERGROUPING_ON
    }

// --------------------------------------------------------------------------
// CPbk2NumberGroupingFormatter::NewL
// --------------------------------------------------------------------------
//
CPbk2NumberGroupingFormatter* CPbk2NumberGroupingFormatter::NewL
        ( TInt aMaxDisplayLength )
    {
    CPbk2NumberGroupingFormatter* self =
        new( ELeave ) CPbk2NumberGroupingFormatter;
    CleanupStack::PushL( self );
    self->ConstructL( aMaxDisplayLength );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NumberGroupingFormatter::SetMaxBufferLengthL
// --------------------------------------------------------------------------
//
void CPbk2NumberGroupingFormatter::SetMaxBufferLengthL( TInt aMaxLength )
    {
    if ( aMaxLength > iNumberGrouping->MaxDisplayLength() )
        {
        CPNGNumberGrouping* newNumberGrouping =
            CPNGNumberGrouping::NewL( aMaxLength );
        delete iNumberGrouping;
        iNumberGrouping = newNumberGrouping;
        iFormatterNumberBuffer =
            iFormatterNumberBuffer->ReAllocL( KGroupingFactor * aMaxLength );
        }

#ifdef PBK2_FORCE_NUMBERGROUPING_ON
    iNumberGrouping->iForceLanguage = ELangAmerican;
#endif // PBK2_FORCE_NUMBERGROUPING_ON
    }

// --------------------------------------------------------------------------
// CPbkNumberGroupingFormatter::FormatPhoneNumberForDisplay
// --------------------------------------------------------------------------
//
TPtrC CPbk2NumberGroupingFormatter::FormatPhoneNumberForDisplay(
        const TDesC& aOriginalPhoneNumber )
    {
    if ( aOriginalPhoneNumber.Length() <=
         iNumberGrouping->MaxDisplayLength() )
        {
        iNumberGrouping->Set( aOriginalPhoneNumber );
        TPtr formatterNumber = iFormatterNumberBuffer->Des();
        formatterNumber.Copy( iNumberGrouping->FormattedNumber() );

        AknTextUtils::DisplayTextLanguageSpecificNumberConversion
            ( formatterNumber );
        return formatterNumber;
        }
    else
        {
        // Too long number to format, just return the original. This is
        // in line with MPbkPhoneNumberFormatter interface's
        // best-effort promise.
        return aOriginalPhoneNumber;
        }
    }

// End of File
