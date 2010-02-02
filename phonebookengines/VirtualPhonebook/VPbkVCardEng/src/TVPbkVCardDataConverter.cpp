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
* Description:  Converting data according data's vcard uid.
*
*/

#include "TVPbkVCardDataConverter.h"
#include "CVPbkVCardContactFieldData.h"

#include <vprop.h>  // CParserPropertyValue

TVPbkVCardDataConverter::TVPbkVCardDataConverter()
    {    
    }

CVPbkVCardContactFieldData* TVPbkVCardDataConverter::CreateDataLC
        ( CParserProperty& aProperty, 
          RPointerArray<const MVPbkFieldType>& aTypes,
          MVPbkContactAttribute* aAttribute,
          TInt aCursor )
    {
    CParserPropertyValue* value = aProperty.Value();
    CVPbkVCardContactFieldData* data = NULL;
    if ( value->Uid() == TUid::Uid( KVersitPropertyHBufCUid ) )   
        {
        CParserPropertyValueHBufC* hbufVal = 
	        static_cast<CParserPropertyValueHBufC*>( value );
        const TDesC& desValue = hbufVal->Value(); 
        data = 
            CVPbkVCardContactFieldDesCData::NewLC( *aTypes[aCursor], desValue );        
        }
    else if ( value->Uid() == TUid::Uid( KVersitPropertyCDesCArrayUid ) )
        {
        CParserPropertyValue* value = aProperty.Value();
        CParserPropertyValueCDesCArray* arrayProperty = 
    	        static_cast<CParserPropertyValueCDesCArray*>( value );
        CDesCArray* values = arrayProperty->Value(); 
        const TDesC& desValue = values->MdcaPoint( aCursor );
        data = 
            CVPbkVCardContactFieldDesCData::NewLC( *aTypes[aCursor], desValue );        
        }
    else if ( value->Uid() == TUid::Uid( KVersitPropertyBinaryUid ) )
        {
        CParserPropertyValueBinary* val = 
		    static_cast<CParserPropertyValueBinary*>( value );
		HBufC8* buffer = GetHBufCOfCBufSegL( const_cast< CBufSeg* >( val->Value() ) );
        CleanupStack::PushL( buffer );
        data = 
            CVPbkVCardContactFieldBinaryData::NewLC( *aTypes[aCursor], *buffer );
        CleanupStack::Pop( data );    
        CleanupStack::PopAndDestroy( buffer );
        CleanupStack::PushL( data );
        }
    else if ( value->Uid() == TUid::Uid( KVersitPropertyDateUid ) )
        {
        CParserPropertyValueDate* dval = 
    		static_cast<CParserPropertyValueDate*>( value );
	    TVersitDateTime* dateval = dval->Value();
	    data = 
	        CVPbkVCardContactFieldDateTimeData::NewLC( *aTypes[aCursor], *dateval );
        }   
        
    if ( data )
        {
        data->AppendAttribute( aAttribute );
        }
    return data; 
    }
    
const TDesC& TVPbkVCardDataConverter::GetDesCData( CVPbkVCardContactFieldData& aData )
    {    
    const TDesC& value = 
	        ( static_cast<CVPbkVCardContactFieldDesCData*>( &aData ) )->Value();
    return value;
    }
    
const TDesC8& TVPbkVCardDataConverter::GetBinaryData( CVPbkVCardContactFieldData& aData )
    {
    const TDesC8& binaryValue = 
	    ( static_cast<CVPbkVCardContactFieldBinaryData*>( &aData ) )->Value();
	    
    return binaryValue;
    }

const TVersitDateTime* TVPbkVCardDataConverter::GetDateTimeData( 
        CVPbkVCardContactFieldData& aData )
    {
    const TVersitDateTime* dval = 
		&( static_cast<CVPbkVCardContactFieldDateTimeData*>( &aData ) )->Value();
    return dval;
    }
        
HBufC8* TVPbkVCardDataConverter::GetHBufCOfCBufSegL( CBufSeg* aBufSeg ) const
    {
    TInt size = aBufSeg->Size();
    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 bufferPtr = buffer->Des();
    
    TInt i = 0;
    TPtrC8 tmpPtr = aBufSeg->Ptr( i );
    while ( tmpPtr.Length() > 0 )
        {
        bufferPtr.Append( tmpPtr );
        i += tmpPtr.Length();
        tmpPtr.Set( aBufSeg->Ptr(i) );
        }
    
    CleanupStack::Pop(); // buffer
    return buffer;
    }
    
// End of file
