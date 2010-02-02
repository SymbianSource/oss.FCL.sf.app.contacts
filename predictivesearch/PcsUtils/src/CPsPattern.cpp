/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility class to hold a match pattern for predictive search.
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include "CPsPattern.h"

// ============================== MEMBER FUNCTIONS ============================


// ----------------------------------------------------------------------------
// CPsPattern::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
EXPORT_C CPsPattern* CPsPattern::NewL()
{
	CPsPattern* self = new (ELeave) CPsPattern();	
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
	
}

// ----------------------------------------------------------------------------
// CPsPattern::CPsPattern
// Default constructor
// ----------------------------------------------------------------------------
CPsPattern::CPsPattern()
{
}

// ----------------------------------------------------------------------------
// CPsPattern::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CPsPattern::ConstructL()
{
    iPattern = NULL;
    iFirstIndex = -1;
}

// ----------------------------------------------------------------------------
// CPsPattern::~CPsPattern
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CPsPattern::~CPsPattern()
{	
    if ( iPattern )
    {
    	delete iPattern;
    	iPattern = NULL;
    }
}

// ----------------------------------------------------------------------------
// CPsPattern::SetPatternL
// Sets the match pattern
// ----------------------------------------------------------------------------
EXPORT_C void CPsPattern::SetPatternL(TDesC& aPattern)
{   
    iPattern = aPattern.AllocL();
}

// ----------------------------------------------------------------------------
// CPsPattern::SetFirstIndex
// Set the first index
// ----------------------------------------------------------------------------
EXPORT_C void CPsPattern::SetFirstIndex(TInt aFirstIndex)
{
	iFirstIndex = aFirstIndex;
}

// ----------------------------------------------------------------------------
// CPsPattern::Pattern
// Returns the match pattern
// ----------------------------------------------------------------------------
EXPORT_C TDesC& CPsPattern::Pattern()
{
    return *iPattern;
}

// ----------------------------------------------------------------------------
// CPsPattern::FirstIndex
// Returns the first index of this pattern
// ----------------------------------------------------------------------------
EXPORT_C TInt CPsPattern::FirstIndex()
{
    return iFirstIndex;
}

// ----------------------------------------------------------------------------
// CPsPattern::ExternalizeL
// Writes 'this' to aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsPattern::ExternalizeL(RWriteStream& aStream)
{
	TInt length = iPattern->Length();
        
    // Write the char sequence length	        
    aStream.WriteUint16L(length);	
        
    // Write the char sequence
    aStream << *iPattern;
	
	// Write first index
	aStream.WriteInt32L(iFirstIndex);
}

// ----------------------------------------------------------------------------
// CPsPattern::InternalizeL
// Initializes 'this' with the contents of aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsPattern::InternalizeL(RReadStream& aStream)
{    
	// Size of sequence
	TInt szSeq = aStream.ReadUint16L();

	// Character sequence
	iPattern = HBufC::NewLC(aStream, szSeq);
	CleanupStack::Pop();
    
    // Read first index
    iFirstIndex = aStream.ReadInt32L();
}

// End of file

