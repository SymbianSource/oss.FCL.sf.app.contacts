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
* Description:  Utility class to hold a single item for Adaptive Grid.
*               Used to provide quickly to client applications (Phonebook)
*               The Adaptive Grid for the full list of contacts, which is
*               the one that is displayed when the Find Box is empty.
*/


// INCLUDE FILES
#include "CPcsDebug.h"
#include "CPcsAlgorithm1Utils.h"
#include "CPcsAdaptiveGridItem.h"


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAdaptiveGridItem::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAdaptiveGridItem* CPcsAdaptiveGridItem::NewL( const TChar& aChar )
{
	return new ( ELeave ) CPcsAdaptiveGridItem( aChar );
} 

// ----------------------------------------------------------------------------
// CPcsAdaptiveGridItem::CPcsAdaptiveGridItem
// Constructor
// ----------------------------------------------------------------------------
CPcsAdaptiveGridItem::CPcsAdaptiveGridItem( const TChar& aChar ):
        iCharacter(User::UpperCase( aChar ))
{
    // Set zeroes in the reference counters array
    for ( TInt i=0; i < CPcsAdaptiveGridItem::ENumberCounters; i++ )
        {
        iCountersArr[i] = 0;
        }
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGridItem::~CPcsAdaptiveGridItem
// Destructor
// ----------------------------------------------------------------------------
CPcsAdaptiveGridItem::~CPcsAdaptiveGridItem( )
{
    iCountersArr.Reset();
}

// ----------------------------------------------------------------------------
// CPcsAlgorithm1Utils::CompareByCharacter()
// TLinearOrder rule for comparison of data objects
// ----------------------------------------------------------------------------
TInt CPcsAdaptiveGridItem::CompareByCharacter( const CPcsAdaptiveGridItem& aObject1, 
                                               const CPcsAdaptiveGridItem& aObject2 )
{
    return CPcsAlgorithm1Utils::CompareByCharacter(
            aObject1.Character(), aObject2.Character() );
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGridItem::Character
// Return character of the Adaptive Grid Item 
// ----------------------------------------------------------------------------
TChar CPcsAdaptiveGridItem::Character( ) const
{
    return iCharacter;
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGridItem::GetRefCounter
// Return the reference counter for one selector 
// ----------------------------------------------------------------------------
TUint CPcsAdaptiveGridItem::GetRefCounter( const TUint aSelector )
{
    if ( aSelector < CPcsAdaptiveGridItem::ENumberCounters )
        {
        return iCountersArr[aSelector];
        }
    else
        {
        return 0;
        }
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGridItem::IncrementRefCounter
// Increment the reference counter for one selector 
// ----------------------------------------------------------------------------
void CPcsAdaptiveGridItem::IncrementRefCounter( const TUint aSelector )
{
    if ( aSelector < CPcsAdaptiveGridItem::ENumberCounters )
        {
        iCountersArr[aSelector]++;
        }
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGridItem::DecrementRefCounter
// Decrement the reference counter for one selector 
// ----------------------------------------------------------------------------
void CPcsAdaptiveGridItem::DecrementRefCounter( const TUint aSelector )
{
    if ( aSelector < CPcsAdaptiveGridItem::ENumberCounters )
        {
        if ( iCountersArr[aSelector] > 0 )
            {
            iCountersArr[aSelector]--;
            }
        else
            {
             PRINT2 ( _L("CPcsAdaptiveGridItem::DecrementRefCounter: ERROR Decrement ref counter 0, Character=%c, Selector=%d"),
                      (TUint) Character(), aSelector );
            }
        }
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGridItem::AreRefCountersZero
// Check if reference counters for all selectors are 0 
// ----------------------------------------------------------------------------
TBool CPcsAdaptiveGridItem::AreRefCountersZero( )
{
    TBool retValue = ETrue;

    for ( TInt i=0; i < CPcsAdaptiveGridItem::ENumberCounters; i++ )
        {
        if ( iCountersArr[i] > 0 )
            {
            retValue = EFalse;
            break;
            }
        }

    return retValue;
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGridItem::IsRefCounterNonZero
// Check if reference counters for all selectors are 0 
// ----------------------------------------------------------------------------
TBool CPcsAdaptiveGridItem::IsRefCounterNonZero( const TBool aCompanyName )
{
    TBool retValue = EFalse;

    if ( iCountersArr[CPcsAdaptiveGridItem::EFirstNameLastName] > 0 || 
         iCountersArr[CPcsAdaptiveGridItem::EUnnamedCompanyName] > 0 ||
         ( aCompanyName && iCountersArr[CPcsAdaptiveGridItem::ECompanyName] > 0 ) )
        {
        retValue = ETrue;
        }

    return retValue;
}

// End of file
