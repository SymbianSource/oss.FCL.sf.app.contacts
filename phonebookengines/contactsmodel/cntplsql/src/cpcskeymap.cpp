/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Retrieves the character map for each of the numeric keys.
*/

// INCLUDE FILES
#include "cpcskeymap.h"

// Treat space as sepator
#define KSpaceChar      ' '

// Separator character stored in predictive search table columns
_LIT(KSeparator, " ");


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsKeyMap::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsKeyMap* CPcsKeyMap::NewL()
	{
    CPcsKeyMap* self = new ( ELeave ) CPcsKeyMap();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	} 

// ----------------------------------------------------------------------------
// CPcsKeyMap::CPcsKeyMap
// Constructor
// ----------------------------------------------------------------------------
CPcsKeyMap::CPcsKeyMap()
	{
	}

void CPcsKeyMap::ConstructL()
	{   
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::~CPcsKeyMap
// Destructor
// ----------------------------------------------------------------------------
CPcsKeyMap::~CPcsKeyMap()
	{
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetNumericKeyStringL
// If aPlainConversion is EFalse, supports sub-string searches and space is
// converted to a separator character (not zero).
// ----------------------------------------------------------------------------
HBufC* CPcsKeyMap::GetNumericKeyStringL(const TDesC& aSource,
                                        TBool aPlainConversion) const
    {
    TInt length = aSource.Length();
    if (!aPlainConversion)
        {
        ++length; // One extra character for leading separator
        }
    HBufC* destination = HBufC::NewL(length);
    TPtr ptr = destination->Des();

    if (!aPlainConversion)
        {
        ptr.Append(KSeparator);
        }

	for (TInt i = 0; i < aSource.Length(); ++i)
        {
		if (!aPlainConversion && aSource[i] == KSpaceChar)
			{
			ptr.Append(KSeparator);
			}
		else
			{
			TChar a = aSource[i];
			TChar b = a.GetUpperCase();
			ptr.Append(GetNumericValueForChar(b));
			}
        }

    return destination;
    }

TChar CPcsKeyMap::GetNumericValueForChar(TChar input ) const
    {
    
    TChar ret = '0';
    switch (input)
        {
        case 'A': 
        case 'B': 
        case 'C':
               ret = '2';
               break;
        case 'D': 
        case 'E':
        case 'F':
            ret = '3';
            break;
            
        case 'G': 
        case 'H':
        case 'I':
            ret = '4';
            break;

        case 'J': 
        case 'K': 
        case 'L':
            ret = '5';
            break;

        case 'M': 
        case 'N': 
        case 'O':
            ret = '6';
            break;
      
        case 'P': 
        case 'Q':
        case 'R': 
        case 'S':
            ret = '7';
            break;

        case 'T':
        case 'U': 
        case 'V':
            ret = '8';
            break;

        case 'W':
        case 'X':
        case 'Y': 
        case 'Z':
            ret = '9';
            break;
            
		case ' ':
			ret = '0';
			break;

		default: // Other chars, e.g. numbers
			ret = input;
        }
    return ret;    
    }

// End of file
