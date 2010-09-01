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
*                Uses services provided by the PTI Engine.
*
*/

// INCLUDE FILES

#include "CPcsAlgorithm2.h"
#include "CPcsAlgorithm2Utils.h"
#include "FindUtilChineseECE.h"
#include "CPcsDebug.h"
#include "CPcsKeyMap.h"
#include "CPsQueryItem.h"
#include <PtiEngine.h>
#include <PtiKeyMapData.h>
#include <bldvariant.hrh>
#include <AknFepInternalCRKeys.h>


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
CPcsKeyMap* CPcsKeyMap::NewL(CPcsAlgorithm2* aAlgorithm)
    {
    PRINT ( _L("Enter CPcsKeyMap::NewL") );

    CPcsKeyMap* self = new (ELeave) CPcsKeyMap();
    CleanupStack::PushL(self);
    self->ConstructL(aAlgorithm);
    CleanupStack::Pop(self);

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
void CPcsKeyMap::ConstructL(CPcsAlgorithm2* aAlgorithm)
    {
    iLanguageNotSupported.Append(ELangJapanese);
    iLanguageNotSupported.Append(ELangKorean);

    iAlgorithm = aAlgorithm;
    iPtiEngine = CPtiEngine::NewL();

    SetupKeyboardTypesL();
    ConstructKeymapL();
    SetSpaceAndZeroOnSameKey();
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ReconstructKeymapL
// When the writing language is changed, the keymap needs reconstruct.
// ----------------------------------------------------------------------------
void CPcsKeyMap::ReconstructKeymapL()
    {
    // Clear the keymap data array first when reconstruct the keymap.
    ResetKeyMap();
    
    // Always add English mappings first
    AddKeyMapforConcreteKeyboardL( ELangEnglish );
    
    // Then append the mappings of the current input language
    TLanguage lang = iAlgorithm->FindUtilECE()->CurrentInputLanguage();
    if ( lang != ELangEnglish && IsLanguageSupportedL(lang) )
        {
        AddKeyMapforConcreteKeyboardL( lang );
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ConstructKeymapL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructKeymapL()
    {
    ConstructConcreteKeyMapL();
    
    // Always add English mappings first
    AddKeyMapforConcreteKeyboardL( ELangEnglish );
    
    // Then append the mappings of the current input language
    TLanguage lang = iAlgorithm->FindUtilECE()->CurrentInputLanguage();
    if ( lang != ELangEnglish && IsLanguageSupportedL(lang) )
        {
        AddKeyMapforConcreteKeyboardL( lang );
        }
    
    PRINT ( _L("----------------------------------------"));
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::SetupKeyboardTypesL
// Initialise the keyboard type variables
// ----------------------------------------------------------------------------
void CPcsKeyMap::SetupKeyboardTypesL()
    {
    TInt physicalKeyboard = 0;
    CRepository* aknFepRepository = CRepository::NewL( KCRUidAknFep );
    aknFepRepository->Get( KAknFepPhysicalKeyboards, physicalKeyboard );
    delete aknFepRepository;

    PRINT1 ( _L("CPcsKeyMap::ConstructL: Physical keyboard support flag = 0x%02X"), physicalKeyboard );

    // Constants follow the definition of KAknFepPhysicalKeyboards
    const TInt KPtiKeyboard12Key = 0x01;
    const TInt KPtiKeyboardQwerty4x12 = 0x02;
    const TInt KPtiKeyboardQwerty4x10 = 0x04;
    const TInt KPtiKeyboardQwerty3x11 = 0x08;
    const TInt KPtiKeyboardHalfQwerty = 0x10;
    const TInt KPtiKeyboardCustomQwerty = 0x20; 

    // Setup ITU-T mode first.
    // Use always 12-key mode since all the supported devices should have at least
    // virtual ITU-T available.
    iItutKeyboardType = EPtiKeyboard12Key;
    // TODO: ITU-T type could be set to "none" if device does not have either
    // virtual keypad or hardware ITU-T available. This could be decided according
    // some cenrep value, feature flag, device model, or platform version.
    
    // Then setup QWERTY mode. On real-life devices there should never
    // be more than one QWERTY keyboard available but on emulator there can be several.
    // Use the first one found in the following precedence
    if ( physicalKeyboard & KPtiKeyboardQwerty3x11 )
        {
        iQwertyKeyboardType = EPtiKeyboardQwerty3x11;
        }
    else if ( physicalKeyboard & KPtiKeyboardQwerty4x10 )
        {
        iQwertyKeyboardType = EPtiKeyboardQwerty4x10;
        }
    else if ( physicalKeyboard & KPtiKeyboardQwerty4x12 )
        {
        iQwertyKeyboardType = EPtiKeyboardQwerty4x12;
        }
    else if ( physicalKeyboard & KPtiKeyboardCustomQwerty )
        {
        iQwertyKeyboardType = EPtiKeyboardCustomQwerty;
        }
    else if ( physicalKeyboard & KPtiKeyboardHalfQwerty )
        {
        iQwertyKeyboardType = EPtiKeyboardHalfQwerty;
        }
    else
        {
        iQwertyKeyboardType = EPtiKeyboardNone;
        }
    
    // Decide, which keyboard is used for the "default" matching mode.
    // If there is physical ITU-T available, or there's no physical QWERTY,
    // then ITU-T is default.
    iItutIsDefault = (physicalKeyboard & KPtiKeyboard12Key) || 
                     (iQwertyKeyboardType == EPtiKeyboardNone);
    
    PRINT1 ( _L("CPcsKeyMap::ConstructL: ITU-T Keyboard chosen for Predictive Search = %d"), iItutKeyboardType );
    PRINT1 ( _L("CPcsKeyMap::ConstructL: QWERTY Keyboard chosen for Predictive Search = %d"), iQwertyKeyboardType );
    }


// ----------------------------------------------------------------------------
// CPcsKeyMap::ConstructConcreteKeyMapL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructConcreteKeyMapL()
    {
    if ( iItutKeyboardType != EPtiKeyboardNone )
        {
        // Construct for Itut keyboard by default
        PRINT ( _L("CPcsKeyMap::ConstructConcreteKeyMapL: Construct keymap for ITU-T"));
        ConstructForItutKeyboardL();
        }
    if ( iQwertyKeyboardType != EPtiKeyboardNone )
        {
        // Construct for any QWERTY keyboard
        PRINT1 ( _L("CPcsKeyMap::ConstructConcreteKeyMapL: Construct keymap for QWERTY keyboard type %d"), 
                 iQwertyKeyboardType );
        ConstructForQwertyKeyboardL( iQwertyKeyboardType );
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddKeyMapforConcreteKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddKeyMapforConcreteKeyboardL( TLanguage aLanguage )
    {
    if ( iItutKeyboardType != EPtiKeyboardNone )
        {
        AddKeyMapforItutL( aLanguage );
        }
    
    if ( iQwertyKeyboardType != EPtiKeyboardNone )
        {
        AddKeyMapForQwertyKeyboardL( aLanguage, iQwertyKeyboardType );
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ResetKeyMap
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::ResetKeyMap()
    {
    const TInt ItutKeyMappingsCount = iItutKeyMappings.Count();
    const TInt qwertyKeyMappingsCount = iQwertyKeyMappings.Count();
    for (TInt i = 0; i < ItutKeyMappingsCount; i++)
        {
        iItutKeyMappings[i]->iKeyMappingArray.Reset();
        }
    for (TInt i = 0; i < qwertyKeyMappingsCount; i++)
        {
        iQwertyKeyMappings[i]->iKeyMappingArray.Reset();
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::~CPcsKeyMap
// Destructor
// ----------------------------------------------------------------------------
CPcsKeyMap::~CPcsKeyMap()
    {
    ResetKeyMap();

    // Cleanup local arrays
    iLanguageNotSupported.Reset();
    iItutKeyMappings.ResetAndDestroy();
    iItutKeys.Close();
    iQwertyKeyMappings.ResetAndDestroy();
    iQwertyKeys.Close();
    delete iPtiEngine;
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

    const TInt srcDataLength = aSrcData.Length(); 
    for ( TInt i = 0; i < srcDataLength; ++i )
        {
        TChar character( aSrcData[i] );
        character.LowerCase();
        if ( i < aSrcQuery.Count() )
            {
            CPsQueryItem& currentItem = aSrcQuery.GetItemAtL(i);
            TKeyboardModes curMode = ResolveKeyboardMode( currentItem.Mode() );
            TPtiKey key = KeyForCharacter( aSrcData[i], curMode );
            // If a character is not mapped to any key or it's entered in non-predictive mode,
            // then append the character as exact.
            if ( EPtiKeyNone == key )
                {
                aDestStr.Append( character );
                }
            else
                {
                aDestStr.Append( DefaultCharForKey(key, curMode) );
                }
            }
        else
            {
            // characters over query length are taken as exact
            aDestStr.Append( character );
            }
        }

    PRINT1 ( _L("CPcsKeyMap::GetMixedKeyStringForDataL: Return string: \"%S\""),
             &aDestStr );

    PRINT ( _L("End CPcsKeyMap::GetMixedKeyStringForDataL") );
}

// ----------------------------------------------------------------------------
// CPcsKeyMap::KeyForCharacter
// 
// ----------------------------------------------------------------------------
TPtiKey CPcsKeyMap::KeyForCharacter(TText aChar, TKeyboardModes aKbMode) const
    {
    const RPointerArray<TKeyMappingData>* keyMappings = KeyMappings( aKbMode );
    
    // Don't return any key in the exact mode
    if ( !keyMappings )
        {
        return EPtiKeyNone;
        }
    
    TInt index = KErrNotFound;

    TText lChar = User::LowerCase(aChar);

    const TInt count = keyMappings->Count() - 1;

    for (TInt i = 0; i < count; i++)
        {
        index = (*keyMappings)[i]->iKeyMappingArray.Find(lChar);
        if (index != KErrNotFound)
            {
            return (*keyMappings)[i]->iKey;
            }
        }

    return EPtiKeyNone;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddKeyMapForQwertyKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddKeyMapForQwertyKeyboardL(TLanguage aLanguage, TPtiKeyboardType aKbType)
    {
#ifdef RD_INTELLIGENT_TEXT_INPUT
    iPtiEngine->ActivateLanguageL(aLanguage);
    iPtiEngine->SetKeyboardType(aKbType);

    // Make a language object based on current language
    CPtiCoreLanguage* coreLanguage = static_cast<CPtiCoreLanguage*>(iPtiEngine->GetLanguage( aLanguage ));

    //Perfrom the key mappings only if the corelanguage is available
    if (coreLanguage)
        {
        // Get the keyboard mappings for the language
        CPtiKeyMapData* ptiKeyMapData = coreLanguage->RawKeyMapData();

        const TInt qwertyKeyMappingsCount = iQwertyKeyMappings.Count() - 1;
        for (TInt i = 0; i < qwertyKeyMappingsCount; i++)
            {
            AddDataForQwertyKeyboardL( ptiKeyMapData, aKbType, 
                    iQwertyKeys[i], *(iQwertyKeyMappings[i]) );
            }
        }
#endif // RD_INTELLIGENT_TEXT_INPUT        
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ConstructForQwertyKeyboardL
// Destructor
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructForQwertyKeyboardL(TPtiKeyboardType aKbType)
    {
#ifdef RD_INTELLIGENT_TEXT_INPUT
    CreateKeyListFromKeyBindingTable( iQwertyKeys, aKbType );

    // Now add the keymap arrays to hold the keymap data
    const TInt qwertyKeysCount = iQwertyKeys.Count();
    for (TInt i = 0; i < qwertyKeysCount; i++)
        {
        TKeyMappingData* keyData = new (ELeave) TKeyMappingData;
        keyData->iKey = iQwertyKeys[i];
        iQwertyKeyMappings.Append(keyData);
        }

    PRINT ( _L("----------------------------------------"));
#endif //RD_INTELLIGENT_TEXT_INPUT
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddDataForQwertyKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddDataForQwertyKeyboardL(CPtiKeyMapData* aPtiKeyMapData,
                                           TPtiKeyboardType aKbType,
                                           TPtiKey aKey,
                                           TKeyMappingData& aKeyDataList)
    {
#ifdef RD_INTELLIGENT_TEXT_INPUT     	
    TPtiTextCase caseArray[] =
        {
        EPtiCaseUpper,
        EPtiCaseLower,
        EPtiCaseFnLower,
        EPtiCaseFnUpper,
        EPtiCaseChrLower,
        EPtiCaseChrUpper
        };
    
    for (TInt i = 0; i < sizeof(caseArray) / sizeof(TPtiTextCase); i++)
        {
        TPtrC result = aPtiKeyMapData->DataForKey(aKbType, aKey, caseArray[i]);

        const TInt resultLength = result.Length(); 
        for (TInt j = 0; j < resultLength; j++)
            aKeyDataList.iKeyMappingArray.Append(result[j]);

        PRINT2 ( _L("CPcsKeyMap: Mapping for Key %c = %S"), aKey, &result)
        }
#endif //RD_INTELLIGENT_TEXT_INPUT        
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ContructForItutKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructForItutKeyboardL()
    {
    // Add the keys for Pool formation
    iItutKeys.Append(EPtiKey0);
    iItutKeys.Append(EPtiKey1);
    iItutKeys.Append(EPtiKey2);
    iItutKeys.Append(EPtiKey3);
    iItutKeys.Append(EPtiKey4);
    iItutKeys.Append(EPtiKey5);
    iItutKeys.Append(EPtiKey6);
    iItutKeys.Append(EPtiKey7);
    iItutKeys.Append(EPtiKey8);
    iItutKeys.Append(EPtiKey9);
    // one additional pool for special characters not mapped too any keys. 
    // This should always be the last one in the arrary
    iItutKeys.Append(EPtiKeyNone);

    // Now add the keymap arrays to hold the keymap data
    const TInt ltutKeysCount = iItutKeys.Count();
    for (TInt i = 0; i < ltutKeysCount; i++)
        {
        TKeyMappingData *keyData = new (ELeave) TKeyMappingData;
        keyData->iKey = iItutKeys[i];
        iItutKeyMappings.Append(keyData);
        }

    PRINT ( _L("----------------------------------------"));
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddKeyMapforItutLanguageL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddKeyMapforItutL(TLanguage aLanguage)
    {
    // Activate given language and get corresponding language object
    iPtiEngine->ActivateLanguageL(aLanguage);
    CPtiCoreLanguage* coreLanguage = static_cast<CPtiCoreLanguage*> (iPtiEngine->GetLanguage(aLanguage));

    //Perfrom the key mappings only if the corelanguage is available
    if (coreLanguage)
        {
        // Get the keyboard mappings for the language
        CPtiKeyMapData* ptiKeyMapData = coreLanguage->RawKeyMapData();

        const TInt ltutKeyMappingsCount = iItutKeyMappings.Count() - 1;
        for (TInt i = 0; i < ltutKeyMappingsCount; i++)
            {
            AddDataForItutKeyboardL(ptiKeyMapData, iItutKeys[i], *(iItutKeyMappings[i]));
            }
        }

    if ( (aLanguage == ELangPrcChinese || aLanguage == ELangTaiwanChinese || aLanguage == ELangHongKongChinese) && 
         (iAlgorithm->FindUtilECE()->CurrentSearchMethod() == EAdptSearchStroke) )
        {
        (*(iItutKeyMappings[1])).iKeyMappingArray.Append(0x4E00); //heng
        (*(iItutKeyMappings[2])).iKeyMappingArray.Append(0x4E28); //shu
        (*(iItutKeyMappings[3])).iKeyMappingArray.Append(0x4E3F); //pie
        (*(iItutKeyMappings[4])).iKeyMappingArray.Append(0x4E36); //dian
        (*(iItutKeyMappings[5])).iKeyMappingArray.Append(0x4E5B); //zhe
        }

    }
// ----------------------------------------------------------------------------
// CPcsKeyMap::AddDataForItutKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddDataForItutKeyboardL(CPtiKeyMapData* aPtiKeyMapData, TPtiKey aKey, TKeyMappingData& aKeyDataList)
    {
    TPtiTextCase caseArray[] = { EPtiCaseUpper, EPtiCaseLower };
    for (TInt i = 0; i< sizeof(caseArray) / sizeof(TPtiTextCase); i++)
        {
        TPtrC result = aPtiKeyMapData->DataForKey(EPtiKeyboard12Key, aKey, caseArray[i]);
        const TInt resultLength =  result.Length();
        for (TInt j = 0; j < resultLength; j++)
            aKeyDataList.iKeyMappingArray.Append(result[j]);
        PRINT2 ( _L("CPcsKeyMap: Mapping for Key %c = %S"), aKey,&result)
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::CreateKeyMapFromKeyBindingTable
//
// ----------------------------------------------------------------------------
void CPcsKeyMap::CreateKeyListFromKeyBindingTable( RArray<TPtiKey>& aKeyArray, 
        TPtiKeyboardType aKbType )
    {
    PRINT ( _L("Enter CPcsKeyMap::CreateKeyListFromKeyBindingTable") );

    // Use Eglish mappings to list the keys on the keyboard
    TRAP_IGNORE( iPtiEngine->ActivateLanguageL( ELangEnglish ) );
    CPtiCoreLanguage* ptiLang = 
            static_cast<CPtiCoreLanguage*>(iPtiEngine->GetLanguage( ELangEnglish ));

    if (ptiLang)
        {
        const CPtiKeyMapData* keyMapData = ptiLang->RawKeyMapData();
        const TPtiKeyBinding* table = NULL;
        TInt numItems = 0;
        if ( keyMapData )
            {
            table = keyMapData->KeyBindingTable(aKbType, numItems);
            }
        else
            {
            PRINT( _L("CPcsKeyMap::CreateKeyListFromKeyBindingTable: #### Failed to get RawKeyMapData ####") );
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

    // one additional pool for special characters not mapped too any keys. 
    // This should always be the last one in the arrary
    aKeyArray.Append(EPtiKeyNone);

    PRINT ( _L("End CPcsKeyMap::CreateKeyListFromKeyBindingTable") );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::IsLanguageSupportedL
// Returns ETrue if this language is supported
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::IsLanguageSupportedL(TUint32 aLang)
    {

    TBool flag = ETrue;
    const TInt languageNotSupportedCount = iLanguageNotSupported.Count();
    for (TInt i = 0; i < languageNotSupportedCount; i++)
        {
        if (iLanguageNotSupported[i] == aLang)
            {
            flag = EFalse;
            }
        }

    return flag;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::PoolIdForKey
//
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::PoolIdForKey(TPtiKey aKey, TKeyboardModes aKbMode) const
    {
    __ASSERT_DEBUG( (aKbMode==EPredictiveItuT || aKbMode==EPredictiveQwerty),
                    Panic(EPanic_InvalidKeyboardType) );
    
    // From logical point of view, the Pool ID is an index of the key in
    // an array which is formed by concatenating QWERTY keys array in the end
    // of the ITU-T keys array.
    TInt poolId = KErrNotFound;
    if ( aKbMode == EPredictiveItuT && iItutKeys.Count() )
        {
        poolId = iItutKeys.Find(aKey);
        // IF the key is not found, then it should go to the special pool,
        // which is the pool of the dummy key in the ITU-T keys array
        if (KErrNotFound == poolId)
            {
            poolId = iItutKeys.Count() - 1;
            }
        }
    else if ( aKbMode == EPredictiveQwerty && iQwertyKeys.Count() )
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
TInt CPcsKeyMap::PoolIdForCharacter( TChar aChar, TKeyboardModes aKbMode )
    {
    // Pools are formed according the predictive keyboard mapping(s).
    // When selecting pool for non-predictive mode, we use the pool of the
    // default keyboard. The non-predictive matches should be a sub set of the
    // predictive matches of the default keyboard, although strictly speaking,
    // there' no guarantee for this.
    if ( aKbMode == ENonPredictive || aKbMode == EPredictiveDefaultKeyboard )
        {
        aKbMode = ( iItutIsDefault ? EPredictiveItuT : EPredictiveQwerty );
        }

    // If character is a Chinese word character, then we select the
    // pool ID according the first character of the first spelling of the word.
    TRAP_IGNORE( aChar = FirstCharFromSpellingL( aChar ) );

    TPtiKey key = KeyForCharacter( aChar, aKbMode );
    TInt poolId = PoolIdForKey( key, aKbMode );

    return poolId;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::PoolCount
// 
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::PoolCount()
    {
    return iItutKeyMappings.Count() + iQwertyKeyMappings.Count();
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetSpaceAndZeroOnSameKey
// 
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::GetSpaceAndZeroOnSameKey( TKeyboardModes aMode )
    {
    // Resolve ambiguous keyboard mode
    aMode = ResolveKeyboardMode( aMode );
    
    if ( aMode == EPredictiveItuT )
        {
        return iSpaceAndZeroOnSameKeyOnItut;
        }
    else if ( aMode == EPredictiveQwerty )
        {
        return iSpaceAndZeroOnSameKeyOnQwerty;
        }
    else
        {
        return EFalse;
        }
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
    keySpace = KeyForCharacter(charSpace, EPredictiveItuT);
    keyZero = KeyForCharacter(charZero, EPredictiveItuT);
    iSpaceAndZeroOnSameKeyOnItut = (keySpace == keyZero);
    
    // QWERTY mode
    keySpace = KeyForCharacter(charSpace, EPredictiveQwerty);
    keyZero = KeyForCharacter(charZero, EPredictiveQwerty);
    iSpaceAndZeroOnSameKeyOnQwerty = (keySpace == keyZero);
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::CPcsKeyMap::FirstCharFromSpellingL
// 
// ----------------------------------------------------------------------------
TChar CPcsKeyMap::FirstCharFromSpellingL( TChar aChar ) const
    {
    TChar translated( aChar );
    TBuf<1> temp;
    temp.Append( aChar );
    if ( iAlgorithm->FindUtilECE()->IsChineseWordIncluded( temp ) )
        {
        RPointerArray<HBufC> spellList;
        CleanupResetAndDestroyPushL( spellList );
        if (iAlgorithm->FindUtilECE()->DoTranslationL(aChar, spellList))
            {
            translated = (*spellList[0])[0];
            }
        CleanupStack::PopAndDestroy( &spellList ); // ResetAndDestroy
        }
    return translated;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::KeyMappings
//
// ----------------------------------------------------------------------------
const RPointerArray<TKeyMappingData>* CPcsKeyMap::KeyMappings( TKeyboardModes aMode ) const
    {
    const RPointerArray<TKeyMappingData>* mappings = NULL;
    
    if ( aMode == EPredictiveItuT )
        {
        mappings = &iItutKeyMappings;
        }
    else if ( aMode == EPredictiveQwerty )
        {
        mappings = &iQwertyKeyMappings;
        }
    else if ( aMode == ENonPredictive )
        {
        mappings = NULL;
        }
    else
        {
        mappings = NULL;
        __ASSERT_DEBUG( EFalse, Panic( EPanic_InvalidKeyboardType ) );
        }
    
    return mappings;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ResolveKeyboardMode
// 
// ----------------------------------------------------------------------------
TKeyboardModes CPcsKeyMap::ResolveKeyboardMode( TKeyboardModes aKbMode ) const
    {
    TKeyboardModes resolvedMode = aKbMode;
    
    // Substitute "default predictive" mode with actual mode
    if ( resolvedMode == EPredictiveDefaultKeyboard )
        {
        resolvedMode = ( iItutIsDefault ? EPredictiveItuT : EPredictiveQwerty );
        }

    // Substitute predictive mode with non-predictive mode if corresponding
    // keyboard mappings are not available.
    if ( ( resolvedMode == EPredictiveItuT && iItutKeyboardType == EPtiKeyboardNone ) ||
         ( resolvedMode == EPredictiveQwerty && iQwertyKeyboardType == EPtiKeyboardNone ) )
        {
        PRINT1( _L("CPcsKeyMap::ResolveKeyboardMode: Mappings for requested mode %d unavailable. Falling back to non-predictive mode!"), aKbMode );
        resolvedMode = ENonPredictive;
        }
    
    return resolvedMode;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::DefaultCharForKey
// 
// ----------------------------------------------------------------------------
TText CPcsKeyMap::DefaultCharForKey( TPtiKey aKey, TKeyboardModes aKbMode ) const
    {
    // On ITU-T, the Pti key code can be directly interpreted as unicode character.
    // Thus, default characters for keys are 1,2,3,4,5,6,7,8,9,0,*,#.
    TText defChar = aKey;
    
    // On QWERTY, using PtiKey values directly is not safe since all PtiKey values are
    // not codepoints of printable Unicode characters.
    // Treating these arbitrary numerical values as Unicode characters could break 
    // the uniqueness of keys when collated comparison is used. Also, converting same
    // data multiple times to "compare format" would then break the data.
    if ( aKbMode == EPredictiveQwerty )
        {
        // Take first mapped character of the key and convert it to upper case.
        TInt index = iQwertyKeys.Find( aKey );
        if ( index != KErrNotFound )
            {
            const RArray<TInt>& mappings = iQwertyKeyMappings[index]->iKeyMappingArray;
            if ( mappings.Count() )
                {
                defChar = User::UpperCase( mappings[0] );
                }
            }
        }
    
    return defChar;
    }
// End of file
