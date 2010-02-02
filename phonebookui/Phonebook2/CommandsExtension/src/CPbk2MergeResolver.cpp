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
 * Description:  Phonebook 2 merge contacts resolver.
 *
 */

// INCLUDE FILES

//Phonebook2
#include "CPbk2MergeResolver.h"
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2PresentationContactField.h>
#include <MPbk2FieldProperty.h>

//Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactFieldBinaryData.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

/// Unnamed namespace for local definitions
namespace 
    {
    enum TPbk2PanicCodes
        {
        EPbk2WrongArgument,
        EPbk2FieldTypeNotExists
        };
    
    void Panic(TPbk2PanicCodes aReason)
        {
        _LIT( KPanicText, "CPbk2_Merge_Resolver" );
        User::Panic(KPanicText,aReason);
        }
    
#ifdef _DEBUG
    #define DEBUG_PRINT_FIELD( field ) PrintFieldL( field );
    const TInt KEndLine = '\0';
#else
    #define DEBUG_PRINT_FIELD( field ) 
#endif
    
    } /// namespace



// --------------------------------------------------------------------------
// CPbk2MergeResolver::CPbk2MergeResolver
// --------------------------------------------------------------------------
//
CPbk2MergeResolver::CPbk2MergeResolver(
        CPbk2PresentationContact* aFirstContact, 
        CPbk2PresentationContact* aSecondContact )
    :iFirstContact( aFirstContact ), 
     iSecondContact( aSecondContact )
    {
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::~CPbk2MergeResolver
// --------------------------------------------------------------------------
//
CPbk2MergeResolver::~CPbk2MergeResolver()
    {
    iConflicts.ResetAndDestroy();
    iMerged.Close();
    delete iFirstContact;
    delete iSecondContact;
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::NewL
// --------------------------------------------------------------------------
//
CPbk2MergeResolver* CPbk2MergeResolver::NewL( 
        CPbk2PresentationContact* aFirstContact, 
        CPbk2PresentationContact* aSecondContact )
    {
    CPbk2MergeResolver* self = 
        new ( ELeave ) CPbk2MergeResolver( aFirstContact, aSecondContact );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::MergeL
// --------------------------------------------------------------------------
//
void CPbk2MergeResolver::MergeL()
    {
    iConflicts.ResetAndDestroy();
    iMerged.Close();
    
    CPbk2PresentationContactFieldCollection & firstCollection = iFirstContact->PresentationFields();
    CPbk2PresentationContactFieldCollection & secondCollection = iSecondContact->PresentationFields();
    
    RArray<TInt> duplicatesInFirst;
    CleanupClosePushL(duplicatesInFirst);
    RArray<TInt> duplicatesInSecond;
    CleanupClosePushL(duplicatesInSecond);
    
    TInt countFirst = firstCollection.FieldCount();
    TInt countSecond = secondCollection.FieldCount();
    TBool addressesMerged = EFalse;
    
    for( TInt idxFirst = 0; idxFirst < countFirst; idxFirst++)
        {
        const MVPbkStoreContactField& fieldFirst = firstCollection.FieldAt( idxFirst );
       
        for( TInt idxSecond = 0; idxSecond < countSecond; idxSecond++)
            {
            const MVPbkStoreContactField& fieldSecond = secondCollection.FieldAt( idxSecond );
            
            if( AddressField( fieldFirst ) )
                {
                if( !addressesMerged )
                    {
                    MergeAddressesL();
                    addressesMerged = ETrue;
                    }
                continue;
                }
            
            if( AddressField( fieldSecond ) )
                {
                continue;
                }
                
            
            TBool sameTypes = AreFieldsTypeSame( fieldFirst, fieldSecond );
            TBool sameValues = EqualsValues( fieldFirst, fieldSecond );
            
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2MergeResolver::MergeL - Fields:") );
            DEBUG_PRINT_FIELD( fieldFirst )
            DEBUG_PRINT_FIELD( fieldSecond )
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2MergeResolver::MergeL - Fields are the same:%d"), sameTypes);
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2MergeResolver::MergeL - Fields have the same values:%d"), sameValues);
            
            if( sameTypes )
                {
                if( IsTopContactField( fieldFirst ) )
                    {
                    if( CompareTopConactFields( fieldFirst, fieldSecond ) <= 0 )
                        {
                        duplicatesInSecond.AppendL( idxSecond );
                        }
                    else
                        {
                        duplicatesInFirst.AppendL( idxFirst );
                        }
                    }
                else
                    {
                    if( !sameValues && FieldMultiplicity( fieldFirst ) == EPbk2FieldMultiplicityOne  )
                        {
                        AddConflictL( fieldFirst, fieldSecond );
                        }
                    if( sameValues )
                        {
                        duplicatesInSecond.AppendL( idxSecond );
                        }
                    }
                }
            }
        }
    
    if( !addressesMerged )
        {
        MergeAddressesL();
        }
    
    CompleteMergeArrL( duplicatesInFirst, duplicatesInSecond );
    
    CleanupStack::PopAndDestroy( &duplicatesInSecond );
    CleanupStack::PopAndDestroy( &duplicatesInFirst );
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::MergeAddressesL
// --------------------------------------------------------------------------
//
void CPbk2MergeResolver::MergeAddressesL()
    {
    for (TInt groupId = EPbk2FieldGroupIdPostalAddress; groupId
            <= EPbk2FieldGroupIdCompanyAddress; groupId++)
        {
        ECompareAddress compareAddresses = CompareAddresses(
                ( TPbk2FieldGroupId) groupId );
        switch ( compareAddresses )
            {
            case ECASame:
                CopyAddressToMergedL( *iFirstContact, 
                        ( TPbk2FieldGroupId) groupId );
                break;
            case ECAFirstIncludesSecond:
                CopyAddressToMergedL( *iFirstContact,
                        ( TPbk2FieldGroupId) groupId );
                break;
            case ECASecondIncludesFirst:
                CopyAddressToMergedL( *iSecondContact,
                        ( TPbk2FieldGroupId) groupId );
                break;
            case ECADifferent:
                AddAddressConflictL( (TPbk2FieldGroupId) groupId );
                break;
            case ECAUndefined:
                // Both addresses don't exist
                break;
            default:
                Panic( EPbk2WrongArgument );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::CompleteMergeArrL
// --------------------------------------------------------------------------
//
void CPbk2MergeResolver::CompleteMergeArrL( 
        RArray<TInt>& aDuplicatesInFirst, 
        RArray<TInt>& aDuplicatesInSecond )
    {
    CPbk2PresentationContactFieldCollection & firstCollection = iFirstContact->PresentationFields();
    CPbk2PresentationContactFieldCollection & secondCollection = iSecondContact->PresentationFields();
        
    TInt countFirst = firstCollection.FieldCount();
    TInt countSecond = secondCollection.FieldCount();

    for( TInt idxFirst = 0; idxFirst < countFirst; idxFirst++)
        {
        const MVPbkStoreContactField& fieldFirst = firstCollection.FieldAt( idxFirst );
        if( FindConflictByFieldL( fieldFirst ) == KErrNotFound &&
            aDuplicatesInFirst.Find( idxFirst ) == KErrNotFound  &&
            !AddressField( fieldFirst ) )
            {
            iMerged.AppendL( &fieldFirst );
            
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2MergeResolver::MergeL - Field added to merged(first contact):"));
            DEBUG_PRINT_FIELD( fieldFirst )
            }
        }
    
    for( TInt idxSecond = 0; idxSecond < countSecond; idxSecond++)
        {
        const MVPbkStoreContactField& fieldSecond = secondCollection.FieldAt( idxSecond );
        if( FindConflictByFieldL( fieldSecond ) == KErrNotFound && 
            aDuplicatesInSecond.Find( idxSecond ) == KErrNotFound  && 
            !AddressField( fieldSecond ) )
            {
            iMerged.AppendL( &fieldSecond );
            
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ("CPbk2MergeResolver::MergeL - Field added to merged(second contact):"));
            DEBUG_PRINT_FIELD( fieldSecond )
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::IsTopContactField
// --------------------------------------------------------------------------
//
TBool CPbk2MergeResolver::IsTopContactField( 
        const MVPbkStoreContactField& aField )
    {
    const MVPbkFieldType* fieldType = aField.BestMatchingFieldType();
    __ASSERT_ALWAYS( fieldType, Panic( EPbk2FieldTypeNotExists ) );
    TArray<TVPbkFieldVersitProperty> versitProp = fieldType->VersitProperties();
    
    TBool topContact = EFalse;
    
    for( TInt idx = 0; idx < versitProp.Count() && !topContact; idx++ )
        {
        TVPbkFieldVersitProperty versitPropFirst = versitProp[idx];
        if( versitPropFirst.Name() == EVPbkVersitNameTopContact )
            {
            topContact = ETrue;
            }
        }
    
    return topContact;
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::CompareTopConactFields
// --------------------------------------------------------------------------
//
TInt CPbk2MergeResolver::CompareTopConactFields(
        const MVPbkStoreContactField& aFieldFirst,
        const MVPbkStoreContactField& aFieldSecond )
    {
    const MVPbkContactFieldData& dataFirst = aFieldFirst.FieldData();
    TVPbkFieldStorageType storageTypeFirst = dataFirst.DataType();
    
    const MVPbkContactFieldData& dataSecond = aFieldSecond.FieldData();
    TVPbkFieldStorageType storageTypeSecond = dataSecond.DataType();
        
    TInt ret = KErrGeneral;
    
    if( storageTypeFirst != storageTypeSecond ||
        storageTypeFirst != EVPbkFieldStorageTypeText )
        {
        return ret;
        }
    
    TInt valueFirst;
    TInt valueSecond;
    TInt errorCode;
    TLex lex( MVPbkContactFieldTextData::Cast( dataFirst ).Text() );
    errorCode = lex.Val( valueFirst );
    
    if( errorCode == KErrNone )
        {
        TLex lex( MVPbkContactFieldTextData::Cast( dataSecond ).Text() );
        errorCode = lex.Val( valueSecond );
        }
    
    if( errorCode == KErrNone )
        {
        if( valueFirst > valueSecond )
            {
            ret = 1;
            }
        else if( valueFirst < valueSecond )
            {
            ret = -1;
            }
        else
            {
            ret = 0;
            }
        }
    
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::CountConflicts
// --------------------------------------------------------------------------
//
TInt CPbk2MergeResolver::CountConflicts()
    {
    return iConflicts.Count();
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::GetConflictAt
// --------------------------------------------------------------------------
//
MPbk2MergeConflict& CPbk2MergeResolver::GetConflictAt( TInt aConflict )
    {
    __ASSERT_ALWAYS( aConflict >= 0 && aConflict < CountConflicts(), Panic( EPbk2WrongArgument ) );
    
    return *iConflicts[aConflict];
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::CountMerged
// --------------------------------------------------------------------------
//
TInt CPbk2MergeResolver::CountMerged()
    {
    return iMerged.Count();
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::GetMergedAtL
// --------------------------------------------------------------------------
//
MVPbkStoreContactField& CPbk2MergeResolver::GetMergedAt( TInt aMarged )
    {
    __ASSERT_ALWAYS( aMarged >= 0 && aMarged < CountMerged(), Panic( EPbk2WrongArgument ) );
    
    return *iMerged[aMarged];
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::CompareAddresses
// --------------------------------------------------------------------------
//
CPbk2MergeResolver::ECompareAddress CPbk2MergeResolver::CompareAddresses( 
        TPbk2FieldGroupId aGroupId )
    {
    ECompareAddress retCompare = ECAUndefined;
    
    TInt countEquals = 0;
    
    CPbk2PresentationContactFieldCollection& firstCollection = iFirstContact->PresentationFields();
    CPbk2PresentationContactFieldCollection& secondCollection = iSecondContact->PresentationFields();
    
    TInt countFirst = firstCollection.FieldCount();
    TInt countSecond = secondCollection.FieldCount();
    
    TInt countAddressFieldsFirst = 0;
    TInt countAddressFieldsSecond = 0;
    
    for( TInt idxFirst = 0; idxFirst < countFirst && retCompare != ECADifferent; idxFirst++)
        {
        const CPbk2PresentationContactField& fieldFirst = firstCollection.At( idxFirst );
        const MPbk2FieldProperty& propFirst = fieldFirst.FieldProperty();
        
        if( !AddressField( fieldFirst ) || propFirst.GroupId() != aGroupId )
            {
            continue;
            }
        
        countAddressFieldsFirst++;
        
        for( TInt idxSecond = 0; idxSecond < countSecond; idxSecond++)
            {
            const CPbk2PresentationContactField& fieldSecond = secondCollection.At( idxSecond );
            const MPbk2FieldProperty& propSecond = fieldSecond.FieldProperty();
            
            if( !AddressField( fieldSecond ) || propSecond.GroupId() != aGroupId )
                {
                continue;
                }
            
            if( countAddressFieldsFirst == 1 )
                {
                countAddressFieldsSecond++;  
                }
            
            if( !AreAddressesSubTypeSame( fieldFirst, fieldSecond ) )
                {
                continue;
                }
            
            if( EqualsValues( fieldFirst, fieldSecond ) )
                {
                countEquals++;
                }
            else
                {
                return ECADifferent;
                }
            }
        }
    
    if( countAddressFieldsFirst == 0 )
        {
        for( TInt idxSecond = 0; idxSecond < countSecond; idxSecond++)
            {
            const CPbk2PresentationContactField& fieldSecond = secondCollection.At( idxSecond );
            const MPbk2FieldProperty& propSecond = fieldSecond.FieldProperty();
            
            if( AddressField( fieldSecond ) && propSecond.GroupId() == aGroupId )
                {
                countAddressFieldsSecond++; 
                }
            }
        }

    return DoCompareAddresses( countEquals, countAddressFieldsFirst, countAddressFieldsSecond );
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::CompareAddresses
// --------------------------------------------------------------------------
//
CPbk2MergeResolver::ECompareAddress CPbk2MergeResolver::DoCompareAddresses( 
        TInt aCountEquals, 
        TInt aCountAddressFieldsFirst,
        TInt aCountAddressFieldsSecond )
    {
    ECompareAddress retCompare = ECAUndefined;
    
    if( aCountAddressFieldsFirst == 0 && aCountAddressFieldsSecond != 0 )
        {
        retCompare = ECASecondIncludesFirst;
        }
    else if( aCountAddressFieldsFirst != 0 && aCountAddressFieldsSecond == 0 )
        {
        retCompare = ECAFirstIncludesSecond;
        }
    else if( aCountAddressFieldsFirst != 0 && aCountAddressFieldsSecond != 0 )
        {
        if( aCountEquals == aCountAddressFieldsFirst && aCountEquals == aCountAddressFieldsSecond )
            {
            retCompare = ECASame;
            }
        else if( aCountEquals == aCountAddressFieldsFirst )
            {
            retCompare = ECASecondIncludesFirst;
            }
        else if( aCountEquals == aCountAddressFieldsSecond )
            {
            retCompare = ECAFirstIncludesSecond;
            }
        else
            {
            retCompare = ECADifferent;
            }
        }
    
    return retCompare;
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::AddressField
// --------------------------------------------------------------------------
//
TBool CPbk2MergeResolver::AddressField( const MVPbkStoreContactField& aField )
    {
    const MVPbkFieldType* fieldType = aField.BestMatchingFieldType();
    __ASSERT_ALWAYS( fieldType, Panic( EPbk2FieldTypeNotExists ) );
    TArray<TVPbkFieldVersitProperty> versitProp = fieldType->VersitProperties();
    
    TBool address = EFalse;
    
    for( TInt idx = 0; idx < versitProp.Count() && !address; idx++ )
        {
        TVPbkFieldVersitProperty versitPropFirst = versitProp[idx];
        if( versitPropFirst.Name() == EVPbkVersitNameADR ||
            versitPropFirst.Name() == EVPbkVersitNameGEO )
            {
            address = ETrue;
            }
        }
    
    return address;
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::FieldMultiplicity
// --------------------------------------------------------------------------
//
TPbk2FieldMultiplicity CPbk2MergeResolver::FieldMultiplicity( const MVPbkStoreContactField& aField )
    {
    const CPbk2PresentationContactField& presentationField = 
            static_cast<const CPbk2PresentationContactField&>( aField );
    
    return presentationField.FieldProperty().Multiplicity();
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::EqualsValues
// --------------------------------------------------------------------------
//
TBool CPbk2MergeResolver::EqualsValues( 
        const MVPbkStoreContactField& aFieldFirst,
        const MVPbkStoreContactField& aFieldSecond )
    {
    const MVPbkContactFieldData& dataFirst = aFieldFirst.FieldData();
    TVPbkFieldStorageType storageTypeFirst = dataFirst.DataType();
    
    const MVPbkContactFieldData& dataSecond = aFieldSecond.FieldData();
    TVPbkFieldStorageType storageTypeSecond = dataSecond.DataType();
        
    if( storageTypeFirst != storageTypeSecond )
        {
        return EFalse;
        }
    
    TBool sameValues = EFalse;
    
    switch( storageTypeFirst )
        {
        case EVPbkFieldStorageTypeText:
            {
            if( MVPbkContactFieldTextData::Cast( dataFirst ).Text() == 
                MVPbkContactFieldTextData::Cast( dataSecond ).Text() )
                {
                sameValues = ETrue;
                }
            }
            break;
        case EVPbkFieldStorageTypeDateTime:
            {
            if( MVPbkContactFieldDateTimeData::Cast( dataFirst ).DateTime() == 
                MVPbkContactFieldDateTimeData::Cast( dataSecond ).DateTime() )
                {
                sameValues = ETrue;
                }
            }
            break;
        case EVPbkFieldStorageTypeUri:
            {
            if( MVPbkContactFieldUriData::Cast( dataFirst ).Text() == 
                MVPbkContactFieldUriData::Cast( dataSecond ).Text() )
                {
                sameValues = ETrue;
                }
            }
            break;
        case EVPbkFieldStorageTypeBinary:
            {
            const MVPbkContactFieldBinaryData& binaryFirst = 
                    MVPbkContactFieldBinaryData::Cast( dataFirst );
            const MVPbkContactFieldBinaryData& binarySecond = 
                    MVPbkContactFieldBinaryData::Cast( dataSecond );
            if( binaryFirst.BinaryData() == binarySecond.BinaryData() )
                {
                sameValues = ETrue;
                }
            break;
            }
        // Ignore other field types
        }
    
    return sameValues;
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::AreAddressesSubTypeSame
// --------------------------------------------------------------------------
//
TBool CPbk2MergeResolver::AreAddressesSubTypeSame( 
        const MVPbkStoreContactField& aFieldFirst,
        const MVPbkStoreContactField& aFieldSecond )
    {
    const MVPbkFieldType* fieldTypeFirst = aFieldFirst.BestMatchingFieldType();
    __ASSERT_ALWAYS( fieldTypeFirst, Panic( EPbk2FieldTypeNotExists ) );
    
    const MVPbkFieldType* fieldTypeSecond = aFieldSecond.BestMatchingFieldType();
    __ASSERT_ALWAYS( fieldTypeSecond, Panic( EPbk2FieldTypeNotExists ) );
    
    TArray<TVPbkFieldVersitProperty> versitPropArrFirst = fieldTypeFirst->VersitProperties();
    TArray<TVPbkFieldVersitProperty> versitPropArrSecond = fieldTypeSecond->VersitProperties();
    
    TInt countFirst = versitPropArrFirst.Count();
    TInt countSecond = versitPropArrSecond.Count();

    TBool retCompare = EFalse;
    
    for( TInt idxFirst = 0; idxFirst < countFirst && !retCompare; idxFirst++ )
        {
        TVPbkFieldVersitProperty versitPropFirst = versitPropArrFirst[idxFirst];
        for( TInt idxSecond = 0; idxSecond < countSecond; idxSecond++ )
            {
            TVPbkFieldVersitProperty versitPropSecond = versitPropArrSecond[idxSecond];
            if( versitPropFirst.SubField() == versitPropSecond.SubField() )
                {
                retCompare = ETrue;
                break;
                }
            }
        }
    
    return retCompare;
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::AreFieldsTypeSame
// --------------------------------------------------------------------------
//
TBool CPbk2MergeResolver::AreFieldsTypeSame( 
        const MVPbkStoreContactField& aFieldFirst,
        const MVPbkStoreContactField& aFieldSecond )
    {
    const MVPbkFieldType* fieldTypeFirst = aFieldFirst.BestMatchingFieldType();
    __ASSERT_ALWAYS( fieldTypeFirst, Panic( EPbk2FieldTypeNotExists ) );
    
    const MVPbkFieldType* fieldTypeSecond = aFieldSecond.BestMatchingFieldType();
    __ASSERT_ALWAYS( fieldTypeSecond, Panic( EPbk2FieldTypeNotExists ) );
    
    return fieldTypeFirst->IsSame( *fieldTypeSecond );
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::AddConflictL
// --------------------------------------------------------------------------
//
void CPbk2MergeResolver::AddConflictL( 
        const MVPbkStoreContactField& aFieldFirst,
        const MVPbkStoreContactField& aFieldSecond )
    {
    CPbk2MergeConflict* conflictNew = CPbk2MergeConflict::NewL();
    CleanupStack::PushL( conflictNew );
    conflictNew->AddFields( aFieldFirst, aFieldSecond );
    iConflicts.AppendL( conflictNew );
    CleanupStack::Pop( conflictNew );
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING( "CPbk2MergeResolver::AddConflictL - Conflict created" ) );
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::AddAddressConflictL
// --------------------------------------------------------------------------
//
void CPbk2MergeResolver::AddAddressConflictL( TPbk2FieldGroupId aGroupId )
    {
    CPbk2MergeConflictAddress* conflictNew = CPbk2MergeConflictAddress::NewL();
    CleanupStack::PushL( conflictNew );
    conflictNew->AddAddress( *iFirstContact, *iSecondContact, aGroupId );
    iConflicts.AppendL( conflictNew );
    CleanupStack::Pop( conflictNew );
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING( "CPbk2MergeResolver::AddAddressConflictL - Conflict created" ) );
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::FindConflictByFieldL
// --------------------------------------------------------------------------
//
TInt CPbk2MergeResolver::FindConflictByFieldL( const MVPbkStoreContactField& aField )
    {
    TInt countConflicts = iConflicts.Count();
    for( TInt idx = 0; idx < countConflicts; idx++ )
        {
        if( iConflicts[idx] && 
            ( iConflicts[idx]->GetConflictType() == EPbk2ConflictTypeNormal || 
              iConflicts[idx]->GetConflictType() == EPbk2ConflictTypeImage ) )
            {
            CPbk2MergeConflict* conflict = static_cast<CPbk2MergeConflict*>( iConflicts[idx] );
            if( conflict )
                {
                const MVPbkStoreContactField* fieldFirst = NULL;
                const MVPbkStoreContactField* fieldSecond = NULL;
                conflict->GetFieldsL( fieldFirst, fieldSecond );
                if( fieldFirst == &aField || fieldSecond == &aField )
                    {
                    return idx;
                    }
                }
            }
        }
    return KErrNotFound;
    }

// --------------------------------------------------------------------------
// CPbk2MergeResolver::CopyAddressToMergedL
// --------------------------------------------------------------------------
//
void CPbk2MergeResolver::CopyAddressToMergedL( 
        const CPbk2PresentationContact& aContact, 
        TPbk2FieldGroupId aAddressGroup )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2MergeResolver::CopyAddressToMergedL - Add address(group id:%d) to merged. Fields:"), 
        aAddressGroup);
    CPbk2PresentationContactFieldCollection& fields = aContact.PresentationFields();
    for( TInt idx = 0; idx < fields.FieldCount(); idx++ )
        {
        CPbk2PresentationContactField& field = fields.At( idx );
        const MPbk2FieldProperty& property = field.FieldProperty();
        if( property.GroupId() == aAddressGroup )
            {
            DEBUG_PRINT_FIELD( field )
            iMerged.AppendL( &field );
            }
        }
    }

#ifdef _DEBUG
// --------------------------------------------------------------------------
// CPbk2MergeResolver::PrintFieldL
// --------------------------------------------------------------------------
//
void CPbk2MergeResolver::PrintFieldL( const MVPbkStoreContactField& aField )
    {
    const MVPbkContactFieldData& data = aField.FieldData();
    TVPbkFieldStorageType storageType = data.DataType();
    
    RBuf lebel;
    CleanupClosePushL( lebel );
    RBuf value;
    CleanupClosePushL( value );
    
    lebel.CreateL( aField.FieldLabel().Length() + 1 );
    lebel = aField.FieldLabel();
    lebel.SetLength( aField.FieldLabel().Length() + 1 );
    lebel[aField.FieldLabel().Length()] = KEndLine;
    
    switch( storageType )
        {
        case EVPbkFieldStorageTypeText:
            {
            value.CreateL( MVPbkContactFieldTextData::Cast( data ).Text().Length() + 1 );
            value = MVPbkContactFieldTextData::Cast( data ).Text();
            value.SetLength( MVPbkContactFieldTextData::Cast( data ).Text().Length() + 1 );
            value[MVPbkContactFieldTextData::Cast( data ).Text().Length()] = KEndLine;
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ( "CPbk2MergeResolver: Field Info: %s:%s "), lebel.Ptr(), value.Ptr() );
            }
            break;
        case EVPbkFieldStorageTypeDateTime:
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ( "CPbk2MergeResolver: Field Info: %s"), lebel.Ptr() );
            }
            break;
        case EVPbkFieldStorageTypeUri:
            {
            value.CreateL( MVPbkContactFieldUriData::Cast( data ).Text().Length() + 1 );
            value = MVPbkContactFieldUriData::Cast( data ).Text();
            value.SetLength( MVPbkContactFieldUriData::Cast( data ).Text().Length() + 1 );
            value[MVPbkContactFieldUriData::Cast( data ).Text().Length()] = KEndLine;
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                ( "CPbk2MergeResolver: Field Info: %s:%s "), lebel.Ptr(), value.Ptr() );
            }
            break;
        }
    CleanupStack::PopAndDestroy( &value );
    CleanupStack::PopAndDestroy( &lebel );
    }
#endif
