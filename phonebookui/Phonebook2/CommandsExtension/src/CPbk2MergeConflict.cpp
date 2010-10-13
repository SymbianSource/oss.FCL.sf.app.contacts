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
 * Description:  Phonebook 2 merge conflict.
 *
 */

// Phonebook2 includes
#include <Pbk2AddressTools.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2PresentationContactField.h>
#include <MPbk2FieldProperty.h>

//Virtual Phonebook
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactFieldBinaryData.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>

// System includes
#include <StringLoader.h>
#include <avkon.rsg>

#include "CPbk2MergeConflict.h"

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

/// Unnamed namespace for local definitions
namespace 
    {
    const TInt KMaxDateLenght = 64;
    enum TPbk2PanicCodes
        {
        EPbk2NotInitialized,
        EPbk2NotChosen,
        EPbk2UnexpectedCase,
        EPbk2FieldTypeNotExists
        };
    
    void Panic(TPbk2PanicCodes aReason)
        {
        _LIT( KPanicText, "CPbk2_Merge_Conflict" );
        User::Panic( KPanicText, aReason );
        }
    } /// namespace

// --------------------------------------------------------------------------
// CPbk2MergeConflict::GetConflictType
// --------------------------------------------------------------------------
//
EPbk2ConflictType CPbk2MergeConflict::GetConflictType() 
    {
    return iConflictType;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::CPbk2MergeConflict
// --------------------------------------------------------------------------
//
CPbk2MergeConflict::CPbk2MergeConflict() 
    {
    iConflictType = EPbk2ConflictTypeNormal;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::~CPbk2MergeConflict
// --------------------------------------------------------------------------
//
CPbk2MergeConflict::~CPbk2MergeConflict()
    {
    delete iDisplayFieldFirst;
    delete iDisplayFieldSecond;    
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::NewL
// --------------------------------------------------------------------------
//
CPbk2MergeConflict* CPbk2MergeConflict::NewL() 
    {
    CPbk2MergeConflict* self = new ( ELeave ) CPbk2MergeConflict();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::GetFieldsL
// --------------------------------------------------------------------------
//
void CPbk2MergeConflict::GetFieldsL(  
        const MVPbkStoreContactField*& aFieldFirst, 
        const MVPbkStoreContactField*& aFieldSecond )
    {
    __ASSERT_ALWAYS( iFieldFirst && iFieldSecond, Panic( EPbk2NotInitialized ) );
    
    aFieldFirst = iFieldFirst;
    aFieldSecond = iFieldSecond;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::AddFields
// --------------------------------------------------------------------------
//
void CPbk2MergeConflict::AddFields(  
        const MVPbkStoreContactField& aFieldFirst, 
        const MVPbkStoreContactField& aFieldSecond ) 
    {
    iFieldFirst = &aFieldFirst;
    iFieldSecond = &aFieldSecond;
    
    const MVPbkFieldType* fieldType = aFieldFirst.BestMatchingFieldType();
    if ( fieldType )
        {            
        if( fieldType->NonVersitType() == EVPbkNonVersitTypeCodImage )
            {
            iConflictType = EPbk2ConflictTypeImage;
            }
        else
            {
            TArray<TVPbkFieldVersitProperty> versitPropArr = fieldType->VersitProperties();
            TInt count = versitPropArr.Count();
        
            for( TInt idx = 0; idx < count; idx++ )
                {
                TVPbkFieldVersitProperty versitProp = versitPropArr[idx];
                if( versitProp.Name() == EVPbkVersitNameLOGO ||
                    versitProp.Name() == EVPbkVersitNamePHOTO )
                    {
                    iConflictType = EPbk2ConflictTypeImage;
                    break;
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::AddDisplayFields
// --------------------------------------------------------------------------
//
void CPbk2MergeConflict::AddDisplayFields(  
        HBufC* aDisplayFieldFirst, 
        HBufC* aDisplayFieldSecond )
    {
    iDisplayFieldFirst = aDisplayFieldFirst;
    iDisplayFieldSecond = aDisplayFieldSecond;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::GetLabelLC
// --------------------------------------------------------------------------
//
HBufC* CPbk2MergeConflict::GetLabelLC() 
    {
    __ASSERT_ALWAYS( iFieldFirst && iFieldSecond, Panic( EPbk2NotInitialized ) );
    
    return iFieldFirst->FieldLabel().AllocLC();
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::GetTextLC
// --------------------------------------------------------------------------
//
HBufC* CPbk2MergeConflict::GetTextLC( EPbk2ConflictedNumber aNumber ) 
    {
    __ASSERT_ALWAYS( iFieldFirst && iFieldSecond, Panic( EPbk2NotInitialized ) );
    
    const MVPbkStoreContactField* field;
    HBufC* retText = NULL;
    
    if( aNumber == EPbk2ConflictedFirst )
        {
        field = iFieldFirst;
        }
    else
        {
        field = iFieldSecond;
        }
    
    //Prefer display texts if both available 
    if( iDisplayFieldFirst && iDisplayFieldSecond )
        {
        retText = aNumber==EPbk2ConflictedFirst ? 
            iDisplayFieldFirst->AllocLC() : iDisplayFieldSecond->AllocLC();
        }
    else
        {
        retText = GetContentTextLC( aNumber , field);    
        }
    return retText;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::GetContentTextLC
// --------------------------------------------------------------------------
//
HBufC* CPbk2MergeConflict::GetContentTextLC( 
        EPbk2ConflictedNumber aNumber,
        const MVPbkStoreContactField* aField )
    {
    const MVPbkContactFieldData& data = aField->FieldData();
    TVPbkFieldStorageType storageType = data.DataType();
   
    HBufC* retText = NULL;
    
    switch( storageType )
        {
        case EVPbkFieldStorageTypeText:
            retText = MVPbkContactFieldTextData::Cast( data ).Text().AllocLC();
            break;
        case EVPbkFieldStorageTypeUri:
            retText =  MVPbkContactFieldUriData::Cast( data ).Text().AllocLC();
            break;
        case EVPbkFieldStorageTypeBinary:
            // Bad method for getting binary data, get it from field
            Panic( EPbk2UnexpectedCase );
            break;
        case EVPbkFieldStorageTypeDateTime:
            {
            const MVPbkContactFieldDateTimeData& date = 
                MVPbkContactFieldDateTimeData::Cast( data );

            TTime time( date.DateTime() );
            TBuf<KMaxDateLenght> dateBuffer;
            HBufC* dateFormat = CCoeEnv::Static()->AllocReadResourceLC
                ( R_QTN_DATE_USUAL_WITH_ZERO ); // in avkon.rsg
            
            time.FormatL( dateBuffer, *dateFormat, TLocale() );
            CleanupStack::PopAndDestroy( dateFormat );
            retText = dateBuffer.AllocLC();
            }
            break;
        default:
            Panic( EPbk2UnexpectedCase );
        }
    TPtr ptr = retText->Des();
    CustomizeTextValueL( *aField, ptr );
    return retText;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::ResolveConflict
// --------------------------------------------------------------------------
//
void CPbk2MergeConflict::ResolveConflict( EPbk2ConflictedNumber aNumber ) 
    {
    __ASSERT_ALWAYS( iFieldFirst && iFieldSecond, Panic( EPbk2NotInitialized ) );
    
    if( aNumber == EPbk2ConflictedFirst )
        {
        iChosenField = iFieldFirst;
        }
    else
        {
        iChosenField = iFieldSecond;
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::GetChosenFieldsL
// --------------------------------------------------------------------------
//
void CPbk2MergeConflict::GetChosenFieldsL( 
        RPointerArray<MVPbkStoreContactField>& aAddressFields )
    {
    __ASSERT_ALWAYS( iFieldFirst && iFieldSecond, Panic( EPbk2NotChosen ) );
    CleanupClosePushL( aAddressFields );
    
    if ( iChosenField )
        {
        aAddressFields.AppendL( iChosenField );
        }
    CleanupStack::Pop();
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflict::CustomizeTextValueL
// --------------------------------------------------------------------------
//
void CPbk2MergeConflict::CustomizeTextValueL( const MVPbkStoreContactField& aField, TDes& aBuf )
    {
    const MVPbkFieldType* fieldType= aField.BestMatchingFieldType();
    if ( fieldType )
        {
        TVPbkNonVersitFieldType nonVersitType = fieldType->NonVersitType();
        if ( nonVersitType == EVPbkNonVersitTypeRingTone )
            {
            TParsePtr fileName = TParsePtr( aBuf );
            if ( fileName.NamePresent() )
                {
                TPtrC namePtr = fileName.Name();
                HBufC* name = namePtr.AllocL();
                aBuf.Copy( *name );
                delete name;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflictAddress::CPbk2MergeConflictAddress
// --------------------------------------------------------------------------
//
CPbk2MergeConflictAddress::CPbk2MergeConflictAddress()
    {
    iConflictType = EPbk2ConflictTypeAddress;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflictAddress::GetConflictType
// --------------------------------------------------------------------------
//
EPbk2ConflictType CPbk2MergeConflictAddress::GetConflictType() 
    {
    return iConflictType;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflictAddress::NewL
// --------------------------------------------------------------------------
//
CPbk2MergeConflictAddress* CPbk2MergeConflictAddress::NewL() 
    {
    CPbk2MergeConflictAddress* self = new ( ELeave ) CPbk2MergeConflictAddress();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflictAddress::AddAddress
// --------------------------------------------------------------------------
//
void CPbk2MergeConflictAddress::AddAddress( 
        CPbk2PresentationContact& aContactFirst, 
        CPbk2PresentationContact& aContactSecond, 
        TPbk2FieldGroupId aAddressGroup ) 
    {
    iContactFirst = &aContactFirst;
    iContactSecond = &aContactSecond;
    iAddressGroup = aAddressGroup;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflictAddress::GetLabelLC
// --------------------------------------------------------------------------
//
HBufC* CPbk2MergeConflictAddress::GetLabelLC() 
    {
    HBufC* retLebel = NULL;
    switch( iAddressGroup )
        {
        case EPbk2FieldGroupIdPostalAddress:
            retLebel = StringLoader::LoadLC( R_QTN_PHOB_HEADER_ADDRESS );
            break;
        case EPbk2FieldGroupIdHomeAddress:
            retLebel = StringLoader::LoadLC( R_QTN_PHOB_HEADER_ADDRESS_HOME );
            break;
        case EPbk2FieldGroupIdCompanyAddress:
            retLebel = StringLoader::LoadLC( R_QTN_PHOB_HEADER_ADDRESS_WORK );
            break;
        default:
            Panic( EPbk2NotInitialized );
        }
    return retLebel;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflictAddress::GetTextLC
// --------------------------------------------------------------------------
//
HBufC* CPbk2MergeConflictAddress::GetTextLC( EPbk2ConflictedNumber aNumber ) 
    {
    __ASSERT_ALWAYS( iContactFirst && iContactSecond, Panic( EPbk2NotInitialized ) );
    
    RBuf addressText;
    if( aNumber == EPbk2ConflictedFirst )
        {
        Pbk2AddressTools::GetAddressPreviewLC( *iContactFirst ,
                    iAddressGroup, addressText );
        }
    else
        {
        Pbk2AddressTools::GetAddressPreviewLC( *iContactSecond , 
                    iAddressGroup, addressText );
        }
    
    HBufC* retText = addressText.AllocL();
    CleanupStack::PopAndDestroy( &addressText );
    CleanupStack::PushL( retText );
    return retText;
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflictAddress::ResolveConflict
// --------------------------------------------------------------------------
//
void CPbk2MergeConflictAddress::ResolveConflict( EPbk2ConflictedNumber aNumber ) 
    {
    __ASSERT_ALWAYS( iContactFirst && iContactSecond, Panic( EPbk2NotInitialized ) );
    
    if( aNumber == EPbk2ConflictedFirst )
        {
        iContactChosenAddress = iContactFirst;
        }
    else
        {
        iContactChosenAddress = iContactSecond;
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeConflictAddress::GetChosenFieldsL
// --------------------------------------------------------------------------
//
void CPbk2MergeConflictAddress::GetChosenFieldsL( 
        RPointerArray<MVPbkStoreContactField>& aAddressFields )
    {
    __ASSERT_ALWAYS( iContactChosenAddress, Panic( EPbk2NotChosen ) );
    
    CleanupClosePushL( aAddressFields );
    CPbk2PresentationContactFieldCollection& fields = iContactChosenAddress->PresentationFields();
    
    for( TInt idx = 0; idx < fields.FieldCount(); idx++ )
        {
        CPbk2PresentationContactField& field = fields.At( idx );
        const MPbk2FieldProperty& property = field.FieldProperty();
        if( property.GroupId() == iAddressGroup )
            {
            aAddressFields.AppendL( &field );
            }
        }
    CleanupStack::Pop();
    }

