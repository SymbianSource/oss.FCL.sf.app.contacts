/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Client side class to hold the result of predictive search.
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include <collate.h>

#include "CPsClientData.h"
#include "CPcsDefs.h"

// ============================== MEMBER FUNCTIONS ============================


// ----------------------------------------------------------------------------
// CPsClientData::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
EXPORT_C CPsClientData* CPsClientData::NewL()
{
	CPsClientData* self = new (ELeave) CPsClientData();	
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
	
}

// ----------------------------------------------------------------------------
// CPsClientData::CPsClientData
// Default constructor
// ----------------------------------------------------------------------------
CPsClientData::CPsClientData()
{
}

// ----------------------------------------------------------------------------
// CPsClientData::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CPsClientData::ConstructL()
{
    iData = new (ELeave) RPointerArray<HBufC>(1);
    iDataExtension = NULL;
    iMarked = EFalse;
}

// ----------------------------------------------------------------------------
// CPsClientData::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CPsClientData::ConstructL(const CPsClientData& aRhs)
{
	iId = aRhs.Id();
	iUri = aRhs.Uri()->AllocL();
    iData = new (ELeave) RPointerArray<HBufC>(1);
    for(TInt i=0; i < aRhs.DataElementCount(); i++ )
    {
    	SetDataL(i,*(aRhs.Data(i)));
    	
    	     
    }
    
    iFiledMatches = aRhs.FieldMatch();
    if(aRhs.DataExtension())
        {
        iDataExtension = ((HBufC8*)(aRhs.DataExtension()))->AllocL();	
        }
    
    if(aRhs.IsMarked())
    	iMarked = ETrue;
    else
    	iMarked = EFalse;
}

// ----------------------------------------------------------------------------
// CPsClientData::~CPsClientData
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CPsClientData::~CPsClientData()
{
	if ( iData )
	{
		iData->ResetAndDestroy();
		delete iData;
	}
	if(iDataExtension)
    {
		delete iDataExtension;
		iDataExtension = NULL;
    }
    if(iUri)
    {
    	delete iUri;
    	iUri = NULL;
    }
}

// ----------------------------------------------------------------------------
// CPsClientData::CPsClientData
// Returns the clone of this object
// ----------------------------------------------------------------------------
EXPORT_C CPsClientData* CPsClientData::CloneL() const
{
	CPsClientData* clone = new (ELeave) CPsClientData();	
    CleanupStack::PushL(clone);
    clone->ConstructL(*this);
    
    CleanupStack::Pop();
    return clone;

}
// ----------------------------------------------------------------------------
// CPsClientData::Id
// Returns the unique id
// ----------------------------------------------------------------------------
EXPORT_C  TInt CPsClientData::Id() const
{
    return iId;
}

// ----------------------------------------------------------------------------
// CPsClientData::SetId
// Sets the unique id
// ----------------------------------------------------------------------------
EXPORT_C void CPsClientData::SetId (const TInt aId)
{
	iId = aId;
}

// ----------------------------------------------------------------------------
// CPsClientData::Uri
// Returns the unique URI
// ----------------------------------------------------------------------------
EXPORT_C  HBufC* CPsClientData::Uri() const
{
    return iUri;
}

// ----------------------------------------------------------------------------
// CPsClientData::SetUriL
// Sets the unique URI
// ----------------------------------------------------------------------------
EXPORT_C void CPsClientData::SetUriL(const TDesC& aUri)
{
	iUri = aUri.AllocL();
}

// ----------------------------------------------------------------------------
// CPsClientData::Data
// Returns the data
// ----------------------------------------------------------------------------
EXPORT_C HBufC* CPsClientData::Data(TInt aIndex) const
{
    if ( aIndex >= iData->Count() )    
         return NULL;
    
    return (*iData)[aIndex];
}

// ----------------------------------------------------------------------------
// CPsClientData::SetDataL
// Sets the data
// ----------------------------------------------------------------------------
EXPORT_C void CPsClientData::SetDataL(TInt /*aIndex*/, const TDesC& aData)
{   
    iData->AppendL(aData.AllocL());
}

// ----------------------------------------------------------------------------
// CPsClientData::DataElementCount
// 
// ----------------------------------------------------------------------------
EXPORT_C TInt CPsClientData::DataElementCount() const
{
	return (iData->Count());
}


// ----------------------------------------------------------------------------
// CPsClientData::ExternalizeL
// Writes 'this' to aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsClientData::ExternalizeL(RWriteStream& aStream) const
{
    aStream.WriteInt16L(iId); // Write unique ID to the stream
    
    aStream.WriteUint8L(iFiledMatches); // Field Matches
    
    aStream.WriteUint8L(iUri->Size());  // write URI length to stream
    aStream<<*iUri;                      // write URI to stream
    
    aStream.WriteUint8L(iData->Count()); // Number of data elements

	for ( int index = 0; index < iData->Count(); index++ )
	{
	    HBufC* data = (*iData)[index];
	    if ( data ) // Write data to the stream (or a NULL descriptor)
	    {
	      	aStream.WriteInt16L( data->Size() ); // data size
			aStream << *data; // data
	    }
	    else
	    {
	    	aStream.WriteInt16L( 0 );
	    	aStream << KNullDesC;
	    }
	}
	
	if ( iDataExtension )
    {
        HBufC8* dataExt = (HBufC8*)iDataExtension;
    	aStream.WriteUint8L( dataExt->Size() );
    	aStream << *dataExt;
    }
    else 
    {
    	aStream.WriteUint8L( 0 );
    	aStream << KNullDesC8;	    	
    }
}

// ----------------------------------------------------------------------------
// CPsClientData::InternalizeL
// Initializes 'this' with the contents of aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsClientData::InternalizeL(RReadStream& aStream)
{
    // Read unique id
    TInt tmp = aStream.ReadInt16L();
    iId = tmp;
    
    // Read Field Match
	iFiledMatches  = aStream.ReadUint8L();
	
    // Read the URI length
    TUint8 len = aStream.ReadUint8L();
    // read uri
    iUri = HBufC::NewL(aStream, len);
    
    // Read number of data elements
    TInt szNumElements = aStream.ReadUint8L();
    
    for ( int index = 0; index < szNumElements; index++ )
    {
        // Read data size
	    TInt szData = aStream.ReadInt16L();
    	    
    	HBufC* data =  HBufC::NewL(aStream, szData);
	    iData->InsertL(data, index);
	    
    }
	
	// Read data extension size
	TUint8 szDataExt = aStream.ReadUint8L();
    if(iDataExtension)
    {
         delete iDataExtension;
        iDataExtension = NULL;
    }
    
    HBufC8* dataExt =  HBufC8::NewL(aStream, szDataExt);
    iDataExtension = dataExt;
}

// ----------------------------------------------------------------------------
// CPsClientData::DataExtension
// 
// ----------------------------------------------------------------------------
EXPORT_C TAny* CPsClientData::DataExtension() const
{
	return iDataExtension;
}

// ----------------------------------------------------------------------------
// CPsClientData::SetDataExtensionL
// 
// ----------------------------------------------------------------------------
EXPORT_C void CPsClientData::SetDataExtensionL(TAny* aDataExt)
{
	if(aDataExt == NULL)
		return;
	HBufC8* dataExt = (HBufC8*)aDataExt;
	iDataExtension = dataExt->AllocL();
}

// ----------------------------------------------------------------------------
// CPsClientData::SetBookMark
// 
// ----------------------------------------------------------------------------
EXPORT_C void CPsClientData::SetMark()
{
	iMarked = ETrue;
}

// ----------------------------------------------------------------------------
// CPsClientData::SetBookMark
// 
// ----------------------------------------------------------------------------
EXPORT_C TBool CPsClientData::IsMarked() const
{
	return iMarked;
}

// ----------------------------------------------------------------------------
// CPsClientData::SetFieldMatch
// 
// ----------------------------------------------------------------------------
EXPORT_C void CPsClientData::SetFieldMatch(TUint8 aFieldMatch)
{	
	iFiledMatches = aFieldMatch;
}
// ----------------------------------------------------------------------------
// CPsClientData:FieldMatch
// Return the Field match attribute
// ----------------------------------------------------------------------------		
EXPORT_C TUint8 CPsClientData::FieldMatch() const
{
    return iFiledMatches;
}

