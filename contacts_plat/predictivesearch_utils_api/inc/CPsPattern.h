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
* Description:  Utility class to hold a result pattern for predictive search.
*
*/


#ifndef __CPS_PATTERN_H__
#define __CPS_PATTERN_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <CPcsDefs.h>

// CLASS DECLARATION

/**
 * Utility class to hold a result pattern for predictive search.
 * 
 * @lib pcsutils.lib
 * @since S60 v3.2
 */
class CPsPattern : public CBase
{
	public:

		/**
		* Two phase construction
		*
		* @return Instance of CPsPattern
		*/
		IMPORT_C static CPsPattern* NewL();

		/**
		* Destructor
		*/
		IMPORT_C ~CPsPattern();

		/**
		* Sets the pattern
		*
		* @param aPattern The match pattern 
		*/
		IMPORT_C void SetPatternL(TDesC& aPattern);

		/**
		* Sets the first index
		*
		* @param aCharacter The first index
		*/
		IMPORT_C void SetFirstIndex(TInt aIndex);

		/**
		* Returns the match pattern
		*
		* @return Mode of input for this character
		*/
		IMPORT_C TDesC& Pattern();

		/**
		* Returns the first index
		*
		* @return First index
		*/
		IMPORT_C TInt FirstIndex();

		/**
		* Writes 'this' to the stream
		*
		* @param aStream WriteStream with externalized contents
		*/
		IMPORT_C virtual void ExternalizeL(RWriteStream& aStream);		

		/**
		* Initializes 'this' from stream
		*
		* @param aStream ReadStream with data contents to be internalized
		*/
		IMPORT_C virtual void InternalizeL(RReadStream& aStream);
			

	private:

		/**
		* Default Constructor
		*/
		CPsPattern();

		/**
		* Second phase constructor
		*/
		void ConstructL();		
				
	private:	

		/**
		* Match pattern.
		*/
		HBufC* iPattern;
		
		/**
		* First index of this pattern in result set.
		*/
		TInt iFirstIndex;

};

#endif // __CPS_PATTERN_H__

// End of file
