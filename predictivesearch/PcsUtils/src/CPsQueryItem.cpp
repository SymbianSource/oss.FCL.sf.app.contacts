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
* Description:  Utility class to hold a single query item for predictive search.
*                Used to represent the search sequence character-by-character
*                along with the keyboard type(eg ITU-T, QWERTY ) in which the 
*                character was input.
*
*/


// INCLUDE FILES
#include <s32mem.h>
#include "CPsQueryItem.h"

// ============================== MEMBER FUNCTIONS ============================


// ----------------------------------------------------------------------------
// CPsQueryItem::NewL
// Two Phase constructor
// ----------------------------------------------------------------------------
EXPORT_C CPsQueryItem* CPsQueryItem::NewL()
{
	CPsQueryItem* self = new (ELeave) CPsQueryItem();	
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
	
}

// ----------------------------------------------------------------------------
// CPsQueryItem::CPsQueryItem
// Default constructor
// ----------------------------------------------------------------------------
CPsQueryItem::CPsQueryItem()
{
}

// ----------------------------------------------------------------------------
// CPsQueryItem::ConstructL
// Two phase construction
// ----------------------------------------------------------------------------
void CPsQueryItem::ConstructL()
{
}

// ----------------------------------------------------------------------------
// CPsQueryItem::~CPsQueryItem
// Destructor
// ----------------------------------------------------------------------------
EXPORT_C CPsQueryItem::~CPsQueryItem()
{	
}

// ----------------------------------------------------------------------------
// CPsQueryItem::SetMode
// Sets the keyboard mode eg ITU-T, QWERTY etc
// ----------------------------------------------------------------------------
EXPORT_C void CPsQueryItem::SetMode(const TKeyboardModes aMode)
{   
    iMode = aMode;
}

// ----------------------------------------------------------------------------
// CPsQueryItem::SetCharacter
// Sets a character of the search sequence
// ----------------------------------------------------------------------------
EXPORT_C void CPsQueryItem::SetCharacter(const TChar aCharacter)
{
	iCharacter = aCharacter;
}

// ----------------------------------------------------------------------------
// CPsQueryItem::Mode
// Returns the keyboard mode eg ITU-T, QWERTY etc
// ----------------------------------------------------------------------------
EXPORT_C TKeyboardModes CPsQueryItem::Mode() const
{
    return iMode;
}

// ----------------------------------------------------------------------------
// CPsQueryItem::Character
// Returns a character of the search sequence
// ----------------------------------------------------------------------------
EXPORT_C TChar CPsQueryItem::Character() const
{
    return iCharacter;
}

// ----------------------------------------------------------------------------
// CPsQueryItem::ExternalizeL
// Writes 'this' to aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsQueryItem::ExternalizeL(RWriteStream& aStream) const
{
	aStream.WriteInt16L(iMode);
	aStream.WriteInt16L(iCharacter);
}

// ----------------------------------------------------------------------------
// CPsQueryItem::InternalizeL
// Initializes 'this' with the contents of aStream
// ----------------------------------------------------------------------------
EXPORT_C void CPsQueryItem::InternalizeL(RReadStream& aStream)
{
    iMode = (TKeyboardModes)aStream.ReadInt16L();
    iCharacter = aStream.ReadInt16L();
}

