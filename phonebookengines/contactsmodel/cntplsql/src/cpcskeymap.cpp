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

// If defined, only the currently used language's keymap is used
//#define USE_ONLY_DEFAULT_LANG_KEYMAP


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
// How many keys have mappings in ITU-T keypad (keys 0..9, * and # have mappings)
const TInt KAmountOfKeys = 12;

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
	EKey0,
	EKeyStar,
	EKeyHash,
	ELastKey = EKeyHash
    };

const QChar KStar = '*';
const QChar KPlus = '+';
const QChar KHash = '#';
#endif // #if defined(USE_ORBIT_KEYMAP)

// * key is mapped to this
const TChar KMappedCharForStar = 'a';
// # key is mapped to this
const TChar KMappedCharForHash = 'b';
// Unmapped (unknown) characters are replaced with this
const TChar KPadChar = 'f';


// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsKeyMap::NewL
// ----------------------------------------------------------------------------
CPcsKeyMap* CPcsKeyMap::NewL()
	{
    PRINT(_L("Enter CPcsKeyMap::NewL"));
    
    CPcsKeyMap* self = new (ELeave) CPcsKeyMap();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    PRINT(_L("End CPcsKeyMap::NewL"));
    return self;
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::~CPcsKeyMap
// ----------------------------------------------------------------------------
CPcsKeyMap::~CPcsKeyMap()
    {
    PRINT(_L("Enter CPcsKeyMap::~CPcsKeyMap"));    
    PRINT(_L("End CPcsKeyMap::~CPcsKeyMap"));
    }

// ----------------------------------------------------------------------------
// CPcsKeyMap::GetNumericKeyStringL
// If aPlainConversion is EFalse, supports sub-string searches and space is
// converted to a separator character (not zero).
// ----------------------------------------------------------------------------
HBufC* CPcsKeyMap::GetNumericKeyStringL(const TDesC& aSource,
                                        TBool aPlainConversion) const
    {
    PRINT1(_L("Enter CPcsKeyMap::GetNumericKeyStringL input '%S'"), &aSource);

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

    PRINT1(_L("End CPcsKeyMap::GetNumericKeyStringL result '%S'"), destination);
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
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::ConstructL
// ----------------------------------------------------------------------------
void CPcsKeyMap::ConstructL()
	{
	TInt err(KErrNone);
	QT_TRYCATCH_ERROR(err, ContructKeyboardMappings());
    if (err != KErrNone)
        {
        PRINT1(_L("ContructKeyboardMappings threw exception, err=%d"), err);
        User::Leave(err);
        }
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::ContructKeyboardMappings
// Fetch keymap for every language/country pair present.
// Even though most languages map *, + and # to 1-key, they are here mapped to
// the distinct *-key or #-key of the 12-key ITU-T keypad.
// ----------------------------------------------------------------------------
void CPcsKeyMap::ContructKeyboardMappings()
	{
    PRINT(_L("Enter CPcsKeyMap::ContructKeyboardMappings"));

	for (TInt i = 0; i < KAmountOfKeys; ++i)
        {
        iKeyMapping << QString("");
        }
	
	iKeyMapping[EKeyStar].append(KStar);
	iKeyMapping[EKeyStar].append(KPlus);
	iKeyMapping[EKeyHash].append(KHash);
	iHardcodedChars.append(KStar);
	iHardcodedChars.append(KPlus);
	iHardcodedChars.append(KHash);

#if defined(_DEBUG)
    TInt count(0);
#endif
    QList<HbInputLanguage> languages;
#if defined(USE_ONLY_DEFAULT_LANG_KEYMAP)
	HbInputLanguage inputLanguage(QLocale::system().language()); 
	languages << inputLanguage;
#else
    languages = AvailableLanguages();
#endif
	TInt languageCount = languages.size();
	for (TInt lang = 0; lang < languageCount; ++lang)
		{
        PRINT2(_L("(%d) handle language %d"), lang, languages[lang].language());
		if (IsLanguageSupported(languages[lang].language()))
			{
/*			PRINT2(_L("Constructing keymap for lang=%d,var=%d"),
				   languages[lang].language(),
				   languages[lang].variant()); */
			const HbKeymap* keymap =
				HbKeymapFactory::instance()->keymap(languages[lang].language(),
                                                    languages[lang].variant());
			if (keymap)
			    {
				for (TInt key = EKey1; key <= ELastKey; ++key) 
                    {
//                    PRINT1(_L("handle key(enum value %d)"), key); // test
                    const HbMappedKey* mappedKey = keymap->keyForIndex(HbKeyboardVirtual12Key, key);
					// mappedKey can be NULL, as most languages don't have mapping for EKeyStar, EKeyHash
                    if (mappedKey)
                        {
						const QString lowerCase = mappedKey->characters(HbModifierNone); // "abc2.."
						const QString upperCase = mappedKey->characters(HbModifierShiftPressed); // "ABC2.."
						const QString charsForKey = lowerCase + upperCase; 
	    
						// Filter out duplicate characters
						for (TInt i = charsForKey.length() - 1; i >= 0 ; --i) 
							{
							QChar ch = charsForKey[i];
							if (!iKeyMapping[key].contains(ch) &&
								!iHardcodedChars.contains(ch))
								{
/*								PRINT3(_L("CPcsKeyMap: map key(%d) <-> char='%c'(0x%x)"),
									   KeyIdToNumber(key),
									   ch.toAscii(),
									   ch.toAscii()); */
#if defined(_DEBUG)
								++count;
#endif
								iKeyMapping[key] += ch;
								}
							}
						}
                    }
			    }
			else
                {
                PRINT(_L("CPcsKeyMap::ContructKeyboardMapping keymap not found"));
                }
			}
		}

#if defined(_DEBUG)
    PRINT1(_L("End CPcsKeyMap::ContructKeyboardMappings keymap has %d chars"), count);
#endif
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::AvailableLanguages
// Put default language at the beginning of the language list, so that
// ContructKeyboardMappingsL handles it first. 
// ----------------------------------------------------------------------------
QList<HbInputLanguage> CPcsKeyMap::AvailableLanguages() const
    {
    PRINT(_L("Enter CPcsKeyMap::AvailableLanguages"));

	QList<HbInputLanguage> languages = HbKeymapFactory::availableLanguages();

#if 1 // This code would make sure the default language is at the beginning of
	  // list of available languages. But since there is resource leak, the code
	  // is currently commented out until the leak is fixed.
	QLocale::Language currentLanguage = QLocale::system().language();
    if (!IsLanguageSupported(currentLanguage))
        {
        PRINT(_L("current lang not supported, use english")); //test
        currentLanguage = QLocale::English;
        }
    HbInputLanguage defaultLanguage(currentLanguage);    
    if (languages.contains(defaultLanguage))
        {
        PRINT(_L("remove default lang")); //test
        languages.removeOne(defaultLanguage);
        }
    PRINT(_L("insert default lang as first lang")); //test
    languages.prepend(defaultLanguage); // THIS LEAKS RESOURCES!
#endif

    PRINT1(_L("End CPcsKeyMap::AvailableLanguages found %d languages"),
           languages.count());
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

    PRINT2(_L("CPcsKeyMap::KeyForCharacter no mapping for char '%c' (0x%x)"),
           (TUint)aChar, (TUint)aChar);
	return KPadChar;
    }

#if defined(_DEBUG)
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
		case EKeyStar:
		case EKeyHash:
			return aKeyId + 1;
		case EKey0:
			return 0;
		default:
		    PRINT1(_L("CPcsKeyMap::KeyIdToNumber invalid index %d"), aKeyId);
			User::Panic(_L("CPcsKeyMap::KeyIdToNumber"), KErrArgument);
			return 0;
		}
	}
#endif

// ----------------------------------------------------------------------------
// CPcsKeyMap::ArrayIndexToNumberChar
// Map index of iKeyMapping list, to the number char that the mapping is for.
// ----------------------------------------------------------------------------
TChar CPcsKeyMap::ArrayIndexToNumberChar(TInt aArrayIndex) const
	{
	__ASSERT_DEBUG(aArrayIndex < KAmountOfKeys,
				   User::Panic(_L("CPcsKeyMap::ArrayIndexToNumberChar"),
				   KErrOverflow));
	switch (aArrayIndex)
		{
		case EKey0:
			return '0';
		case EKeyStar:
			return KMappedCharForStar;
		case EKeyHash:
			return KMappedCharForHash;
		default:
			return aArrayIndex + '1';
		}
	}
#else // #if defined(USE_ORBIT_KEYMAP)
CPcsKeyMap::CPcsKeyMap()
	{
	}

void CPcsKeyMap::ConstructL()
	{
	}

TChar CPcsKeyMap::GetNumericValueForChar(TChar input) const
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

		case '*':
		case '+':
		    ret = KMappedCharForStar;
		    break;
		    
		case '#':
		    ret = KMappedCharForHash;
		    break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case ' ':
            ret = input; // Numbers and space
            break;

		default: // Other (unknown) chars
		    ret = KPadChar;
        }
    return ret;    
    }
#endif // #if defined(USE_ORBIT_KEYMAP)

// End of file
