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
* Description:  A class that can convert sim contacts to etel contacts and vice
*                versa
*
*/



// INCLUDE FILES
#include "CVPbkETelCntConverter.h"

// From Virtual Phonebook
#include "VPbkSimStoreImplError.h"
#include <VPbkSimCntFieldTypes.hrh>
#include <CVPbkSimCntField.h>
#include <CVPbkSimContact.h>
#include <VPbkSimStoreCommon.h>
#include <CVPbkSimContactBuf.h>
#include <VPbkDebug.h>

// System includes
#include <mpbutil.h> // CPhoneBookBuffer

namespace   {
// CONSTANTS
// An amount of space to add to estimate of the size of the field
// due to ETel padding bytes.
const TInt KExtraFieldDataSpace = 3;
// A buf size for the test buffer used to test CPhoneBookBuffer
const TInt KTestBufSize = 20;
}
// ============================= LOCAL FUNCTIONS ===============================

namespace {

// -----------------------------------------------------------------------------
// FieldConversion
// Converts native sim field type to ETel field type
// -----------------------------------------------------------------------------
//
TUint8 FieldConversion( TVPbkSimCntFieldType aType )
    {
    TUint8 result = 0;
    switch ( aType )
        {
        case EVPbkSimName:
            {
            result = RMobilePhoneBookStore::ETagPBText;
            break;
            }
        case EVPbkSimGsmNumber: // FALLTHROUGH
        case EVPbkSimAdditionalNumber:
            {
            result = RMobilePhoneBookStore::ETagPBNumber;
            break;
            }
        case EVPbkSimNickName: // FALLTHROUGH
        case EVPbkSimReading: 
            {
            result = RMobilePhoneBookStore::ETagPBSecondName;
            break;
            }
        case EVPbkSimEMailAddress:
            {
            result = RMobilePhoneBookStore::ETagPBEmailAddress;
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// FieldConversion
// Converts ETel field type to native field type
// -----------------------------------------------------------------------------
//
TVPbkSimCntFieldType FieldConversion( TUint8 aType )
    {
    TVPbkSimCntFieldType result = EVPbkSimUnknownType;
    switch ( aType )
        {
        case RMobilePhoneBookStore::ETagPBText:
            {
            result = EVPbkSimName;
            break;
            }
        case RMobilePhoneBookStore::ETagPBNumber:
            {
            result = EVPbkSimGsmNumber;
            break;
            }
        case RMobilePhoneBookStore::ETagPBSecondName:
            {
            result = EVPbkSimNickName;
            break;
            }
        case RMobilePhoneBookStore::ETagPBEmailAddress:
            {
            result = EVPbkSimEMailAddress;
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    return result;
    }
    
// -----------------------------------------------------------------------------
// GetFieldData
// Gets the first field data of given ETel field type
// -----------------------------------------------------------------------------
//
template<class T>
void GetFieldData( CPhoneBookBuffer& aETelBuffer, T& aData, TUint8 aFieldType )
    {
    TUint8 fieldTag;
	CPhoneBookBuffer::TPhBkTagType dataType;
    while ( aETelBuffer.GetTagAndType( fieldTag, dataType) == KErrNone )
		{
        if ( fieldTag == aFieldType )
            {
            aETelBuffer.GetValue( aData );
            return;
			}
        else
            {
            aETelBuffer.SkipValue( dataType );
            }
        }
    }

// -----------------------------------------------------------------------------
// EstimateDataSize
// Estimates the size that the data will take in ETel buffer
// -----------------------------------------------------------------------------
//
inline TInt EstimateDataSize( const TArray<CVPbkSimCntField*>& aFieldArray )
    {
    // Initialize to zero size
    TInt size = 0;
    TInt count = aFieldArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        // Add one byte for the field tag, then two bytes for data size
        // information, so 3 bytes will be added 
        size += KExtraFieldDataSpace;
        
        // Add the size of the data
        size += aFieldArray[i]->Data().Size();
        
        // ETel buffer uses word aligned data -> 0-3 padding bytes
        size += KExtraFieldDataSpace;
        }
    return size;
    }

// -----------------------------------------------------------------------------
// Adds a new data field to ETel contact
// -----------------------------------------------------------------------------
//
inline TInt AddNewETelField( CPhoneBookBuffer& aETelBuffer, 
    CVPbkSimCntField& aField )
    {
    TUint8 etelType = FieldConversion( aField.Type() );
    __ASSERT_DEBUG( etelType != 0, VPbkSimStoreImpl::Panic( 
        VPbkSimStoreImpl::EInvalidVPbkToETelTypeConversion ) );
    const TDesC& data = aField.Data();
    if ( data.Length() > 0 )
        {
        if ( aField.Type() == EVPbkSimAdditionalNumber )
            {
            // Additional number needs own tag that must be added before data
            aETelBuffer.AddNewNumberTag();
            }
        return aETelBuffer.PutTagAndValue( etelType, data );    
        }
    return KErrNotFound;
    }

} // unnamed

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::CVPbkETelCntConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVPbkETelCntConverter::CVPbkETelCntConverter()
    {
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVPbkETelCntConverter::ConstructL()
    {
    // Create a Symbian utility for the contact data
    iETelBuffer = new( ELeave ) CPhoneBookBuffer;
    // Check how much space new entry tag takes and how much space
    // is needed for the contact that has only sim index
    HBufC8* testBuf = HBufC8::NewLC( KTestBufSize );
    TPtr8 ptr( testBuf->Des() );
    iETelBuffer->Set( &ptr );
    iETelBuffer->AddNewEntryTag();
    iNewEntryTagLength = ptr.Length();
    TUint16 exampleIndex = 1;
    iETelBuffer->PutTagAndValue( 
        RMobilePhoneBookStore::ETagPBAdnIndex, exampleIndex );
    // Safe solution -> double the space
    iMinContactLength = 2 * ptr.Length();
    CleanupStack::PopAndDestroy( testBuf );
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkETelCntConverter* CVPbkETelCntConverter::NewL()
    {
    CVPbkETelCntConverter* self = NewLC();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkETelCntConverter* CVPbkETelCntConverter::NewLC()
    {
    CVPbkETelCntConverter* self = new( ELeave ) CVPbkETelCntConverter;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor
CVPbkETelCntConverter::~CVPbkETelCntConverter()
    {
    delete iETelBuffer;
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::ConvertFromETelToVPbkSimContactsL
// -----------------------------------------------------------------------------
//
void CVPbkETelCntConverter::ConvertFromETelToVPbkSimContactsL( 
    RPointerArray<CVPbkSimContactBuf>& aResultArray,
    TDes8& aETelContactData, MVPbkSimCntStore& aSimStore ) const
    {
    iETelBuffer->Set( &aETelContactData );
    User::LeaveIfError( StartRead() );
    
    TInt cntCounter = 0;
    TPtrC8 etelCnt( GetNextContactData( aETelContactData ) );
    while ( etelCnt.Length() > 0 )
        {
        CVPbkSimContactBuf* cnt = CVPbkSimContactBuf::NewLC( etelCnt, 
            aSimStore );
        aResultArray.AppendL( cnt );
        CleanupStack::Pop();
        ++cntCounter;
        etelCnt.Set( GetNextContactData( aETelContactData ) );
        }

    // Check if there was no contacts in the ETel descriptor
    if ( cntCounter == 0 )
        {
        User::Leave( KErrBadDescriptor );
        }
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::ConvertFromETelToVPbkSimContactL
// -----------------------------------------------------------------------------
//
void CVPbkETelCntConverter::ConvertFromETelToVPbkSimContactL( 
    TDes8& aETelContact, CVPbkSimContact& aEmptyContact ) const
    {
    CVPbkSimCntField* tmpField = 
        aEmptyContact.CreateFieldLC( EVPbkSimUnknownType );
    TInt error = KErrNone;
    const TInt count = DoFieldCount( aETelContact, error );
    User::LeaveIfError( error );

    for ( TInt i = 0; i < count; ++i )
        {
        FieldAt( aETelContact, i, *tmpField );
        CVPbkSimCntField* newField = 
            aEmptyContact.CreateFieldLC( tmpField->Type() );
        newField->SetInitialSimDataL( tmpField->Data() );
        aEmptyContact.AddFieldL( newField );
        CleanupStack::Pop( newField );
        }
    CleanupStack::PopAndDestroy( tmpField );
    aEmptyContact.SetSimIndex( SimIndex( aETelContact ) );
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::ConvertFromVPbkSimFieldsToETelCntLC
// -----------------------------------------------------------------------------
//
HBufC8* CVPbkETelCntConverter::ConvertFromVPbkSimFieldsToETelCntLC( 
    const TArray<CVPbkSimCntField*>& aFieldArray, TInt aSimIndex ) const
    {
    // Create a buffer for the ETel contact
    TInt size = iMinContactLength + EstimateDataSize( aFieldArray );
    HBufC8* buf = HBufC8::NewLC( size );
    TPtr8 ptr( buf->Des() );
    iETelBuffer->Set( &ptr );
    
    // Add new entry tag and the SIM index if not a new contact
    iETelBuffer->AddNewEntryTag();
    if ( aSimIndex != KVPbkSimStoreFirstFreeIndex )
        {
        iETelBuffer->PutTagAndValue( 
            RMobilePhoneBookStore::ETagPBAdnIndex, (TUint16) aSimIndex );
        }

    const TInt count = aFieldArray.Count();
    for( TInt i = 0; i < count; ++i )
        {
        TInt result = AddNewETelField( *iETelBuffer, *aFieldArray[i] );
        while ( result == KErrOverflow )
            {
            // If the buffer is too small for some reason -> double the size
            HBufC8* newBuf = buf->ReAllocL( 2 * buf->Size() );
            CleanupStack::Pop( buf );
            buf = newBuf;
            CleanupStack::PushL( buf );
            
            ptr.Set( buf->Des() );
            iETelBuffer->Set( &ptr );
            result = AddNewETelField( *iETelBuffer, *aFieldArray[i] );
            }
        }
    return buf;
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::SimIndex
// -----------------------------------------------------------------------------
//
TInt CVPbkETelCntConverter::SimIndex( TDes8& aETelContact ) const
    {
    iETelBuffer->Set( &aETelContact );
    if ( StartRead() == KErrNone )
        {
        TUint16 simIndex = 0;
        GetFieldData( *iETelBuffer, simIndex, 
            RMobilePhoneBookStore::ETagPBAdnIndex );
        if ( simIndex > 0 )
            {
            return simIndex;
            }
        else
            {
            return KVPbkSimStoreFirstFreeIndex;
            }
        }
    return KErrBadDescriptor;
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::FieldCount
// -----------------------------------------------------------------------------
//
TInt CVPbkETelCntConverter::FieldCount( TDes8& aETelContact ) const
    {
    // If client gives an invalid buffer KErrBadDescriptor is returned
    TInt error = KErrNone;
    TInt result = DoFieldCount( aETelContact, error );
    if ( error == KErrNone )
        {
        return result;
        }
    return error;
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::FieldAt
// -----------------------------------------------------------------------------
//
void CVPbkETelCntConverter::FieldAt( TDes8& aETelContact, TInt aIndex, 
    CVPbkSimCntField& aField ) const
    {
    iETelBuffer->Set( &aETelContact );
    if ( StartRead() == KErrNone )
        {
        for ( TInt i = 0; i <= aIndex; ++i )
            {
            GetNextField( aField );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::MoveToNextContact
// Moves pointer to the next contact
// -----------------------------------------------------------------------------
//
TBool CVPbkETelCntConverter::MoveToNextContact() const
    {
    TUint8 fieldTag;
	CPhoneBookBuffer::TPhBkTagType dataType;
    while ( iETelBuffer->GetTagAndType( fieldTag, dataType) == KErrNone )
		{
		switch ( fieldTag )
			{
			case RMobilePhoneBookStore::ETagPBNewEntry:
				{
                return ETrue; 
				}
            default:
                {
                iETelBuffer->SkipValue( dataType );
                break;
                }
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::StartRead
// Sets the pointer in the ETel buffer to the beginning of the first contact
// Returns KErrBadDescriptor if the header was not found
// -----------------------------------------------------------------------------
//
TInt CVPbkETelCntConverter::StartRead() const
    {
    iETelBuffer->StartRead();

    if ( MoveToNextContact() )
        {
        return KErrNone;
        }
    return KErrBadDescriptor;
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::GetNextField
// Gets the next field that has native support, returns EFalse if not found
// -----------------------------------------------------------------------------
//
TBool CVPbkETelCntConverter::GetNextField( CVPbkSimCntField& aField ) const
    {
    TPtrC16 data;
    TUint8 fieldTag;
	CPhoneBookBuffer::TPhBkTagType dataType;
	TBool readingAnr = EFalse;
    while ( iETelBuffer->GetTagAndType( fieldTag, dataType) == KErrNone )
		{
		if ( fieldTag == RMobilePhoneBookStore::ETagPBAnrStart )
		    {
		    readingAnr = ETrue;
		    }
        else if ( dataType == CPhoneBookBuffer::EPhBkTypeDes16 )
            {
            iETelBuffer->GetValue( data );
            TVPbkSimCntFieldType nativeType = FieldConversion( fieldTag );
            /// Text field of the ANR is not supported. If it's supported
            /// then the CVPbkSimCntField must have a label and it should
            /// be set here.
            if ( readingAnr && nativeType == EVPbkSimGsmNumber )
                {
                // VPbk SIM contact has an own type for additional number
                nativeType = EVPbkSimAdditionalNumber;
                }
            
            if ( nativeType != EVPbkSimUnknownType )
                {
                aField.SetDataPtr( data );
                aField.SetType( nativeType );
                return ETrue;
                }            
			}
        else
            {
            iETelBuffer->SkipValue( dataType );
            }
        }
    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVPbkETelCntConverter::NumberOfFields
// Returns the number of VPbk supported fields in the buffer
// -----------------------------------------------------------------------------
//
TInt CVPbkETelCntConverter::NumberOfFields() const
    {
    TUint8 fieldTag;
	CPhoneBookBuffer::TPhBkTagType dataType;
	TBool readingAnr = EFalse;
	TInt count = 0;
    while ( iETelBuffer->GetTagAndType( fieldTag, dataType) == KErrNone )
		{
		if ( fieldTag == RMobilePhoneBookStore::ETagPBAnrStart )
		    {
		    // Tag type is EPhBkTypeNoData so it's not needed to get value 
		    readingAnr = ETrue;
		    ++count;
		    }
        else if ( dataType == CPhoneBookBuffer::EPhBkTypeDes16 )
            {
            TVPbkSimCntFieldType nativeType = FieldConversion( fieldTag );
            if ( !readingAnr && nativeType != EVPbkSimUnknownType )
                {
                ++count;
                }
            // ANR number field ends the reading of ANR
            else if ( nativeType == EVPbkSimGsmNumber )
                {
                readingAnr = EFalse;
                }
            // Data must be always read from the buffer to make it work
            iETelBuffer->SkipValue( dataType );
			}
        else
            {
            iETelBuffer->SkipValue( dataType );
            }
        }
    
    return count;
    }

// -----------------------------------------------------------------------------
// GetNextContactData
// Gets the next contact from the buffer that contains many contacts
// -----------------------------------------------------------------------------
//
TPtrC8 CVPbkETelCntConverter::GetNextContactData( TDes8& aBuffer ) const
    {
    TInt newEntryTagLength = 1;
    TInt remainBefore = iETelBuffer->RemainingReadLength();
    
    if ( MoveToNextContact() )
        {
        TInt remainAfter = iETelBuffer->RemainingReadLength();
        TInt contactLength = remainBefore - remainAfter;
        TInt startPos = iETelBuffer->BufferLength() - remainBefore - 
            newEntryTagLength;
        return aBuffer.Mid( startPos, contactLength );
        }
    
    // 1 or 0 contacts left
    if ( remainBefore > 0 )
        {
        // 1 contact left
        TInt startPos = iETelBuffer->BufferLength() - remainBefore - 
            newEntryTagLength;
        return aBuffer.Mid( startPos );
        }
    else
        {
        // 0 contacts left, return zero length pointer
        return TPtrC8();
        }
    }

// -----------------------------------------------------------------------------
// DoFieldCount
// Counts the number of fields of the contact
// -----------------------------------------------------------------------------
//
TInt CVPbkETelCntConverter::DoFieldCount(
        TDes8& aETelContact, TInt& aError ) const
    {
    TInt count = 0;
    iETelBuffer->Set( &aETelContact );

    TInt result = StartRead();
    if ( result == KErrNone )
        {
        count = NumberOfFields();
        }
    aError = result;

    return count;
    }

//  End of File  
