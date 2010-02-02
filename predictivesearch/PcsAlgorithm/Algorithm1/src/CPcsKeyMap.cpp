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
* Description:  Retrieves the character map for each of the numeric keys.
*               Uses services provided by the PTI Engine.
*
*/

// INCLUDE FILES
#include "CPcsDebug.h"
#include "CPcsKeyMap.h"
#include <CPcsDefs.h>
#include <bldvariant.hrh>
#include <PtiDefs.h>
#include <PtiKeyMappings.h>
#include <PtiKeyMapData.h>
#include <AknFepInternalCRKeys.h>
#include <AvkonInternalCRKeys.h>
#include <centralrepository.h>
#include <CPsQuery.h>
#include <CPsQueryItem.h>

// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
    enum TPanicCode
    {
        EPanicPreCond_MultipleSingleCharMatching = 1,
        EPanicPreCond_MultipleUIPriorityMatching = 2,
        EPanicPreCond_MultipleEnglishPriorityMatching = 3,
        EPanicPreCond_MultipleOthersPriorityMatching = 4,
   };

    void Panic(TInt aReason)
    {
        _LIT(KPanicText, "CPcsKeyMap");
        User::Panic(KPanicText, aReason);
    }
#endif // DEBUG

} // namespace

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsKeyMap::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsKeyMap* CPcsKeyMap::NewL()
    {
    PRINT ( _L("Enter CPcsKeyMap::NewL") );

    CPcsKeyMap* self = new ( ELeave ) CPcsKeyMap();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    PRINT ( _L("End CPcsKeyMap::NewL") );

    return self;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::CPcsKeyMap
// Constructor
// ----------------------------------------------------------------------------
CPcsKeyMap::CPcsKeyMap()
    {
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ConstructL
// 2nd Phase Constructor
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructL()
    {
    PRINT ( _L("Enter CPcsKeyMap::ConstructL") );

    // UI Language
    iUILanguage = User::Language();
    
    // List of non-supported languages for this Algorithm
    iLanguageNotSupported.Append(ELangJapanese);
    iLanguageNotSupported.Append(ELangPrcChinese);
    iLanguageNotSupported.Append(ELangHongKongChinese);
    iLanguageNotSupported.Append(ELangTaiwanChinese);

#ifdef RD_INTELLIGENT_TEXT_INPUT

    TInt physicalKeyboard = 0;
    CRepository* aknFepRepository = CRepository::NewL( KCRUidAknFep );
    aknFepRepository->Get( KAknFepPhysicalKeyboards, physicalKeyboard );
    delete aknFepRepository;

    PRINT1 ( _L("CPcsKeyMap::ConstructL: Physical keyboard support flag = 0x%02X"), physicalKeyboard );

    // Constants follow the definition of KAknFepPhysicalKeyboards
    const TInt ptiKeyboard12Key = 0x01;
    //const TInt ptiKeyboardQwerty4x12 = 0x02;    // Not used at the moment
    const TInt ptiKeyboardQwerty4x10 = 0x04;
    const TInt ptiKeyboardQwerty3x11 = 0x08;
    const TInt ptiKeyboardHalfQwerty = 0x10;
    //const TInt ptiKeyboardCustomQwerty = 0x20;  // Not used at the moment

    // The following if contains the order of precedence given for keyboards
    // i.e. one phone has both "ITUT 12 Key" and "Qwerty 4x10" physical keyboards
    if ( physicalKeyboard & ptiKeyboard12Key )
        {
        iKeyboardType = EPtiKeyboard12Key;
        }
    else if ( physicalKeyboard & ptiKeyboardHalfQwerty )
        {
        iKeyboardType = EPtiKeyboardHalfQwerty;
        }
    else if ( physicalKeyboard & ptiKeyboardQwerty4x10 )
        {
        iKeyboardType = EPtiKeyboardQwerty4x10;
        }
    else if ( physicalKeyboard & ptiKeyboardQwerty3x11 )
        {
        iKeyboardType = EPtiKeyboardQwerty3x11;
        }
    else
#endif // RD_INTELLIGENT_TEXT_INPUT
        {
        iKeyboardType = EPtiKeyboard12Key; // default
        }

    PRINT1 ( _L("CPcsKeyMap::ConstructL: Keyboard chosen for Predictive Search = %d"), iKeyboardType );

    // Create structure for holding characters<-->key mappings
    CreateKeyMappingL();

    // Sets attribute for holding info if "0" and " " are on the same key
    // Needed for decision if the "0" should be considered as a possible separator
    SetSpaceAndZeroOnSameKey();
    
    PRINT ( _L("CPcsKeyMap::ConstructL: ----------------------------------------"));

    PRINT ( _L("End CPcsKeyMap::ConstructL") );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::~CPcsKeyMap
// Destructor
// ----------------------------------------------------------------------------
CPcsKeyMap::~CPcsKeyMap()
    {
    PRINT ( _L("Enter CPcsKeyMap::~CPcsKeyMap") );

    // Cleanup local arrays
    iLanguageNotSupported.Reset();

    for (TInt i = 0; i < PoolCount(); i++)
        {
        for (TInt j = 0; j < TKeyMappingData::EKeyMapNumberArr; j++)
            {
            iKeyMapPtrArr[i]->iKeyMapCharArr[j].Close();
            }
        }
    iKeyMapPtrArr.ResetAndDestroy();
    iKeysArr.Close();

    PRINT ( _L("End CPcsKeyMap::~CPcsKeyMap") );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetMixedKeyStringForQueryL
// aDestStr will have the length as the number of items in aSrcQuery.
// ----------------------------------------------------------------------------
void CPcsKeyMap::GetMixedKeyStringForQueryL(CPsQuery& aSrcQuery, TDes& aDestStr)
{
    PRINT ( _L("Enter CPcsKeyMap::GetMixedKeyStringForQueryL") ); 

    GetMixedKeyStringForDataL( aSrcQuery, aSrcQuery.QueryAsStringLC(), aDestStr );
    CleanupStack::PopAndDestroy(); //result of QueryAsStringLC

    PRINT ( _L("End CPcsKeyMap::GetMixedKeyStringForQueryL") );
}

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetMixedKeyStringForDataL
// aDestStr will have the same length as aSrcData. aSrcQuery can be shorter.
// ----------------------------------------------------------------------------
void CPcsKeyMap::GetMixedKeyStringForDataL(
        CPsQuery& aSrcQuery, const TDesC& aSrcData, TDes& aDestStr)
{
    PRINT ( _L("Enter CPcsKeyMap::GetMixedKeyStringForDataL") );

    for ( TInt i = 0; i < aSrcData.Length(); ++i )
        {
        TChar character( aSrcData[i] );
        character.LowerCase();
        if ( i < aSrcQuery.Count() )
            {
            CPsQueryItem& currentItem = aSrcQuery.GetItemAtL(i);
            switch ( currentItem.Mode() )
                {
                case EItut:
                    {
                    TPtiKey key = KeyForCharacterMultiMatch(aSrcData[i]);
                    // If a character is not mapped to numeric key append the character
                    if ( EPtiKeyNone == key )
                        {
                        PRINT3 ( _L("CPcsKeyMap::GetMixedKeyStringForDataL: Char at index %d not mapped to a key, appending char '%c' (#%d)"),
                                 i, (TUint) character, (TUint) character );

                        aDestStr.Append( character );
                        }
                    else 
                        {
                        aDestStr.Append( key );
                        }
                    }
                    break;
                case EQwerty:
                    //fall through
                default:
                    PRINT2 ( _L("CPcsKeyMap::GetMixedKeyStringForDataL: Char '%c' (#%d) is taken exact (non predictive in query)"),
                            (TUint) character, (TUint) character );
                    aDestStr.Append( character );
                    break;
                }
            }
        else
            {            
            PRINT2 ( _L("CPcsKeyMap::GetMixedKeyStringForDataL: Char '%c' (#%d) is taken exact (over query length)"),
                    (TUint) character, (TUint) character );

            aDestStr.Append( character );
            }
        }

    PRINT1 ( _L("CPcsKeyMap::GetMixedKeyStringForDataL: Return string: \"%S\""),
             &aDestStr );

    PRINT ( _L("End CPcsKeyMap::GetMixedKeyStringForDataL") );
}

// ----------------------------------------------------------------------------
// CPcsKeyMap::CharacterForKeyMappingExists
// Returns true if the character is mapped to the key
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::CharacterForKeyMappingExists(TKeyMappingData& aKeyMap, const TUint aIntChar)
    {
    TBool found = EFalse;
    
    for (TInt j = TKeyMappingData::EKeyMapUILangArr; j <= TKeyMappingData::EKeyMapOthersLangArr; j++)
        {
        if ( KErrNotFound != aKeyMap.iKeyMapCharArr[j].Find(aIntChar) )
            {
            found = ETrue;
            break;
            }
        
        }
    
    return found;
    }

#ifdef _DEBUG
// ----------------------------------------------------------------------------
// CPcsKeyMap::CheckPotentialErrorConditions
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::CheckPotentialErrorConditions(RArray<TInt>& aPoolIndexArr, const TChar& aChar)
    {
    PRINT ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: ===================================================") );
    PRINT ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Checking potential error conditions") );
    PRINT ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: ---------------------------------------------------") );

    _LIT(KTextCharArr0, "is single char");
    _LIT(KTextCharArr1, "has \"UI\" priority");
    _LIT(KTextCharArr2, "has \"English\" priority");
    _LIT(KTextCharArr3, "has \"Others\" priority");
    const TPtrC charArrStr[] =
        { KTextCharArr0(), KTextCharArr1(), KTextCharArr2(), KTextCharArr3() };
    
    TFixedArray<TUint, TKeyMappingData::EKeyMapNumberArr> countArr;

    for ( TInt j = TKeyMappingData::EKeyMapSingleCharArr; j <= TKeyMappingData::EKeyMapOthersLangArr; j++ )
        {
        countArr[j] = 0;
        for ( TInt i = 0; i < aPoolIndexArr.Count(); i++ )
            {
            if ( KErrNotFound != iKeyMapPtrArr[aPoolIndexArr[i]]->iKeyMapCharArr[j].Find((TUint) aChar) )
                {
                PRINT5 ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Char '%c' (#%d) %S for pool %d with key '%c'"),
                        (TUint) aChar, (TUint) aChar, &charArrStr[j], aPoolIndexArr[i], iKeysArr[aPoolIndexArr[i]] );
                countArr[j]++;
                }
            }
        }

    PRINT ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: ===================================================") );
    
#ifdef __WINS__
    // Check in debug mode if we have wrong situations
    __ASSERT_DEBUG( (countArr[TKeyMappingData::EKeyMapSingleCharArr]  > 1), Panic(EPanicPreCond_MultipleSingleCharMatching) );
    __ASSERT_DEBUG( (countArr[TKeyMappingData::EKeyMapUILangArr]      > 1), Panic(EPanicPreCond_MultipleUIPriorityMatching) );
    __ASSERT_DEBUG( (countArr[TKeyMappingData::EKeyMapEnglishLangArr] > 1), Panic(EPanicPreCond_MultipleEnglishPriorityMatching) );
    __ASSERT_DEBUG( (countArr[TKeyMappingData::EKeyMapOthersLangArr]  > 1), Panic(EPanicPreCond_MultipleOthersPriorityMatching) );
#endif // __WINS__
    }
#endif // _DEBUG        

// ----------------------------------------------------------------------------
// CPcsKeyMap::KeyForCharacterMultiMatch
// Considers possible that the same character can be in more pools.
// This case was verified at least in the following verified cases:
// - For some language version (i.e. Scandinavian with keys 'ä', 'å', 'ö'.
// - For some special characters in ITU-T 12 keys keyboards.
//   Example: '$' is under key '1' and '7', '£' is under key '1' and '5'.
// In case the character is in more pools the selection strategy is the following:
//   - 1st choice: if the keyboard is EPtiKeyboard12Key exclude the pool with '1'.
//   - 2nd choice: choose the 1st pool that has for a language the char as single char.
//   - 3rd choice: choose the 1st pool that has the char mapped for the UI language.
//   - 4th choice: choose the 1st pool that has the char mapped for the English language.
//   - 5th choice: choose the 1st pool that has the char mapped for the Other languages.
// ----------------------------------------------------------------------------
TPtiKey CPcsKeyMap::KeyForCharacterMultiMatch(const TChar& aChar)
    {
    // Set an array of pool index matches (more matches are possible)
    RArray<TInt> poolIndexArr;
    for ( TInt i = 0; i < PoolCount(); i++ )
        {
        if ( CharacterForKeyMappingExists(*iKeyMapPtrArr[i], (TUint) aChar) )
            {
            poolIndexArr.Append(i);

            PRINT4 ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Char '%c' (#%d) found in pool %d with key '%c'"),
                    (TUint) aChar, (TUint) aChar, i, iKeysArr[i] );
            }
        }

    /* Choose the pool index depending on the number of matches.
     * If there are 2 or more matches, then the choice of the pool index depends on
     * a serie of conditions.
     */
    TInt poolIndex = KErrNotFound;

    // Character not found in any pool
    if ( poolIndexArr.Count() == 0 )
        {
        PRINT2 ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Char '%c' (#%d) NOT found in all pools"),
                 (TUint) aChar, (TUint) aChar );
        }

    // Character found in one pool (normal case)
    else if ( poolIndexArr.Count() == 1 )
        {
        poolIndex = poolIndexArr[0];

        PRINT2 ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Chosen (for unique match) pool %d with key '%c'"),
                poolIndex, iKeysArr[poolIndex] );
        }

    /* Character found in more pools, this can happen in some known conditions:
     * - In some variants 'Ä' is in key 'A' and in key 'Ä'
     * - In ITUT keyboard '$' is in keys '1' and '7'. This happens for more chars.
     * - For different languages in N97 '/' is in different keys.
     */
    else // ( poolIndexArr.Count() > 1 )
        {

        /* If keybord is ITUT we exclude the key '1' from the posibility of being choosen,
         * infact some characters can be typed by key '1' and another key.
         * For example:
         *     '$' is under key '1' and '7',
         *     '£' is under key '1' and '5'.
         *
         * This is needed when the client passes updated chars to the engine.
         * i.e.: Existing contact are "!Mat" and "$Nathan",
         *       User types '7' and client passes '7' or 'p' matching only contact "$Nathan",
         *       In next query user types '2' and client passes '$2' or '$a' matching "!Mat" and "$Nathan".
         *       This is wrong, as "!Mat" should not be matched.
         * When the client will not pass updated chars to the engine, but the original (i.e.: "72" or "pa"),
         * this piece of code shall be removed.
         * With "client" it is referred to MCL contacts client.
         */
        if (EPtiKeyboard12Key == iKeyboardType)
            {
            for ( TInt i = 0; i < poolIndexArr.Count() ; i++ )
                {
                if ( EPtiKey1 == iKeysArr[poolIndexArr[i]] )
                    {
                    poolIndexArr.Remove(i);
    
                    PRINT ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Key '1' excluded (for multi match - EPtiKeyboard12Key)") );
                    break;
                    }
                }
            if ( poolIndexArr.Count() == 1 )
                {
                poolIndex = poolIndexArr[0];

                PRINT2 ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Chosen (for unique match after removing key '1') pool %d with key '%c'"),
                        poolIndex, iKeysArr[poolIndex] );
                }
            }

#ifdef _DEBUG
        CheckPotentialErrorConditions(poolIndexArr, aChar);
#endif // _DEBUG        

        // Search the char in the char arrays in priority order, the 1st match is taken
        for ( TInt j = TKeyMappingData::EKeyMapSingleCharArr; j <= TKeyMappingData::EKeyMapOthersLangArr; j++ )
            {
            if ( KErrNotFound == poolIndex )
                {
                TInt positionLeftMostOnKeys = KErrNotFound; 

                for ( TInt i = 0; i < poolIndexArr.Count(); i++ )
                    {
                    TInt position = iKeyMapPtrArr[poolIndexArr[i]]->iKeyMapCharArr[j].Find((TUint) aChar);
                    if ( KErrNotFound != position )
                        {
                        // Get the key that has the char in the leftmost index.
                        // We consider that the char can be mapped in more than one key (this is really
                        // the case for instance in some Scandinavian variants).
                        // With this guess there is more possibility that we choose the key where the
                        // char is physically printed on the keyboard key.
                        // In order for this guessing strategy to work, chars for TPtiTextCase EPtiCaseUpper
                        // have to be added to the char arrays before any other TPtiTextCase.
                        if ( TKeyMappingData::EKeyMapUILangArr == j )
                            {
                            if ( ( KErrNotFound == positionLeftMostOnKeys ) || ( position < positionLeftMostOnKeys ) )
                                {
                                poolIndex = poolIndexArr[i];
                                positionLeftMostOnKeys = position;
                                }

                            }
                        // Get the 1st key that has the char mapped to it
                        else
                            {
                            poolIndex = poolIndexArr[i];
                    
                            PRINT3 ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Chosen (for multi match - char arr: %d) pool %d with key '%c'"),
                                     j, poolIndex, iKeysArr[poolIndex] );
                        
                            break;
                            }
                        }
                    }
                }
            }
                    
        } // else -> ( poolIndexArr.Count() > 1 )

    // Set the key value from the pool index
    TPtiKey key = EPtiKeyNone;
    if ( KErrNotFound != poolIndex )
        {
        key = iKeysArr[poolIndex];
        }
    
    poolIndexArr.Close();
    return key;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::PoolIdForKey
//
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::PoolIdForKey(const TPtiKey aKey)
    {
    TInt poolId = iKeysArr.Find(aKey);

    // IF the key is not found, then it should go to the special pool,
    // which is the last pool of iKeyMapPtrArr
    if (KErrNotFound == poolId)
        {
        poolId = PoolCount() - 1;
        }

    return poolId;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::PoolIdForCharacter
//
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::PoolIdForCharacter(const TChar& aChar)
    {
    TPtiKey key = KeyForCharacterMultiMatch(aChar);

    TInt poolId = PoolIdForKey(key);

    return poolId;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::PoolCount
//
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::PoolCount()
    {
    return iKeyMapPtrArr.Count();
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::SetSpaceAndZeroOnSameKey
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::SetSpaceAndZeroOnSameKey()
    {
    const TInt KSpace = 32; // ASCII for " "
    const TInt KZero  = 48; // ASCII for "0"

    TChar charSpace(KSpace);
    TChar charZero(KZero);

    TPtiKey keySpace = KeyForCharacterMultiMatch(charSpace);
    TPtiKey keyZero = KeyForCharacterMultiMatch(charZero);

    iSpaceAndZeroOnSameKey = (keySpace == keyZero);
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetSpaceAndZeroOnSameKey
// 
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::GetSpaceAndZeroOnSameKey()
    {
    return iSpaceAndZeroOnSameKey;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetKeyboardKeyMapping
// Helper function to get Key Mappings depending on keyboard type
// ----------------------------------------------------------------------------
MPtiKeyMappings* CPcsKeyMap::GetKeyboardKeyMapping(CPtiCoreLanguage& aCurrLanguage)
    {
    MPtiKeyMappings* ptiKeyMappings;

    switch ( iKeyboardType )
        {
        case EPtiKeyboardQwerty4x12:
        case EPtiKeyboardQwerty4x10:
        case EPtiKeyboardQwerty3x11:
        case EPtiKeyboardCustomQwerty:
            ptiKeyMappings = aCurrLanguage.GetQwertyKeymappings();
            break;

        case EPtiKeyboard12Key:
            ptiKeyMappings = aCurrLanguage.GetKeymappings();
            break;

        case EPtiKeyboardHalfQwerty:
            ptiKeyMappings = aCurrLanguage.GetHalfQwertyKeymappings();
            break;

        default:
            ptiKeyMappings = aCurrLanguage.GetKeymappings();
            break;
        }

    return ptiKeyMappings;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetKeyMapData
//
// ----------------------------------------------------------------------------
CPtiKeyMapData* CPcsKeyMap::GetKeyMapData(CPtiCoreLanguage& aCurrLanguage)
    {
    MPtiKeyMappings* ptiKeyMappings = GetKeyboardKeyMapping( aCurrLanguage );

    CPtiKeyMapData* keyMapData;

    switch ( iKeyboardType )
        {
        case EPtiKeyboardQwerty4x12:
        case EPtiKeyboardQwerty4x10:
        case EPtiKeyboardQwerty3x11:
        case EPtiKeyboardCustomQwerty:
            keyMapData = static_cast<CPtiQwertyKeyMappings*>(ptiKeyMappings)->KeyMapData();
            break;

        case EPtiKeyboard12Key:
            keyMapData = static_cast<CPtiKeyMappings*>(ptiKeyMappings)->KeyMapData();
            break;

        case EPtiKeyboardHalfQwerty:
            keyMapData = static_cast<CPtiHalfQwertyKeyMappings*>(ptiKeyMappings)->KeyMapData();
            break;

        default:
            keyMapData = static_cast<CPtiKeyMappings*>(ptiKeyMappings)->KeyMapData();
            break;
        }

    return keyMapData;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AppendEntryWithFakeKeyToKeyList
// Add pool with unused id for for key
// ----------------------------------------------------------------------------
void CPcsKeyMap::AppendEntryWithFakeKeyToKeyList()
    {
    TUint keyUInt = (TUint) EPtiKeyNone + 1;

    while ( KErrNotFound != iKeysArr.Find( (TPtiKey) keyUInt) )
        {
        keyUInt++;
        }

    TPtiKey key = (TPtiKey) keyUInt;
    iKeysArr.Append( key );

    // This should always be the last one in the array
    PRINT2 ( _L("CPcsKeyMap::AppendEntryWithFakeKeyToKeyList: Added additional last pool %d with key id #%d"),
             iKeysArr.Count()-1, key );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::CreateKeyMapFromITUTHardcodedKeys
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::CreateKeyListFromITUTHardcodedKeys()
    {
    PRINT ( _L("Enter CPcsKeyMap::CreateKeyListFromITUTHardcodedKeys") );

    iKeysArr.Append( EPtiKey0 );
    iKeysArr.Append( EPtiKey1 );
    iKeysArr.Append( EPtiKey2 );
    iKeysArr.Append( EPtiKey3 );
    iKeysArr.Append( EPtiKey4 );
    iKeysArr.Append( EPtiKey5 );
    iKeysArr.Append( EPtiKey6 );
    iKeysArr.Append( EPtiKey7 );
    iKeysArr.Append( EPtiKey8 );
    iKeysArr.Append( EPtiKey9 );

    AppendEntryWithFakeKeyToKeyList();

    PRINT ( _L("End CPcsKeyMap::CreateKeyListFromITUTHardcodedKeys") );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::CreateKeyMapFromKeyBindingTable
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::CreateKeyListFromKeyBindingTable( CPtiEngine* aPtiEngine )
    {
    PRINT ( _L("Enter CPcsKeyMap::CreateKeyListFromKeyBindingTable") );

    // Get the user language
    TLanguage keyMapLanguage = iUILanguage;
    PRINT1 ( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: Current User Language is %d"),
            keyMapLanguage  );

    // If the user language is not supported, then we use ELangEnglish as default
    if (!IsLanguageSupported((TInt) keyMapLanguage))
        {
        PRINT2 ( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: Language %d is not supported, defaulting to %d"),
                keyMapLanguage, ELangEnglish );

        keyMapLanguage = ELangEnglish;
        }

    CPtiCoreLanguage* currLanguage = static_cast<CPtiCoreLanguage*>(aPtiEngine->GetLanguage( keyMapLanguage ));

    // If we fail to get the language, we try again trying with ELangEnglish
    if ( (!currLanguage) && (ELangEnglish != keyMapLanguage) )
        {
        PRINT1 ( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: Failed to get Current Language, trying with %d"),
                 ELangEnglish );

        currLanguage = static_cast<CPtiCoreLanguage*>(aPtiEngine->GetLanguage( ELangEnglish ));
        }

    if (currLanguage)
        {
        TInt numItems = 0;
        const TPtiKeyBinding* table = GetKeyMapData(*currLanguage)->KeyBindingTable(iKeyboardType, numItems);
        
        PRINT1 ( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: Num of Items in KeyBindingTable is %d"), numItems );

        // Get from the key table the keys for constructing the pools
        if (table)
            {
            for (TInt i = 0; i < numItems; i++)
                {
                TPtiKey key = (TPtiKey) table[i].iScanCode;
                // Get all keys with same EPtiCaseLower or EPtiCaseUpper case
                // Only for one of the casing to avoid repetitions
                if ( (EPtiKeyNone != key) && (EPtiCaseLower == table[i].iCase) )
                    {
                    PRINT3 ( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: Adding pool %d with key '%c' (%d)"),
                            iKeysArr.Count(), key, key );
                    iKeysArr.Append( key );
                    }
                }
            }
        else
            {
            PRINT ( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: ##### Failed to create Key List (KeyBindingTable) #####") );
            }
        }
    else
        {
        PRINT ( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: ##### Failed to create Key List (Language) #####") );
        }

    AppendEntryWithFakeKeyToKeyList();

    PRINT ( _L("End CPcsKeyMap::CreateKeyListFromKeyBindingTable") );
}

// ----------------------------------------------------------------------------
// CPcsKeyMap::CreateKeyListL
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::CreateKeyListL( CPtiEngine* aPtiEngine )
    {
    PRINT ( _L("Enter CPcsKeyMap::CreateKeyListL") );

    switch ( iKeyboardType )
        {
        case EPtiKeyboardQwerty4x12:
        case EPtiKeyboardQwerty4x10:
        case EPtiKeyboardQwerty3x11:
        case EPtiKeyboardCustomQwerty:
            CreateKeyListFromKeyBindingTable( aPtiEngine );
            break;

        case EPtiKeyboard12Key:
            CreateKeyListFromITUTHardcodedKeys( );
            break;

        case EPtiKeyboardHalfQwerty:
            CreateKeyListFromKeyBindingTable( aPtiEngine );
            break;

        default:
            CreateKeyListFromKeyBindingTable( aPtiEngine );
            break;
        }

    PRINT ( _L("End CPcsKeyMap::CreateKeyListL") );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::IsLanguageSupported
// Returns ETrue if this language is supported
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::IsLanguageSupported(TInt aLang)
    {
    return (KErrNotFound == iLanguageNotSupported.Find((TLanguage) aLang));
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::CreateKeyMappingL
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::CreateKeyMappingL()
    {
    PRINT ( _L("Enter CPcsKeyMap::CreateKeyMappingL") );

    // Instantiate the engine
    CPtiEngine* ptiEngine = CPtiEngine::NewL(ETrue);
    CleanupStack::PushL( ptiEngine );

    ptiEngine->SetKeyboardType( iKeyboardType );

    CreateKeyListL( ptiEngine );

    // Now add the keymap arrays to hold the keymap data
    for (TInt i = 0; i < iKeysArr.Count(); i++)
    {
        TKeyMappingData *keyData = new(ELeave) TKeyMappingData;
        iKeyMapPtrArr.Append(keyData);
    }

    // Get the available Languages on the phone
    RArray<TInt> LanguagesOnThisPhone;
    ptiEngine->GetAvailableLanguagesL(LanguagesOnThisPhone);
    PRINT2 ( _L("CPcsKeyMap::CreateKeyMappingL: Languages on this phone %d, maximum is set to %d"),
             LanguagesOnThisPhone.Count(), KMaxNbrOfLangKeymapping );
    
    // Remove the non-supported languages
    for (TInt i = 0; i < LanguagesOnThisPhone.Count(); /* do not increment i */)
        {
        if ( (IsLanguageSupported(LanguagesOnThisPhone[i]))
#ifdef __WINS__
             && (ELangEnglish == LanguagesOnThisPhone[i]) // Only English for emulator 
#endif // __WINS__
           )
            {
            i++;
            }
        else
            {
            PRINT1 ( _L("CPcsKeyMap::CreateKeyMappingL: Removing not supported Language %d"),
                     LanguagesOnThisPhone[i] );
            LanguagesOnThisPhone.Remove(i);
            }
        }

    // Set current UI language as 1st language and English as 2nd language,
    // if present in AvailableLanguages
    TInt langIndex;
    if ( KErrNotFound != (langIndex = LanguagesOnThisPhone.Find(ELangEnglish)) )
        {
        LanguagesOnThisPhone.Remove(langIndex);
        LanguagesOnThisPhone.Insert(ELangEnglish, 0);
        }
    if ( KErrNotFound != (langIndex = LanguagesOnThisPhone.Find(iUILanguage)) )
        {
        LanguagesOnThisPhone.Remove(langIndex);
        LanguagesOnThisPhone.Insert(iUILanguage, 0);
        }

    // Set max number of languages for Key Map
    TInt count = LanguagesOnThisPhone.Count();
    if (count > KMaxNbrOfLangKeymapping)
        {
        PRINT2 ( _L("CPcsKeyMap::CreateKeyMappingL: Supported Languages on this phone %d, limiting to %d"),
                 count, KMaxNbrOfLangKeymapping );
        count = KMaxNbrOfLangKeymapping;
        }

    // Add Key Map for the languages
    for (TInt i = 0; i < count; i++)
        {
        TLanguage languageToUse = (TLanguage) LanguagesOnThisPhone[i];

        PRINT1 ( _L("CPcsKeyMap::CreateKeyMappingL: Constructing Key Map for Language %d"), languageToUse );
        TInt errStatus = ptiEngine->ActivateLanguageL(languageToUse);
        if (KErrNone == errStatus)
            {
            TRAP_IGNORE(AddKeyMappingForActiveLanguageL(ptiEngine, languageToUse));
            }
        else
            {
            PRINT1 ( _L("CPcsKeyMap::CreateKeyMappingL: ##### Unable to activate Language %d #####"),
                     languageToUse );
            }

        // Close the currently activated language
        ptiEngine->CloseCurrentLanguageL();

        PRINT ( _L("CPcsKeyMap::CreateKeyMappingL: ===================================================") );
        }

    LanguagesOnThisPhone.Close();

    CleanupStack::PopAndDestroy( ptiEngine );

    PRINT ( _L("End CPcsKeyMap::CreateKeyMappingL") );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddKeyMappingForActiveLanguageL
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddKeyMappingForActiveLanguageL(CPtiEngine* aPtiEngine, TLanguage aLanguage)
{
    PRINT ( _L("Enter CPcsKeyMap::AddKeyMappingForActiveLanguageL") );

    // Make a language object based on the language
    CPtiCoreLanguage* activeLanguage = static_cast<CPtiCoreLanguage*>(aPtiEngine->GetLanguage( aLanguage ));

    if (activeLanguage)
        {
        // Get the keyboard Mappings for the Active Language
        MPtiKeyMappings* ptiKeyMappings = GetKeyboardKeyMapping( *activeLanguage );

        for (TInt i = 0; i < PoolCount() - 1; i++)
            {
            TPtiKey key = iKeysArr[i];

            PRINT ( _L("CPcsKeyMap::AddKeyMappingForActiveLanguageL: ===================================================") );
            PRINT4 ( _L("CPcsKeyMap::AddKeyMappingForActiveLanguageL: Adding characters for Key '%c' (%d) at Pool %d and Language %d"),
                     (TInt) key, (TInt) key, PoolIdForKey(key), aLanguage );

            // Get the pointer to the language class (UI, English, Others)
            RArray<TUint>* keyMapLang;
            if (aLanguage == iUILanguage)
                {
                keyMapLang = &(iKeyMapPtrArr[i]->iKeyMapCharArr[TKeyMappingData::EKeyMapUILangArr]);
                PRINT1 ( _L("CPcsKeyMap::AddKeyMappingForActiveLanguageL: Language %d is the UI language"), aLanguage );
                }
            else if (aLanguage == ELangEnglish)
                {
                // If (iUILanguage == ELangEnglish) ok to be in previous if case
                keyMapLang = &(iKeyMapPtrArr[i]->iKeyMapCharArr[TKeyMappingData::EKeyMapEnglishLangArr]);
                PRINT1 ( _L("CPcsKeyMap::AddKeyMappingForActiveLanguageL: Language %d is English language"), aLanguage );
                }
            else
                {
                keyMapLang = &(iKeyMapPtrArr[i]->iKeyMapCharArr[TKeyMappingData::EKeyMapOthersLangArr]);
                PRINT1 ( _L("CPcsKeyMap::AddKeyMappingForActiveLanguageL: Language %d is in the Other languages"), aLanguage );
                }
            
            PRINT ( _L("CPcsKeyMap::AddKeyMappingForActiveLanguageL: ---------------------------------------------------") );

            TBool isSingleCharForKey = ETrue;
            TUint singleChar = 0;

            // EPtiCaseUpper must be the 1st TPtiTextCase for iUILanguage
            AddCharactersToKey( *ptiKeyMappings, key, EPtiCaseUpper, *iKeyMapPtrArr[i], *keyMapLang, isSingleCharForKey, singleChar );
            AddCharactersToKey( *ptiKeyMappings, key, EPtiCaseLower, *iKeyMapPtrArr[i], *keyMapLang, isSingleCharForKey, singleChar );

            // 1. No other TPtiTextCase values for ITUT keyboard
            // 2. No language variants handling for ITUT keyboard
            if ( EPtiKeyboard12Key != iKeyboardType )
                {
                AddCharactersToKey( *ptiKeyMappings, key, EPtiCaseFnLower,  *iKeyMapPtrArr[i], *keyMapLang, isSingleCharForKey, singleChar );
                AddCharactersToKey( *ptiKeyMappings, key, EPtiCaseFnUpper,  *iKeyMapPtrArr[i], *keyMapLang, isSingleCharForKey, singleChar );
                AddCharactersToKey( *ptiKeyMappings, key, EPtiCaseChrLower, *iKeyMapPtrArr[i], *keyMapLang, isSingleCharForKey, singleChar );
                AddCharactersToKey( *ptiKeyMappings, key, EPtiCaseChrUpper, *iKeyMapPtrArr[i], *keyMapLang, isSingleCharForKey, singleChar );

                // Support for key guessing given the char in some phone language variants
                if ( (isSingleCharForKey) && (0 != singleChar) )
                    {
                    iKeyMapPtrArr[i]->iKeyMapCharArr[TKeyMappingData::EKeyMapSingleCharArr].Append(singleChar); // singleChar is in LowerCase
                    iKeyMapPtrArr[i]->iKeyMapCharArr[TKeyMappingData::EKeyMapSingleCharArr].Append(User::UpperCase(singleChar));

                    PRINT ( _L("CPcsKeyMap::AddKeyMappingForActiveLanguageL: ---------------------------------------------------") );
                    PRINT5 ( _L("CPcsKeyMap::AddKeyMappingForActiveLanguageL: For Language %d and key '%c' of pool %d single char is '%c' (#%d)"),
                            aLanguage, (TInt) key, i, singleChar, singleChar );
                    }
                }
            }
        PRINT ( _L("CPcsKeyMap::AddKeyMappingForActiveLanguageL: ===================================================") );
        }

    PRINT ( _L("End CPcsKeyMap::AddKeyMappingForActiveLanguageL") );
}

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetCharactersForKey
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::GetCharactersForKey(MPtiKeyMappings& aPtiKeyMappings,
                                     TPtiKey aKey,
                                     TPtiTextCase aTextCase,
                                     TDes& aResult)
    {
    switch ( iKeyboardType )
        {
        case EPtiKeyboardQwerty4x12:
        case EPtiKeyboardQwerty4x10:
        case EPtiKeyboardQwerty3x11:
        case EPtiKeyboardCustomQwerty:
            (static_cast<CPtiQwertyKeyMappings*>(&aPtiKeyMappings))->GetDataForKey(aKey, aResult, aTextCase);
            break;

        case EPtiKeyboard12Key:
            (static_cast<CPtiKeyMappings*>(&aPtiKeyMappings))->GetDataForKey(aKey, aResult, aTextCase);
            break;

        case EPtiKeyboardHalfQwerty:
            (static_cast<CPtiHalfQwertyKeyMappings*>(&aPtiKeyMappings))->GetDataForKey(aKey, aResult, aTextCase);
            break;

        default:
            (static_cast<CPtiKeyMappings*>(&aPtiKeyMappings))->GetDataForKey(aKey, aResult, aTextCase);
            break;
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddDataForKeyL
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddCharactersToKey(MPtiKeyMappings& aPtiKeyMappings,
                                    TPtiKey aKey,
                                    TPtiTextCase aTextCase,
                                    TKeyMappingData& aKeyDataList,
                                    RArray<TUint>& aKeyMapLang,
                                    TBool& aIsSingleCharForKey,
                                    TUint& aSingleChar)
    {
    PRINT ( _L("Enter CPcsKeyMap::AddCharactersToKey") );

    TBuf<255> result;
    GetCharactersForKey(aPtiKeyMappings, aKey, aTextCase, result);

    PRINT3 ( _L("CPcsKeyMap::AddCharactersToKey: Get mapping chars for Key '%c' (%d) and TextCase %d"),
             (TInt) aKey, (TInt) aKey, aTextCase );
    PRINT1 ( _L("CPcsKeyMap::AddCharactersToKey: Got chars: \"%S\""), &result );

    for ( TInt i = 0; i < result.Length(); i++ )
        {
        if ( !CharacterForKeyMappingExists(aKeyDataList, (TUint) result[i]) )
            {
            PRINT1 ( _L("CPcsKeyMap::AddCharactersToKey: ----- Appending char to list: '%c'"), result[i] );
            aKeyMapLang.Append(result[i]);
            }
        else
            {
            PRINT1 ( _L("CPcsKeyMap::AddCharactersToKey: ***** NOT Appending char to list: '%c'"), result[i] );
            }

        // No language variants handling for ITUT keyboard
        if ( EPtiKeyboard12Key != iKeyboardType )
            {
            // Support for key guessing given the char in some phone language variants
            if ( aIsSingleCharForKey )
                {
                if ( 0 == aSingleChar )
                    {
                    aSingleChar = User::LowerCase(result[i]);
                    }
                else
                    {
                    TInt newChar = User::LowerCase(result[i]);
                    if (newChar != aSingleChar)
                        {
                        aSingleChar = 0;
                        aIsSingleCharForKey = EFalse;
                        }
                    }
                }
            }
        }

    PRINT ( _L("CPcsKeyMap::AddCharactersToKey: ---------------------------------------------------") );

    PRINT ( _L("End CPcsKeyMap::AddCharactersToKey") );
    }

// End of file
