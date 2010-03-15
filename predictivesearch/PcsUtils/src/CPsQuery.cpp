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
* Description:  Utility class to hold the search sequence for predictive search.
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include "CPsQuery.h"
#include "CPsQueryItem.h"
#include "CPcsDebug.h"

// ============================== MEMBER FUNCTIONS ============================


// ----------------------------------------------------------------------------
// CPsQuery::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
EXPORT_C CPsQuery* CPsQuery::NewL()
{
	CPsQuery* self = new (ELeave) CPsQuery();	
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
	
}

// ----------------------------------------------------------------------------
// CPsQuery::CPsQuery
// Default constructor
// ----------------------------------------------------------------------------
CPsQuery::CPsQuery()
{
}

// ----------------------------------------------------------------------------
// CPsQuery::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CPsQuery::ConstructL()
{ 
}

// ----------------------------------------------------------------------------
// CPsQuery::~CPsQuery
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CPsQuery::~CPsQuery()
{
	Reset();
}

// ----------------------------------------------------------------------------
// CPsQuery::InsertL
// Add a query item to the search sequence
// ----------------------------------------------------------------------------
EXPORT_C void CPsQuery::InsertL(const CPsQueryItem& aQueryItem, TInt aIndex)
{   
	if( (aIndex >= 0) 
	   &&  (aIndex <= (iSearchQuery.Count() + 1)) 
	   && (this->Count() < KPsQueryMaxLen) )
	{
		iSearchQuery.Insert(&aQueryItem, aIndex);		
	}    
}

// ----------------------------------------------------------------------------
// CPsQuery::AppendL
// Appends a query item to the search sequence
// ----------------------------------------------------------------------------
EXPORT_C void CPsQuery::AppendL(const CPsQueryItem& aQueryItem)
{
	if(this->Count() < KPsQueryMaxLen )
		iSearchQuery.AppendL(&aQueryItem);
}

// ----------------------------------------------------------------------------
// CPsQuery::GetItemAtL
// Returns the query item at the specified index
// ----------------------------------------------------------------------------
EXPORT_C CPsQueryItem& CPsQuery:: GetItemAtL(TInt aIndex)
{    
	if ( aIndex < 0 && aIndex >= iSearchQuery.Count()) 
	{
	    User::Leave(KErrArgument);		
	}
	
	return (*(iSearchQuery[aIndex]));
}

// ----------------------------------------------------------------------------
// CPsQuery::QueryAsStringL
// Returns the search sequence
// ----------------------------------------------------------------------------
EXPORT_C TDesC& CPsQuery:: QueryAsStringLC()
{
	HBufC* query = HBufC::NewLC(KPsQueryMaxLen);	

    for(TInt arrayIndex =0 ; arrayIndex < iSearchQuery.Count();arrayIndex++)
    {
    	query->Des().Append((iSearchQuery[arrayIndex])->Character());
    }
    
    return *query;
}

// ----------------------------------------------------------------------------
// CPsQuery::Remove
// Deletes the query item at the specified index
// ----------------------------------------------------------------------------
EXPORT_C void CPsQuery:: Remove(TInt aIndex)
{
	if (aIndex >= 0 && aIndex <= (iSearchQuery.Count() - 1))
	{
	    delete iSearchQuery[aIndex];
		iSearchQuery.Remove(aIndex);		
	}   
}

// ----------------------------------------------------------------------------
// CPsQuery::Reset
// Deletes the entire search query
// ----------------------------------------------------------------------------
EXPORT_C void CPsQuery:: Reset()
{
	for ( TInt arrayIndex = 0; arrayIndex < iSearchQuery.Count(); arrayIndex++ )
	{
		delete iSearchQuery[arrayIndex];
	}   
    iSearchQuery.Reset();  
}

// ----------------------------------------------------------------------------
// CPsQuery::KeyboardModeL
// Returns the keyboard input mode for the search query
// ----------------------------------------------------------------------------
EXPORT_C TKeyboardModes CPsQuery:: KeyboardModeL()
{
	TKeyboardModes keyboardMode = EModeUndefined;
    
    if(iSearchQuery.Count() > 0)
    {
    	keyboardMode = iSearchQuery[0]->Mode();
	    for(TInt arrayIndex =1 ; arrayIndex < iSearchQuery.Count() ;arrayIndex++)
	    {
	        if(keyboardMode != iSearchQuery[arrayIndex]->Mode())
	        {
	        	keyboardMode = EModeUndefined;
	        	break;	
	        }
	    }
    }
	
    return keyboardMode;
}

// ----------------------------------------------------------------------------
// CPsQuery::Count
// Returns the length of the query
// ----------------------------------------------------------------------------
EXPORT_C TInt CPsQuery:: Count()
{
	return (iSearchQuery.Count());
}

// ----------------------------------------------------------------------------
// Deprecated: Use CPcsDebug::PrintQuery
// CPsQuery::PrintQuery
// Prints the query as array of query items (query items cannot be spaces)
// Used only for debugging
// ----------------------------------------------------------------------------
EXPORT_C void CPsQuery:: PrintQuery()
{
    PRINT1 ( _L("CPsQuery::PrintQuery: Number of items : %d"), iSearchQuery.Count()); 
	for(TInt i = 0; i < iSearchQuery.Count(); i++)
	{
		TUint inputKey = iSearchQuery[i]->Character().GetUpperCase();
		TBuf<2> buffer;
		buffer.Format(_L("%c"), inputKey);
	    switch(iSearchQuery[i]->Mode())
	    {
	    	case EPredictiveDefaultKeyboard:
	    	    PRINT2 ( _L("Character at index %d: '%S' (EPredictiveDefaultKeyboard)"),
	    	             i, &buffer);
	    	    break;
	    	case ENonPredictive:
	    	    PRINT2 ( _L("Character at index %d: '%S' (ENonPredictive)"),
	    	             i, &buffer);
	    	    break;
	    	case EPredictiveItuT:
	    	    PRINT2 ( _L("Character at index %d: '%S' (EPredictiveItuT)"),
	    	             i, &buffer);
	    	    break;
	    	case EPredictiveQwerty:
	    	    PRINT2 ( _L("Character at index %d: '%S' (EPredictiveQwerty)"),
	    	             i, &buffer);
	    	    break;
	    	default:
	    	    PRINT3 ( _L("Character at index %d: '%S' (mode=%d)"),
	    	             i, &buffer, iSearchQuery[i]->Mode());
	    	    break;
	    }
	}
}

// ----------------------------------------------------------------------------
// CPsQuery::ExternalizeL
// Writes 'this' to aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsQuery::ExternalizeL(RWriteStream& aStream) const
{	
    aStream.WriteUint8L(iSearchQuery.Count()); // Number of query items
	for ( TInt index = 0; index < iSearchQuery.Count(); index++ )
	{
		(iSearchQuery[index])->ExternalizeL(aStream);
	}
}

// ----------------------------------------------------------------------------
// CPsQuery::InternalizeL
// Initializes 'this' with the contents of aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsQuery::InternalizeL(RReadStream& aStream)
{
    // Read number of query items
    TInt numQueryItems = aStream.ReadUint8L();
    
    // Internalize each item
    for ( TInt index = 0; index < numQueryItems; index++ )
    {
        CPsQueryItem *item = CPsQueryItem::NewL();
        item->InternalizeL(aStream);
        this->AppendL(*item);
    }
}

