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
* Description:  Exporting the contact field from contact to 
*                CParserProperty field
*
*/


#include "CVPbkVCardPropertyExporter.h"
#include "CVPbkVCardParserParamArray.h"
#include "CVPbkVCardFieldTypeProperty.h"
#include "CVPbkVCardIdNameMapping.h"
#include "VPbkVCardEngError.h"
#include "CVPbkVCardData.h"
#include <MVPbkContactStore.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldBinaryData.h>
#include <VPbkError.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactAttributeManager.h>
#include <CVPbkDefaultAttribute.h>
#include <CVPbkLocalVariationManager.h>

// System includes
#include <vcard.h>
#include <bldvariant.hrh>

// Debug includes
#include <VPbkDebug.h>

#include <centralrepository.h> 
#include <MVPbkStoreContactProperties.h>

// CONSTANTS
const TInt KNsmlDsGeoField = 0x0000;  //  defined in DS cenrep
const TUid KCRUidGeoFieldInternalKeys = { 0x20022EE1 };  // 0x20022EE1 defined in DS cenrep

_LIT8(KGEO, "GEO");
_LIT8(KREV, "REV");



namespace
    {
    _LIT8(KPropEncoding, "ENCODING");
    _LIT8(KPropBase64, "BASE64");
    _LIT8(KPropType, "TYPE");
    _LIT(KFormatJPEG, "JPEG");
    _LIT8(KPropPREF, "PREF");
    const TInt KOne = 1;
    }

CVPbkVCardPropertyExporter* CVPbkVCardPropertyExporter::NewLC(
            CVPbkVCardData& aData,
            const MVPbkStoreContact* aSourceItem,
            RWriteStream& aDestStream,
            MVPbkSingleContactOperationObserver& aObserver,
            const TBool aBeamed)
    {
    CVPbkVCardPropertyExporter* self =
        new (ELeave) CVPbkVCardPropertyExporter(
            aData,
            aSourceItem,
            aDestStream,
            aObserver,
            aBeamed);
    CleanupStack::PushL( self );
    self->ConstructL(aData);
    return self;
    }

CVPbkVCardPropertyExporter::CVPbkVCardPropertyExporter(
            CVPbkVCardData& aData,
            const MVPbkStoreContact* aSourceItem,
            RWriteStream& aDestStream,
            MVPbkSingleContactOperationObserver& aObserver,
            const TBool aBeamed)
    :   CVPbkVCardActiveIncremental( EPriorityStandard,
                                     *this,
                                     aData,
                                     aObserver),
        iContact ( aSourceItem ),
        iDestinationStream( aDestStream ),
        iCurrentPropertyIndex( 0 ),
        iMaxPropertyCount( aData.Properties().Count() ),
        iState( EExporting ),
        iMaskedFieldsArray(aData.GetMaskedFields()),
        iBeamed(aBeamed)
    {
    }

CVPbkVCardPropertyExporter* CVPbkVCardPropertyExporter::NewLC(
            CVPbkVCardData& aData,
            RWriteStream& aDestStream,
            MVPbkSingleContactOperationObserver& aObserver,
            const TBool aBeamed)
    {
    CVPbkVCardPropertyExporter* self =
        new (ELeave) CVPbkVCardPropertyExporter(
            aData,
            aDestStream,
            aObserver,
            aBeamed);
    CleanupStack::PushL( self );
    self->ConstructL(aData);
    return self;
    }

CVPbkVCardPropertyExporter::CVPbkVCardPropertyExporter(
            CVPbkVCardData& aData,
            RWriteStream& aDestStream,
            MVPbkSingleContactOperationObserver& aObserver,
            const TBool aBeamed)
    :   CVPbkVCardActiveIncremental( EPriorityStandard,
                                     *this,
                                     aData,
                                     aObserver),
        iDestinationStream( aDestStream ),
        iCurrentPropertyIndex( 0 ),
        iMaxPropertyCount( aData.Properties().Count() ),
        iState( ECreatingRetriever ),
        iMaskedFieldsArray(aData.GetMaskedFields()),
        iBeamed(aBeamed)
    {
    }

CVPbkVCardPropertyExporter::~CVPbkVCardPropertyExporter()
    {
    iValues.ResetAndDestroy();
    delete iOwnedContact;
    delete iOperation;
    delete iParser;
    delete iOwncardHandler;
    delete iGroupcardHandler;
    }

void CVPbkVCardPropertyExporter::ExportFieldL( CVPbkVCardFieldTypeProperty& aVCardProperty )
    {
    iVCardProperty = &aVCardProperty;
    
   if ( iVCardProperty->Name().Compare(KGEO) == 0 )
   {
   	   	        // Read from cenrep	
	      CRepository* repository = CRepository::NewLC( KCRUidGeoFieldInternalKeys );
	      TBool geoFeatureValue(1); 
	      // Get the geo feature value..
	      TInt err = repository->Get( KNsmlDsGeoField, geoFeatureValue );
	      User::LeaveIfError(err);
	      CleanupStack::PopAndDestroy(repository);
	      if (!geoFeatureValue)
	        {
	          return; //no Need to add GEO field during Sync/BT
	        }
   }  
    
    // If the current operation is a beamed export
    if (iBeamed)
    {
        TInt count(iMaskedFieldsArray ? iMaskedFieldsArray->Count() : 0);
        TInt val(-1);
        for (TInt i(0); i < count; i++)
        {
            val = iVCardProperty->Name().Find( (*iMaskedFieldsArray)[i] );
            if (0 == val)
            {
                // This is a masked field
                return;
            }
        }
    }
    
    GetValuesFromIndexedPropertiesL( iVCardProperty->Count() == KOne );
    }

void CVPbkVCardPropertyExporter::GetValuesFromIndexedPropertiesL(
    TBool aSingleItem )
    {
    CVPbkVCardParserParamArray* params = NULL;
    TInt findIndex( 0 );

    if ( aSingleItem )
        {
        TBool doContinue( ETrue );
        const TVPbkVCardIndexedProperty& indexedProperty =
            iVCardProperty->IndexedPropertyAt( 0 );

        while( doContinue )
            {
            InitDataL();
            params =
                new (ELeave) CVPbkVCardParserParamArray( KOne );
            CleanupStack::PushL( params );

            doContinue = GetSingleValueAndParamsL( indexedProperty,
                                                   *params,
                                                   findIndex );
            ++findIndex;
            CleanupStack::Pop( params );
            // The params is pushed straight back to the CleanupStack in AddPropertyParserL
            // unless AddPropertyToParserL returns EFalse
            if( !AddPropertyToParserL( iValues, params ) )
                {
                delete params;
                params = NULL;
                }
            }
        }
    else
        {
        TInt indexedPropertyCount( iVCardProperty->Count() );
        InitDataL();
        params =
            new (ELeave) CVPbkVCardParserParamArray( indexedPropertyCount );
        CleanupStack::PushL( params );

        for ( TInt i = 0; i < indexedPropertyCount; ++i )
            {
            const TVPbkVCardIndexedProperty& indexedProperty =
                iVCardProperty->IndexedPropertyAt( i );
            findIndex = 0;
            GetSingleValueAndParamsL( indexedProperty, *params, findIndex );
            }

        CleanupStack::Pop( params );
        // The params are pushed straight back to the CleanupStack in 
        // AddPropertyParserL unless AddPropertyToParserL returns EFalse
        if( !AddPropertyToParserL( iValues, params ) )
            {
            delete params;
            params = NULL;
            }
        }
    }

TBool CVPbkVCardPropertyExporter::GetSingleValueAndParamsL(
    const TVPbkVCardIndexedProperty& aIndexedProperty,
    CVPbkVCardParserParamArray& aParamArray,
    TInt& aFindIndex )
    {
    MVPbkStoreContactField* field =
        FindFieldFromContact( aIndexedProperty, aFindIndex );
    if( !field )
        {
        return EFalse;
        }
    // Fetch field value and additional params
    CParserPropertyValue* value = FetchDataFromFieldL( *field, aParamArray );
    if( !value )
        {
        return ETrue;
        }
    CleanupStack::PushL( value );
    TInt index ( aIndexedProperty.Index() );
    iValues[ index ] = value;
    CleanupStack::Pop( value );

    return ETrue;
    }

void CVPbkVCardPropertyExporter::ConstructL(CVPbkVCardData& aData)
    {
    iParser = CParserVCard::NewL();
    
    iOwncardHandler = CVPbkOwnCardHandler::NewL(aData);
    iGroupcardHandler = CVPbkGroupCardHandler::NewL(aData);
    }

MVPbkStoreContactField*
CVPbkVCardPropertyExporter::FindFieldFromContact(
    const TVPbkVCardIndexedProperty& aIndexedProperty, TInt& aIndex )
    {
    const MVPbkFieldType* exportPropertyType =
        aIndexedProperty.FindMatch( iData.SupportedFieldTypes());
    
    TInt fieldCount = iContact->Fields().FieldCount();
    for ( TInt n = aIndex; n < fieldCount; ++n )
        {
        // We have to find out additional properties of the contact field so
        // we need non-const reference of the field class.
        MVPbkStoreContactField& sourceField =
            const_cast<MVPbkStoreContact*>( iContact )->Fields().FieldAt( n );

        const TInt maxMatchPriority = 
                    iData.SupportedFieldTypes().MaxMatchPriority();
        for ( TInt matchPriority = 0;
             matchPriority <= maxMatchPriority;
             ++matchPriority )
            {
            const MVPbkFieldType* sourceFieldType =
                sourceField.MatchFieldType( matchPriority );
            if ( sourceFieldType &&
                 exportPropertyType->IsSame( *sourceFieldType ) )
                {
                aIndex = n;
                return &sourceField;
                }
            }
        }
    return NULL;
    }

CParserPropertyValue* CVPbkVCardPropertyExporter::FetchDataFromFieldL(
    MVPbkStoreContactField& aField, CVPbkVCardParserParamArray& aParameters )
    {
    CParserPropertyValue* value = NULL;
    TVPbkFieldStorageType storageType = aField.FieldData().DataType();
    switch ( storageType )
        {
        case EVPbkFieldStorageTypeText:
            {
            value = GetExportTextValueL( aField );
            break;
            }
            
         case EVPbkFieldStorageTypeUri:
            {
            value = GetExportUriValueL( aField );
            break;
            }    

        case EVPbkFieldStorageTypeBinary:
            {
            value = GetExportBinaryValueL(aField, aParameters);
            break;
            }

        case EVPbkFieldStorageTypeDateTime:
            {
            value = GetExportDateValueL( aField );
            break;
            }

        default:
            {
            value = NULL;
            break;
            }
        }

    if ( value )
        {
        CleanupStack::PushL( value );
        GetAdditionalParamsL( aField, aParameters );
        CleanupStack::Pop( value );
        }

    return value;
    }


TBool CVPbkVCardPropertyExporter::AddPropertyToParserL(
    RPointerArray< CParserPropertyValue >& aValues,
    CVPbkVCardParserParamArray* aParams )
    {
    if( !HasValues() )
        {
        return EFalse;
        }

    CleanupStack::PushL( aParams );
    CParserPropertyValue* value = GetValueL( aValues );

    __ASSERT_DEBUG( value,
        VPbkVCardEngError::Panic( VPbkVCardEngError::ENullParserProperty ) );

    CleanupStack::PushL(value);
    CParserProperty* property =
        CParserProperty::NewL( *value, iVCardProperty->Name(), aParams );
    CleanupStack::Pop( value );
    CleanupStack::Pop( aParams ); // The ownership has taken by CParserProperty
    // NOTE: Versit pushes the CParserProperty* pointer stright away
    // to the cleanup stack, so to avoid duplicate entries in
    // the stack pointer must not be pushed there.
    iParser->AddPropertyL( property );

    return ETrue;
    }

TBool CVPbkVCardPropertyExporter::HasValues()
    {
    for ( TInt i = 0; i < iValues.Count(); ++i )
        {
        if ( iValues[ i ] )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

CParserPropertyValue* CVPbkVCardPropertyExporter::GetValueL(
    RPointerArray< CParserPropertyValue >& aValues)
    {
    CParserPropertyValue* value = NULL;
    if ( aValues.Count() == KOne )
        {
        value = aValues[ 0 ];
        aValues.Remove( 0 );
        }
    else
        {
        CDesCArrayFlat* desarr = new(ELeave) CDesCArrayFlat( aValues.Count() );
        CleanupStack::PushL( desarr );

        while ( aValues.Count() > 0 )
            {
            CParserPropertyValueHBufC* tmpValue =
                static_cast<CParserPropertyValueHBufC*>( aValues[ 0 ] );
            CleanupStack::PushL( tmpValue );
            aValues.Remove( 0 );

            if ( !tmpValue || tmpValue->Value().Length() == 0 )
                {
                desarr->AppendL( KNullDesC );
                }
            else
                {
                desarr->AppendL( tmpValue->Value() );
                }

            CleanupStack::PopAndDestroy( tmpValue );
            }

        value = new (ELeave) CParserPropertyValueCDesCArray( desarr );
        CleanupStack::Pop( desarr );
        }

    __ASSERT_DEBUG( aValues.Count() == 0,
        VPbkVCardEngError::Panic(
            VPbkVCardEngError::EInvalidLengthOfPropertyArray ) );

    return value;
    }

CParserPropertyValue* CVPbkVCardPropertyExporter::GetExportTextValueL(
    const MVPbkBaseContactField& aField )
    {
    TPtrC text( MVPbkContactFieldTextData::Cast( aField.FieldData() ).Text() );
    if( !ContainsValidTextualDataL( text ) )
        {
        return NULL;
        }

    CParserPropertyValueHBufC* value = CParserPropertyValueHBufC::NewL( text );

    return value;
    }
    
 CParserPropertyValue* CVPbkVCardPropertyExporter::GetExportUriValueL(
    const MVPbkBaseContactField& aField )
    {
    TPtrC text( MVPbkContactFieldUriData::Cast( aField.FieldData() ).Uri() );
    if( !ContainsValidTextualDataL( text ) )
        {
        return NULL;
        }

    CParserPropertyValueHBufC* value = CParserPropertyValueHBufC::NewL( text );

    return value;
    }
    

CParserPropertyValue* CVPbkVCardPropertyExporter::GetExportDateValueL(
    const MVPbkBaseContactField& aField)
    {
    TTime time =
        MVPbkContactFieldDateTimeData::Cast( aField.FieldData() ).DateTime();
    TVersitDateTime* vdt =
        new (ELeave) TVersitDateTime( time.DateTime(),
                                      TVersitDateTime::EIsMachineLocal );
    CleanupStack::PushL( vdt );
    CParserPropertyValueDate* value =
        new (ELeave) CParserPropertyValueDate( vdt );
    CleanupStack::Pop( vdt );
    return value;
    }

CParserPropertyValue*
CVPbkVCardPropertyExporter::GetExportBinaryValueL(
    const MVPbkBaseContactField& aField,
    CVPbkVCardParserParamArray& aParameters )
    {
    const TDesC8& data =
        MVPbkContactFieldBinaryData::Cast( aField.FieldData() ).BinaryData();

    CParserParam* param = CParserParam::NewL( KPropEncoding, KPropBase64 );
    CleanupStack::PushL( param );
    aParameters.AppendL( param );
    CleanupStack::Pop( param );

    // format is always jpeg when retrieved from db
    param = CParserParam::NewL( KPropType, KFormatJPEG );
    CleanupStack::PushL( param );
    aParameters.AppendL( param );
    CleanupStack::Pop( param );

    CParserPropertyValueBinary* value = CParserPropertyValueBinary::NewL( data );
    return value;
    }

TBool CVPbkVCardPropertyExporter::ContainsValidTextualDataL(
    const TDesC& aDataText) const
    {
    if ( !aDataText.Length() )
        {
        return EFalse;
        }

    HBufC* tmpText = aDataText.AllocLC();
    TPtr tmpPtr = tmpText->Des();
    tmpPtr.Trim();

    if ( tmpPtr.Length() == 0 )
        {
        CleanupStack::PopAndDestroy(); // tmpText
        return EFalse;
        }

    CleanupStack::PopAndDestroy(); // tmpText
    return ETrue;
    }

void CVPbkVCardPropertyExporter::GetAdditionalParamsL(
    MVPbkStoreContactField& aField, CVPbkVCardParserParamArray& aParams )
    {
    if ( aParams.Count() > 0 ) // Fetch params only once.
        {
        return;
        }

    const MVPbkFieldType* fieldType = NULL;
    const TInt maxMatchPriority = iData.SupportedFieldTypes().MaxMatchPriority();
    for (TInt matchPriority = 0;
         matchPriority <= maxMatchPriority && !fieldType;
         ++matchPriority)
        {
        fieldType = aField.MatchFieldType( matchPriority );
        }
    if ( !fieldType )
        {
        return;
        }

    TBool match( EFalse );
    TVPbkFieldTypeParameters params;
    TArray<TVPbkFieldVersitProperty> array = fieldType->VersitProperties();
    for ( TInt n = 0; n < array.Count() && !match; ++n )
        {
        TVPbkFieldVersitProperty vProperty = array[ n ];
        for ( TInt priority = 0;
            priority <= maxMatchPriority && !match;
            ++priority )
            {
            match = fieldType->Matches( vProperty, priority );
            if ( match )
                {
                params.AddAll( vProperty.Parameters() );
                }
            }
        }
    
    TInt count = iData.ParamIdNameMappings().Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( params.Contains(
                static_cast<TVPbkFieldTypeParameter>(
                    iData.ParamIdNameMappings()[ i ]->Id() ) ) )
            {
            CParserParam* param =
                CParserParam::NewL( iData.ParamIdNameMappings()[ i ]->Name(),
                                    KNullDesC );
            CleanupStack::PushL( param );
            aParams.AppendL( param );
            CleanupStack::Pop( param );
            }
        }

    // Set PREF param if field has default attribute set
    if ( iData.AttributeManagerL().HasFieldAttributeL(
                                CVPbkDefaultAttribute::Uid(), aField) )
        {
        CParserParam* param = CParserParam::NewL( KPropPREF, KNullDesC );
        CleanupStack::PushL( param );
        aParams.AppendL( param );
        CleanupStack::Pop( param );
        }
    }

void CVPbkVCardPropertyExporter::InitDataL()
    {
    TInt indexedPropertyCount( iVCardProperty->Count() );

    iValues.ResetAndDestroy();
    // Initialize array with NULL values
    while ( iValues.Count() < indexedPropertyCount )
        {
        iValues.AppendL( NULL );
        }
    }

TInt CVPbkVCardPropertyExporter::Next(TInt& aRemainingSteps)
    {
    __ASSERT_DEBUG( iState == EExporting,
        VPbkVCardEngError::Panic( VPbkVCardEngError::EInvalidStateOfExporter ) );
    TInt err( KErrNone );
    TRAP( err, DoNextL() );

    ++iCurrentPropertyIndex;
    aRemainingSteps = (iCurrentPropertyIndex < iMaxPropertyCount ) ?
        ( iMaxPropertyCount - iCurrentPropertyIndex ) :
        0;
    
 	if(!iBeamed && err == KErrNone && !aRemainingSteps )
	    {
	    TRAP( err, HandleOwnCardL() );
            if(err == KErrNone ) 
                TRAP( err, HandleGroupCardL() );
            
            // Parse the REV (Revision) field if any
            if(err == KErrNone ) 
            	TRAP( err, AddRevL() ); 
	    }
    return err;
    }

// ----------------------------------------------------------------------------
// CVPbkVCardPropertyExporter::HandleGroupCardL
// ----------------------------------------------------------------------------
void CVPbkVCardPropertyExporter::HandleGroupCardL()
    {
    CParserProperty* property = iGroupcardHandler->CreateXGroupPropertyL(iContact);
    if (property)
        {
        CleanupStack::PushL( property );
        iParser->AddPropertyL( property );  
        CleanupStack::Pop( property );
        }
    }

void CVPbkVCardPropertyExporter::HandleOwnCardL()
	{
	iOwncardHandler->DoInitL(iContact->ParentStore());
    if(iOwncardHandler->IsOwnContactL(iContact))
    	{
    	CParserProperty* property =iOwncardHandler->CreateXSelfPropertyL();
    	CleanupStack::PushL( property );
	   	iParser->AddPropertyL( property );	
	   	CleanupStack::Pop( property );
    	}
	}
void CVPbkVCardPropertyExporter::DoNextL()
    {
    ExportFieldL( *iData.Properties()[ iCurrentPropertyIndex ] );
    }

void CVPbkVCardPropertyExporter::CloseL()
    {
    iParser->ExternalizeL( iDestinationStream );
    iObserver.VPbkSingleContactOperationComplete( *this, NULL );
    }

void CVPbkVCardPropertyExporter::HandleLeave( TInt aError )
    {
    iObserver.VPbkSingleContactOperationFailed( *this, aError );
    }

void CVPbkVCardPropertyExporter::HandleCancel()
    {
    iObserver.VPbkSingleContactOperationFailed( *this, KErrCancel );
    }

void CVPbkVCardPropertyExporter::InitLinkL(
    const MVPbkContactLink& aContactLink,
    CVPbkContactManager& aContactManager )
    {
    iOperation =
        aContactManager.RetrieveContactL( aContactLink, *this );
    }

void CVPbkVCardPropertyExporter::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& VPBK_DEBUG_ONLY( aOperation ),
        MVPbkStoreContact* aContact)
    {
    __ASSERT_DEBUG( iOperation == &aOperation,
        VPbkVCardEngError::Panic( VPbkVCardEngError::EStrayOperation ) );
    iOwnedContact = aContact;
    iContact = iOwnedContact;
    iState = EExporting;
    Complete( KErrNone );
    }

void CVPbkVCardPropertyExporter::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& VPBK_DEBUG_ONLY( aOperation ), TInt aError )
    {
    __ASSERT_DEBUG( iOperation == &aOperation,
        VPbkVCardEngError::Panic( VPbkVCardEngError::EStrayOperation ) );
    Complete( aError );
    }

void CVPbkVCardPropertyExporter::StartL()
    {
    if ( iState == ECreatingRetriever )
        {
        StartWaiting();
        }
    else
        {
        Start();
        }
    }

void CVPbkVCardPropertyExporter::AddRevL()
    {
    MVPbkStoreContact* tempContact = const_cast<MVPbkStoreContact*>(iContact);
    if( tempContact )
        {
        MVPbkStoreContact2* tempContact2 = 	reinterpret_cast<MVPbkStoreContact2*>
            (tempContact->StoreContactExtension (KMVPbkStoreContactExtension2Uid) );	    
        if(tempContact2)
            {
            MVPbkStoreContactProperties* propreties = tempContact2->PropertiesL();
            if( propreties )
                {
                CleanupDeletePushL(propreties);

                // Convert the last_modified field defined in Contact Model database 
                // to REV (Revision) field in vCard Specification 2.1
                CParserPropertyValueDateTime* valueDateTime =
                        CreateDateTimePropertyL(
                            propreties->LastModifiedL(),
                            TVersitDateTime::EIsUTC );
                CleanupStack::PopAndDestroy(propreties);
                
                CleanupStack::PushL(valueDateTime);
                CParserProperty* property =
                        CParserGroupedProperty::NewL(*valueDateTime, KREV, NULL, NULL);
                
                CleanupStack::PushL( property );
                iParser->AddPropertyL(property); //takes ownership
                CleanupStack::Pop(2);//valueDateTime and property
                }
            }
        }
    }	
	
CParserPropertyValueDateTime* CVPbkVCardPropertyExporter::CreateDateTimePropertyL
    (const TTime& aDateTime, TVersitDateTime::TRelativeTime aRelativeTime)
    {
    TVersitDateTime* dateTime= new(ELeave)TVersitDateTime(aDateTime.DateTime(), aRelativeTime);
    CleanupStack::PushL(dateTime);
    CParserPropertyValueDateTime* dateTimeValue=new(ELeave) CParserPropertyValueDateTime(dateTime);
    CleanupStack::Pop(dateTime);	
    return dateTimeValue;
    }

// End of file
