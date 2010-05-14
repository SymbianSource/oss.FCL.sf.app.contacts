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


// If this macro is defined, Orbit code is used to build the keymap.
// But this might crash at startup for unknown reason.
//
// If macro is not defined, a hardcoded keymap is used. That code does not
// crash, but it supports just a limited range of characters.
//#define USE_ORBIT_KEYMAP


// INCLUDES
#if defined(USE_ORBIT_KEYMAP)
#include <QList>
#include <QLocale>
#endif

#include <e32base.h>

// FORWARD DECLARATIONS
#if defined(USE_ORBIT_KEYMAP)
class QString;
class HbInputLanguage;
class HbKeymap;
#endif

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
		 * aPlainConversion ETrue Perform a simple conversion, without special
		 *                        handling for sepator characters
		 *					EFalse Convert spaces to separator characters
		 * returns Conversion result, ownership is transferred
		 */
		HBufC* GetNumericKeyStringL(const TDesC& aSource,
                                    TBool aPlainConversion) const;

#if defined(USE_ORBIT_KEYMAP)
		/*
		 * Return the separator character.
		 */
		QChar Separator() const;
#endif

	private:
        /**
         * Constructor
         */
		CPcsKeyMap();
		
		/**
		 * Second phase constructor
		 */
		void ConstructL();

#if defined(USE_ORBIT_KEYMAP)
		/**
		 * Construct mappings between keys and characters.
		 */
		void ContructKeyboardMappings();

		/**
         * Returns a list of languages installed on the phone.
         */
		QList<HbInputLanguage> AvailableLanguages() const;
		
        /**
         * Returns ETrue if this language is supported
         */
        TBool IsLanguageSupported(QLocale::Language aLanguage) const;

        /**
         * Returns the numeric key id corresponding to a specific character
         * Considers possible multiple matches for some phone variants
         */
        TChar KeyForCharacter(const TChar& aChar) const;

#if defined(_DEBUG)
        TInt KeyIdToNumber(TInt aKeyId) const;
#endif // #if defined(_DEBUG)

        TChar ArrayIndexToNumberChar(TInt aArrayIndex) const;
#else // #if defined(USE_ORBIT_KEYMAP)
		TChar GetNumericValueForChar(TChar input) const;
#endif // #if defined(USE_ORBIT_KEYMAP)

    private: // Data

#if defined(USE_ORBIT_KEYMAP)
		// One QString for each key of the keyboard (1-9,0,*,#).
		// Each contains all the characters that can originate from that key,
        // considering the languages available on the device.
		//
		// iKeyMapping[0] has mappings for 1-key, iKeyMapping[1] for 2-key, ...
		// iKeyMapping[8] for 9-key, iKeyMapping[9] for 0-key,
		// iKeyMapping[10] for *-key, iKeyMapping[11] for #-key.
        QList<QString> iKeyMapping;

		// Characters that have been hardcoded to certain keys, regardless of
		// actual keymap.
		QString iHardcodedChars;
#endif // #if defined(USE_ORBIT_KEYMAP)

		// For unit testing
		friend class UT_CPcsKeyMap;
	};

#endif // __CPCSKEYMAP_H__

// End of file
