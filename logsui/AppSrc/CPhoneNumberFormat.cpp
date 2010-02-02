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
*     Implementation for number formatting
*
*/


// INCLUDE FILES
#include <NumberGrouping.h>
#include <AknUtils.h>

#include "CPhoneNumberFormat.h"   
#include "LogsConstants.hrh"

#include <featmgr.h>
#include <bldvariant.hrh>  // IDs

#include <centralrepository.h>  
#include <NumberGroupingCRKeys.h>


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

const TInt KDTMF1 ='w';
const TInt KDTMF2 ='W';
const TInt KDTMF3 ='p';
const TInt KDTMF4 ='P';

const TInt KSS1 ='#';
const TInt KSS2 ='*';

// CONSTANTS FOR DEBUGGING
#ifdef _DEBUG
_LIT( KPanic, "CPhoneNumberFormat");
#endif

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================

/**
*   Class used for phone number formatting
*/



CPhoneNumberFormat* CPhoneNumberFormat::NewL()
    {
    CPhoneNumberFormat* self = new ( ELeave ) CPhoneNumberFormat();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );  // self
    return self;
    }


// C++ default constructor cannot contain any code that might leave
CPhoneNumberFormat::CPhoneNumberFormat()
    {
    }


// Destructor
CPhoneNumberFormat::~CPhoneNumberFormat()
    {
    if (iNumberGrouping)
        {
        delete iNumberGrouping;
        }
    }


//  Constructor for the recent views, second phase
void CPhoneNumberFormat::ConstructL()
    {
    CRepository* repository = NULL;
    TInt value( 0 );
    TRAPD( err, repository = CRepository::NewL( KCRUidNumberGrouping ) );

    if( err == KErrNone )
        {
        err = repository->Get( KNumberGrouping, value );
        }
    delete repository;

    if( value == 1 )
        {
        iNumberGrouping = CPNGNumberGrouping::NewL( KLogsPhoneNumberMaxLen );
        }
    else
        {
        iNumberGrouping = NULL;
        }
    }


TFormatResult CPhoneNumberFormat::DescriptorCopy
    (   const TDesC& aSource
    ,   TDes& aDestination
    )
    {
    TFormatResult rc( EResultOK );
    
    if( aDestination.MaxLength() < aSource.Length() )
        {
        rc = EResultTruncated;
        aDestination = aSource.Right( aSource.Length() -
                ( aSource.Length() - aDestination.MaxLength() ) 
                );
        }
    else
        {
        aDestination = aSource;
        }

    return rc;
    }


TFormatResult CPhoneNumberFormat::PhoneNumberFormat
    (   TPtrC aNumberToFormat
    ,   TDes& aFormattedString
    )
    {
    TFormatResult rc( EResultOK );

    if( iNumberGrouping )
        {
        TBuf<KLogsPhoneNumberMaxLen> buf;
        rc = DescriptorCopy( aNumberToFormat, buf );  //Src, dest
        AknTextUtils::LanguageSpecificNumberConversion( buf );  
        iNumberGrouping->Set( buf );

        rc = DescriptorCopy( iNumberGrouping->FormattedNumber(),
                             aFormattedString);
        }
    else
        {
        rc = DescriptorCopy( aNumberToFormat, aFormattedString );  //Src, dest
        AknTextUtils::LanguageSpecificNumberConversion( aFormattedString ); 
        }

    return rc; 
    }


TFormatResult CPhoneNumberFormat::PhoneNumberFormat
    (   TPtrC aNumberToFormat
    ,   TDes& aFormattedString
    ,   const CFont* aFont
    ,   TInt aPixelWidth
    )
    {
    TFormatResult rc = PhoneNumberFormat( aNumberToFormat, aFormattedString );
    if( AknTextUtils::ClipToFit
                        (   aFormattedString
                        ,   *aFont
                        ,   aPixelWidth
                        ,   AknTextUtils::EClipFromBeginning ) )
        {
        rc = EResultClipped;
        }

    return rc;
    }


TFormatResult CPhoneNumberFormat::PhoneNumberFormat
    (   TPtrC aNumberToFormat
    ,   TDes& aFormattedString
    ,   AknTextUtils::TClipDirection aClipDirection
    ,   CEikFormattedCellListBox *aListBox
    ,   TInt aItemIndex
    ,   TInt aSubCellNumber
    )
    {
    TFormatResult rc = PhoneNumberFormat( aNumberToFormat, aFormattedString );
    if( AknTextUtils::ClipToFit
                        (   aFormattedString
                        ,   aClipDirection
                        ,   aListBox
                        ,   aItemIndex
                        ,   aSubCellNumber
                        ) )
        {
        rc = EResultClipped;
        }

    return rc;
    }


TFormatResult CPhoneNumberFormat::PhoneNumberFormat
    (   TPtrC aNumberToFormat
    ,   TDes& aFormattedString
    ,   AknTextUtils::TClipDirection aClipDirection
    ,   CEikColumnListBox *aListBox
    ,   TInt aItemIndex
    ,   TInt aColumnNumber
    )
    {
    TFormatResult rc = PhoneNumberFormat( aNumberToFormat, aFormattedString );
    if( AknTextUtils::ClipToFit
                        (   aFormattedString
                        ,   aClipDirection
                        ,   aListBox
                        ,   aItemIndex
                        ,   aColumnNumber
                        ) )
        {
        rc = EResultClipped;
        }

    return rc;
    }

TInt CPhoneNumberFormat::IsDtmf( TChar aChar )
    {
    return aChar == KDTMF1 || aChar == KDTMF2 || aChar == KDTMF3 || aChar == KDTMF4;
    }

TInt CPhoneNumberFormat::IsSS( TChar aChar )
    {
    return aChar == KSS1 || aChar == KSS2;
    }

TInt CPhoneNumberFormat::DTMFStrip(
                    TPtrC aNumberToFormat,
                    TDes& aFormattedString )
    {
    	
  //  __ASSERT_DEBUG( aFormattedString.MaxLength() >= aNumberToFormat.Length(), User::Panic(KPanic, KErrCorrupt));
    
    TInt start = 0;
    if ( aFormattedString.MaxLength() < aNumberToFormat.Length() )
        {
        start = aNumberToFormat.Length() - aFormattedString.MaxLength();
        }

    aFormattedString.Zero();
    TBool ss(EFalse);
    TBool dtmf(EFalse);
    TChar prevChar(' ');
    for( TInt i = start ; i < aNumberToFormat.Length() && !dtmf ;  i ++ )
        {
        dtmf = IsDtmf( aNumberToFormat[i] );
        if( IsSS( aNumberToFormat[i] ) && !IsSS(prevChar) )
            {
            ss ? ss = EFalse: ss = ETrue;
            }

        if( !dtmf && !ss && !IsSS(aNumberToFormat[i]))
            {
            aFormattedString.Append(aNumberToFormat[i] );
            }
        prevChar = aNumberToFormat[i];
        }
    if( aFormattedString.Length() == 0 && (dtmf || ss) )
        { // fallback, full string shown since invalid phone number.
        aFormattedString = aNumberToFormat;
        return KErrArgument;
        }
    return KErrNone;
    }

//  End of File  
