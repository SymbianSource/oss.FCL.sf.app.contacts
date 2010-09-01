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
#include <PtiEngine.h>
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
        EPanic_OverflowInPoolIndex = 5,
        EPanic_InvalidKeyboardType = 6
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
    iLanguageNotSupported.Append(ELangKorean);

    TPtiKeyboardType physicalItutKbType;
    TPtiKeyboardType physicalQwertyKbType;    
    GetPhysicalKeyboardTypesL( physicalItutKbType, physicalQwertyKbType );

    iItutKeyboardType = EPtiKeyboard12Key; // We want to support Virtual Itu-T in most devices
    CreateKeyMappingL( EPredictiveItuT ); // Create structure for holding characters<-->key mappings
    if (iItutKeys.Count() == 0)
        {
        iItutKeyboardType = EPtiKeyboardNone;
        }

    iQwertyKeyboardType = physicalQwertyKbType;
    if (physicalQwertyKbType != EPtiKeyboardNone)
        {
        CreateKeyMappingL( EPredictiveQwerty ); // Create structure for holding characters<-->key mappings
        if (iQwertyKeys.Count() == 0)
            {
            iQwertyKeyboardType = EPtiKeyboardNone;
            }
        }

    // Set the Default Predictive keyboard mode. If Qwerty keyboard exists, it is always physical.
    iPredictiveDefaultKeyboardMode =
        (IsItutPredictiveAvailable() && physicalItutKbType != EPtiKeyboardNone) || !IsQwertyPredictiveAvailable() ?
            EPredictiveItuT : EPredictiveQwerty;

    PRINT1 ( _L("CPcsKeyMap::ConstructL: ITU-T Keyboard chosen for Predictive Search = %d"), iItutKeyboardType );
    PRINT1 ( _L("CPcsKeyMap::ConstructL: QWERTY Keyboard chosen for Predictive Search = %d"), iQwertyKeyboardType );

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

    for (TInt i = 0; i < iItutKeyMaps.Count(); i++)
        {
        for (TInt j = 0; j < TKeyMappingData::EKeyMapNumberArr; j++)
            {
            iItutKeyMaps[i]->iKeyMapCharArr[j].Close();
            }
        }
    iItutKeyMaps.ResetAndDestroy();
    
    for (TInt i = 0; i < iQwertyKeyMaps.Count(); i++)
        {
        for (TInt j = 0; j < TKeyMappingData::EKeyMapNumberArr; j++)
            {
            iQwertyKeyMaps[i]->iKeyMapCharArr[j].Close();
            }
        }
    iQwertyKeyMaps.ResetAndDestroy();
    
    iItutKeys.Close();
    iQwertyKeys.Close();

    PRINT ( _L("End CPcsKeyMap::~CPcsKeyMap") );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::IsItutPredictiveAvailable
// 
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::IsItutPredictiveAvailable() const
    {
    return ( iItutKeyboardType != EPtiKeyboardNone );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::IsQwertyPredictiveAvailable
// 
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::IsQwertyPredictiveAvailable() const
    {
    return ( iQwertyKeyboardType != EPtiKeyboardNone );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::IsModePredictive
// 
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::IsModePredictive( TKeyboardModes aKbMode )
    {
    return ( (EPredictiveDefaultKeyboard == aKbMode) ||
             (EPredictiveItuT == aKbMode) ||
             (EPredictiveQwerty == aKbMode) );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ResolveKeyboardMode
// Resolve EPredictiveDefaultKeyboard or ENonPredictive mode to EPredictiveItuT
// or EPredictiveQwerty mode
// ----------------------------------------------------------------------------
TKeyboardModes CPcsKeyMap::ResolveKeyboardMode( TKeyboardModes aKbMode,
                                                TKeyboardModes aKbModeToResolve ) const
    {    
    // Substitute "default predictive" mode with actual mode
    if ( (aKbMode == aKbModeToResolve) &&
         ((aKbMode == ENonPredictive) || (aKbMode == EPredictiveDefaultKeyboard)) )
        {
        return iPredictiveDefaultKeyboardMode;
        }
    else
        {
        return aKbMode;
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetMixedKeyStringForQueryL
// aDestStr will have the length as the number of items in aSrcQuery.
// ----------------------------------------------------------------------------
void CPcsKeyMap::GetMixedKeyStringForQueryL(
        CPsQuery& aSrcQuery, TDes& aDestStr) const
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
        CPsQuery& aSrcQuery, const TDesC& aSrcData, TDes& aDestStr) const
    {
    PRINT ( _L("Enter CPcsKeyMap::GetMixedKeyStringForDataL") );

    for ( TInt i = 0; i < aSrcData.Length(); ++i )
        {
        TChar character( aSrcData[i] );
        character.LowerCase();
        if ( i < aSrcQuery.Count() )
            {
            CPsQueryItem& currentItem = aSrcQuery.GetItemAtL(i);
            TPtiKey key = KeyForCharacterMultiMatch( aSrcData[i], currentItem.Mode() );
            // If a character is not mapped to any key or it's entered in non-predictive mode,
            // then append the character.
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
TBool CPcsKeyMap::CharacterForKeyMappingExists(
        TKeyMappingData& aKeyMap, TUint aIntChar) const
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
void CPcsKeyMap::CheckPotentialErrorConditions(const RArray<TInt>& aPoolIndexArr, 
                                               const TChar& aChar,
                                               const RArray<TPtiKey>& aPtiKeys,
                                               const RPointerArray<TKeyMappingData>& aKeyMappings) const
    {
    PRINT ( _L("CPcsKeyMap::CheckPotentialErrorConditions: ===================================================") );
    PRINT ( _L("CPcsKeyMap::CheckPotentialErrorConditions: Checking potential error conditions") );
    PRINT ( _L("CPcsKeyMap::CheckPotentialErrorConditions: ---------------------------------------------------") );

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
            if ( KErrNotFound != aKeyMappings[aPoolIndexArr[i]]->iKeyMapCharArr[j].Find((TUint) aChar) )
                {
                PRINT5 ( _L("CPcsKeyMap::CheckPotentialErrorConditions: Char '%c' (0x%04X) %S for pool %d with key '%c'"),
                        (TUint) aChar, (TUint) aChar, &charArrStr[j], aPoolIndexArr[i], aPtiKeys[aPoolIndexArr[i]] );
                countArr[j]++;
                }
            }
        }

    PRINT ( _L("CPcsKeyMap::CheckPotentialErrorConditions: ===================================================") );
    
#ifdef __WINS__
    /*
    The reference 4x10 QWERTY mappings of 9.2 emulator have each number mapped to two keys.
    That kind of mappings can't be handled correctly, so panic in debug would basically be correct.
    However, assertions are commented out for now to be able to do some testing.
    
    // Check in debug mode if we have wrong situations
    __ASSERT_DEBUG( (countArr[TKeyMappingData::EKeyMapSingleCharArr]  <= 1), Panic(EPanicPreCond_MultipleSingleCharMatching) );
    __ASSERT_DEBUG( (countArr[TKeyMappingData::EKeyMapUILangArr]      <= 1), Panic(EPanicPreCond_MultipleUIPriorityMatching) );
    __ASSERT_DEBUG( (countArr[TKeyMappingData::EKeyMapEnglishLangArr] <= 1), Panic(EPanicPreCond_MultipleEnglishPriorityMatching) );
    __ASSERT_DEBUG( (countArr[TKeyMappingData::EKeyMapOthersLangArr]  <= 1), Panic(EPanicPreCond_MultipleOthersPriorityMatching) );
    */
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
TPtiKey CPcsKeyMap::KeyForCharacterMultiMatch( const TChar& aChar, TKeyboardModes aKbMode ) const
    {    
    aKbMode = ResolveKeyboardMode( aKbMode, EPredictiveDefaultKeyboard ); 

    // Select key arrays to use according to keyboard mode
    const RArray<TPtiKey>* ptiKeyArray;
    const RPointerArray<TKeyMappingData>* keyMappingArray;
    TPtiKeyboardType kbType;
    GetPredictiveKeyboardData( aKbMode, ptiKeyArray, keyMappingArray, kbType );
    if ( !ptiKeyArray || !keyMappingArray || ptiKeyArray->Count() == 0 )
        {
        // No mappings available. This may be, for example, because aKbMode is non-predictive.
        // Return no mapping in that case to indicate that character should be treated in
        // non-predictive way.
        return EPtiKeyNone;
        }
    
    // Set an array of pool index matches (more matches are possible)
    RArray<TInt> poolIndexArr;
    for ( TInt i = 0; i < keyMappingArray->Count(); i++ )
        {
        if ( CharacterForKeyMappingExists(*(*keyMappingArray)[i], (TUint) aChar) )
            {
            poolIndexArr.Append(i);

            PRINT4 ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Char '%c' (0x%04X) found in pool %d with key '%c'"),
                    (TUint) aChar, (TUint) aChar, i, (*ptiKeyArray)[i] );
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
        PRINT2 ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Char '%c' (0x%04X) NOT found from any pool"),
                 (TUint) aChar, (TUint) aChar );
        }

    // Character found in one pool (normal case)
    else if ( poolIndexArr.Count() == 1 )
        {
        poolIndex = poolIndexArr[0];

        PRINT2 ( _L("CPcsKeyMap::KeyForCharacterMultiMatch: Chosen (for unique match) pool %d with key '%c'"),
                poolIndex, (*ptiKeyArray)[poolIndex] );
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
        if (EPtiKeyboard12Key == kbType)
            {
            for ( TInt i = 0; i < poolIndexArr.Count() ; i++ )
                {
                if ( EPtiKey1 == (*ptiKeyArray)[poolIndexArr[i]] )
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
                        poolIndex, (*ptiKeyArray)[poolIndex] );
                }
            }

#ifdef _DEBUG
        CheckPotentialErrorConditions(poolIndexArr, aChar, *ptiKeyArray, *keyMappingArray);
#endif // _DEBUG        

        // Search the char in the char arrays in priority order, the 1st match is taken
        for ( TInt j = TKeyMappingData::EKeyMapSingleCharArr; j <= TKeyMappingData::EKeyMapOthersLangArr; j++ )
            {
            if ( KErrNotFound == poolIndex )
                {
                TInt positionLeftMostOnKeys = KErrNotFound; 

                for ( TInt i = 0; i < poolIndexArr.Count(); i++ )
                    {
                    TInt position = (*keyMappingArray)[poolIndexArr[i]]->iKeyMapCharArr[j].Find((TUint) aChar);
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
                                     j, poolIndex, (*ptiKeyArray)[poolIndex] );
                        
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
        key = (*ptiKeyArray)[poolIndex];
        }
    
    poolIndexArr.Close();
    return key;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::PoolIdForKey
//
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::PoolIdForKey(const TPtiKey aKey, TKeyboardModes aKbMode)
    {
    aKbMode = ResolveKeyboardMode( aKbMode, EPredictiveDefaultKeyboard ); 

    // From logical point of view, the Pool ID is an index of the key in
    // an array which is formed by concatenating QWERTY keys array in the end
    // of the ITU-T keys array.
    TInt poolId = KErrNotFound;
    if ( aKbMode == EPredictiveItuT && IsItutPredictiveAvailable() )
        {
        poolId = iItutKeys.Find(aKey);
        // IF the key is not found, then it should go to the special pool,
        // which is the pool of the dummy key in the ITU-T keys array
        if (KErrNotFound == poolId)
            {
            poolId = iItutKeys.Count() - 1;
            }
        }
    else if ( aKbMode == EPredictiveQwerty && IsQwertyPredictiveAvailable() )
        {
        poolId = iQwertyKeys.Find(aKey);
        // IF the key is not found, then it should go to the special pool,
        // which is the pool of the dummy key in the QWERTY keys array
        if (KErrNotFound == poolId)
            {
            poolId = iQwertyKeys.Count() - 1;
            }
        // Pools of QWERTY keys come after pools of ITU-T keys
        poolId += iItutKeys.Count();
        }

    // Pool ID must never exceed value 63, because CPcsCache class
    // stores these values as bitmask into 64 bit variable.
    __ASSERT_DEBUG( poolId < 64, Panic(EPanic_OverflowInPoolIndex) );

    return poolId;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::PoolIdForCharacter
//
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::PoolIdForCharacter(const TChar& aChar, TKeyboardModes aKbMode)
    {
    // Pools are formed according the predictive keyboard mapping(s).
    // When selecting pool for non-predictive mode, we use the pool of the
    // default keyboard. The non-predictive matches should be a sub set of the
    // predictive matches of the default keyboard, although strictly speaking,
    // there is no guarantee for this.
    aKbMode = ResolveKeyboardMode( aKbMode, ENonPredictive ); 

    TPtiKey key = KeyForCharacterMultiMatch( aChar, aKbMode );

    TInt poolId = PoolIdForKey(key, aKbMode);

    return poolId;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::PoolCount
//
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::PoolCount()
    {
    return iItutKeyMaps.Count() + iQwertyKeyMaps.Count();
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::SetSpaceAndZeroOnSameKey
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::SetSpaceAndZeroOnSameKey()
    {
    static const TInt KSpace = 0x20; // ASCII for " "
    static const TInt KZero  = 0x30; // ASCII for "0"

    TChar charSpace(KSpace);
    TChar charZero(KZero);

    TPtiKey keySpace;
    TPtiKey keyZero;
	
    // ITU-T mode
    keySpace = KeyForCharacterMultiMatch(charSpace, EPredictiveItuT);
    keyZero = KeyForCharacterMultiMatch(charZero, EPredictiveItuT);
    iSpaceAndZeroOnSameKeyOnItut = (keySpace == keyZero && keyZero != EPtiKeyNone);
    PRINT1 ( _L("CPcsKeyMap::iSpaceAndZeroOnSameKeyOnItut = %d"), iSpaceAndZeroOnSameKeyOnItut );
    
    // QWERTY mode
    keySpace = KeyForCharacterMultiMatch(charSpace, EPredictiveQwerty);
    keyZero = KeyForCharacterMultiMatch(charZero, EPredictiveQwerty);
    iSpaceAndZeroOnSameKeyOnQwerty = (keySpace == keyZero && keyZero != EPtiKeyNone);
    PRINT1 ( _L("CPcsKeyMap::iSpaceAndZeroOnSameKeyOnQwerty = %d"), iSpaceAndZeroOnSameKeyOnQwerty );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetSpaceAndZeroOnSameKey
// 
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::GetSpaceAndZeroOnSameKey( TKeyboardModes aKbMode )
    {
    TBool result = EFalse;

    aKbMode = ResolveKeyboardMode( aKbMode, EPredictiveDefaultKeyboard );

    if ( aKbMode == EPredictiveItuT )
        {
        result = iSpaceAndZeroOnSameKeyOnItut;
        }
    else if ( aKbMode == EPredictiveQwerty )
        {
        result = iSpaceAndZeroOnSameKeyOnQwerty;
        }

    return result;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetPhysicalKeyboardTypesL
// Initialise the keyboard type variables
// ----------------------------------------------------------------------------
void CPcsKeyMap::GetPhysicalKeyboardTypesL( TPtiKeyboardType& aItutKbType,
                                            TPtiKeyboardType& aQwertyKbType )
    {
    TInt physicalKeyboard = 0;
    CRepository* aknFepRepository = CRepository::NewL( KCRUidAknFep );
    aknFepRepository->Get( KAknFepPhysicalKeyboards, physicalKeyboard );
    delete aknFepRepository;

    PRINT1 ( _L("CPcsKeyMap::GetPhysicalKeyboardTypesL: Physical keyboard support flag = 0x%02X"), physicalKeyboard );

    // Constants follow the definition of KAknFepPhysicalKeyboards
    const TInt KPtiKeyboard12Key = 0x01;
    const TInt KPtiKeyboardQwerty4x12 = 0x02;
    const TInt KPtiKeyboardQwerty4x10 = 0x04;
    const TInt KPtiKeyboardQwerty3x11 = 0x08;
    const TInt KPtiKeyboardHalfQwerty = 0x10;
    const TInt KPtiKeyboardCustomQwerty = 0x20; 

    // Get ITU-T mode first.
    // Use always 12-key mode since all the supported devices should have at least
    // virtual ITU-T available.
    // It will be set to EPtiKeyboardNone if getting the key list will fail.
    if ( physicalKeyboard & KPtiKeyboard12Key )
        {
        aItutKbType = EPtiKeyboard12Key;
        }
    else
        {
        aItutKbType = EPtiKeyboardNone;
        }
    
    // Then get QWERTY mode. On real-life devices there should never
    // be more than one QWERTY keyboard available but on emulator there can be several.
    // Use the first one found in the following precedence
    // It will be set to EPtiKeyboardNone if getting the key list will fail.
    if ( physicalKeyboard & KPtiKeyboardQwerty3x11 )
        {
        aQwertyKbType = EPtiKeyboardQwerty3x11;
        }
    else if ( physicalKeyboard & KPtiKeyboardQwerty4x10 )
        {
        aQwertyKbType = EPtiKeyboardQwerty4x10;
        }
    else if ( physicalKeyboard & KPtiKeyboardQwerty4x12 )
        {
        aQwertyKbType = EPtiKeyboardQwerty4x12;
        }
    else if ( physicalKeyboard & KPtiKeyboardCustomQwerty )
        {
        aQwertyKbType = EPtiKeyboardCustomQwerty;
        }
    else if ( physicalKeyboard & KPtiKeyboardHalfQwerty )
        {
        aQwertyKbType = EPtiKeyboardHalfQwerty;
        }
    else
        {
        aQwertyKbType = EPtiKeyboardNone;
        }
        
    PRINT1 ( _L("CPcsKeyMap::GetPhysicalKeyboardTypesL: Physical ITU-T Keyboard = %d"), aItutKbType );
    PRINT1 ( _L("CPcsKeyMap::GetPhysicalKeyboardTypesL: Physical QWERTY Keyboard = %d"), aQwertyKbType );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AppendEntryWithFakeKeyToKeyList
// Add pool with unused id for for key
// ----------------------------------------------------------------------------
void CPcsKeyMap::AppendEntryWithFakeKeyToKeyList( RArray<TPtiKey>& aKeyArray )
    {
    TUint keyUInt = (TUint) EPtiKeyNone + 1;

    while ( KErrNotFound != aKeyArray.Find( (TPtiKey) keyUInt) )
        {
        keyUInt++;
        }

    TPtiKey key = (TPtiKey) keyUInt;
    aKeyArray.Append( key );

    // This should always be the last one in the array
    PRINT2 ( _L("CPcsKeyMap::AppendEntryWithFakeKeyToKeyList: Added additional last pool %d with key id #%d"),
            aKeyArray.Count()-1, key );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::CreateKeyMapFromKeyBindingTable
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::CreateKeyListFromKeyBindingTable( RArray<TPtiKey>& aKeyArray, 
        TPtiKeyboardType aKbType, CPtiEngine* aPtiEngine )
    {
    PRINT ( _L("Enter CPcsKeyMap::CreateKeyListFromKeyBindingTable") );

    PRINT1 ( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: Creating KeyList for TPtiKeyboardType=%d"),
             aKbType  );

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
        TInt langCode = currLanguage->LanguageCode();
        TRAP_IGNORE( aPtiEngine->ActivateLanguageL( langCode ) );
        const CPtiKeyMapData* keyMapData = currLanguage->RawKeyMapData();
        const TPtiKeyBinding* table = NULL;
        TInt numItems = 0;
        if ( keyMapData )
            {
            table = keyMapData->KeyBindingTable(aKbType, numItems);
            }
        else
            {
            PRINT1( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: #### Failed to get RawKeyMapData for language %d ####"), langCode );
            }
        
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
                    PRINT3 ( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: Adding pool %d with key '%c' (0x%02X)"),
                            aKeyArray.Count(), key, key );
                    aKeyArray.Append( key );
                    }
                }
            // Add a fake key at the end if the KeyList is not empty
            if (aKeyArray.Count() > 0)
                {
                AppendEntryWithFakeKeyToKeyList(aKeyArray);
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

    PRINT ( _L("End CPcsKeyMap::CreateKeyListFromKeyBindingTable") );
}

// ----------------------------------------------------------------------------
// CPcsKeyMap::IsLanguageSupported
// Returns ETrue if this language is supported
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::IsLanguageSupported(TInt aLang)
    {
    TBool supported = 
#ifdef __WINS__
            // Only few languages for emulator 
            ( ELangEnglish == aLang || 
              ELangRussian == aLang || 
              ELangHebrew == aLang || 
              ELangFinnish == aLang ) &&
#endif // __WINS__
            ( KErrNotFound == iLanguageNotSupported.Find((TLanguage)aLang) );
                
    return supported;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::CreateKeyMappingL
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::CreateKeyMappingL( TKeyboardModes aKbMode )
    {
    PRINT ( _L("Enter CPcsKeyMap::CreateKeyMappingL") );

    __ASSERT_DEBUG( (aKbMode==EPredictiveItuT || aKbMode==EPredictiveQwerty),
                    Panic(EPanic_InvalidKeyboardType) );

    // Select the arrays we are operating on
    RArray<TPtiKey>* ptiKeysArray;
    RPointerArray<TKeyMappingData>* keyMappingArray;
    TPtiKeyboardType kbType;
    GetPredictiveKeyboardData( aKbMode, ptiKeysArray, keyMappingArray, kbType );
    if ( kbType == EPtiKeyboardNone )
        {
        PRINT1( _L("CPcsKeyMap::CreateKeyMappingL: ##### No Keyboard available for mode %d => skipping #####"), aKbMode );
        return;
        }
    
    // Instantiate the engine
    CPtiEngine* ptiEngine = CPtiEngine::NewL(ETrue);
    CleanupStack::PushL( ptiEngine );

    ptiEngine->SetKeyboardType( kbType );

    CreateKeyListFromKeyBindingTable( *ptiKeysArray, kbType, ptiEngine );

    if ( ptiKeysArray->Count() > 0 )
        {
        // Now add the keymap arrays to hold the keymap data
        for (TInt i = 0; i < ptiKeysArray->Count(); i++)
            {
            TKeyMappingData *keyData = new(ELeave) TKeyMappingData;
            keyMappingArray->Append(keyData);
            }
    
        // Get the available Languages on the phone
        RArray<TInt> languagesOnThisPhone;
        CleanupClosePushL( languagesOnThisPhone );
        ptiEngine->GetAvailableLanguagesL(languagesOnThisPhone);
        PRINT2 ( _L("CPcsKeyMap::CreateKeyMappingL: Languages on this phone %d, maximum is set to %d"),
                 languagesOnThisPhone.Count(), KMaxNbrOfLangKeymapping );
        
        // Remove the non-supported languages
        for (TInt i = 0; i < languagesOnThisPhone.Count(); /* do not increment i */)
            {
            if ( IsLanguageSupported(languagesOnThisPhone[i]) )
                {
                i++;
                }
            else
                {
                PRINT1 ( _L("CPcsKeyMap::CreateKeyMappingL: Removing not supported Language %d"),
                         languagesOnThisPhone[i] );
                languagesOnThisPhone.Remove(i);
                }
            }
    
        // Set current UI language as 1st language and English as 2nd language,
        // if present in AvailableLanguages
        TInt langIndex;
        if ( KErrNotFound != (langIndex = languagesOnThisPhone.Find(ELangEnglish)) )
            {
            languagesOnThisPhone.Remove(langIndex);
            languagesOnThisPhone.Insert(ELangEnglish, 0);
            }
        if ( KErrNotFound != (langIndex = languagesOnThisPhone.Find(iUILanguage)) )
            {
            languagesOnThisPhone.Remove(langIndex);
            languagesOnThisPhone.Insert(iUILanguage, 0);
            }
    
        // Set max number of languages for Key Map
        TInt count = languagesOnThisPhone.Count();
        if (count > KMaxNbrOfLangKeymapping)
            {
            PRINT2 ( _L("CPcsKeyMap::CreateKeyMappingL: Supported Languages on this phone %d, limiting to %d"),
                     count, KMaxNbrOfLangKeymapping );
            count = KMaxNbrOfLangKeymapping;
            }
    
        // Add Key Map for the languages
        for (TInt i = 0; i < count; i++)
            {
            TLanguage languageToUse = (TLanguage) languagesOnThisPhone[i];
    
            PRINT1 ( _L("CPcsKeyMap::CreateKeyMappingL: Constructing Key Map for Language %d"), languageToUse );
            TRAPD( leaveCode, AddKeyMappingForLanguageL(ptiEngine, languageToUse, kbType, *ptiKeysArray, *keyMappingArray) );
            if ( leaveCode )
                {
                PRINT2 ( _L("CPcsKeyMap::CreateKeyMappingL: ##### Adding mappings for language %d failed, leave code = %d #####"),
                         languageToUse, leaveCode );
                }
    
            PRINT ( _L("CPcsKeyMap::CreateKeyMappingL: ===================================================") );
            }
    
        CleanupStack::PopAndDestroy( &languagesOnThisPhone );
        }

    CleanupStack::PopAndDestroy( ptiEngine );

    PRINT ( _L("End CPcsKeyMap::CreateKeyMappingL") );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddKeyMappingForActiveLanguageL
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddKeyMappingForLanguageL(
        CPtiEngine* aPtiEngine, 
        TLanguage aLanguage,
        TPtiKeyboardType aKbType,
        const RArray<TPtiKey>& aPtiKeys,
        RPointerArray<TKeyMappingData>& aResultMapping )
{
    PRINT ( _L("Enter CPcsKeyMap::AddKeyMappingForLanguageL") );

    TInt err = aPtiEngine->ActivateLanguageL( aLanguage );
    if ( err )
        {
        PRINT2 ( _L("##### CPcsKeyMap::AddKeyMappingForLanguageL failed to activate language %d, error=%d) #####"),
                aLanguage, err );
        User::Leave( err );
        }

    // Make a language object based on the language
    CPtiCoreLanguage* language = static_cast<CPtiCoreLanguage*>(aPtiEngine->GetLanguage( aLanguage ));

    if (language)
        {
        for (TInt i = 0; i < aResultMapping.Count() - 1; i++)
            {
            TPtiKey key = aPtiKeys[i];

            PRINT ( _L("CPcsKeyMap::AddKeyMappingForLanguageL: ===================================================") );
            PRINT4 ( _L("CPcsKeyMap::AddKeyMappingForLanguageL: Adding chars for Key '%c' (0x%02X) for Keyboard Type %d and Language %d"),
                     key, key, aKbType, aLanguage );

            // Get the pointer to the language class (UI, English, Others)
            RArray<TUint>* keyMapLang;
            if (aLanguage == iUILanguage)
                {
                keyMapLang = &(aResultMapping[i]->iKeyMapCharArr[TKeyMappingData::EKeyMapUILangArr]);
                PRINT1 ( _L("CPcsKeyMap::AddKeyMappingForLanguageL: Language %d is the UI language"), aLanguage );
                }
            else if (aLanguage == ELangEnglish)
                {
                // If (iUILanguage == ELangEnglish) ok to be in previous if case
                keyMapLang = &(aResultMapping[i]->iKeyMapCharArr[TKeyMappingData::EKeyMapEnglishLangArr]);
                PRINT1 ( _L("CPcsKeyMap::AddKeyMappingForLanguageL: Language %d is English language"), aLanguage );
                }
            else
                {
                keyMapLang = &(aResultMapping[i]->iKeyMapCharArr[TKeyMappingData::EKeyMapOthersLangArr]);
                PRINT1 ( _L("CPcsKeyMap::AddKeyMappingForLanguageL: Language %d is in the Other languages"), aLanguage );
                }
            
            PRINT ( _L("CPcsKeyMap::AddKeyMappingForLanguageL: ---------------------------------------------------") );

            TBool isSingleCharForKey = ETrue;
            TUint singleChar = 0;

            // EPtiCaseUpper must be the 1st TPtiTextCase for iUILanguage
            AddCharactersToKey( *language, aKbType, key, EPtiCaseUpper, *aResultMapping[i], *keyMapLang, isSingleCharForKey, singleChar );
            AddCharactersToKey( *language, aKbType, key, EPtiCaseLower, *aResultMapping[i], *keyMapLang, isSingleCharForKey, singleChar );

            // 1. No other TPtiTextCase values for ITUT keyboard
            // 2. No language variants handling for ITUT keyboard
            if ( EPtiKeyboard12Key != aKbType )
                {
                AddCharactersToKey( *language, aKbType, key, EPtiCaseFnLower,  *aResultMapping[i], *keyMapLang, isSingleCharForKey, singleChar );
                AddCharactersToKey( *language, aKbType, key, EPtiCaseFnUpper,  *aResultMapping[i], *keyMapLang, isSingleCharForKey, singleChar );
                AddCharactersToKey( *language, aKbType, key, EPtiCaseChrLower, *aResultMapping[i], *keyMapLang, isSingleCharForKey, singleChar );
                AddCharactersToKey( *language, aKbType, key, EPtiCaseChrUpper, *aResultMapping[i], *keyMapLang, isSingleCharForKey, singleChar );

                // Support for key guessing given the char in some phone language variants
                if ( (isSingleCharForKey) && (0 != singleChar) )
                    {
                    aResultMapping[i]->iKeyMapCharArr[TKeyMappingData::EKeyMapSingleCharArr].Append(singleChar); // singleChar is in LowerCase
                    aResultMapping[i]->iKeyMapCharArr[TKeyMappingData::EKeyMapSingleCharArr].Append(User::UpperCase(singleChar));

                    PRINT ( _L("CPcsKeyMap::AddKeyMappingForLanguageL: ---------------------------------------------------") );
                    PRINT5 ( _L("CPcsKeyMap::AddKeyMappingForLanguageL: For Language %d and key '%c' of pool %d single char is '%c' (0x%04X)"),
                            aLanguage, key, i, singleChar, singleChar );
                    }
                }
            }
        PRINT ( _L("CPcsKeyMap::AddKeyMappingForLanguageL: ===================================================") );
        }

    PRINT ( _L("End CPcsKeyMap::AddKeyMappingForLanguageL") );
}

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetCharactersForKey
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::GetCharactersForKey(CPtiCoreLanguage& aPtiLanguage,
                                     TPtiKeyboardType aKbType,
                                     TPtiKey aKey,
                                     TPtiTextCase aTextCase,
                                     TDes& aResult)
    {
    const CPtiKeyMapData* keyMapData = aPtiLanguage.RawKeyMapData();
    if  ( keyMapData )
        {
        TPtrC dataPtr = keyMapData->DataForKey( aKbType, aKey, aTextCase );
        // truncate results if supplied buffer is not large enough
        aResult = dataPtr.Left( aResult.MaxLength() );
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddDataForKeyL
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddCharactersToKey(CPtiCoreLanguage& aPtiLanguage,
                                    TPtiKeyboardType aKbType,
                                    TPtiKey aKey,
                                    TPtiTextCase aTextCase,
                                    TKeyMappingData& aKeyDataList,
                                    RArray<TUint>& aKeyMapLang,
                                    TBool& aIsSingleCharForKey,
                                    TUint& aSingleChar)
    {
    PRINT ( _L("Enter CPcsKeyMap::AddCharactersToKey") );

    TBuf<255> result;
    GetCharactersForKey(aPtiLanguage, aKbType, aKey, aTextCase, result);

    PRINT3 ( _L("CPcsKeyMap::AddCharactersToKey: Get mapping chars for Key '%c' (0x%02X) and TextCase %d"),
             aKey, aKey, aTextCase );
    PRINT1 ( _L("CPcsKeyMap::AddCharactersToKey: Got chars: \"%S\""), &result );

    for ( TInt i = 0; i < result.Length(); i++ )
        {
        TText character = result[i];
        if ( !CharacterForKeyMappingExists(aKeyDataList, character ) )
            {
            PRINT2 ( _L("CPcsKeyMap::AddCharactersToKey: ----- Appending char to list: '%c' (0x%04X)"), character, character );
            aKeyMapLang.Append(character);
            }
        else
            {
            PRINT2 ( _L("CPcsKeyMap::AddCharactersToKey: ***** NOT Appending char to list: '%c' (0x%04X)"), character, character );
            }

        // No language variants handling for ITUT keyboard
        if ( EPtiKeyboard12Key != aKbType )
            {
            // Support for key guessing given the char in some phone language variants
            if ( aIsSingleCharForKey )
                {
                if ( 0 == aSingleChar )
                    {
                    aSingleChar = User::LowerCase(character);
                    }
                else
                    {
                    TInt newChar = User::LowerCase(character);
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

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetPredictiveKeyboardData
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::GetPredictiveKeyboardData( TKeyboardModes aKbMode, 
                                            RArray<TPtiKey>*& aPtiKeys, 
                                            RPointerArray<TKeyMappingData>*& aKeyMappings, 
                                            TPtiKeyboardType& aKbType )
    {
    // Get the data from constant version of this function
    const RArray<TPtiKey>* constKeys;
    const RPointerArray<TKeyMappingData>* constMappings;
    GetPredictiveKeyboardData( aKbMode, constKeys, constMappings, aKbType );
    
    // Convert pointers to non-const
    aPtiKeys = const_cast< RArray<TPtiKey>* >( constKeys );
    aKeyMappings = const_cast< RPointerArray<TKeyMappingData>* >( constMappings );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetPredictiveKeyboardData
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::GetPredictiveKeyboardData( TKeyboardModes aKbMode, 
                                            const RArray<TPtiKey>*& aPtiKeys, 
                                            const RPointerArray<TKeyMappingData>*& aKeyMappings, 
                                            TPtiKeyboardType& aKbType ) const
    {
    PRINT1 ( _L("CPcsKeyMap::GetPredictiveKeyboardData: aKbMode=%d "), aKbMode );
    
    // EPredictiveItuT or EPredictiveQwerty mode to ENonPredictive mode if keyboard is not mapped
    if ( aKbMode == EPredictiveItuT && !IsItutPredictiveAvailable() )
        {
        aKbMode = ENonPredictive;
        }
    if ( aKbMode == EPredictiveQwerty && !IsQwertyPredictiveAvailable() )
        {
        aKbMode = ENonPredictive;
        }

    // Get Predictive Keyboard Data
    switch (aKbMode)
        {
        case EPredictiveItuT:
            aPtiKeys = &iItutKeys;
            aKeyMappings = &iItutKeyMaps;
            aKbType = iItutKeyboardType;
            break;

        case EPredictiveQwerty:
            aPtiKeys = &iQwertyKeys;
            aKeyMappings = &iQwertyKeyMaps;
            aKbType = iQwertyKeyboardType;
            break;

        case ENonPredictive:
            aPtiKeys = NULL;
            aKeyMappings = NULL;
            aKbType = EPtiKeyboardNone;
            break;

        // EPredictiveDefaultKeyboard must have been resolved previously
        // to EPredictiveItuT or EPredictiveQwerty mode
        case EPredictiveDefaultKeyboard: 
        default:
            aPtiKeys = NULL;
            aKeyMappings = NULL;
            aKbType = EPtiKeyboardNone;
            __ASSERT_DEBUG( EFalse, Panic( EPanic_InvalidKeyboardType ) );
            break;
        }
    }

// End of file
