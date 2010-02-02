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
* Description:  Utility class to hold data for predictive search.
*                Used to marshal data between the client, server and data
*                plugins.
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include <collate.h>

#include "CPsData.h"
#include "CPcsDefs.h"
// ============================== MEMBER FUNCTIONS ============================


// ----------------------------------------------------------------------------
// CPsData::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
EXPORT_C CPsData* CPsData::NewL()
{
	CPsData* self = new (ELeave) CPsData();	
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
	
}

// ----------------------------------------------------------------------------
// CPsData::CPsData
// Default constructor
// ----------------------------------------------------------------------------
CPsData::CPsData()
{
}

// ----------------------------------------------------------------------------
// CPsData::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CPsData::ConstructL()
{
    iData = new (ELeave) RPointerArray<HBufC>(1);
    iGc = new (ELeave) RPointerArray<HBufC>(1);
    iDataExtension = NULL;
}

// ----------------------------------------------------------------------------
// CPsData::~CPsData
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CPsData::~CPsData()
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
    if(iIntDataExt && iIntDataExt->Count())
    {
    	
    	iIntDataExt->Close();
    	delete iIntDataExt;
    }
    if ( iGc )
    {
    	iGc->ResetAndDestroy();
    	delete iGc;
    }
}

// ----------------------------------------------------------------------------
// CPsData::Id
// Returns the unique id
// ----------------------------------------------------------------------------
EXPORT_C  TInt CPsData::Id() const
{
    return iId;
}

// ----------------------------------------------------------------------------
// CPsData::SetId
// Sets the unique id
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::SetId (const TInt aId)
{
	iId = aId;
}

// ----------------------------------------------------------------------------
// CPsData::UriId
// Returns the unique URI id
// ----------------------------------------------------------------------------
EXPORT_C  TUint8 CPsData::UriId() const
{
    return iUriId;
}

// ----------------------------------------------------------------------------
// CPsData::SetUriId
// Sets the unique URI id
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::SetUriId(const TUint8 aUriId)
{
	iUriId = aUriId;
}

// ----------------------------------------------------------------------------
// CPsData::Data1
// Returns the data1
// ----------------------------------------------------------------------------
EXPORT_C HBufC* CPsData::Data(TInt aIndex) const
{
    if ( aIndex >= iData->Count() )    
         return NULL;
    
    return (*iData)[aIndex];
}

// ----------------------------------------------------------------------------
// CPsData::SetData1L
// Sets the data1
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::SetDataL(TInt aIndex, const TDesC& aData)
{   
    // Every call to insert makes the array to grow.
    // This check is required to optimize memory.
    if ( aIndex < iData->Count() )
    {
       //delete (*iData)[aIndex];     // Commented as it is delaying the contact caching as well as search time     
       iGc->Append((*iData)[aIndex]); // Instead append to garbage collector for cleanup later
       (*iData)[aIndex] = aData.AllocL();
    }
    else 
    {  
       iData->InsertL( aData.AllocL(), aIndex);
    }
}

// ----------------------------------------------------------------------------
// CPsData::ExternalizeL
// Writes 'this' to aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::ExternalizeL(RWriteStream& aStream) const
{
    aStream.WriteInt16L(iId); // Write unique ID to the stream
    
    aStream.WriteUint8L(iUriId); // Write URI ID to the stream
 
    aStream.WriteUint8L(iDataMatches); // Write data matches attribute 
    
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
    
    TInt intDataExtCount = 0;
    if (iIntDataExt)
    {
        intDataExtCount = iIntDataExt->Count();
    }
    if(intDataExtCount)
    {
    	aStream.WriteInt32L(intDataExtCount);
    	for(TInt i =0 ; i < intDataExtCount;  i++ )
	  	    aStream.WriteInt32L((*iIntDataExt)[i]);
    }
    else
    {
    	aStream.WriteInt32L(0);
    	
    }
}

// ----------------------------------------------------------------------------
// CPsData::InternalizeL
// Initializes 'this' with the contents of aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::InternalizeL(RReadStream& aStream)
{
    // Read unique id
    TInt tmp = aStream.ReadInt16L();
    iId = tmp;
    
    // Read the URI Id
    iUriId = aStream.ReadUint8L();    
    
    // Read data matches
    iDataMatches = aStream.ReadUint8L();

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
    
    TInt intDataExtCount = aStream.ReadInt32L();
    if(intDataExtCount)
    {
        if(!iIntDataExt)
		{
		 iIntDataExt = new (ELeave) RArray<TInt> (1);	
		}
	 
    	for(TInt i =0 ; i < intDataExtCount;  i++ )
    	{
    	    TInt tempDataExt = aStream.ReadInt32L();
    	    iIntDataExt->Append(tempDataExt);
    	}
    }
    else
    {
    	// Do Nothing since there are no elements
    }
}

// ----------------------------------------------------------------------------
// CPsData::CompareByData
// Compare function
// ----------------------------------------------------------------------------
EXPORT_C TInt CPsData::CompareByData( const CPsData& aObject1, 
                                      const CPsData& aObject2 )
{
	// Get the standard method
    TCollationMethod meth = *Mem::CollationMethodByIndex( 0 );
    meth.iFlags |= TCollationMethod::EIgnoreNone;
    meth.iFlags |= TCollationMethod::EFoldCase;

    HBufC* str1;
    HBufC* str2 ;

    TInt compareRes(0);
    for ( int cnt1 = 0, cnt2 = 0;
          (cnt1 < aObject1.DataElementCount()) && (cnt2 < aObject2.DataElementCount());
          cnt1++, cnt2++ )
        {
        if ( aObject1.Data(cnt1) )
            {
            //data1 += aObject1.Data(i)->Des();
            str1 =  aObject1.Data(cnt1)->Des().AllocL();    
            str1->Des().TrimAll();
            }	
        else
            {
            str1 = HBufC::NewL(2);
            str1->Des().Copy(KNullDesC);
            }
        CleanupStack::PushL( str1 );
        if ( aObject2.Data(cnt2) )
            {
            str2 = aObject2.Data(cnt2)->Des().AllocL();
            str2->Des().TrimAll();
            }	     
        else
            {
            str2 = HBufC::NewL(2);
            str2->Des().Copy(KNullDesC);
            }
        CleanupStack::PushL( str2 );
        compareRes = str1->Des().CompareC( str2->Des(), 3, &meth );
        CleanupStack::PopAndDestroy(str2);
        CleanupStack::PopAndDestroy(str1);
        if( !compareRes )
            {
            break;
			}
        }
    return compareRes;
}

// ----------------------------------------------------------------------------
// CPsData::CompareById
// Compare function
// ----------------------------------------------------------------------------
EXPORT_C TBool CPsData::CompareById(const CPsData& aObject1, const CPsData& aObject2)
{
	if(aObject1.Id() == aObject2.Id())
	{
		return ETrue;
	}
	
	return EFalse;
}
				     

// ----------------------------------------------------------------------------
// CPsData::IsDataMatch
// Return TRUE if data is matched for predictive search
// ----------------------------------------------------------------------------		
EXPORT_C TBool CPsData::IsDataMatch(TInt aIndex)
{	
    TReal val;
	Math::Pow(val, 2, aIndex);

	return(iDataMatches & (TUint8)val);
}

// ----------------------------------------------------------------------------
// CPsData:DataMatch
// Return the data match attribute
// ----------------------------------------------------------------------------		
EXPORT_C TUint8 CPsData::DataMatch()
{
    return iDataMatches;
}

// ----------------------------------------------------------------------------
// CPsData::SetData1Match
// 
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::SetDataMatch(TInt aIndex)
{	
    TReal val;
	Math::Pow(val, 2, aIndex);

	iDataMatches |= (TUint8)val;
}

// ----------------------------------------------------------------------------
// CPsData::ClearDataMatches
// 
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::ClearDataMatches()
{
	iDataMatches = 0x0;
}

// ----------------------------------------------------------------------------
// CPsData::DataElementCount
// 
// ----------------------------------------------------------------------------
EXPORT_C TInt CPsData::DataElementCount() const
{
	return (iData->Count());
}


// ----------------------------------------------------------------------------
// CPsData::DataExtension
// 
// ----------------------------------------------------------------------------
EXPORT_C TAny* CPsData::DataExtension() const
{
	return iDataExtension;
}

// ----------------------------------------------------------------------------
// CPsData::SetDataExtension
// 
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::SetDataExtension(TAny* aDataExt)
{
	iDataExtension = aDataExt;
}

// ----------------------------------------------------------------------------
// CPsData::AddIntDataExtL
// 
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::AddIntDataExtL(TInt aDataExt)
{	if(!iIntDataExt)
	{
		 iIntDataExt = new (ELeave) RArray<TInt> (1);	
	}
	
	iIntDataExt->Append(aDataExt);
}

// ----------------------------------------------------------------------------
// CPsData::RemoveIntDataExt
// 
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::RemoveIntDataExt(TInt aIndex)
{
	if(iIntDataExt && (iIntDataExt->Count() > aIndex))
	{
		iIntDataExt->Remove(aIndex);
	}
}

// ----------------------------------------------------------------------------
// CPsData::IntDataExt
// 
// ----------------------------------------------------------------------------
EXPORT_C void CPsData::IntDataExt(RArray<TInt>& aDataExtArray)
{   
	aDataExtArray.Reset();
	if(iIntDataExt)
	{
		for(TInt i =0 ; i < iIntDataExt->Count(); i++ )
		  	aDataExtArray.Insert((*iIntDataExt)[i],0);
	}
}
