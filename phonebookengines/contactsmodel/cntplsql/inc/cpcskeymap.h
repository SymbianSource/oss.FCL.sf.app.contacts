/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Class to hold the character to key mappings and provide
*   conversion of strings to their numerical mappings. 
*
*/

#ifndef __CPCSKEYMAP_H__
#define __CPCSKEYMAP_H__


///// Since the real Orbit keymap code crashes,
///// use this hardcoded keymap for now


// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
NONSHARABLE_CLASS(CPcsKeyMap) : public CBase
	{
    public:    
        /**
         * Two phase construction
         */
        static CPcsKeyMap* NewL();
        
        /**
         * Destructor
         */
		virtual ~CPcsKeyMap();

		/**
		 * Converts a string to a numeric string.
		 * aSource String to be converted
		 * aPlainConversion ETrue do a simple conversion, without special
		 *                        handling for sepator characters
		 *					EFalse add a separator character at the beginning
		 *					       and convert spaces to separator characters
		 * returns Conversion result, ownership is transferred		       
		 */
		HBufC* GetNumericKeyStringL(const TDesC& aSource,
                                    TBool aPlainConversion) const;								  

	private:
		TChar GetNumericValueForChar(TChar input) const;

        /**
         * Constructor
         */
		CPcsKeyMap();
		
		/**
		 * Second phase constructor
		 */
		void ConstructL();
	};

#endif // __CPCSKEYMAP_H__

// End of file
