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
* Description:  Parsing one CParserProperty into the TVPbkFieldVersitProperty
*
*/


// Internal includes
#include "CVPbkVCardPropertyParser.h"
#include "CVPbkVCardContactFieldIterator.h"
#include "TVPbkMappingsFindTool.h"
#include "CVPbkVCardData.h"
#include "CVPbkVCardFieldTypeProperty.h"
#include "CVPbkVCardAttributeHandler.h"
#include "VPbkVCardEngError.h"
#include "CVPbkVCardParser.h"

// System includes
#include <vcard.h>

// Local definitions
namespace 
    {
    const TInt KAtFirstIndex ( 0 );
    } // unnamed namespace

CVPbkVCardPropertyParser* CVPbkVCardPropertyParser::NewL(
    CVPbkVCardData& aData, CVPbkVCardAttributeHandler& aAttributeHandler)
    {
    CVPbkVCardPropertyParser* self = new (ELeave) 
        CVPbkVCardPropertyParser(aData, aAttributeHandler);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
CVPbkVCardPropertyParser::CVPbkVCardPropertyParser(
    CVPbkVCardData& aData, CVPbkVCardAttributeHandler& aAttributeHandler)
:   iData( aData ), iAttributeHandler( aAttributeHandler )
    {
    }
    
void CVPbkVCardPropertyParser::ConstructL()
    {
    // Do nothing
    }
    
CVPbkVCardPropertyParser::~CVPbkVCardPropertyParser()
    {
    }
    
CVPbkVCardContactFieldIterator* CVPbkVCardPropertyParser::ParseNextL( CParserProperty* aProperty )
    {
    iCurrentProperty = aProperty;
    return ComposeVersitPropertyL();
    }
    
CVPbkVCardContactFieldIterator* CVPbkVCardPropertyParser::ComposeVersitPropertyL()
    {
    CVPbkVCardContactFieldIterator* iterator = 
        CVPbkVCardContactFieldIterator::NewLC( *iCurrentProperty, iData );
    if ( !Ignore( iCurrentProperty ) )
        {
        ResolveNameL( iCurrentProperty, *iterator );
        ResolveParamsL( iCurrentProperty, *iterator );
        ResolveSubfieldsL( iCurrentProperty, *iterator );
        }    
    
    CleanupStack::Pop(); // iterator
    return iterator;
    }
    
TBool CVPbkVCardPropertyParser::Ignore( CParserProperty* aProperty )
    {
    // Check if this property is BEGIN/END/VERSION token
    if ( aProperty->NameUid() == TUid::Uid( KVersitTokenBeginUid ) ||
         aProperty->NameUid() == TUid::Uid( KVersitTokenEndUid ) ||
         aProperty->NameUid() == TUid::Uid( KVersitTokenVersionUid ) )
        {
        return ETrue;
        }

    return EFalse;
    }
    
void CVPbkVCardPropertyParser::ResolveNameL( CParserProperty* aProperty, 
    CVPbkVCardContactFieldIterator& /*aIterator*/ )
    {
    HBufC8* name = ConvertToUpperCaseLC( aProperty->Name() );

    iVersitProperty.SetName( 
        static_cast<TVPbkFieldTypeName>(
            TVPbkMappingsFindTool::FindIdByName( *name, 
                                   iData.VNameIdNameMappings() ) ) );
    // Set extension name if exists
    // All VCard properties
    for ( TInt i = 0; i < iData.Properties().Count(); ++i )
        {
        CVPbkVCardFieldTypeProperty* vcardProperty = iData.Properties()[ i ];
        if ( vcardProperty->Name().Compare( *name ) == 0 )
            {
            const TVPbkVCardIndexedProperty& indexedProperty = 
                vcardProperty->IndexedPropertyAt( KAtFirstIndex );                  
            
            // Add extension name
            if ( indexedProperty.Property().ExtensionName().Length() > 0 )
                {
                iVersitProperty.SetExtensionName
                    ( indexedProperty.Property().ExtensionName() );
                break;
                }
            }
        }    
    CleanupStack::PopAndDestroy( name );
    }
    
void CVPbkVCardPropertyParser::ResolveParamsL( CParserProperty* aProperty,
    CVPbkVCardContactFieldIterator& aIterator )
    {
    TArray< CVPbkVCardIdNameMapping* > paramArray = 
                                            iData.ParamIdNameMappings();
    TVPbkFieldTypeParameters parameters;

    for ( TInt n = 0; n < paramArray.Count(); ++n )
        {
        CParserParam* param = aProperty->Param( paramArray[ n ]->Name() );        
        if ( param )
            {
            HBufC8* name = ConvertToUpperCaseLC( param->Name() );

            TVPbkFieldTypeParameter fieldTypeParam = 
                static_cast<TVPbkFieldTypeParameter>( 
                    TVPbkMappingsFindTool::FindIdByName( *name, paramArray ) );
            if (!iAttributeHandler.CreateAttributeL(fieldTypeParam, 
            		iVersitProperty.Name()))
                {
                // No attribute created from fieldTypeParam,
                // so add to parameters list
                parameters.Add(fieldTypeParam);                  
                }                                
            aIterator.AppendAttribute( iAttributeHandler.Attribute() );
            CleanupStack::PopAndDestroy( name );
            }
        else
            {
            // If EMAIL property not include default parameter then we need to 
            // set it here, because otherwise it not mapped to any field in VPbk
            if ( paramArray[ n ]->Id() == EVPbkVersitParamINTERNET )
                {            
                HBufC8* name = ConvertToUpperCaseLC( paramArray[ n ]->Name() );
                
                TVPbkFieldTypeParameter fieldTypeParam = 
                static_cast<TVPbkFieldTypeParameter>( 
                    TVPbkMappingsFindTool::FindIdByName( *name, paramArray ) );
                if (!iAttributeHandler.CreateAttributeL(fieldTypeParam, 
                		iVersitProperty.Name()))
                    {
                    // No attribute created from fieldTypeParam,
                    // so add to parameters list
                    parameters.Add(fieldTypeParam);                  
                    }                                
                aIterator.AppendAttribute( iAttributeHandler.Attribute() );
                CleanupStack::PopAndDestroy( name );                   
                }
            }
        }

    iVersitProperty.SetParameters( parameters );
    }        
    
void CVPbkVCardPropertyParser::ResolveSubfieldsL( CParserProperty* aProperty,
    CVPbkVCardContactFieldIterator& aIterator )
    {
    CParserPropertyValue* value = aProperty->Value();

    if ( value->Uid() == TUid::Uid( KVersitPropertyHBufCUid ) ||
         value->Uid() == TUid::Uid( KVersitPropertyBinaryUid ) ||
         value->Uid() == TUid::Uid( KVersitPropertyDateUid ) )
        {
        iVersitProperty.SetSubField(
            static_cast<TVPbkSubFieldType>(
                EVPbkVersitSubFieldNone ) );
        aIterator.AppendVersitProperty( iVersitProperty );
        }
    else if ( value->Uid() == TUid::Uid( KVersitPropertyCDesCArrayUid ) )
        {        
        CParserPropertyValue* value = aProperty->Value();
        CParserPropertyValueCDesCArray* arrayProperty = 
    	        static_cast<CParserPropertyValueCDesCArray*>( value );
        CDesCArray* values = arrayProperty->Value(); 
        
        for ( TInt i = 0; i < values->MdcaCount(); ++i )
            {            
            iVersitProperty.SetSubField(
                static_cast<TVPbkSubFieldType>( i ) );
            aIterator.AppendVersitProperty( iVersitProperty );
            }
        }
    }  
    
HBufC8* CVPbkVCardPropertyParser::ConvertToUpperCaseLC( 
    const TDesC8& aOriginalDesC )
    {
    __ASSERT_DEBUG( aOriginalDesC.Length() > 0, 
        VPbkVCardEngError::Panic( VPbkVCardEngError::EInvalidDescriptor ) );

    HBufC8* buffer = HBufC8::NewLC( aOriginalDesC.Length() );
    TPtr8 bufferPtr( buffer->Des() );
    bufferPtr.Copy( aOriginalDesC );
    bufferPtr.UpperCase();
    return buffer;
    } 

// End of file
