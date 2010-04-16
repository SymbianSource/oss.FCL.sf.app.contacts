/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Retrieves the character map for each of the numeric keys.
*/

// INCLUDE FILES
#include "cpcskeymap.h"

#if defined(USE_ORBIT_KEYMAP)
#include <QLocale>
#include <hbinputlanguage.h>
#include <hbinputkeymap.h>
#include <hbinputkeymapfactory.h>
#endif // #if defined(USE_ORBIT_KEYMAP)


#if defined(_DEBUG)
#include <e32debug.h> // RDebug

#define PRINT(x)			RDebug::Print(x)
#define PRINT1(x,y)			RDebug::Print(x,y)
#define PRINT2(x,y,z)		RDebug::Print(x,y,z)
#define PRINT3(w,x,y,z)		RDebug::Print(w,x,y,z)
#else // #if defined(_DEBUG)
#define PRINT(x)            
#define PRINT1(x,y)         
#define PRINT2(x,y,z)       
#define PRINT3(w,x,y,z)
#endif // #if defined(_DEBUG)


#define KSpaceChar      ' '

#if defined(USE_ORBIT_KEYMAP)
#define KSeparatorChar  ' '
#endif // #if defined(USE_ORBIT_KEYMAP)

// Separator character stored in predictive search table columns
_LIT(KSeparator, " ");

#if defined(USE_ORBIT_KEYMAP)
// How many keys have mappings (keys 0..9 have mapping)
const TInt KAmountOfKeys = 10;

// The first key of the keyboard has value zero ('1' in the 12-key virtual keypad) 
enum TKeyId
    {
    EKey1 = 0,
    EKey2,
    EKey3,
    EKey4,
    EKey5,
    EKey6,
    EKey7,
    EKey8,
    EKey9,
	EKey0
    };
#endif // #if defined(USE_ORBIT_KEYMAP)

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsKeyMap::NewL
// ----------------------------------------------------------------------------
CPcsKeyMap* CPcsKeyMap::NewL()
	{
    PRINT( _L("Enter CPcsKeyMap::NewL") );
    
    CPcsKeyMap* self = new ( ELeave ) CPcsKeyMap();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    PRINT( _L("End CPcsKeyMap::NewL") );
    return self;
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::~CPcsKeyMap
// ----------------------------------------------------------------------------
CPcsKeyMap::~CPcsKeyMap()
    {
    PRINT( _L("Enter CPcsKeyMap::~CPcsKeyMap") );    
    PRINT( _L("End CPcsKeyMap::~CPcsKeyMap") );
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetNumericKeyStringL
// If aPlainConversion is EFalse, supports sub-string searches and space is
// converted to a separator character (not zero).
// ----------------------------------------------------------------------------
HBufC* CPcsKeyMap::GetNumericKeyStringL(const TDesC& aSource,
                                        TBool aPlainConversion) const
    {
    PRINT1( _L("Enter CPcsKeyMap::GetNumericKeyStringL input '%S'"), &aSource );    

    TInt length = aSource.Length();
    HBufC* destination = HBufC::NewL(length);
    TPtr ptr = destination->Des();

    for (TInt i = 0; i < aSource.Length(); ++i)
        {
        if (!aPlainConversion && aSource[i] == KSpaceChar)
            {
            ptr.Append(KSeparator);
            }
        else
            {
#if defined(USE_ORBIT_KEYMAP)
            ptr.Append(KeyForCharacter(aSource[i]));
#else
			TChar a = aSource[i];
			TChar b = a.GetUpperCase();
			ptr.Append(GetNumericValueForChar(b));
#endif
            }
        }

    PRINT1( _L("End CPcsKeyMap::GetNumericKeyStringL result '%S'"), destination );
    return destination;
    }

#if defined(USE_ORBIT_KEYMAP)
// ----------------------------------------------------------------------------
// CPcsKeyMap::Separator
// ----------------------------------------------------------------------------
QChar CPcsKeyMap::Separator() const
    {
    return KSeparatorChar;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::CPcsKeyMap
// Fill QList with empty strings
// ----------------------------------------------------------------------------
CPcsKeyMap::CPcsKeyMap() :
    iKeyMapping() 
	{    
    for (TInt i = 0; i < KAmountOfKeys; ++i)
        {
        iKeyMapping << QString("");
        }
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::ConstructL
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructL()
	{
	ContructKeyboardMappingsL();
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::ContructKeyboardMappingsL
// Fetch keymap for every language/country pair present.
// 10.1 only has virtual 12 key ITU-T keyboard
// ----------------------------------------------------------------------------
void CPcsKeyMap::ContructKeyboardMappingsL()
	{
    PRINT( _L("Enter CPcsKeyMap::ContructKeyboardMappingsL") );

#if defined(_DEBUG)
    TInt count(0);
#endif
	QList<HbInputLanguage> languages = AvailableLanguages();

	// Calling HbKeymapFactory::keymap() causes no memory exception after
	// ~20 different language/variant combinations in emulator.
	// In text shell all languages can be handled successfully.
	// In device, already the first call to HbKeymapFactory::keymap()
	// crashes.
	const TInt KMaxLanguages = 30;
	TInt handleMaxLanguages = languages.size();
	if (handleMaxLanguages > KMaxLanguages)
	    {
        handleMaxLanguages = KMaxLanguages;
	    }
	PRINT1( _L("handle %d languages"), handleMaxLanguages ); // test

	for (TInt lang = 0; lang < handleMaxLanguages; ++lang)
		{
        PRINT1( _L("handle language %d"), languages[lang].language() ); // test
		if (IsLanguageSupported(languages[lang].language()))
			{
			PRINT2(_L("Constructing keymap for lang=%d,var=%d"),
				   languages[lang].language(),
				   languages[lang].variant());
#if 0
			const HbKeymap* keymap =
               HbKeymapFactory::instance()->keymap(languages[lang].language(),
												   languages[lang].variant());
#else
			const HbKeymap* keymap = GetKeymap(languages[lang]);
#endif
			if (keymap)
			    {
				for (TInt key = EKey1; key <= EKey0; ++key) 
                    {
                    PRINT1( _L("handle key(enum value %d)"), key ); // test
                    const HbMappedKey* mappedKey = keymap->keyForIndex(HbKeyboardVirtual12Key, key);
                    if (!mappedKey)
                        {
                        PRINT1(_L("Mapped key not found, key(%d)"), KeyIdToNumber(key));
                        User::Leave(KErrNotFound);
                        }
                    // Get both upper and lowercase letters
                    const QString lowerCase = mappedKey->characters(HbModifierNone); // e.g. "abc2.."
                    const QString upperCase = mappedKey->characters(HbModifierShiftPressed); // e.g. "ABC2.."
                    const QString charsForKey = lowerCase + upperCase; 
    
                    // Filter out duplicate characters
                    for (TInt i = charsForKey.length() - 1; i >= 0 ; --i) 
                        {
                        QChar ch = charsForKey[i];
                        // Key '1' is at index 0 in iKeyMapping, key '2' at index 1 etc.
                        // and key '0' at the last index.
                        TInt index = key - EKey1;
                        if (!iKeyMapping[index].contains(ch))
                            {
/*
                            PRINT3(_L("CPcsKeyMap: map key(%d) <-> char='%c'(0x%x)"),
                                   KeyIdToNumber(key),
                                   ch.toAscii(),
                                   ch.toAscii()); */
#if defined(_DEBUG)
                            ++count;
#endif
                            iKeyMapping[index] += ch;
                            }
                        }
                    }
			    }
			else
                {
                PRINT(_L("CPcsKeyMap: keymap not found"));
                }
			}
		}

#if defined(_DEBUG)
    PRINT1( _L("End CPcsKeyMap::ContructKeyboardMappingsL key map has %d chars"), count );
#endif
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::AvailableLanguages
// Put default language at the beginning of the language list, so that
// ContructKeyboardMappingsL handles it first. 
// ----------------------------------------------------------------------------
QList<HbInputLanguage> CPcsKeyMap::AvailableLanguages() const
    {
    PRINT( _L("Enter CPcsKeyMap::AvailableLanguages") );

    QLocale locale;
    QLocale::Language currentLanguage = locale.language();
    if (!IsLanguageSupported(currentLanguage))
        {
        PRINT( _L("current lang not supported, use english") ); //test
        currentLanguage = QLocale::English;
        }
    HbInputLanguage defaultLanguage(currentLanguage);

    QList<HbInputLanguage> languages = HbKeymapFactory::availableLanguages();
    if (languages.contains(defaultLanguage))
        {
        PRINT( _L("remove default lang") ); //test
        languages.removeOne(defaultLanguage);
        }
    PRINT( _L("insert default lang as first lang") ); //test
    languages.insert(0, defaultLanguage);

    PRINT1( _L("End CPcsKeyMap::AvailableLanguages found %d languages"),
            languages.count() );
    return languages;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::IsLanguageSupported
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::IsLanguageSupported(QLocale::Language aLanguage) const
	{
	return (aLanguage != QLocale::Japanese && aLanguage != QLocale::Chinese);
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::KeyForCharacter
// Loop all QStrings of iKeyMapping to find one containing the character.
// If the character is not mapped, use the character itself.
// ----------------------------------------------------------------------------
TChar CPcsKeyMap::KeyForCharacter(const TChar& aChar) const
    {
    TUint charValue(aChar);
    QChar ch(charValue);
    
    for (TInt index = 0; index < KAmountOfKeys; ++index)
        {
        if (iKeyMapping[index].contains(ch))
            {
            return ArrayIndexToNumberChar(index);
            }
        }

    PRINT2( _L("CPcsKeyMap::KeyForCharacter no mapping for char '%c' (0x%x)"),
            (TUint)aChar, (TUint)aChar ); // test
    return aChar;
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::KeyIdToNumber
// Map Orbit API's key id to the number that the key results when pressed. 
// ----------------------------------------------------------------------------
TInt CPcsKeyMap::KeyIdToNumber(TInt aKeyId) const
	{
	switch (aKeyId)
		{
		case EKey1:
		case EKey2:
		case EKey3:
		case EKey4:
		case EKey5:
		case EKey6:
		case EKey7:
		case EKey8:
		case EKey9:
			return aKeyId + 1;
		case EKey0:
			return 0;
		default:
		    PRINT1( _L("CPcsKeyMap::KeyIdToNumber invalid index %d"), aKeyId );
			User::Panic(_L("CPcsKeyMap::KeyIdToNumber"), KErrArgument);
			return 0;
		}
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::ArrayIndexToNumberChar
// Map index of iKeyMapping list, to the number char that the mapping is for.
// ----------------------------------------------------------------------------
TChar CPcsKeyMap::ArrayIndexToNumberChar(TInt aArrayIndex) const
	{
	__ASSERT_DEBUG(aArrayIndex < KAmountOfKeys,
				   User::Panic(_L("CPcsKeyMap::ArrayIndexToNumberChar"),
				   KErrOverflow));
	if (aArrayIndex == KAmountOfKeys - 1)
		{
		return '0';
		}
	return aArrayIndex + '1';
	}

const HbKeymap* CPcsKeyMap::GetKeymap(HbInputLanguage aLanguage) const
    {
    PRINT(_L("CPcsKeyMap::GetKeymap"));

    const HbKeymap* keymap(NULL);
    TInt err(KErrNone);
    // Catches C++ exception and converts it to Symbian error code
    QT_TRYCATCH_ERROR(err, keymap = HbKeymapFactory::instance()->keymap(aLanguage.language(),
                                                                        aLanguage.variant()));
    if (err != KErrNone)
        {
        // In emulator, trying to get the 20th keymap results KErrNoMemory 
        PRINT1(_L("factory->keymap threw exception, err=%d"), err);
        return NULL;
        }
    PRINT(_L("CPcsKeyMap::GetKeymap got keymap"));
    return keymap;
    }
#else // #if defined(USE_ORBIT_KEYMAP)
CPcsKeyMap::CPcsKeyMap()
	{
	}

void CPcsKeyMap::ConstructL()
	{   
	}

TChar CPcsKeyMap::GetNumericValueForChar(TChar input ) const
    {
	TChar ret = '0';
    switch (input)
        {
        case 'A': 
        case 'B': 
        case 'C':
               ret = '2';
               break;
        case 'D': 
        case 'E':
        case 'F':
            ret = '3';
            break;
            
        case 'G': 
        case 'H':
        case 'I':
            ret = '4';
            break;

        case 'J': 
        case 'K': 
        case 'L':
            ret = '5';
            break;

        case 'M': 
        case 'N': 
        case 'O':
            ret = '6';
            break;
      
        case 'P': 
        case 'Q':
        case 'R': 
        case 'S':
            ret = '7';
            break;

        case 'T':
        case 'U': 
        case 'V':
            ret = '8';
            break;

        case 'W':
        case 'X':
        case 'Y': 
        case 'Z':
            ret = '9';
            break;
            
		case ' ':
			ret = '0';
			break;

		default: // Other chars, e.g. numbers
			ret = input;
        }
    return ret;    
    }
#endif // #if defined(USE_ORBIT_KEYMAP)

// End of file
