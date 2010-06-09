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

#ifndef __CPS_ADAPTIVE_GRID_ITEM_H__
#define __CPS_ADAPTIVE_GRID_ITEM_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <CPcsDefs.h>

// CLASS DECLARATION

/**
 * Utility class to hold a single item for the Adaptive Grid.
 * This class is used to represent one character of the Adaptive Grid
 * with its reference counters.
 * 
 * @lib pcsutils.lib
 * @since S60 v3.2
 */
class CPcsAdaptiveGridItem : public CBase
{
	public:

		/**
		 * Two phase construction
		 *
		 * @return Instance of CPcsAdaptiveGridItem
		 */
		static CPcsAdaptiveGridItem* NewL( const TChar& aChar );

		/**
		 * Destructor
		 */
		~CPcsAdaptiveGridItem( );

		/**
		 * TLinearOrder rule for comparison of data objects
         *
         * @return order
		 */
		static TInt CompareByCharacter( const CPcsAdaptiveGridItem& aObject1, 
		                                const CPcsAdaptiveGridItem& aObject2 );

		/**
		 * Returns the character of the Adaptive Grid item
		 *
		 * @return Character input
		 */
		TChar Character( ) const;
			
        /**
         * Selectors for Adaptive Grid character reference counters
         */
        enum TCounterSelector
            {
            EFirstNameLastName = 0,
            EUnnamedCompanyName,
            ECompanyName,
            ENumberCounters
            };

        /**
         * Return the Adaptive Grid item reference counter for a given selector
         * 
         * @param Reference counter selector
         * @return Reference counter value
         */
        TUint GetRefCounter( const TUint aSelector );
        
        /**
         * Increments the Adaptive Grid item reference counter for a given selector
         * 
         * @param Reference counter selector
         */
        void IncrementRefCounter( const TUint aSelector );
        
        /**
         * Decrements the Adaptive Grid item reference counter for a given selector
         * 
         * @param Reference counter selector
         */
        void DecrementRefCounter( const TUint aSelector );

        /**
         * Checks if Adaptive Grid item reference counters for all selectors are 0
         * 
         * @return ETrue if all reference counters are 0
         *         EFalse otherwise
         */
        TBool AreRefCountersZero( );
        
        /**
         * Checks if Adaptive Grid item reference counters for the requested selectors are not 0
         * 
         * @return ETrue if one at least of the reference counters is > 0
         *         EFalse otherwise
         */
        TBool IsRefCounterNonZero( const TBool aCompanyName );
        
	private:

		/**
		 * Default Constructor
		 */
		CPcsAdaptiveGridItem( const TChar& aChar );

		/**
		 * Character of the Adaptive Grid
		 */
		TChar iCharacter;

	    /**
	     *  Reference counters for the Adaptive Grid characters
	     */
        TFixedArray<TUint, CPcsAdaptiveGridItem::ENumberCounters> iCountersArr; 
};

#endif // __CPS_ADAPTIVE_GRID_ITEM_H__

// End of file
