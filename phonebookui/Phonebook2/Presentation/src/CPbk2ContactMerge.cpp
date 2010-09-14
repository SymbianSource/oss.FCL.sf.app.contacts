/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class for combining data of two contacts
*
*/


// INCLUDE FILES
#include <CPbk2ContactMerge.h>

// From Phonebook2
#include "Pbk2PresentationUtils.h"
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2FieldPropertyArray.h>
#include <MPbk2FieldProperty.h>
#include <RPbk2LocalizedResourceFile.h>
#include <pbk2presentation.rsg>
#include <Pbk2DataCaging.hrh>


// From Virtual Phonebook
#include <MVPbkFieldType.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldBinaryData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <TVPbkFieldVersitProperty.h>
#include <CVPbkFieldTypeSelector.h>

// From System
#include <barsread.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbk2ContactMerge::CPbk2ContactMerge
// ---------------------------------------------------------------------------
//
CPbk2ContactMerge::CPbk2ContactMerge( 
        CVPbkContactManager& aContactManager, 
        MPbk2ContactNameFormatter& aNameFormatter,
        const MPbk2FieldPropertyArray& aFieldProperties )
        :   iContactManager( aContactManager ),
            iNameFormatter( aNameFormatter ),
            iFieldProperties( aFieldProperties )
    {
    }

// ---------------------------------------------------------------------------
// CPbk2ContactMerge::~CPbk2ContactMerge
// ---------------------------------------------------------------------------
//
CPbk2ContactMerge::~CPbk2ContactMerge()
    {
    delete iEMailSelector;
    delete iNumberSelector;
    }
    
// ---------------------------------------------------------------------------
// CPbk2ContactMerge::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CPbk2ContactMerge* CPbk2ContactMerge::NewL(
        CVPbkContactManager& aContactManager, 
        MPbk2ContactNameFormatter& aNameFormatter,
        const MPbk2FieldPropertyArray& aFieldProperties )
    {
    CPbk2ContactMerge* self = new( ELeave ) CPbk2ContactMerge( 
        aContactManager, aNameFormatter, aFieldProperties );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPbk2ContactMerge::ConstructL
// ---------------------------------------------------------------------------
//
void CPbk2ContactMerge::ConstructL()
    {
    RPbk2LocalizedResourceFile resFile( &iContactManager.FsSession() );
    resFile.OpenLC( KPbk2RomFileDrive,
        KDC_RESOURCE_FILES_DIR,
        Pbk2PresentationUtils::PresentationResourceFile() );
    
    TResourceReader reader;
    reader.SetBuffer( resFile.AllocReadLC( R_PBK2_EMAIL_SELECTOR ) );
    
    iEMailSelector = CVPbkFieldTypeSelector::NewL(
        reader,
        iContactManager.FieldTypes() );
    CleanupStack::PopAndDestroy(); // reader
    
    reader.SetBuffer( resFile.AllocReadLC( R_PBK2_NUMBER_SELECTOR ) );
    iNumberSelector  = CVPbkFieldTypeSelector::NewL(
        reader,
        iContactManager.FieldTypes() );
    CleanupStack::PopAndDestroy(2); // reader, resFile
    }
    

// ---------------------------------------------------------------------------
// CPbk2ContactMerge::MergeDataL
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ContactMerge::MergeDataL( 
        const MVPbkBaseContact& aContactFrom, MVPbkStoreContact& aContactTo )
    {
    TInt srcTitleFieldCount = 0;
    TInt copiedFieldCount = 0;
    const MVPbkBaseContactFieldCollection& srcFields = aContactFrom.Fields();
    const TInt srcFieldCount = srcFields.FieldCount();
    const MVPbkFieldTypeList& targetTypes = 
        aContactTo.ParentStore().StoreProperties().SupportedFields();
    for ( TInt i = 0; i < srcFieldCount; ++i )
        {
        const MVPbkBaseContactField& srcField = srcFields.FieldAt( i );
        const MVPbkFieldType* type = srcField.BestMatchingFieldType();
        // Title fields are not merged
        TBool titleField = iNameFormatter.IsTitleFieldType( *type );
        if ( titleField )
            {
            ++srcTitleFieldCount;
            }
        else if ( type && targetTypes.ContainsSame( *type ) )
            {            
            CVPbkBaseContactFieldTypeIterator* iter = 
                    CVPbkBaseContactFieldTypeIterator::NewLC( *type, 
                        aContactTo.Fields() );
            TInt maxNumOfFields = aContactTo.MaxNumberOfFieldL( *type );
            TInt curFieldCount = 0;
            while ( iter->HasNext() )
                {
                iter->Next();
                ++curFieldCount;
                }
            CleanupStack::PopAndDestroy( iter );
            
            // Check field multiplicity
            const MPbk2FieldProperty* prop = 
                iFieldProperties.FindProperty( *type );
            if ( prop && prop->Multiplicity() == EPbk2FieldMultiplicityOne )
                {
                // If UI specification limits the number of certain type
                // of field to one then check if the target contact
                // already contains that type of field
                if ( curFieldCount == 0 )
                    {
                    CopyFieldDataL( srcField, aContactTo );
                    ++copiedFieldCount;
                    }
                }
            else if ( ( maxNumOfFields == KVPbkStoreContactUnlimitedNumber ||
                      curFieldCount < aContactTo.MaxNumberOfFieldL( *type )) &&
                      !FindMatch( srcField, aContactTo ))
                {
                // Field can be copied from source to destination. This is 
                // because target field count is not limited or target contact
                // field limit is not exceeded.
                CopyFieldDataL( srcField, aContactTo );
                ++copiedFieldCount;
                }
            }
        }
    // Return ETrue if all data fields from source contact were copied to target
    return ( srcTitleFieldCount + copiedFieldCount ) == srcFieldCount;
    }

// ---------------------------------------------------------------------------
// CPbk2ContactMerge::CopyFieldDataL
// ---------------------------------------------------------------------------
//
void CPbk2ContactMerge::CopyFieldDataL( const MVPbkBaseContactField& aField, 
        MVPbkStoreContact& aContactTo )
    {
    // Field has always a non NULL field type when this function is called
    MVPbkStoreContactField* field = aContactTo.CreateFieldLC(
        *aField.BestMatchingFieldType() );
    field->FieldData().CopyL( aField.FieldData() );
    aContactTo.AddFieldL( field );
    CleanupStack::Pop(); // field
    }

// ---------------------------------------------------------------------------
// CPbk2ContactMerge::FindMatch
// ---------------------------------------------------------------------------
//    
TBool CPbk2ContactMerge::FindMatch( const MVPbkBaseContactField& aField, 
        MVPbkStoreContact& aContactTo )
    {
    TBool result = EFalse;
    
    MVPbkStoreContactFieldCollection& fields = aContactTo.Fields();
    const TInt count = fields.FieldCount();
    // Check if any of the email fields contains a same email address
    if ( iEMailSelector->IsFieldIncluded( aField ) )
        {
        for ( TInt i = 0 ; i < count && !result; ++i )
            {
            MVPbkStoreContactField& field = fields.FieldAt( i );
            if ( iEMailSelector->IsFieldIncluded( field ) )
                {
                result = CompareData( aField.FieldData(), 
                    field.FieldData() );
                }
            }    
        }
    // Check if any of the number fields contains a same number
    else if ( iNumberSelector->IsFieldIncluded( aField ) )
        {
        for ( TInt i = 0 ; i < count && !result; ++i )
            {
            MVPbkStoreContactField& field = fields.FieldAt( i );
            if ( iNumberSelector->IsFieldIncluded( field ) )
                {
                result = CompareData( aField.FieldData(), 
                    field.FieldData() );
                }
            }    
        }
    // Check if there is a same type of field and if found, compare the data
    else
        {
        const MVPbkFieldType* type = aField.BestMatchingFieldType();
        if ( type )
            {
            // If iterator creation fails we cannot make comparison and 
            // the result is false.
            TRAP_IGNORE(
                {
                CVPbkBaseContactFieldTypeIterator* iter = 
                    CVPbkBaseContactFieldTypeIterator::NewLC( *type, fields );
                while ( iter->HasNext() && !result )
                    {
                    const MVPbkBaseContactField* field = iter->Next();
                    result = CompareData( aField.FieldData(), field->FieldData() );
                    }
                CleanupStack::PopAndDestroy( iter );                
                });
            }
        }
         
    return result;
    }

// ---------------------------------------------------------------------------
// CPbk2ContactMerge::CompareTextData
// ---------------------------------------------------------------------------
//        
TBool CPbk2ContactMerge::CompareData( const MVPbkContactFieldData& aData1,
        const MVPbkContactFieldData& aData2 )
    {
    TBool result = EFalse;
    TVPbkFieldStorageType dataType = aData1.DataType();
    if ( dataType == aData2.DataType() )
        {
        if ( dataType == EVPbkFieldStorageTypeText )
            {
            result = CompareTextData( 
                MVPbkContactFieldTextData::Cast( aData1 ),
                MVPbkContactFieldTextData::Cast( aData2 ) );
            }
        else if ( dataType == EVPbkFieldStorageTypeBinary )
            {
            result = CompareBinaryData( 
                MVPbkContactFieldBinaryData::Cast( aData1 ),
                MVPbkContactFieldBinaryData::Cast( aData2 ) );
            }
        else if ( dataType == EVPbkFieldStorageTypeDateTime )
            {
            result = CompareDateTimeData( 
                MVPbkContactFieldDateTimeData::Cast( aData1 ),
                MVPbkContactFieldDateTimeData::Cast( aData2 ) );
            }
        }
    return result;
    }
    
// ---------------------------------------------------------------------------
// CPbk2ContactMerge::CompareTextData
// ---------------------------------------------------------------------------
//        
TBool CPbk2ContactMerge::CompareTextData( 
        const MVPbkContactFieldTextData& aData1, 
        const MVPbkContactFieldTextData& aData2)
    {
    return aData1.Text().CompareF( aData2.Text() ) == 0;
    }

// ---------------------------------------------------------------------------
// CPbk2ContactMerge::CompareBinaryData
// ---------------------------------------------------------------------------
//            
TBool CPbk2ContactMerge::CompareBinaryData( 
        const MVPbkContactFieldBinaryData& aData1, 
        const MVPbkContactFieldBinaryData& aData2 )
    {
    return aData1.BinaryData().Compare( aData2.BinaryData() ) == 0;
    }
    
// ---------------------------------------------------------------------------
// CPbk2ContactMerge::CompareDateTimeData
// ---------------------------------------------------------------------------
//            
TBool CPbk2ContactMerge::CompareDateTimeData( 
        const MVPbkContactFieldDateTimeData& aData1, 
        const MVPbkContactFieldDateTimeData& aData2 )
    {
    return aData1.DateTime() == aData2.DateTime();
    }
