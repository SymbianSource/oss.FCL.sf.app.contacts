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

#ifndef __CPS_QUERY_ITEM_H__
#define __CPS_QUERY_ITEM_H__

// SYSTEM INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <CPcsDefs.h>

// CLASS DECLARATION

/**
 * Utility class to hold a single query item for predictive search.
 * This class that is used to represent one character of the search sequence.
 * 
 * @lib pcsutils.lib
 * @since S60 v3.2
 */
class CPsQueryItem : public CBase
{
	public:

		/**
		* Two phase construction
		*
		* @return Instance of CPsQueryItem
		*/
		IMPORT_C static CPsQueryItem* NewL();

		/**
		* Destructor
		*/
		IMPORT_C ~CPsQueryItem();

		/**
		* Sets the keyboard mode eg ITU-T, QWERTY etc
		*
		* @param aMode The keyborad mode 
		*/
		IMPORT_C void SetMode(const TKeyboardModes aMode);

		/**
		* Sets a character of the search sequence
		*
		* @param aCharacter Character of the search sequence
		*/
		IMPORT_C void SetCharacter(const TChar aCharacter);

		/**
		* Returns the keyboard mode eg ITU-T, QWERTY etc
		*
		* @return Mode of input for this character
		*/
		IMPORT_C TKeyboardModes Mode() const;

		/**
		* Returns a character of the search sequence
		*
		* @return Character input
		*/
		IMPORT_C TChar Character() const;

		/**
		* Writes 'this' to the stream
		*
		* @param aStream WriteStream with externalized contents
		*/
		IMPORT_C virtual void ExternalizeL(RWriteStream& aStream) const;		

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
		CPsQueryItem();

		/**
		* Second phase constructor
		*/
		void ConstructL();		
				
		private:	
				
		/**
		*  Keyboard mode e.g. ITU-T, QWERTY
		*/
		TKeyboardModes iMode;

		/**
		* Character of the serach sequence
		*/
		TChar iCharacter;

};

#endif // __CPS_QUERY_ITEM_H__

// End of file
