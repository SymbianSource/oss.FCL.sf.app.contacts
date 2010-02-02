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
* Description:  Phonebook 2 default phone number formatter.
*
*/


#include "CPbk2DefaultPhoneNumberFormatter.h"

// System includes
#include <AknUtils.h>

// --------------------------------------------------------------------------
// CPbk2DefaultPhoneNumberFormatter::CPbk2DefaultPhoneNumberFormatter
// --------------------------------------------------------------------------
//
CPbk2DefaultPhoneNumberFormatter::CPbk2DefaultPhoneNumberFormatter()
    {
    }

// --------------------------------------------------------------------------
// CPbk2DefaultPhoneNumberFormatter::~CPbk2DefaultPhoneNumberFormatter
// --------------------------------------------------------------------------
//
CPbk2DefaultPhoneNumberFormatter::~CPbk2DefaultPhoneNumberFormatter()
    {
    delete iFormatterNumberBuffer;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultPhoneNumberFormatter::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2DefaultPhoneNumberFormatter::ConstructL
        ( TInt aMaxDisplayLength )
    {
    iFormatterNumberBuffer = HBufC::NewL( aMaxDisplayLength );
    }

// --------------------------------------------------------------------------
// CPbk2DefaultPhoneNumberFormatter::NewL
// --------------------------------------------------------------------------
//
CPbk2DefaultPhoneNumberFormatter* CPbk2DefaultPhoneNumberFormatter::NewL
        ( TInt aMaxDisplayLength )
    {
    CPbk2DefaultPhoneNumberFormatter* self =
        new( ELeave ) CPbk2DefaultPhoneNumberFormatter;
    CleanupStack::PushL( self );
    self->ConstructL( aMaxDisplayLength );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2DefaultPhoneNumberFormatter::SetMaxBufferLengthL
// --------------------------------------------------------------------------
//
void CPbk2DefaultPhoneNumberFormatter::SetMaxBufferLengthL( TInt aMaxLength )
    {
    if ( aMaxLength > iFormatterNumberBuffer->Des().MaxLength() )
        {
        iFormatterNumberBuffer =
            iFormatterNumberBuffer->ReAllocL( aMaxLength );
        }
    }

// --------------------------------------------------------------------------
// CPbk2DefaultPhoneNumberFormatter::FormatPhoneNumberForDisplay
// --------------------------------------------------------------------------
//
TPtrC CPbk2DefaultPhoneNumberFormatter::FormatPhoneNumberForDisplay
        ( const TDesC& aOriginalPhoneNumber )
    {
    // Convert number according to active number setting
    if ( aOriginalPhoneNumber.Length() <=
         iFormatterNumberBuffer->Des().MaxLength() )
        {
        TPtr formatterNumber = iFormatterNumberBuffer->Des();
        formatterNumber.Copy( aOriginalPhoneNumber );
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion
            ( formatterNumber );
        return formatterNumber;
        }
    else
        {
        return aOriginalPhoneNumber;
        }
    }

// End of File
