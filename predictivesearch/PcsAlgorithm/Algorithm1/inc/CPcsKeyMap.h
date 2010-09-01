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
#include <PtiDefs.h>
#include <AvkonInternalCRKeys.h>
#include <e32property.h>
#include <CPcsDefs.h>

class CPsQuery;
class CPtiEngine;
class CPtiCoreLanguage;


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

        TBool IsItutPredictiveAvailable() const;
        TBool IsQwertyPredictiveAvailable() const;

        static TBool IsModePredictive( TKeyboardModes aKbMode );

        /**
         * Convert functions to get the key string from query and/or chars string
         */
        void GetMixedKeyStringForQueryL(CPsQuery& aSrcQuery, TDes& aDestStr) const;
        void GetMixedKeyStringForDataL(CPsQuery& aSrcQuery, const TDesC& aSrcData, TDes& aDestStr) const;

        /**
         * Returns true if the character is mapped to the key
         */
        TBool CharacterForKeyMappingExists(TKeyMappingData& aKeyMap, TUint aIntChar) const;

        /**
         * Returns the numeric key id corresponding to a specific character
         * Considers possible multiple matches for some phone variants
         */
        TPtiKey KeyForCharacterMultiMatch( const TChar& aChar, TKeyboardModes aKbMode ) const;

        /**
         * Returns true if the characters "0" and " " are on the same key
         */
        TBool GetSpaceAndZeroOnSameKey( TKeyboardModes aKbMode );

        /**
         * Returns true if this language is supported
         */
        TBool IsLanguageSupported(TInt aLang);

        /**
         * Returns the pool Id for a character
         */
        TInt PoolIdForCharacter(const TChar& aChar, TKeyboardModes aKbMode);

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
         * Get physical Itu-T and QWERTY keyboard types of the device
         */
        void GetPhysicalKeyboardTypesL( TPtiKeyboardType& aItutKbType,
                                        TPtiKeyboardType& aQwertyKbType );

        /**
         * Returns the pool Id for a key
         */
        TInt PoolIdForKey(const TPtiKey aKey, TKeyboardModes aKbMode);

        /**
         * Helper function to construct Key Mappings
         */
        void CreateKeyMappingL( TKeyboardModes aKbMode );

        /**
         * Create list of PTI keys for the given keyboard type
         */
        void CreateKeyListFromKeyBindingTable( RArray<TPtiKey>& aKeyArray, TPtiKeyboardType aKbType, CPtiEngine* aPtiEngine );

        /**
         * Add a non-existing key ID in the end of given array
         */
        void AppendEntryWithFakeKeyToKeyList( RArray<TPtiKey>& aKeyArray );

        /**
         * Helper functions to populate Keyboard Key Mappings (Characters for
         * all Keys) for the given Language
         */
        void AddKeyMappingForLanguageL( CPtiEngine* aPtiEngine, 
                                        TLanguage aLanguage,
                                        TPtiKeyboardType aKbType,
                                        const RArray<TPtiKey>& aPtiKeys,
                                        RPointerArray<TKeyMappingData>& aResultMapping );
        void GetCharactersForKey( CPtiCoreLanguage& aPtiLanguage,
                                  TPtiKeyboardType aKbType,
                                  TPtiKey aKey,
                                  TPtiTextCase aTextCase,
                                  TDes& aResult );
        void AddCharactersToKey( CPtiCoreLanguage& aPtiLanguage,
                                 TPtiKeyboardType aKbType,
                                 TPtiKey aKey,
                                 TPtiTextCase aTextCase,
                                 TKeyMappingData& aKeyDataList,
                                 RArray<TUint>& aKeyMapLang,
                                 TBool& aIsSingleCharForKey,
                                 TUint& aSingleChar );

        /**
         * Sets attribute to true if the characters "0" and " " are on the same key
         */
        void SetSpaceAndZeroOnSameKey();

        /**
         * Gets predictive keyboard mapping data for given match mode.
         * @param   aKbMode      Matching mode for which data is asked. 
         * @param   aPtiKeys     On return, will point to the array of PtiKeys for the given mode.
         * @param   aKeyMappings On return, will point to key mapping array for the given mode.
         * @param   aKbType      On return, will hold the keyboard type for the given mode.
         */
        void GetPredictiveKeyboardData( TKeyboardModes aKbMode, 
                                        RArray<TPtiKey>*& aPtiKeys, 
                                        RPointerArray<TKeyMappingData>*& aKeyMappings, 
                                        TPtiKeyboardType& aKbType );
        /**
         * Constant overload of the GetPredictiveKeyboardData function.
         */
        void GetPredictiveKeyboardData( TKeyboardModes aKbMode, 
                                        const RArray<TPtiKey>*& aPtiKeys, 
                                        const RPointerArray<TKeyMappingData>*& aKeyMappings, 
                                        TPtiKeyboardType& aKbType ) const;

        /**
         * Resolve keyboard mode to "ITU-T predictive" or "QWERTY predictive".
         */
        TKeyboardModes ResolveKeyboardMode( TKeyboardModes aKbMode,
                                            TKeyboardModes aKbModeToResolve ) const;


#ifdef _DEBUG
        /**
         * Check potential error conditions for KeyForCharacterMultiMatch
         */
        void CheckPotentialErrorConditions(const RArray<TInt>& aPoolIndexArr, 
                                           const TChar& aChar,
                                           const RArray<TPtiKey>& aPtiKeys,
                                           const RPointerArray<TKeyMappingData>& aKeyMappings) const;
#endif // _DEBUG

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
         * Type of keyboard used in ITU-T search mode. Typically this is
         * standard ITU-T 12-key keypad.
         */
        TPtiKeyboardType iItutKeyboardType;
        
        /**
         * Type of keyboard used in QWERTY search mode. Typically this is either
         * 3x11 or 4x10 QWERTY or EPtiKeyboardNone if there's no any kind of
         * QWERTY available. This may also be half-QWERTY.
         */
        TPtiKeyboardType iQwertyKeyboardType;

        /**
         * Type of keyboard used in Default Preditive search mode. This will
         * point to ITU-T or QWERTY keypad.
         */
        TKeyboardModes iPredictiveDefaultKeyboardMode;

        /**
         * Pti keys of ITU-T mode
         */
        RArray<TPtiKey> iItutKeys;

        /**
         * Pti keys of QWERTY mode
         */
        RArray<TPtiKey> iQwertyKeys;

        /**
         * Contains all the keys and the characters mapped to each key in ITU-T mode. Owned.
         */
        RPointerArray<TKeyMappingData> iItutKeyMaps;

        /**
         * Contains all the keys and the characters mapped to each key in QWERTY mode. Owned.
         */
        RPointerArray<TKeyMappingData> iQwertyKeyMaps;

        /**
         * True if "0" and " " are on the same key in the ITU-T mode
         */
        TBool iSpaceAndZeroOnSameKeyOnItut;
        
        /**
         * True if "0" and " " are on the same key in the QWERTY mode
         */
        TBool iSpaceAndZeroOnSameKeyOnQwerty;

};

#endif // __CPCS_KEY_MAP_H__

// End of file
