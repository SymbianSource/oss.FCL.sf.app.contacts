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

#ifndef __CPS_ADAPTIVE_GRID_H__
#define __CPS_ADAPTIVE_GRID_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <CPcsDefs.h>

#include "CPcsAdaptiveGridItem.h"

// CLASS DECLARATION

/**
 * Utility class to hold a single item for the Adaptive Grid.
 * This class is used to represent one character of the Adaptive Grid
 * with its reference counters.
 * 
 * @lib pcsutils.lib
 * @since S60 v3.2
 */
class CPcsAdaptiveGrid : public CBase
{
	public:

		/**
		 * Two phase construction
		 *
		 * @return Instance of CPcsAdaptiveGrid
		 */
		static CPcsAdaptiveGrid* NewL( );

		/**
		 * Destructor
		 */
		~CPcsAdaptiveGrid();

        /**
         * Return the count of grid items
         */
		TInt Count();

		/**
		 * Return the index of the Adaptive Grid item, given the char
		 */
		TInt GetAdaptiveGridItemIndex( const TChar& aChar );

        /**
         * Get the pointer to the Adaptive Grid item, given the char
         */
        CPcsAdaptiveGridItem* GetAdaptiveGridItemPtr( const TChar& aChar );

        /**
         * Increment the ref counter of the Adaptive Grid item, given the char
         * and the selector
         */
		void IncrementRefCounterL( const TChar& aChar, const TUint aSelector );
		
        /**
         * Decrement the ref counter of the Adaptive Grid item, given the char
         * and the selector
         */
        void DecrementRefCounter( const TChar& aChar, const TUint aSelector );		
		
        /**
         * Return the Adaptive Grid, given the Company Name
         */
        void GetAdaptiveGrid( const TBool aCompanyName, TDes& aGrid );

        /**
         * Print the Adaptive Grid
         */
        void CPcsAdaptiveGrid::PrintAdatptiveGrid( );
        
        /**
         * Reset the grid
         */
        void Reset();

	private:

		/**
		 * Default Constructor
		 */
		CPcsAdaptiveGrid();
								
        /**
        * Array of Adaptive Grid characters with reference counters.
        */
        RPointerArray<CPcsAdaptiveGridItem> iAdaptiveGrid;
};

#endif // __CPS_ADAPTIVE_GRID_H__

// End of file
