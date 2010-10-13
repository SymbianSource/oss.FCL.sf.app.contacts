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
#include <bldvariant.hrh>
#include <AknFepInternalCRKeys.h>

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

    iAlgorithm = aAlgorithm;
    iPtiEngine = CPtiEngine::NewL();
	
    ConstructKeymapL();
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ReconstructKeymapL
// When the writing language is changed, the keymap needs reconstruct.
// ----------------------------------------------------------------------------
void CPcsKeyMap::ReconstructKeymapL()
    {
    TLanguage lang = iAlgorithm->FindUtilECE()->CurrentInputLanguage();
        if (lang == ELangPrcChinese || !IsLanguageSupportedL(lang))
            lang = ELangEnglish;

    TInt keyboardType = CurrentKeyBoardTypeL();
    
    // Clear the keymap data array first when reconstruct the keymap.
    ResetKeyMap();
    
    // Add new keymap to the keymap data array according the current writing language.
    AddKeyMapforConcreteKeyboardL( keyboardType, lang );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ConstructKeymapL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructKeymapL()
    {
    TLanguage lang = iAlgorithm->FindUtilECE()->CurrentInputLanguage();
    if (lang == ELangPrcChinese || !IsLanguageSupportedL(lang))
        lang = ELangEnglish;

    TInt keyboardType = CurrentKeyBoardTypeL();
    
    ConstructConcreteKeyMapL( keyboardType, lang );
    
    PRINT ( _L("----------------------------------------"));
    
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::CurrentKeyBoardTypeL
// 
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::CurrentKeyBoardTypeL()
    {
    TInt keyBoardType = EPtiKeyboardNone;
    
#ifdef RD_INTELLIGENT_TEXT_INPUT
    
    TInt physicalKeyboard = 0;
    CRepository* aknFepRepository = CRepository::NewL( KCRUidAknFep );
    // Get all the physical keyboards which are connected to the phone currently.
    aknFepRepository->Get( KAknFepPhysicalKeyboards, physicalKeyboard );
    delete aknFepRepository;
    
    
    PRINT1 ( _L("CPcsKeyMap::CurrentKeyBoardTypeL: Physical keyboard support flag = 0x%02X"), physicalKeyboard );

    // Constants follow the definition of KAknFepPhysicalKeyboards
    const TInt ptiKeyboard12Key = 0x01;         // Default
    // const TInt ptiKeyboardQwerty4x12 = 0x02;    // Not used at the moment
    const TInt ptiKeyboardQwerty4x10 = 0x04;
    // const TInt ptiKeyboardQwerty3x11 = 0x08;    // Not used at the moment
    const TInt ptiKeyboardHalfQwerty = 0x10;
    // const TInt ptiKeyboardCustomQwerty = 0x20;  // Not used at the moment

    // If there are more than 1 keyboards on HW, The priority from high to low for 
    // selecting keyboard is EPtiKeyboard12Key, EPtiKeyboardHalfQwerty, EPtiKeyboardQwerty4x10  
    if ( physicalKeyboard & ptiKeyboard12Key )
        {
        keyBoardType = EPtiKeyboard12Key;
        }
    else if ( physicalKeyboard & ptiKeyboardHalfQwerty )
        {
        keyBoardType = EPtiKeyboardHalfQwerty;
        }
    else if ( physicalKeyboard & ptiKeyboardQwerty4x10 )
        {
        keyBoardType = EPtiKeyboardQwerty4x10;
        }
    else
#endif
        {
        keyBoardType = EPtiKeyboard12Key;
        }
    return keyBoardType;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ConstructConcreteKeyMapL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructConcreteKeyMapL( TInt aKeyboardType, TLanguage aLanguage )
    {
    switch ( aKeyboardType )
        {
        case EPtiKeyboard12Key:
            {
            // Construct for Itut keyboard by default
            PRINT ( _L("CPcsKeyMap::ConstructConcreteKeyMapL: Construct keymap for ContructForItutKeyboardL"));
            
            ContructForItutKeyboardL( aLanguage );
            break;
            }
        case EPtiKeyboardHalfQwerty:
            {
            // Construct for Half Qwerty keyboard
            PRINT ( _L("CPcsKeyMap::ConstructConcreteKeyMapL: Construct keymap for ContructForHalfQwertyKeyboardL"));
            
            ContructForHalfQwertyKeyboardL( aLanguage );
            break;
            }
        case EPtiKeyboardQwerty4x10:
            {
            // Construct for 4x10 Qwerty keyboard
            PRINT ( _L("CPcsKeyMap::ConstructConcreteKeyMapL: Construct keymap for ContructFor4x10QwertyKeyboardL"));
            
            ContructFor4x10QwertyKeyboardL( aLanguage );
            break;
            }
        default:
            {
            // Construct for Itut keyboard
            PRINT ( _L("CPcsKeyMap::ConstructConcreteKeyMapL: Construct keymap for ContructForItutKeyboardL"));
            
            ContructForItutKeyboardL( aLanguage );
            }
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddKeyMapforConcreteKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddKeyMapforConcreteKeyboardL( TInt aKeyboardType, TLanguage aLanguage )
    {
    // If it's TW or HK variant, the key map for English also needs add to current key map.
    TBool needAddEnglishKeyMap = ( aLanguage == ELangTaiwanChinese || aLanguage == ELangHongKongChinese );
    
    // Add the keymap for current writing language.
    switch ( aKeyboardType )
        {
        case EPtiKeyboard12Key:
            {
            AddKeyMapforItutL( aLanguage );
            if ( needAddEnglishKeyMap )
                {
                AddKeyMapforItutL( ELangEnglish );
                }
            break;
            }
        case EPtiKeyboardHalfQwerty:
            {
            AddKeyMapforHalfQwertyKeyboardL( aLanguage );
            if ( needAddEnglishKeyMap )
                {
                AddKeyMapforHalfQwertyKeyboardL( ELangEnglish );
                }
            break;
            }
        case EPtiKeyboardQwerty4x10:
            {
            AddKeyMapfor4x10QwertyKeyboardL( aLanguage );
            if ( needAddEnglishKeyMap )
                {
                AddKeyMapfor4x10QwertyKeyboardL( ELangEnglish );
                }
            break;
            }
        default:
            {
            AddKeyMapforItutL( aLanguage );
            if ( needAddEnglishKeyMap )
                {
                AddKeyMapforItutL( ELangEnglish );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ResetKeyMap
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::ResetKeyMap()
    {
    for (TInt i = 0; i < iAllKeyMappingsPtrArr.Count(); i++)
        {
        iAllKeyMappingsPtrArr[i]->iKeyMappingArray.Close();
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
    iAllKeyMappingsPtrArr.ResetAndDestroy();
    iKeysForPoolFormation.Close();
    delete iPtiEngine;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetNumericKeyStringL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::GetNumericKeyString(const TDesC& aSrcStr, TDes& aDestStr)
    {
    for (int i = 0; i < aSrcStr.Length(); i++)
        {
        TInt index = KeyForCharacter(aSrcStr[i]);

        // If a character is not mapped to numeric key append the character
        if (index == -1)
            {
            aDestStr.Append(aSrcStr[i]);
            }
        else
            {
            aDestStr.Append(index);
            }
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::KeyForCharacter
// 
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::KeyForCharacter(const TChar& aChar)
    {
    TInt index = KErrNotFound;

    TChar lChar = User::LowerCase(aChar);

    TInt count = iAllKeyMappingsPtrArr.Count() - 1;

    for (TInt i = 0; i < count; i++)
        {
        index = iAllKeyMappingsPtrArr[i]->iKeyMappingArray.Find(lChar);
        if (index != KErrNotFound)
            {
            return (TInt) iAllKeyMappingsPtrArr[i]->key;
            }
        }

    return index;
    }
// ----------------------------------------------------------------------------
// CPcsKeyMap::ContructForHalfQwertyKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::ContructForHalfQwertyKeyboardL(TLanguage aLanguage)
    {
#ifdef RD_INTELLIGENT_TEXT_INPUT    

    iKeysForPoolFormation.Append(EPtiKeyQwertyQ);
    iKeysForPoolFormation.Append(EPtiKeyQwertyE);
    iKeysForPoolFormation.Append(EPtiKeyQwertyT);
    iKeysForPoolFormation.Append(EPtiKeyQwertyU);
    iKeysForPoolFormation.Append(EPtiKeyQwertyO);
    iKeysForPoolFormation.Append(EPtiKeyQwertyA);
    iKeysForPoolFormation.Append(EPtiKeyQwertyD);
    iKeysForPoolFormation.Append(EPtiKeyQwertyG);
    iKeysForPoolFormation.Append(EPtiKeyQwertyJ);
    iKeysForPoolFormation.Append(EPtiKeyQwertyL);
    iKeysForPoolFormation.Append(EPtiKeyQwertyZ);
    iKeysForPoolFormation.Append(EPtiKeyQwertyC);
    iKeysForPoolFormation.Append(EPtiKeyQwertyB);
    iKeysForPoolFormation.Append(EPtiKeyQwertyM);
    iKeysForPoolFormation.Append(EPtiKeyQwerty0);
    // one additional pool for special characters not mapped too any keys. 
    // This should always be the last one in the arrary
    iKeysForPoolFormation.Append(EPtiKeyNone);

    // Now add the keymap arrays to hold the keymap data
    for (TInt i = 0; i < iKeysForPoolFormation.Count(); i++)
        {
        TKeyMappingData *keyData = new (ELeave) TKeyMappingData;
        keyData->key = iKeysForPoolFormation[i];
        iAllKeyMappingsPtrArr.Append(keyData);
        }
    //  Add the keymap for current language
    AddKeyMapforHalfQwertyKeyboardL(aLanguage);

    // If it's TW or HK variant, add the keymap for English language
    if (aLanguage == ELangTaiwanChinese || aLanguage == ELangHongKongChinese)
        {
        AddKeyMapforHalfQwertyKeyboardL(ELangEnglish);
        }

    PRINT ( _L("----------------------------------------"));
#endif // RD_INTELLIGENT_TEXT_INPUT
    }
// ----------------------------------------------------------------------------
// CPcsKeyMap::AddKeyMapforHalfQwertyKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddKeyMapforHalfQwertyKeyboardL(TLanguage aLanguage)
    {
#ifdef RD_INTELLIGENT_TEXT_INPUT     	
    // Make a language object based on current language
    CPtiCoreLanguage* iCoreLanguage = static_cast<CPtiCoreLanguage*>(iPtiEngine->GetLanguage(  aLanguage));

    //Perfrom the key mappings only if the corelanguage is available
    if (iCoreLanguage)
        {
        // Get the keyboard mappings for the language
        CPtiHalfQwertyKeyMappings* ptiKeyMappings = static_cast<CPtiHalfQwertyKeyMappings*> 
                (iCoreLanguage->GetHalfQwertyKeymappings());

        iPtiEngine->ActivateLanguageL(aLanguage, EPtiEngineHalfQwerty);
        iPtiEngine->SetKeyboardType(EPtiKeyboardHalfQwerty);

        for (TInt i = 0; i < iAllKeyMappingsPtrArr.Count() - 1; i++)
            {
            AddDataForHalfQwertyKeyboardL( ptiKeyMappings, iKeysForPoolFormation[i], *(iAllKeyMappingsPtrArr[i]));
            }
        }
#endif // RD_INTELLIGENT_TEXT_INPUT        
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddDataForHalfQwertyKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddDataForHalfQwertyKeyboardL( CPtiHalfQwertyKeyMappings* aPtiKeyMappings, 
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
    
    TBuf<50> iResult;
    for (int i = 0; i < sizeof(caseArray) / sizeof(TPtiTextCase); i++)
        {
        iResult.Zero();
        aPtiKeyMappings->GetDataForKey(aKey, iResult, caseArray[i]);

        for (int j = 0; j < iResult.Length(); j++)
            aKeyDataList.iKeyMappingArray.Append(iResult[j]);

        PRINT2 ( _L("CPcsKeyMap: Mapping for Key %c = %S"), aKey,&iResult)
        }
#endif // RD_INTELLIGENT_TEXT_INPUT        
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ContructFor4x10QwertyKeyboardL
// Destructor
// ----------------------------------------------------------------------------
void CPcsKeyMap::ContructFor4x10QwertyKeyboardL(TLanguage aLanguage)
    {
#ifdef RD_INTELLIGENT_TEXT_INPUT     
// The special 4x10 Qwerty keyboard for Sonja.
    iKeysForPoolFormation.Append( EPtiKeyQwertyA );
    iKeysForPoolFormation.Append( EPtiKeyQwerty8 );
    iKeysForPoolFormation.Append( EPtiKeyQwertyC );
    iKeysForPoolFormation.Append( EPtiKeyQwertyD );
    iKeysForPoolFormation.Append( EPtiKeyQwertyE );
    iKeysForPoolFormation.Append( EPtiKeyQwerty4 );
    iKeysForPoolFormation.Append( EPtiKeyQwerty5 );
    iKeysForPoolFormation.Append( EPtiKeyQwerty6 );
    iKeysForPoolFormation.Append( EPtiKeyQwertyI );
    iKeysForPoolFormation.Append( EPtiKeyQwertyHash ); // Could have been EPtiKeyQwertyJ, EPtiKeyHash
    iKeysForPoolFormation.Append( EPtiKeyQwertyK );
    iKeysForPoolFormation.Append( EPtiKeyQwertyL );
    iKeysForPoolFormation.Append( EPtiKeyQwerty0 );
    iKeysForPoolFormation.Append( EPtiKeyQwerty9 );
    iKeysForPoolFormation.Append( EPtiKeyQwertyO );
    iKeysForPoolFormation.Append( EPtiKeyQwertyP );
    iKeysForPoolFormation.Append( EPtiKeyQwertyQ );
    iKeysForPoolFormation.Append( EPtiKeyQwerty1 );
    iKeysForPoolFormation.Append( EPtiKeyQwertyS );
    iKeysForPoolFormation.Append( EPtiKeyQwerty2 );
    iKeysForPoolFormation.Append( EPtiKeyStar );    // Could have been EPtiKeyQwertyU
    iKeysForPoolFormation.Append( EPtiKeyQwertyV );    // Could have been EPtiKeyQwerty7
    iKeysForPoolFormation.Append( EPtiKeyQwertyW );
    iKeysForPoolFormation.Append( EPtiKeyQwertyX );
    iKeysForPoolFormation.Append( EPtiKeyQwerty3 );
    iKeysForPoolFormation.Append( EPtiKeyQwertyZ );
    
    // one additional pool for special characters not mapped too any keys. 
    // This should always be the last one in the arrary
    iKeysForPoolFormation.Append(EPtiKeyNone);

    // Now add the keymap arrays to hold the keymap data
    for (TInt i = 0; i < iKeysForPoolFormation.Count(); i++)
        {
        TKeyMappingData *keyData = new (ELeave) TKeyMappingData;
        keyData->key = iKeysForPoolFormation[i];
        iAllKeyMappingsPtrArr.Append(keyData);
        }

    //  Add the keymap for current language
    AddKeyMapfor4x10QwertyKeyboardL(aLanguage);

    // If it's TW or HK variant, add the keymap for English language
    if (aLanguage == ELangTaiwanChinese || aLanguage == ELangHongKongChinese)
        {
        AddKeyMapfor4x10QwertyKeyboardL(ELangEnglish);
        }

    PRINT ( _L("----------------------------------------"));
#endif //RD_INTELLIGENT_TEXT_INPUT
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddKeyMapfor4x10QwertyKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddKeyMapfor4x10QwertyKeyboardL(TLanguage aLanguage)
    {
#ifdef RD_INTELLIGENT_TEXT_INPUT     	
    // Make a language object based on current language
    CPtiCoreLanguage* iCoreLanguage = static_cast<CPtiCoreLanguage*>(iPtiEngine->GetLanguage(aLanguage));

    //Perfrom the key mappings only if the corelanguage is available
    if (iCoreLanguage)
        {
        // Get the keyboard mappings for the language
        CPtiQwertyKeyMappings* iPtiKeyMappings = static_cast<CPtiQwertyKeyMappings *> 
                (iCoreLanguage->GetQwertyKeymappings());
        iPtiEngine->SetKeyboardType(EPtiKeyboardQwerty4x10);

        for (TInt i = 0; i < iAllKeyMappingsPtrArr.Count() - 1; i++)
            {
            AddDataFor4x10QwertyKeyboardL(iPtiKeyMappings, iKeysForPoolFormation[i], *(iAllKeyMappingsPtrArr[i]));
            }

#ifdef HACK_FOR_E72_J_KEY
        AddDataFor4x10QwertyKeyboardE72HackL();
#endif // HACK_FOR_E72_J_KEY
        }
#endif //RD_INTELLIGENT_TEXT_INPUT        

    }

#ifdef HACK_FOR_E72_J_KEY
// ----------------------------------------------------------------------------
// CPcsKeyMap::AddDataFor4x10QwertyKeyboardE72HackL
//
// Key code for J/# key in Sonja is EPtiKeyQwertyHash (127).
// No returned character code ('J', 'j', '#') has 127 as ASCII value.
// In this case we must add key to the list of characters for the pool.
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddDataFor4x10QwertyKeyboardE72HackL()
    {
#ifdef RD_INTELLIGENT_TEXT_INPUT    
    PRINT ( _L("Enter CPcsKeyMap::AddDataFor4x10QwertyKeyboardE72HackL") );

    TPtiKey keyValue = (TPtiKey) EPtiKeyQwertyHash;
    TInt keyIndex = iKeysForPoolFormation.Find(keyValue);    

    PRINT3 ( _L("CPcsKeyMap: ===== At index %d appending key to list: '%c' (#%d) -Hack-for-E72-"),
             keyIndex, (TInt) keyValue, (TInt) keyValue);

    if ( KErrNotFound != keyIndex )
        {
        iAllKeyMappingsPtrArr[keyIndex]->iKeyMappingArray.Append(keyValue);
        }

    PRINT ( _L("CPcsKeyMap: ===================================================") );  

    PRINT ( _L("End CPcsKeyMap::AddDataFor4x10QwertyKeyboardE72HackL") );
#endif // RD_INTELLIGENT_TEXT_INPUT    
    }
#endif // HACK_FOR_E72_J_KEY

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddDataFor4x10QwertyKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddDataFor4x10QwertyKeyboardL(CPtiQwertyKeyMappings* iPtiKeyMappings, 
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
        
    TBuf<50> iResult;
    for (int i = 0; i < sizeof(caseArray) / sizeof(TPtiTextCase); i++)
        {
        iResult.Zero();
        iPtiKeyMappings->GetDataForKey(aKey, iResult, caseArray[i]);

        for (int j = 0; j < iResult.Length(); j++)
            aKeyDataList.iKeyMappingArray.Append(iResult[j]);

        PRINT2 ( _L("CPcsKeyMap: Mapping for Key %c = %S"), aKey, &iResult)
        }
#endif //RD_INTELLIGENT_TEXT_INPUT        
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::ContructForItutKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::ContructForItutKeyboardL(TLanguage aLanguage)
    {
    // Add the keys for Pool formation
    iKeysForPoolFormation.Append(EPtiKey0);
    iKeysForPoolFormation.Append(EPtiKey1);
    iKeysForPoolFormation.Append(EPtiKey2);
    iKeysForPoolFormation.Append(EPtiKey3);
    iKeysForPoolFormation.Append(EPtiKey4);
    iKeysForPoolFormation.Append(EPtiKey5);
    iKeysForPoolFormation.Append(EPtiKey6);
    iKeysForPoolFormation.Append(EPtiKey7);
    iKeysForPoolFormation.Append(EPtiKey8);
    iKeysForPoolFormation.Append(EPtiKey9);
    // one additional pool for special characters not mapped too any keys. 
    // This should always be the last one in the arrary
    iKeysForPoolFormation.Append(EPtiKeyNone);

    // Now add the keymap arrays to hold the keymap data
    for (TInt i = 0; i < iKeysForPoolFormation.Count(); i++)
        {
        TKeyMappingData *keyData = new (ELeave) TKeyMappingData;
        keyData->key = iKeysForPoolFormation[i];
        iAllKeyMappingsPtrArr.Append(keyData);
        }

    //  Add the keymap for current language
    AddKeyMapforItutL(aLanguage);

    // If it's TW or HK variant, add the keymap for English language
    if (aLanguage == ELangTaiwanChinese || aLanguage == ELangHongKongChinese)
        {
        AddKeyMapforItutL(ELangEnglish);
        }

    PRINT ( _L("----------------------------------------"));

    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::AddKeyMapforItutLanguageL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddKeyMapforItutL(TLanguage aLanguage)
    {
    // Make a language object based on current language
    CPtiCoreLanguage* iCoreLanguage = static_cast<CPtiCoreLanguage*> (iPtiEngine->GetLanguage(aLanguage));

    //Perfrom the key mappings only if the corelanguage is available
    if (iCoreLanguage)
        {
        // Get the keyboard mappings for the language
        CPtiKeyMappings* iPtiKeyMappings =  static_cast<CPtiKeyMappings*> 
                (iCoreLanguage->GetKeymappings());

        for (TInt i = 0; i < iAllKeyMappingsPtrArr.Count() - 1; i++)
            {
            AddDataForItutKeyboardL(iPtiKeyMappings, iKeysForPoolFormation[i], *(iAllKeyMappingsPtrArr[i]));
            }
        }

    if (iAlgorithm->FindUtilECE()->CurrentInputLanguage() == ELangHongKongChinese)
        {
        (*(iAllKeyMappingsPtrArr[1])).iKeyMappingArray.Append(0x4E00); //heng
        (*(iAllKeyMappingsPtrArr[2])).iKeyMappingArray.Append(0x4E28); //shu
        (*(iAllKeyMappingsPtrArr[3])).iKeyMappingArray.Append(0x4E3F); //pie
        (*(iAllKeyMappingsPtrArr[4])).iKeyMappingArray.Append(0x4E36); //dian
        (*(iAllKeyMappingsPtrArr[5])).iKeyMappingArray.Append(0x4E5B); //zhe
        }

    }
// ----------------------------------------------------------------------------
// CPcsKeyMap::AddDataForItutKeyboardL
// 
// ----------------------------------------------------------------------------
void CPcsKeyMap::AddDataForItutKeyboardL(CPtiKeyMappings* iPtiKeyMappings, TPtiKey aKey, TKeyMappingData& aKeyDataList)
    {
    TPtiTextCase caseArray[] = { EPtiCaseUpper, EPtiCaseLower };
    TBuf<50> iResult;
    for (int i = 0; i< sizeof(caseArray) / sizeof(TPtiTextCase); i++)
        {
        iResult.Zero();
        iPtiKeyMappings->GetDataForKey(aKey, iResult, caseArray[i]);
        for (int j = 0; j < iResult.Length(); j++)
            aKeyDataList.iKeyMappingArray.Append(iResult[j]);
        PRINT2 ( _L("CPcsKeyMap: Mapping for Key %c = %S"), aKey,&iResult)
        }
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::IsLanguageSupportedL
// Returns ETrue if this language is supported
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::IsLanguageSupportedL(TUint32 aLang)
    {

    TBool flag = ETrue;
    for (int i = 0; i < iLanguageNotSupported.Count(); i++)
        {
        if (iLanguageNotSupported[i] == aLang)
            {
            flag = EFalse;
            }
        }

    return flag;
    }
// ----------------------------------------------------------------------------
// CPcsKeyMap::PoolIdForCharacter
// 
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::PoolIdForCharacter(TChar aChar)
    {
    TInt numValue = KErrNotFound;
    
    // If character is a Chinese word character, then we select the
    // pool ID according the first character of the first spelling of the word.
    TRAP_IGNORE( aChar = FirstCharFromSpellingL( aChar ) );
    
    TInt key = KeyForCharacter(aChar);
    if (key != KErrNotFound)
        {
        numValue = iKeysForPoolFormation.Find((TPtiKey) key);
        }
    else
        {
        // IF the key is not found, then it should go to the special pool,
        // which is the last pool of iAllKeyMappingsPtrArr
        numValue = iAllKeyMappingsPtrArr.Count() - 1;
        }

    return numValue;
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
    if ( iAlgorithm->FindUtilECE()->IsChineseWord( temp ) )
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
// CPcsKeyMap::PoolCount
// 
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::PoolCount()
    {
    return iAllKeyMappingsPtrArr.Count();
    }
// End of file
