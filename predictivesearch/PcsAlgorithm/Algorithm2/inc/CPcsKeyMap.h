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


// INCLUDES
#include <e32base.h>
#include <e32property.h>
#include <PtiDefs.h>
#include "CPcsDefs.h"

class CPcsAlgorithm2;
class CPsQuery;
class CPtiEngine;
class CPtiKeyMapData;

class TKeyMappingData
    {
public:
    TPtiKey iKey; // key
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
     * Convert functions to get the key string from query and/or chars string
     */
    void GetMixedKeyStringForQueryL(CPsQuery& aSrcQuery, TDes& aDestStr) const;
    void GetMixedKeyStringForDataL(CPsQuery& aSrcQuery, const TDesC& aSrcData, TDes& aDestStr) const;

    /**
     * Returns the numeric key id corresponding to a specific character
     */
    TPtiKey KeyForCharacter(TText aChar, TKeyboardModes aKbMode) const;

    /**
     * Returns ETrue if this language is supported
     */
    TBool IsLanguageSupportedL(const TUint32 aLanguage);

    void ReconstructKeymapL();
    
    /**
     * Returns the pool Id for a key
     */
    TInt PoolIdForCharacter(TChar aChar, TKeyboardModes aKbMode );
    
    /**
     * Returns total number of pools
     */
    TInt PoolCount();

    /**
     * Returns true if the characters "0" and " " are on the same key
     */
    TBool GetSpaceAndZeroOnSameKey( TKeyboardModes aMode );

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
     * Set flags to indicate if space and zero are on same key.
     * Keymappings must be constructed before calling this.
     */
    void SetSpaceAndZeroOnSameKey();
    
    /**
     * Helper function to construct keyboard specific mappings
     */
    void ConstructForItutKeyboardL();
    void ConstructForQwertyKeyboardL(TPtiKeyboardType aKbType);

    /**
     * Helper function to populate Itut key mappings
     */
    void AddKeyMapforItutL(TLanguage aLanguage);

    void AddDataForItutKeyboardL(CPtiKeyMapData* aPtiKeyMapData,
            TPtiKey aKey, TKeyMappingData& aKeyDataList);
    /**
     * Helper function to populate 4x10 Qwerty Keyboard  key mappings
     */
    void AddKeyMapForQwertyKeyboardL(TLanguage aLanguage, TPtiKeyboardType aKbType);

    void AddDataForQwertyKeyboardL(
            CPtiKeyMapData* aPtiKeyMapData, 
            TPtiKeyboardType aKbType, 
            TPtiKey aKey,
            TKeyMappingData& aKeyDataList);
    
    /**
     * Create list of PTI keys for the given keyboard type
     */
    void CreateKeyListFromKeyBindingTable( RArray<TPtiKey>& aKeyArray, TPtiKeyboardType aKbType );

    void ResetKeyMap();
    
    /**
     * Function to construct key mappings for a particula language
     * 
     */
    void ConstructKeymapL();
    
    /**
     * Select keyboard types according to available physical keyboard(s)
     */
    void SetupKeyboardTypesL();
    
    /**
     * 
     * Construct keymaps
     */
    void ConstructConcreteKeyMapL();
    
    /**
     * 
     * Add keymap for keyboard by type and language
     * @ aKeyboardType, the current keyboard type.
     * @ aLanguage, the keymap construct language.
     */
    void AddKeyMapforConcreteKeyboardL( TLanguage aLanguage );

    /**
     * In case the given character is a Chinese characer, give the first
     * character of it's first spelling using the current spelling mode.
     * Otherwise, the character is returned unmodified.
     */
    TChar FirstCharFromSpellingL( TChar aChar ) const;
    
    TInt PoolIdForKey(TPtiKey aKey, TKeyboardModes aKbMode) const;
    
    /**
     * Gets predictive keyboard mapping data for given match mode.
     * @param   aMode      Matching mode for which keymappings are asked. 
     * @return  Mapping table for the given mode. NULL if asked for non-predictive mode.
     */
    const RPointerArray<TKeyMappingData>* KeyMappings( TKeyboardModes aMode ) const;

    /**
     * Resolve any ambiguity from the given keyboard mode.
     * Keyboard mode "predictive default keyboard" will be replaced by 
     * "ITU-T predictive" or "QWERTY predictive", depending on the configuration
     * of the device. Predictive keyboard modes, which can't be supported
     * in this device, are replaced with "non-predictive" mode.
     */
    TKeyboardModes ResolveKeyboardMode( TKeyboardModes aKbMode ) const;
    
    /**
     * Get the default charcter for the given key in given keyboard.
     * The default character is used to construct the compare string for
     * predictive mode.
     */
    TText DefaultCharForKey( TPtiKey aKey, TKeyboardModes aKbMode ) const;
    
private:
    /**
     * PTI engine istance. Owned.
     */
    CPtiEngine* iPtiEngine;

    /**
     * Algorithm instacne pointer. NOT owned.
     */
    CPcsAlgorithm2* iAlgorithm;
    
    /**
     * Array to hold the list of languages not supported
     */
    RArray<TLanguage> iLanguageNotSupported;

    /**
     * Type of keyboard used in ITU-T search mode. Typically this is standard ITU-T
     * 12-key keypad.
     */
    TPtiKeyboardType iItutKeyboardType;

    /**
     * PTI keys of the ITU-T keyboard.
     */
    RArray<TPtiKey> iItutKeys;

    /**
     * Contains all the ITU-T keys and the characters mapped to each key. Owned.
     */
    RPointerArray<TKeyMappingData> iItutKeyMappings;

    /**
     * True if "0" and " " are on the same key in the ITU-T mode
     */
    TBool iSpaceAndZeroOnSameKeyOnItut;
    
    /**
     * Type of keyboard used in QWERTY search mode. Typically this is either 3x11 or 4x10 QWERTY
     * or EPtiKeyboardNone if there's no any kind of QWERTY available. This may also be
     * half-QWERTY.
     */
    TPtiKeyboardType iQwertyKeyboardType;

    /**
     * PTI keys of the QWERTY keyboard.
     */
    RArray<TPtiKey> iQwertyKeys;

    /**
     * Contains all the QWERTY keys and the characters mapped to each key. Owned.
     */
    RPointerArray<TKeyMappingData> iQwertyKeyMappings;
    
    /**
     * True if "0" and " " are on the same key in the QWERTY mode
     */
    TBool iSpaceAndZeroOnSameKeyOnQwerty;
    
    /**
     * Flag to indicate if the default multi-matching mode is ITU-T or QWERTY mode.
     * ETrue, if ITU-T is defualt and EFalse if QWERTY is default.
     */
    TBool iItutIsDefault;
    };

#endif // __CPCS_KEY_MAP_ALGORITHM_2_H__

// End of file
