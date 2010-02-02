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
* Description:  The sim contact field.
*
*/



// INCLUDE FILES
#include "CVPbkSimCntField.h"

#include "MVPbkSimCntStore.h"
#include "MVPbkSimContact.h"
#include "VPbkSimStoreTemplateFunctions.h"
#include "TVPbkSimStoreProperty.h"

// ============================ LOCAL FUNCTIONS ================================

namespace {

// CONSTANTS

// a plus character the can be used as a prefix number
_LIT( KPlusPrefix, "+" );

// If there is no extension file or it's full then the number
// length is max 20. This comes from 3GPP USIM specification.
// (See spec for e.g EF ADN, 10 bytes for number)
const TInt KMaxNumLengthNotUsingExt = 20;
// If the number uses one record in the extension the max length is 40
// This comes from 3GPP USIM specification. See e.g EF EXT1, one record
// can take 10 bytes number -> if number is splitted to EF ADN + one record
// in EF EXT1 then max length is 40
const TInt KMaxNumLengthOneRecordFromExt = 40;


// -----------------------------------------------------------------------------
// MaxLength
//
// Returns the maximum length of the new field that has the given type
// -----------------------------------------------------------------------------
//
TInt16 MaxLength( TVPbkSimCntFieldType aType, const MVPbkSimCntStore& aStore )
    {
    TInt result = 0;
    if ( aType == EVPbkSimName ||
         aType == EVPbkSimGsmNumber )
        {
        TVPbkGsmStoreProperty prop;
        TInt err = aStore.GetGsmStoreProperties( prop );
        if( err != KErrNone )
        	{
        	result = 0;
        	}
        else if( aType == EVPbkSimName )
            {
            result = prop.iMaxTextLength;
            }
        else
            {
            result = prop.iMaxNumLength;
            }
        }
    else
        {
        TVPbkUSimStoreProperty uprop;
        TInt err = aStore.GetUSimStoreProperties( uprop );
        if( err == KErrNone )
        	{
	        switch ( aType )
	            {
	            case EVPbkSimAdditionalNumber:
	                {
	                result = uprop.iMaxAnrLength;
	                break;
	                }
	            case EVPbkSimReading:
	            case EVPbkSimNickName:
	                {
	                result = uprop.iMaxScndNameLength;
	                break;
	                }
	            case EVPbkSimEMailAddress:
	                {
	                result = uprop.iMaxEmailLength;
	                break;
	                }
	            case EVPbkSimGroupName:
	                {
	                result = uprop.iMaxGroupNameLength;
	                break;
	                }
	            default:
	                {
	                break;
	                }
	            }
        	}
        }
    return (TInt16)result;
    }

// -----------------------------------------------------------------------------
// CalculateNumberMaxLength
//
// Calculates the maximum number length for existing SIM contact.
//
// The logic is based on the file structure in SIM card.
// E.g a record in EF ADN can hold a phonenumber of length 20. If the saved
// number is longer than 20 the rest of the number is saved to EF EXT1.
// The size of EF EXT1 is limited and it doesn't have a space for each
// record in EF ADN.
//
// When getting maximum length from ETel it gives the max length that could
// be used for a new contact. But it's not the same as the max length that
// can be used for existing contact.
//
// This function checks how full the EXT1 file is using the contacts current
// number length and the max length from ETel.
// 
// @param aInitialData the data that is saved in the SIM card
// @param aCurETelMaxLengthWithPlusPrefix a max number length from ETel with
//        '+' prefix.
// @param aSystemMaxLength the system maximum length defined in VPbk.
// -----------------------------------------------------------------------------
//
TInt16 CalculateNumberMaxLength( const TDesC& aInitialData, TInt 
        aCurETelMaxLengthWithPlusPrefix,
        TInt aSystemMaxLength )
    {
    TInt curLength = aInitialData.Length();
    if ( aInitialData.Find( KPlusPrefix ) != KErrNotFound )
        {
        // Number contains '+' -prefix. Take that away to get
        // the actual number length
        --curLength;
        }
    
    // Lets do the checkings without plus prefix
    TInt curETelMaxLength = aCurETelMaxLengthWithPlusPrefix - 1;
    
    TInt result = curETelMaxLength;
    if ( curETelMaxLength > KMaxNumLengthOneRecordFromExt )
        {
        // There is a at least 2 free records in the extension
        result = aSystemMaxLength;
        }
    else if ( curETelMaxLength > KMaxNumLengthNotUsingExt )
        {
        // There is 1 free record in extension
        // Number length can be at least KMaxNumLengthOneRecordFromExt
        result = KMaxNumLengthOneRecordFromExt;
        if ( curLength > KMaxNumLengthOneRecordFromExt )
            {
            // Number already reserves 2 recors from extension
            result = aSystemMaxLength;
            }
        }
    else
        {
        // Extension is full. Number can use the records it already
        // reserves.
        if ( curLength > KMaxNumLengthOneRecordFromExt )
            {
            // Number already uses 2 extension records
            result = aSystemMaxLength;
            }
        else if ( curLength > KMaxNumLengthNotUsingExt )
            {
            // Number already uses 1 extension record
            result = KMaxNumLengthOneRecordFromExt;
            }
        else
            {
            // Number is not using extension records.
            result = curETelMaxLength;
            }
        }
    
    // Finally add the length of '+' prefex and return.
	return ++result;
    }
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVPbkSimCntField::CVPbkSimCntField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVPbkSimCntField::CVPbkSimCntField( TVPbkSimCntFieldType aType,
    const MVPbkSimContact& aParentContact )
    :   iType( aType ),
        iParentContact( aParentContact ),
        iMaxLength( KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
// CVPbkSimCntField::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVPbkSimCntField::ConstructL()
    {
    iDataPtr.Set( KNullDesC );
    }

// -----------------------------------------------------------------------------
// CVPbkSimCntField::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVPbkSimCntField* CVPbkSimCntField::NewL( TVPbkSimCntFieldType aType,
    const MVPbkSimContact& aParentContact )
    {
    CVPbkSimCntField* self = NewLC( aType, aParentContact );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkSimCntField::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVPbkSimCntField* CVPbkSimCntField::NewLC( TVPbkSimCntFieldType aType,
    const MVPbkSimContact& aParentContact )
    {
    CVPbkSimCntField* self = 
        new( ELeave ) CVPbkSimCntField( aType, aParentContact );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CVPbkSimCntField::~CVPbkSimCntField()
    {
    delete iData;
    }

// -----------------------------------------------------------------------------
// CVPbkSimCntField::SetDataL
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkSimCntField::SetDataL( const TDesC& aData )
    {
    TPtr dataPtr( NULL, 0 );
    VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iData, dataPtr, 
        aData.Length() );
    dataPtr.Copy( aData );
    iDataPtr.Set( *iData );
    }

// -----------------------------------------------------------------------------
// CVPbkSimCntField::SetDataPtr
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CVPbkSimCntField::MaxDataLength()
    {
    if ( iMaxLength == KErrNotFound )
        {
        iMaxLength = MaxLength( iType, iParentContact.ParentStore() );
        }
    return iMaxLength;
    }

// -----------------------------------------------------------------------------
// CVPbkSimCntField::SetInitialSimDataL
// -----------------------------------------------------------------------------
//
void CVPbkSimCntField::SetInitialSimDataL( const TDesC& aData )
    {
    SetDataL( aData );
    
    // Max length must be calculated for number types only because other
    // types are not splitted in several records in SIM card.
    if ( iType == EVPbkSimGsmNumber || iType == EVPbkSimAdditionalNumber )
        {
        TInt currentMaxLength = MaxLength( iType, 
            iParentContact.ParentStore() );
        iMaxLength = CalculateNumberMaxLength( aData, currentMaxLength,
            iParentContact.ParentStore().SystemPhoneNumberMaxLength() );
        }
    }
    
// -----------------------------------------------------------------------------
// CVPbkSimCntField::SetDataPtr
// -----------------------------------------------------------------------------
//
void CVPbkSimCntField::SetDataPtr( TPtrC aDataPtr )
    {
    iDataPtr.Set( aDataPtr );
    }

// -----------------------------------------------------------------------------
// CVPbkSimCntField::SetType
// -----------------------------------------------------------------------------
//
void CVPbkSimCntField::SetType( TVPbkSimCntFieldType aType )
    {
    iType = aType;
    }
//  End of File  
