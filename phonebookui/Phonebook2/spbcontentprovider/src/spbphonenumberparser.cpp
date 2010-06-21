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
* Description:  .
*
*/


#include "spbphonenumberparser.h"
#include "spbcontent.h"

#include <MVPbkContactOperationBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <CVPbkDefaultAttribute.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkFieldType.h>
#include <VPbkEng.rsg>						// resource ids for numbers


// ---------------------------------------------------------------------------
// CSpbPhoneNumberParser::NewL
// ---------------------------------------------------------------------------
//
CSpbPhoneNumberParser* CSpbPhoneNumberParser::NewL( 
        CVPbkContactManager& aContactManager,
        CSpbContent& aContent)
    {
    CSpbPhoneNumberParser* self = 
            new (ELeave) CSpbPhoneNumberParser(aContactManager, aContent);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSpbPhoneNumberParser::~CSpbPhoneNumberParser
// ---------------------------------------------------------------------------
//
CSpbPhoneNumberParser::~CSpbPhoneNumberParser()
    {
    delete iOperation;
    iOperation = NULL;
    }

// ---------------------------------------------------------------------------
// CSpbPhoneNumberParser::CSpbPhoneNumberParser
// ---------------------------------------------------------------------------
//
inline CSpbPhoneNumberParser::CSpbPhoneNumberParser(
        CVPbkContactManager& aContactManager,
        CSpbContent& aContent)
: iContactManager(aContactManager),
  iContent(aContent)
    {
    
    }

// ---------------------------------------------------------------------------
// CSpbPhoneNumberParser::ConstructL
// ---------------------------------------------------------------------------
//
inline void CSpbPhoneNumberParser::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CSpbPhoneNumberParser::VPbkSingleContactOperationComplete
// ---------------------------------------------------------------------------
//
void CSpbPhoneNumberParser::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    delete iOperation;
    iOperation = NULL;
    TRAP_IGNORE( SolvePhoneNumberL( *aContact ) );
	delete aContact;
    }

// ---------------------------------------------------------------------------
// CSpbPhoneNumberParser::VPbkSingleContactOperationFailed
// ---------------------------------------------------------------------------
//
void CSpbPhoneNumberParser::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, 
        TInt /*aError*/ )
    {
    delete iOperation;
    iOperation = NULL;
    TRAP_IGNORE( iContent.PhoneNumberUpdatedL( 
            KNullDesC, CSpbContentProvider::ETypePhoneNumber ) );
    }

// ---------------------------------------------------------------------------
// CSpbPhoneNumberParser::SolvePhoneNumberL
// ---------------------------------------------------------------------------
//
void CSpbPhoneNumberParser::SolvePhoneNumberL( MVPbkStoreContact& aContact )
    {
    RBuf number;
    TInt numberCount = 0;
	
    // check if the contact has default a number
    MVPbkContactAttributeManager& attributeManager = 
            iContactManager.ContactAttributeManagerL();
	
    CVPbkDefaultAttribute* attribute = 
            CVPbkDefaultAttribute::NewL( EVPbkDefaultTypePhoneNumber );
    CleanupStack::PushL( attribute );

    TBool hasDefaultNumberField = EFalse;
    CDesC16Array* phoneNumberArray = new (ELeave) CDesC16ArraySeg(8);
    CleanupStack::PushL( phoneNumberArray );

    // get contact fields
    MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    const TInt fieldCount = fields.FieldCount();
    // check fields
    for ( TInt i = 0; i < fieldCount; ++i )
        {
        const MVPbkStoreContactField& field = fields.FieldAt( i );
        // if correct data type
        if( field.FieldData().DataType() == EVPbkFieldStorageTypeText )
            {
            const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
            if( fieldType )
                {
                const TInt fieldTypeResId = fieldType->FieldTypeResId();
                // if one of the number fields
                if( fieldTypeResId == R_VPBK_FIELD_TYPE_LANDPHONEGEN    ||
                    fieldTypeResId == R_VPBK_FIELD_TYPE_LANDPHONEHOME   ||
                    fieldTypeResId == R_VPBK_FIELD_TYPE_LANDPHONEWORK   ||
                    fieldTypeResId == R_VPBK_FIELD_TYPE_MOBILEPHONEGEN  ||
                    fieldTypeResId == R_VPBK_FIELD_TYPE_MOBILEPHONEHOME ||
                    fieldTypeResId == R_VPBK_FIELD_TYPE_MOBILEPHONEWORK ||
                    fieldTypeResId == R_VPBK_FIELD_TYPE_CARPHONE        ||
                    fieldTypeResId == R_VPBK_FIELD_TYPE_ASSTPHONE       ||
                    fieldTypeResId == R_VPBK_FIELD_TYPE_PAGERNUMBER )
                    {
                    const MVPbkContactFieldTextData* textData =
                            &MVPbkContactFieldTextData::Cast( field.FieldData() );
                    if( attributeManager.HasFieldAttributeL( *attribute, field ) )
                        {
                        //default number found
                        number.CreateL( textData->Text() );
                        hasDefaultNumberField = ETrue;
                        break;
                        }
                    const TPtrC phoneNumber( textData->Text() );
                    // we need count phonenumbers same way how this is implemented in CCA
                    // so that we show same count for phonenumbers in names list
                    // CCA uses descriptor folded compare for phonenumbers
                    TInt dummy = 0;
                    if( 0 != phoneNumberArray->FindIsq( phoneNumber, dummy, ECmpFolded ) )
                        {
                        // phone number doesn't exist
                        phoneNumberArray->InsertIsqL( phoneNumber, ECmpFolded );
                        }
                    }
                }
            }
        }
    numberCount = phoneNumberArray->Count();
    
    //default number not found, and only one number
    if( EFalse == hasDefaultNumberField && numberCount == 1 )
        {
            number.CreateL( (*phoneNumberArray)[0] );
        }

    CleanupStack::PopAndDestroy( phoneNumberArray );
    CleanupStack::PopAndDestroy( attribute );
	
    // no number was found
    if( numberCount > 1 )
        {
        // contact has multiple numbers and no default
        TBuf<12> count;
        count.Num( numberCount );
        iContent.PhoneNumberUpdatedL( 
            count, CSpbContentProvider::ETypePhoneNumberMultiple );
        }
    else
        {
        // inform the observer
        iContent.PhoneNumberUpdatedL( 
            number, CSpbContentProvider::ETypePhoneNumber );
        }

    number.Close();
    }

// ---------------------------------------------------------------------------
// CSpbPhoneNumberParser::FetchPhoneNumber
// ---------------------------------------------------------------------------
//
void CSpbPhoneNumberParser::FetchPhoneNumberL(
            const MVPbkContactLink& aLink)
    {
    delete iOperation;
    iOperation = NULL;
                   
    iOperation = iContactManager.RetrieveContactL(aLink, *this);
    }
        
// end of file
