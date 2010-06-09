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
#include "CPcsAdaptiveGridItem.h"
#include "CPcsAdaptiveGrid.h"



// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsAdaptiveGrid::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsAdaptiveGrid* CPcsAdaptiveGrid::NewL( )
{
    return new ( ELeave ) CPcsAdaptiveGrid();
} 

// ----------------------------------------------------------------------------
// CPcsAdaptiveGrid::CPcsAdaptiveGrid
// Constructor
// ----------------------------------------------------------------------------
CPcsAdaptiveGrid::CPcsAdaptiveGrid( )
{
    iAdaptiveGrid.ResetAndDestroy();
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGrid::~CPcsAdaptiveGrid
// Destructor
// ----------------------------------------------------------------------------
CPcsAdaptiveGrid::~CPcsAdaptiveGrid( )
{
    iAdaptiveGrid.ResetAndDestroy();
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGrid::Count
// 
// ----------------------------------------------------------------------------
TInt CPcsAdaptiveGrid::Count( )
{
    return iAdaptiveGrid.Count();
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGrid::GetAdaptiveGridItemIndex
// 
// ----------------------------------------------------------------------------
TInt CPcsAdaptiveGrid::GetAdaptiveGridItemIndex( const TChar& aChar )
{
    TChar aUpperChar = User::UpperCase( aChar );

    TInt retValue = KErrNotFound;

    // The Grid items are sorted alphabetically, anyway the number of items in the
    // array is small so we do linear search
    for ( TInt i=0; i < iAdaptiveGrid.Count(); i++ )
        {
        if ( iAdaptiveGrid[i]->Character( ) == aUpperChar )
            {
            retValue = i;
            break;
            }
        }

    return retValue;
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGrid::GetAdaptiveGridItemPtr
// 
// ----------------------------------------------------------------------------
CPcsAdaptiveGridItem* CPcsAdaptiveGrid::GetAdaptiveGridItemPtr( const TChar& aChar )
{
    CPcsAdaptiveGridItem* retValue = NULL;

    TInt gridItemIndex = GetAdaptiveGridItemIndex( aChar );
    
    if ( gridItemIndex != KErrNotFound )
        {
        retValue = iAdaptiveGrid[gridItemIndex];
        }

    return retValue;
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGrid::IncrementRefCounter
// 
// ----------------------------------------------------------------------------
void CPcsAdaptiveGrid::IncrementRefCounterL( const TChar& aChar, const TUint aSelector )
{
    CPcsAdaptiveGridItem* gridItem = GetAdaptiveGridItemPtr( aChar );
    
    if ( gridItem == NULL )
        {
        TLinearOrder<CPcsAdaptiveGridItem> rule( CPcsAdaptiveGridItem::CompareByCharacter );

        CPcsAdaptiveGridItem* newGridItem = CPcsAdaptiveGridItem::NewL( aChar );
        newGridItem->IncrementRefCounter( aSelector );
        iAdaptiveGrid.InsertInOrder(newGridItem, rule);
        }
    else
        {
        gridItem->IncrementRefCounter( aSelector );
        }

    PRINT2 ( _L("CPcsAdaptiveGrid::IncrementRefCounterL: Char='%c', Selector=%d"),
             (TUint) aChar, aSelector );
    // PrintAdatptiveGrid( );
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGrid::DecrementRefCounter
// 
// ----------------------------------------------------------------------------
void CPcsAdaptiveGrid::DecrementRefCounter( const TChar& aChar, const TUint aSelector )
{
    TInt gridItemIndex = GetAdaptiveGridItemIndex( aChar );

    if ( gridItemIndex != KErrNotFound )
        {
        CPcsAdaptiveGridItem* gridItem = iAdaptiveGrid[gridItemIndex];
 
        gridItem->DecrementRefCounter( aSelector );
        
        // Remove the Grid item, if there is no reference to it anymore
        if ( gridItem->AreRefCountersZero( ) )
            {
            iAdaptiveGrid.Remove( gridItemIndex );
            delete gridItem;
            }
        }   

    PRINT2 ( _L("CPcsAdaptiveGrid::DecrementRefCounter: Char='%c', Selector=%d"),
             (TUint) aChar, aSelector );
    // PrintAdatptiveGrid( );
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGrid::GetAdaptiveGrid
// 
// ----------------------------------------------------------------------------
void CPcsAdaptiveGrid::GetAdaptiveGrid( const TBool aCompanyName, TDes& aGrid )
{
    aGrid.Zero();
    TInt gridCharCount = 0;
    for ( TInt i=0; i < iAdaptiveGrid.Count(); i++ )
        {
        if ( iAdaptiveGrid[i]->IsRefCounterNonZero( aCompanyName ) )
            {
            aGrid.Append( iAdaptiveGrid[i]->Character() );
            gridCharCount++;
            if ( gridCharCount == KPsAdaptiveGridStringMaxLen )
                {
                break;
                }
            }
        }
}

// ----------------------------------------------------------------------------
// CPcsAdaptiveGrid::PrintAdatptiveGrid
// 
// ----------------------------------------------------------------------------
void CPcsAdaptiveGrid::PrintAdatptiveGrid( )
{
    PRINT ( _L("CPcsAdaptiveGrid::PrintAdatptiveGrid: ----------------------------------------") );
    PRINT1 ( _L("CPcsAdaptiveGrid::PrintAdatptiveGrid: Adaptive Grid (%d items)"), iAdaptiveGrid.Count() );
    PRINT ( _L("CPcsAdaptiveGrid::PrintAdatptiveGrid: ----------------------------------------") );

    for ( TInt i=0; i < iAdaptiveGrid.Count(); i++ )
        {
        CPcsAdaptiveGridItem* gridItem = iAdaptiveGrid[i];

        PRINT5 ( _L("CPcsAdaptiveGrid::PrintAdatptiveGrid: Grid[%d].Character=%c, RefCounters[%d,%d,%d]"),
                 i, (TUint) iAdaptiveGrid[i]->Character(),
                 iAdaptiveGrid[i]->GetRefCounter(CPcsAdaptiveGridItem::EFirstNameLastName),
                 iAdaptiveGrid[i]->GetRefCounter(CPcsAdaptiveGridItem::EUnnamedCompanyName),
                 iAdaptiveGrid[i]->GetRefCounter(CPcsAdaptiveGridItem::ECompanyName) );
        }

    PRINT ( _L("CPcsAdaptiveGrid::PrintAdatptiveGrid: ----------------------------------------") );
}

// End of file
