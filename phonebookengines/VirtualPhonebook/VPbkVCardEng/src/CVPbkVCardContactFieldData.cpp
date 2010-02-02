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
* Description:  Classes for storing contact field data of different types.
*
*/

#include "CVPbkVCardContactFieldData.h"

#include <vuid.h>   // Versit uids

CVPbkVCardContactFieldData::CVPbkVCardContactFieldData( const MVPbkFieldType& aFieldType )
: iFieldType( aFieldType )
    {    
    }
 
CVPbkVCardContactFieldData::~CVPbkVCardContactFieldData()
    {    
    }
        
MVPbkContactAttribute* CVPbkVCardContactFieldData::Attribute()    
    {
    return iAttribute;
    }
    
void CVPbkVCardContactFieldData::AppendAttribute( MVPbkContactAttribute* aAttribute )    
    {
    iAttribute = aAttribute;
    }

const MVPbkFieldType& CVPbkVCardContactFieldData::FieldType()
    {
    return iFieldType;
    }

/////////////////////////////////////////////////////////////////////    
CVPbkVCardContactFieldDesCData* CVPbkVCardContactFieldDesCData::NewLC 
    ( const MVPbkFieldType& aFieldType, const TDesC& aValue )
    {
    CVPbkVCardContactFieldDesCData* self = new (ELeave) 
        CVPbkVCardContactFieldDesCData( aFieldType );
    CleanupStack::PushL( self );
    self->ConstructL( aValue );
    return self;
    }
        
CVPbkVCardContactFieldDesCData::CVPbkVCardContactFieldDesCData    
    ( const MVPbkFieldType& aFieldType )
: CVPbkVCardContactFieldData( aFieldType ) 
    {    
    }

void CVPbkVCardContactFieldDesCData::ConstructL( const TDesC& aValue )
    {
    iValue = aValue.AllocL();
    }
    
CVPbkVCardContactFieldDesCData::~CVPbkVCardContactFieldDesCData()
    {    
    delete iValue;
    }
        
const TDesC& CVPbkVCardContactFieldDesCData::Value() const
    {
    return *iValue;
    }
    
TUid CVPbkVCardContactFieldDesCData::Uid()
    {
    return TUid::Uid( KVersitPropertyHBufCUid );
    }
   
/////////////////////////////////////////////////////////////////////    
CVPbkVCardContactFieldBinaryData* CVPbkVCardContactFieldBinaryData::NewLC 
    ( const MVPbkFieldType& aFieldType, const TDesC8& aValue )
    {
    CVPbkVCardContactFieldBinaryData* self = new (ELeave) 
        CVPbkVCardContactFieldBinaryData( aFieldType );
    CleanupStack::PushL( self );
    self->ConstructL( aValue );
    return self;
    }
    
CVPbkVCardContactFieldBinaryData::CVPbkVCardContactFieldBinaryData    
    ( const MVPbkFieldType& aFieldType )
: CVPbkVCardContactFieldData( aFieldType )
    {    
    }
    
void CVPbkVCardContactFieldBinaryData::ConstructL( const TDesC8& aValue )
    {
    iValue = aValue.AllocL();
    }

CVPbkVCardContactFieldBinaryData::~CVPbkVCardContactFieldBinaryData()
    {    
    delete iValue;
    }
    
const TDesC8& CVPbkVCardContactFieldBinaryData::Value() const
    {
    return *iValue;
    }
    
TUid CVPbkVCardContactFieldBinaryData::Uid()
    {
    return TUid::Uid( KVersitPropertyBinaryUid );
    }
    
/////////////////////////////////////////////////////////////////////        
CVPbkVCardContactFieldDateTimeData* CVPbkVCardContactFieldDateTimeData::NewLC 
    ( const MVPbkFieldType& aFieldType, const TVersitDateTime& aValue )
    {
    CVPbkVCardContactFieldDateTimeData* self = new (ELeave) 
        CVPbkVCardContactFieldDateTimeData( aFieldType, aValue );
    CleanupStack::PushL( self );
    return self;
    }
    
CVPbkVCardContactFieldDateTimeData::CVPbkVCardContactFieldDateTimeData    
    ( const MVPbkFieldType& aFieldType, const TVersitDateTime& aValue )
: CVPbkVCardContactFieldData( aFieldType ), iValue( aValue )
    {    
    }
    
CVPbkVCardContactFieldDateTimeData::~CVPbkVCardContactFieldDateTimeData()
    {    
    }    
    
const TVersitDateTime& CVPbkVCardContactFieldDateTimeData::Value() const
    {
    return iValue;
    }
    
TUid CVPbkVCardContactFieldDateTimeData::Uid()
    {
    return TUid::Uid( KVersitPropertyDateUid );
    }
    
// End of file
