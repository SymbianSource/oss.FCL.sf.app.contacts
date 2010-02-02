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
* Description:  Utility class to store settings for predictive search.
*                Used to set the desired data stores to search and 
*                the display fields for predictive search.
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include "CPsSettings.h"

// ============================== MEMBER FUNCTIONS ============================


// ----------------------------------------------------------------------------
// CPsSettings::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
EXPORT_C CPsSettings* CPsSettings::NewL()
{
	CPsSettings* self = new (ELeave) CPsSettings();	
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
	
}

// ----------------------------------------------------------------------------
// CPsSettings::CPsSettings
// Default constructor
// ----------------------------------------------------------------------------
CPsSettings::CPsSettings()
{
}

// ----------------------------------------------------------------------------
// CPsSettings::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CPsSettings::ConstructL()
{
   iMaxResults = -1;
   iSortType = EAlphabetical;
}


// ----------------------------------------------------------------------------
// CPsSettings::CloneL
// Returns the clone.
// ------------------------------------------------------------------------
EXPORT_C CPsSettings*  CPsSettings::CloneL() const
{
    CPsSettings* self = CPsSettings::NewL();	
    CleanupStack::PushL(self);
    // Set the Max reslts
	self->SetMaxResults(iMaxResults);
	self->SetSortType(iSortType);
    
    // Copy the search uris
    RPointerArray <TDesC>  newUris;

    for(TInt i =0; i < iSearchUri.Count() ; i++)
	{
		newUris.Append(iSearchUri[i]->AllocL());
	}
    self->SetSearchUrisL(newUris);
    self->SetSearchUrisL(iSearchUri);
    
    // Set the display fields
    self->SetDisplayFieldsL(iDisplayFields);
     
    newUris.ResetAndDestroy();
    
    CleanupStack::Pop();
    return self;
}

// ----------------------------------------------------------------------------
// CPsSettings::~CPsSettings
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CPsSettings::~CPsSettings()
{
	iSearchUri.ResetAndDestroy();
	iDisplayFields.Reset();
}

// ----------------------------------------------------------------------------
// CPsSettings::SetSearchUrisL
// Sets the list of URIs to search from.
// ----------------------------------------------------------------------------
EXPORT_C void CPsSettings::SetSearchUrisL(const RPointerArray<TDesC>& aSearchUri)
{   
	iSearchUri.ResetAndDestroy();
    for(TInt i =0 ; i < aSearchUri.Count();i++)
	{
		const HBufC* uriToAppend =aSearchUri[i]->AllocL();
		iSearchUri.Append(uriToAppend);
	}

}

// ----------------------------------------------------------------------------
// CPsSettings::SetDisplayFieldsL
// Sets the list of fields to display.
// ----------------------------------------------------------------------------
EXPORT_C void CPsSettings::SetDisplayFieldsL(const RArray<TInt>& aDisplayFields)
{
	iDisplayFields.Reset();
    for(TInt i =0 ; i < aDisplayFields.Count();i++)
	{
		iDisplayFields.Append(aDisplayFields[i]);
	}
	
}

// ----------------------------------------------------------------------------
// CPsSettings::SetMaxResults
// Sets the number of elements (contacts) to be given to client.
// ----------------------------------------------------------------------------
EXPORT_C void CPsSettings::SetMaxResults(const TInt aMaxResults)
{
	if(aMaxResults >= -1)
	{
		iMaxResults = aMaxResults;
	}
	else
	{
		iMaxResults = -1;
	}	
}

// ----------------------------------------------------------------------------
// CPsSettings::SetSortType
// Sets the sort type for the search
// ----------------------------------------------------------------------------
EXPORT_C void CPsSettings::SetSortType(const TSortType aSortType)
{
	iSortType = aSortType;
}
// CPsSettings::SearchUrisL
// Returns the list of URIs configured to search from.
// ----------------------------------------------------------------------------
EXPORT_C void CPsSettings:: SearchUrisL(RPointerArray<TDesC>& aSearchUri) const
{
    //Cleanup already existing memory
    for(TInt i =0 ; i < iSearchUri.Count();i++)
	{
		const HBufC* uriToAppend =iSearchUri[i]->AllocL();
		aSearchUri.Append(uriToAppend);
	}
}

// ----------------------------------------------------------------------------
// CPsSettings::DisplayFieldsL
// Returns the list of fields to display.
// ----------------------------------------------------------------------------
EXPORT_C void CPsSettings:: DisplayFieldsL(RArray<TInt>& aDisplayFields) const
{
    for(TInt i =0 ; i < iDisplayFields.Count();i++)
	{
		aDisplayFields.Append(iDisplayFields[i]);
	}

}

// ----------------------------------------------------------------------------
// CPsSettings::MaxResults
// Returns the number of elements (contacts) to be given to client.
// ----------------------------------------------------------------------------
EXPORT_C TInt CPsSettings::MaxResults() const
{
	return iMaxResults;
}
// ----------------------------------------------------------------------------
// CPsSettings::GetSortType
// Returns the sort type based on which results will be sorted.
// ----------------------------------------------------------------------------
EXPORT_C TSortType CPsSettings::GetSortType() const
{
	return iSortType;
}

// ----------------------------------------------------------------------------
// CPsSettings::GetGroupIdsL
// Returns the array  groupId from the URIs specified in the settings
// ----------------------------------------------------------------------------
EXPORT_C void CPsSettings::GetGroupIdsL(RArray<TInt>& aGroupIdArray) 
{
    aGroupIdArray.Reset();
    
    // The format for groups URI is "cntdb:://group.gdb/group?=<groupId>"
    // We use "=" to identify if it is  a group URI
    TChar equalToCharacter('=');
    
    // Parse through all the URIs
    for ( TInt i =0; i < iSearchUri.Count(); i++ ) 
    {	    	
    	TInt offset = iSearchUri[i]->Locate(equalToCharacter);
		
		if ( offset != KErrNotFound )
		{
			// Parse and Extract the group name
			TInt len = iSearchUri[i]->Length();
			HBufC* grpIdBuf = iSearchUri[i]->Right(len - offset - 1).AllocL();

			// Convert to decimal 
			TInt grpIdVal = 0;
			TLex16 myDocId(*grpIdBuf);
			TInt err = myDocId.Val(grpIdVal);
			 	
			// Append to aGroupIdArray, if conversion is successful
			if( KErrNone == err)
			{
				aGroupIdArray.Append(grpIdVal);
			}
				   
			// Cleanup memory
			delete   grpIdBuf;
			grpIdBuf = NULL;
           
		}
	}		
}

// ----------------------------------------------------------------------------
// CPsSettings::ExternalizeL
// Writes 'this' to aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsSettings::ExternalizeL(RWriteStream& aStream) const
{     
    aStream.WriteUint8L(iSearchUri.Count()); // Number of databse URIs

	for ( int index = 0; index < iSearchUri.Count(); index++ )
	{
	    HBufC* uri = iSearchUri[index]->AllocL();
	    if ( uri ) // Write uri to the stream (or a NULL descriptor)
	    {
	    	aStream.WriteUint8L( uri->Size() ); // uri size
			aStream << *uri; // uri
	    }
	    else
	    {
	    	aStream.WriteUint8L( 0 );
	    	aStream << KNullDesC8;
	    }
	    delete uri;
	    uri = NULL;	    	
	}
	
    aStream.WriteUint8L(iDisplayFields.Count()); // Number of display fields
	for ( int index = 0; index < iDisplayFields.Count(); index++ )
	{
		aStream.WriteInt16L(iDisplayFields[index]);
	}

    aStream.WriteInt16L(iMaxResults); // Number of contacts that will be displayed to the client
    aStream.WriteInt8L(iSortType); // Set the sort type
}

// ----------------------------------------------------------------------------
// CPsSettings::InternalizeL
// Initializes 'this' with the contents of aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsSettings::InternalizeL(RReadStream& aStream)
{
    // Read number of database URIs
    TInt szNumUri = aStream.ReadUint8L();
    
    for ( int index = 0; index < szNumUri; index++ )
    {
        // Read uri size
	    TUint8 szUri = aStream.ReadUint8L();
    	    
    	HBufC* uri =  HBufC::NewL(aStream, szUri);
	    iSearchUri.InsertL(uri, index);
	    
    }
    
    // Read number of display fields
    TInt szNumFields = aStream.ReadUint8L();
    for ( int index = 0; index < szNumFields; index++ )
    {
    	iDisplayFields.InsertL(aStream.ReadInt16L(), index);
    }
    
    // Read Number of contacts that will be displayed to the client
    iMaxResults = aStream.ReadInt16L();
    iSortType = (TSortType)aStream.ReadInt8L();
	
}

