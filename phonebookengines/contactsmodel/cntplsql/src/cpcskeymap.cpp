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
#define USE_ONLY_DEFAULT_LANG_KEYMAP


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
	TBool ok(EFalse);
	TInt err(KErrNone);
	QT_TRYCATCH_ERROR(err, ok = ContructKeyboardMappings());
    if (err != KErrNone)
        {
        PRINT1(_L("ContructKeyboardMappings threw exception, err=%d"), err);
        User::Leave(err);
        }
	if (!ok)
        {
        PRINT(_L("ContructKeyboardMappings returns error"));
        User::Leave(KErrGeneral);
        }
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::ContructKeyboardMappings
// Fetch keymap for every language/country pair present.
// 10.1 only has virtual 12 key ITU-T keyboard
// ----------------------------------------------------------------------------
TBool CPcsKeyMap::ContructKeyboardMappings()
	{
    PRINT( _L("Enter CPcsKeyMap::ContructKeyboardMappings") );

#if defined(_DEBUG)
    TInt count(0);
#endif

    TInt err(KErrNone);
    QList<HbInputLanguage> languages;
#if defined(USE_ONLY_DEFAULT_LANG_KEYMAP)
	HbInputLanguage inputLanguage(QLocale::system().language()); 
	languages << inputLanguage;
#else
    languages = AvailableLanguages();
#endif
	// Calling HbKeymapFactory::keymap() causes "no memory" exception after
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
//        PRINT1( _L("handle language %d"), languages[lang].language() ); // test
		if (IsLanguageSupported(languages[lang].language()))
			{
			PRINT2(_L("Constructing keymap for lang=%d,var=%d"),
				   languages[lang].language(),
				   languages[lang].variant());
			const HbKeymap* keymap =
				HbKeymapFactory::instance()->keymap(languages[lang].language(),
                                                    languages[lang].variant());
			if (keymap)
			    {
				for (TInt key = EKey1; key <= EKey0; ++key) 
                    {
                    PRINT1( _L("handle key(enum value %d)"), key ); // test
                    const HbMappedKey* mappedKey = keymap->keyForIndex(HbKeyboardVirtual12Key, key);
                    if (!mappedKey)
                        {
                        PRINT1(_L("Mapped key not found, key(%d)"), KeyIdToNumber(key));
                        return EFalse;
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
                PRINT(_L("CPcsKeyMap::ContructKeyboardMapping keymap not found"));
                }
			}
		}

#if defined(_DEBUG)
    PRINT1( _L("End CPcsKeyMap::ContructKeyboardMappings keymap has %d chars"), count );
#endif
	return ETrue;
	}

// ----------------------------------------------------------------------------
// CPcsKeyMap::AvailableLanguages
// Put default language at the beginning of the language list, so that
// ContructKeyboardMappingsL handles it first. 
// ----------------------------------------------------------------------------
QList<HbInputLanguage> CPcsKeyMap::AvailableLanguages() const
    {
    PRINT( _L("Enter CPcsKeyMap::AvailableLanguages") );

	QList<HbInputLanguage> languages = HbKeymapFactory::availableLanguages();

#if 0 // This code would make sure the default language is at the beginning of
	  // list of available languages. But since there is resource leak, the code
	  // is currently commented out until the leak is fixed.
	QLocale::Language currentLanguage = QLocale::system().language();
    if (!IsLanguageSupported(currentLanguage))
        {
        PRINT( _L("current lang not supported, use english") ); //test
        currentLanguage = QLocale::English;
        }
    HbInputLanguage defaultLanguage(currentLanguage);    
    if (languages.contains(defaultLanguage))
        {
        PRINT( _L("remove default lang") ); //test
        languages.removeOne(defaultLanguage);
        }
    PRINT( _L("insert default lang as first lang") ); //test
    languages.prepend(defaultLanguage); // THIS LEAKS RESOURCES!
#endif

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
