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
* Description:  Class to hold the character to key mappings
*
*/

#ifndef __CPCS_KEY_MAP_H__
#define __CPCS_KEY_MAP_H__

// INCLUDES
#include <e32base.h>
#include <PtiEngine.h>
#include <PtiKeyMappings.h>
#include <AvkonInternalCRKeys.h>
#include <e32property.h>

class CPsQuery;

class TKeyMappingData
    {
    public:
        // Keep the following order as it follows the search priority
        enum TCharArraySelector
            {
            EKeyMapSingleCharArr = 0,
            EKeyMapUILangArr,
            EKeyMapEnglishLangArr,
            EKeyMapOthersLangArr,
            EKeyMapNumberArr
            };

        TFixedArray<RArray<TUint>, TKeyMappingData::EKeyMapNumberArr> iKeyMapCharArr; 
        
        /* Help for deciding the key from the char in devices language variants
         *   iKeyMapCharArr[EKeyMapSingleCharArr]:  All the single chars for the languages
         * 
         * Sharing the chars in the following 3 classes helps for guessing the key from the key
         * in a more appropriate way (giving language preferences) when the same character can be mapped
         * to more keys considering all the supported languages together
         *   iKeyMapCharArr[EKeyMapUILangArr]:      All chars mapped to the key for UI Language
         *   iKeyMapCharArr[EKeyMapEnglishLangArr]: All remaining chars mapped to the key for English Language
         *   iKeyMapCharArr[EKeyMapOthersLangArr]:  All remaining chars mapped to the key for the other languages
         */
    };

// CLASS DECLARATION
class CPcsKeyMap : public CBase
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
		 * Convert functions to get the key string from query and/or chars string
		 */
		void GetMixedKeyStringForQueryL(CPsQuery& aSrcQuery, TDes& aDestStr);
		void GetMixedKeyStringForDataL(CPsQuery& aSrcQuery, const TDesC& aSrcData, TDes& aDestStr);
		
		/**
		 * Returns true if the character is mapped to the key
		 */
		TBool CharacterForKeyMappingExists(TKeyMappingData& aKeyMap, const TUint aIntChar);

#ifdef _DEBUG
		/**
		 * Check potential error conditions for KeyForCharacterMultiMatch
		 */
		void CheckPotentialErrorConditions(RArray<TInt>& aPoolIndexArr, const TChar& aChar);
#endif // _DEBUG

        /**
         * Returns the numeric key id corresponding to a specific character
         * Considers possible multiple matches for some phone variants
         */
        TPtiKey KeyForCharacterMultiMatch(const TChar& aChar);

        /**
         * Returns true if the characters "0" and " " are on the same key
         */
        TBool GetSpaceAndZeroOnSameKey();

        /**
         * Returns true if this language is supported
         */
        TBool IsLanguageSupported(TInt aLang);

        /**
         * Returns the pool Id for a key
         */
        TInt PoolIdForKey(const TPtiKey aKey);

        /**
         * Returns the pool Id for a character
         */
        TInt PoolIdForCharacter(const TChar& aChar);

        /**
         * Returns total number of pools
         */
        TInt PoolCount();

    private:

        /**
         * Constructor
         */
        CPcsKeyMap();

        /**
         * Second phase constructor
         */
        void ConstructL();

        /**
         * Helper functions to get Key Mappings and Key Mappings Data depending
         * on keyboard type
         */
        MPtiKeyMappings* GetKeyboardKeyMapping(CPtiCoreLanguage& aCurrLanguage);
        CPtiKeyMapData* GetKeyMapData(CPtiCoreLanguage& aCurrLanguage);

        /**
         * Helper functions to construct Key List for different keyboards
         */
        void AppendEntryWithFakeKeyToKeyList();
        void CreateKeyListFromITUTHardcodedKeys();
        void CreateKeyListFromKeyBindingTable( CPtiEngine* aPtiEngine );
        void CreateKeyListL( CPtiEngine* aPtiEngine );

        /**
         * Helper function to construct Key Mappings
         */
        void CreateKeyMappingL();

        /**
         * Helper functions to populate Keyboard Key Mappings (Characters for
         * all Keys) for the Active Language
         */
        void AddKeyMappingForActiveLanguageL(CPtiEngine* aPtiEngine, TLanguage aLanguage);
        void GetCharactersForKey(MPtiKeyMappings& aPtiKeyMappings,
                                 TPtiKey aKey,
                                 TPtiTextCase aTextCase,
                                 TDes& aResult);
        void AddCharactersToKey(MPtiKeyMappings& aPtiKeyMappings,
                                TPtiKey aKey,
                                TPtiTextCase aTextCase,
                                TKeyMappingData& aKeyDataList,
                                RArray<TUint>& aKeyMapLang,
                                TBool& aIsSingleCharForKey,
                                TUint& aSingleChar);

        /**
         * Sets attribute to true if the characters "0" and " " are on the same key
         */
        void SetSpaceAndZeroOnSameKey();

    private:

        /**
         * Array to hold the list of languages not supported
         */
        RArray<TLanguage> iLanguageNotSupported;

        /**
         * UI Language
         */
        TLanguage iUILanguage;

        /**
         * Flag to indicate if Phone is Chinese variant
         */
        RArray<TPtiKey> iKeysArr;

        /**
         * Contains all the keys and the characters mapped to each key own
         */
        RPointerArray<TKeyMappingData> iKeyMapPtrArr;

        /**
         * Type of keyboard for example, half-qwerty, 4x10 qwerty etc
         */
        TPtiKeyboardType iKeyboardType;
        
        /**
         * True if "0" and " " are on the same key
         */
        TBool iSpaceAndZeroOnSameKey;
};

#endif // __CPCS_KEY_MAP_H__

// End of file
