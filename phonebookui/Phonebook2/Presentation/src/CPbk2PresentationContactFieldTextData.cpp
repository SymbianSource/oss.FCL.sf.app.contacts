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
* Description:  Presentation text data of the contact field.
*
*/


// INCLUDES
#include "CPbk2PresentationContactFieldTextData.h"

// Phonebook2
#include <Pbk2FieldProperty.hrh>
#include "CPbk2PresentationContactField.h"

// Virtual Phonebook
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkFieldType.h>
#include <MPbk2FieldProperty.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicFieldDataTypeMismatch = 1,    
    EPanicIncorrectCtrlType
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2PresentationContactFieldTextData" );
    User::Panic(KPanicText,aReason);
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::
//  CPbk2PresentationContactFieldTextData
// --------------------------------------------------------------------------
//
CPbk2PresentationContactFieldTextData::CPbk2PresentationContactFieldTextData( 
        MVPbkContactFieldTextData& aTextData,
        const MPbk2FieldProperty& aFieldProperty,
        const TInt aMaxDataLength  ) : 
    iTextData( aTextData ), 
    iFieldProperty( aFieldProperty ),
    iMaxDataLength( aMaxDataLength )
    {
    // Do nothing    
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::Cast
// --------------------------------------------------------------------------
//    
CPbk2PresentationContactFieldTextData* CPbk2PresentationContactFieldTextData::
    NewL( 
        MVPbkContactFieldTextData& aTextData,
        const MPbk2FieldProperty& aFieldProperty,
        const TInt aMaxDataLength  )
    {
    CPbk2PresentationContactFieldTextData* self = 
        new(ELeave)CPbk2PresentationContactFieldTextData( 
            aTextData, aFieldProperty, aMaxDataLength );
    return self;        
    }


// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::
//    ~CPbk2PresentationContactFieldTextData
// --------------------------------------------------------------------------
//            
CPbk2PresentationContactFieldTextData::
        ~CPbk2PresentationContactFieldTextData()
    {
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::Cast
// --------------------------------------------------------------------------
//
CPbk2PresentationContactFieldTextData& 
    CPbk2PresentationContactFieldTextData::Cast(
        MVPbkContactFieldData& aFieldData )
    {
    __ASSERT_DEBUG( aFieldData.DataType() == EVPbkFieldStorageTypeText,
            Panic(EPanicFieldDataTypeMismatch) );
    return static_cast<CPbk2PresentationContactFieldTextData&>(aFieldData);
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::Cast
// --------------------------------------------------------------------------
//
const CPbk2PresentationContactFieldTextData& 
    CPbk2PresentationContactFieldTextData::Cast(
        const MVPbkContactFieldData& aFieldData )
    {
    __ASSERT_DEBUG( aFieldData.DataType() == EVPbkFieldStorageTypeText,
            Panic(EPanicFieldDataTypeMismatch) );
    return static_cast<const CPbk2PresentationContactFieldTextData&>(
        aFieldData );
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::Text
// --------------------------------------------------------------------------
//    
TPtrC CPbk2PresentationContactFieldTextData::Text() const
    {    
    TPtrC ptr = iTextData.Text();
    switch( iFieldProperty.CtrlType() )
        {
        case EPbk2FieldCtrlTypeNumberEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypePhoneNumberEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypeDTMFEditor:        
        case EPbk2FieldCtrlTypeRingtoneEditor:
        case EPbk2FieldCtrlTypeImageEditor:        
            {
            return ptr.Right( iMaxDataLength );
            }        
        case EPbk2FieldCtrlTypeTextEditor: // FALLTHROUGH
        case EPbk2FieldCtrlURLEditor: // FALLTHROUGH
        case EPbk2FieldCtrlEmailEditor: // FALLTHROUGH
        case EPbk2FieldCtrlPostalCodeEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypeReadingEditor:
        case EPbk2FieldCtrlTypeChoiceItems: // FALLTHROUGH
            {
            return ptr.Left( iMaxDataLength );
            }                
        case EPbk2FieldCtrlTypeNone:
            {
            return ptr;
            }
        case EPbk2FieldCtrlTypeDateEditor: // FALLTHROUGH
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EPanicIncorrectCtrlType ) );
            }
        };
    return ptr;        
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::SetTextL
// --------------------------------------------------------------------------
//
void CPbk2PresentationContactFieldTextData::SetTextL( const TDesC& aText )
    {
    TPtrC ptr = iTextData.Text();
    switch( iFieldProperty.CtrlType() )
        {
        case EPbk2FieldCtrlTypeNumberEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypePhoneNumberEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypeDTMFEditor:        
        case EPbk2FieldCtrlTypeRingtoneEditor:
        case EPbk2FieldCtrlTypeImageEditor:
            {
            iTextData.SetTextL( aText.Right( iMaxDataLength ) );
            break;
            }        
        case EPbk2FieldCtrlTypeTextEditor: // FALLTHROUGH
        case EPbk2FieldCtrlURLEditor: // FALLTHROUGH
        case EPbk2FieldCtrlEmailEditor: // FALLTHROUGH
        case EPbk2FieldCtrlPostalCodeEditor: // FALLTHROUGH
        case EPbk2FieldCtrlTypeReadingEditor:
        case EPbk2FieldCtrlTypeChoiceItems: // FALLTHROUGH
            {
            iTextData.SetTextL( aText.Left( iMaxDataLength ) );
            break;
            }                
        case EPbk2FieldCtrlTypeNone:
            {
            iTextData.SetTextL( aText );
            break;
            }
        case EPbk2FieldCtrlTypeDateEditor: // FALLTHROUGH
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EPanicIncorrectCtrlType ) );
            }
        };             
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::MaxLength
// --------------------------------------------------------------------------
//
TInt CPbk2PresentationContactFieldTextData::MaxLength() const
    {
    return iTextData.MaxLength();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::DataType
// --------------------------------------------------------------------------
//    
TVPbkFieldStorageType CPbk2PresentationContactFieldTextData::DataType() const
    {
    return iTextData.DataType();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::IsEmpty
// --------------------------------------------------------------------------
//    
TBool CPbk2PresentationContactFieldTextData::IsEmpty() const
    {
    return iTextData.IsEmpty();
    }

// --------------------------------------------------------------------------
// CPbk2PresentationContactFieldTextData::CopyL
// --------------------------------------------------------------------------
//    
void CPbk2PresentationContactFieldTextData::CopyL(
        const MVPbkContactFieldData& aFieldData )
    {
    iTextData.CopyL( aFieldData );  
    }
    
// End of file
