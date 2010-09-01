/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Identifies a pool element
*
*/

#ifndef __CPCS_POOL_ELEMENT_H__
#define __CPCS_POOL_ELEMENT_H__

// INCLUDES
#include <e32base.h>
#include <e32math.h>

// FORWARD DECLARATION
class CPsData;

// CLASS DECLARATION
class CPcsPoolElement : public CBase
{

    public:
    
        /**
         * Two phase construction
         */
		static CPcsPoolElement* NewL(CPsData& aPsData);
		
		/**
		 * Destructor
		 */
		virtual ~CPcsPoolElement();
		
		/**
		 * Clears the data match element attribute
		 */	
		void ClearDataMatchAttribute();
				
		/**
		 * Checks if data matched
		 */
		TBool IsDataMatch(TInt aIndex);
		
		/**
		 * Set data match
		 */
		void SetDataMatch(TInt aIndex);
	
		/**
		 * Return the CPsData object of this pool element
		 */
		CPsData* GetPsData();
		
		/**
		 * Compares the two Pool Elements and returms the comparison value.
		 * It takes care of language variance too.
		 */
		 static TInt CompareByData ( const CPcsPoolElement& aObject1, 
					     const CPcsPoolElement& aObject2 );

    private:
    
		/*
		 * Constructor
		 */
		CPcsPoolElement();

		/*
		 * 2nd phase constructor
		 */
		void ConstructL(CPsData& aPsData);

    private:
     
        /*
         * Pointer to contact data
         * Not owned.
         */
         CPsData* iPsData;
         
		/*
		 * This attribute indicates due to which
		 * the data element this item entered the pool
		 *
		 * Bit 0 = Data1 matched
		 * Bit 1 = Data2 matched
		 * Bit 2 = Data3 matched
		 * Bit 3 = Not used
		 * Bit 4 = Not used
		 * Bit 5 = Not used
		 * Bit 6 = Not used
		 * Bit 7 = Not used
		 */
		TUint8 iDataMatchAttribute;
};

#endif // __CPCS_POOL_ELEMENT_H__

// End of file
