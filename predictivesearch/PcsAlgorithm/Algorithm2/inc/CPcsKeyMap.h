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

#ifndef __CPCS_KEY_MAP_ALGORITHM_2_H__
#define __CPCS_KEY_MAP_ALGORITHM_2_H__

// The macro is used for a hack which fix a problem of PtiEngine with Sonja keyboard.
// The problem is that J/# key can't returned character code ('J', 'j', '#') 
#define HACK_FOR_E72_J_KEY

// INCLUDES
#include <e32base.h>
#include <PtiEngine.h>
#include <PtiKeyMappings.h>
#include <AvkonInternalCRKeys.h>
#include <e32property.h>

class CPcsAlgorithm2;
class     CPtiEngine;

class TKeyMappingData
    {
public:
    TPtiKey key; // key
    RArray<TInt> iKeyMappingArray; // All the characters mapped to the key
    };

// CLASS DECLARATION
class CPcsKeyMap : public CBase
    {

public:

    /**
     * Two phase construction
     */
    static CPcsKeyMap* NewL(CPcsAlgorithm2* aAlgorithm);

    /**
     * Destructor
     */
    virtual ~CPcsKeyMap();

    /**
     * Converts the alphanumeric string to numeric string
     */
    void GetNumericKeyString(const TDesC& alphaNumericStr, TDes& numericStr);

    /**
     * Returns the numeric key id corresponding to a specific character		                          
     */
    TInt KeyForCharacter(const TChar& aChar);

    /**
     * Returns ETrue if this language is supported
     */
    TBool IsLanguageSupportedL(const TUint32 aLanguage);

    void ReconstructKeymapL();
    /**
     * Returns the pool Id for a key                               
     */
    TInt CPcsKeyMap::PoolIdForCharacter(const TChar& aChar);
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
    void ConstructL(CPcsAlgorithm2* aAlgorithm);

    /**
     * Helper function to construct keyboard specific mappings
     */
    void ContructForItutKeyboardL(TLanguage aLanguage);
    void ContructForHalfQwertyKeyboardL(TLanguage aLanguage);
    void ContructFor4x10QwertyKeyboardL(TLanguage aLanguage);

    /**
     * Helper function to populate Itut key mappings
     */
    void AddKeyMapforItutL(TLanguage aLanguage);

    void AddDataForItutKeyboardL(CPtiKeyMappings* aPtiKeyMappings,
            TPtiKey aKey, TKeyMappingData& aKeyDataList);
    /**
     * Helper function to populate 4x10 Qwerty Keyboard  key mappings
     */
    void AddKeyMapfor4x10QwertyKeyboardL(TLanguage aLanguage);

    void AddDataFor4x10QwertyKeyboardL(
            CPtiQwertyKeyMappings* aPtiKeyMappings, TPtiKey aKey,
            TKeyMappingData& aKeyDataList);
    
#ifdef HACK_FOR_E72_J_KEY
    void AddDataFor4x10QwertyKeyboardE72HackL(); // Hack for E72 (J/# key)
#endif  // HACK_FOR_E72_J_KEY
    
    /**
     * Helper function to populate alfQwerty Keyboard key mappings
     */
    void AddKeyMapforHalfQwertyKeyboardL(TLanguage aLanguage);

    void AddDataForHalfQwertyKeyboardL(CPtiHalfQwertyKeyMappings* aPtiKeyMappings,
            TPtiKey aKey, TKeyMappingData& aKeyDataList);

    void ResetKeyMap();
    /**
     * Function to construct key mappings for a particula language
     * 
     */
    void ConstructKeymapL();
    
    /**
     * Get the current keyboard type.
     * @return current keyboard type.
     */
    TInt CurrentKeyBoardTypeL();
    
    /**
     * 
     * Construct keymap by type and language
     * @ aKeyboardType, the current keyboard type.
     * @ aLanguage, the keymap construct language.
     */
    void ConstructConcreteKeyMapL( TInt aKeyboardType, TLanguage aLanguage );
    
    /**
     * 
     * Add keymap for keyboard by type and language
     * @ aKeyboardType, the current keyboard type.
     * @ aLanguage, the keymap construct language.
     */
    void AddKeyMapforConcreteKeyboardL( TInt aKeyboardType, TLanguage aLanguage );

private:
    CPtiEngine* iPtiEngine;

    CPcsAlgorithm2* iAlgorithm;
    /**
     * Array to hold the list of languages not supported
     */
    RArray<TLanguage> iLanguageNotSupported;

    /**
     * Flag to indicate if Phone is Chinese variant 
     */
    RArray<TPtiKey> iKeysForPoolFormation;

    /**
     * Contains all the keys and the characters mapped to each key
     * own
     */
    RPointerArray<TKeyMappingData> iAllKeyMappingsPtrArr;
    
    };

#endif // __CPCS_KEY_MAP_ALGORITHM_2_H__

// End of file
