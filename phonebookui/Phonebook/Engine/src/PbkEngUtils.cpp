/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*		Namespace structure for Phonebook engine helper functions
*
*/


// INCLUDE FILES
#include <PbkEngUtils.h>
#include <f32file.h>
#include <barsc.h>
#include <bautils.h>
#include <featmgr.h>
#include "PbkDataCaging.hrh"

/// Unnamed namespace for local definitions
namespace __RVCT_UNS_PbkEngUtils {

// LOCAL CONSTANTS
enum TPanicCode
    {
    EPanicPreCond_CopyAndReplaceChars = 1,
    EPanicPostCond_TrimAllLength,
    EPanicPostCond_TrimRightLength,
    EPanicPreCond_TrimAllAppend,
    EPanicPostCond_TrimAllAppend,
    EPanicPreCond_TrimRightAppend,
    EPanicPostCond_TrimRightAppend

    };

_LIT(KEngDefResFileName, "PbkEng.rsc");  // default resource file name
_LIT(KEngChineseResFileName, "PbkEngChinese.rsc");   // Chinese resource
_LIT(KEngJapaneseResFileName, "PbkEngJapanese.rsc"); // Japanese resource

// ================= LOCAL FUNCTIONS =======================
#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "PbkEngUtils");
    User::Panic(KPanicText, aReason);
    }
#endif

const TUint KPbkZWSCharacter = 0x200B;
const TUint KPbkZWNJCharacter = 0x200C;

// Check if the given char is a Zero Width Space or Non-Joiner character
inline TBool ZWSCharacter( const TChar aChar )
    {
    const TChar zwsChar( KPbkZWSCharacter );
    const TChar zwnjChar( KPbkZWNJCharacter );
    return (aChar == zwsChar) || (aChar == zwnjChar);
    }

/**
 * Implementation class for PbkEngUtils::BreakInWordsLC.
 */
NONSHARABLE_CLASS(CWords) : 
        public CBase, 
        public MDesCArray
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aText the text to separate
		 * @param aWordSeparatorFunc function for detecting
		 *        word separator characters
         */
        static CWords* NewLC
            (const TDesC& aText, 
            PbkEngUtils::TIsWordSeparator aWordSeparatorFunc);
        /**
         * Destructor.
		 */
        ~CWords();
        
    public:  // from MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(TInt aIndex) const;

    private:  // Implementation
        CWords(PbkEngUtils::TIsWordSeparator aWordSeparatorFunc);
        void ConstructL
            (const TDesC& aText);
        static TBool DefaultIsWordSeparator(TChar aChar);

    private:
		/// Own: function for detecting word separator characters
        PbkEngUtils::TIsWordSeparator iWordSeparatorFunc;
		/// Own: array of words
        RArray<TPtrC> iWords;
    };
    
/**
 * Solves correct language specific engine resource filename. 
 *
 * @return A reference to resource file
 */
inline const TDesC& EngResFileName()
    {
    if (FeatureManager::FeatureSupported(KFeatureIdChinese))
        {
        return KEngChineseResFileName;
        }
    else if (FeatureManager::FeatureSupported(KFeatureIdJapanese))
        {
        return KEngJapaneseResFileName;
        }
    return KEngDefResFileName;
    }
    
    
}
using namespace __RVCT_UNS_PbkEngUtils;


// ================= MEMBER FUNCTIONS =======================

EXPORT_C void PbkEngUtils::FindAndOpenResourceFileLC
        (RFs& aFs, 
        const TDesC& aResFileName, 
        const TDesC& aResFilePath, 
        const TDesC& aResFileDrive,
        RResourceFile& aResFile)
    {
    _LIT(KSeparator, "\\");
    TFileName fileName;
    fileName.Copy(aResFileDrive);
    fileName.Append(aResFilePath);
    fileName.Append(KSeparator);
    fileName.Append(aResFileName);

	BaflUtils::NearestLanguageFile(aFs, fileName);
    aResFile.OpenL(aFs, fileName);
    CleanupClosePushL(aResFile);
    }

EXPORT_C void PbkEngUtils::FindAndOpenDefaultResourceFileLC
        (RFs& aFs, 
        RResourceFile& aResFile)
    {
    FindAndOpenResourceFileLC(aFs, EngResFileName(), KDC_RESOURCE_FILES_DIR, 
            KPbkRomFileDrive, aResFile);
    aResFile.ConfirmSignatureL();
    }

EXPORT_C TInt PbkEngUtils::CopyDigitsFromEnd(
        const TDesC& aNumText, 
        TInt aNumberOfDigits,
        TDes& aDigitsText)
    {
    aDigitsText.Zero();
    // Scan aNumText from the end
    TInt pos = aNumText.Length();

    // Loop for number of digits to extract
    TInt digit = 0;
    for (; digit < aNumberOfDigits; ++digit)
        {
        // Scan aNumText backwars for the next number
        while (--pos >= 0 && !TChar(aNumText[pos]).IsDigit()) { }
        if (pos < 0)
            break;
        // Number found, append it to result string
        aDigitsText.Append(aNumText[pos]);
        }

    // Reverse result string to correct order
    TInt end = aDigitsText.Length();
    for (pos = 0; pos < --end ; ++pos)
        {
        const TText tmp = aDigitsText[pos];
        aDigitsText[pos] = aDigitsText[end];
        aDigitsText[end] = tmp;
        }

    // Return number of digits extracted
    return digit;
    }

EXPORT_C TBool PbkEngUtils::ContainSameDigits
        (const TDesC& aNumText1, 
        const TDesC& aNumText2, 
        TInt aNumberOfDigits/*=0*/)
    {
    TInt pos1 = aNumText1.Length();
    TInt pos2 = aNumText2.Length();
    FOREVER
        {
        // Scan for the next digit in both strings
        while (--pos1 >= 0 && !TChar(aNumText1[pos1]).IsDigit()) { }
        while (--pos2 >= 0 && !TChar(aNumText2[pos2]).IsDigit()) { }

        if (pos1 < 0 && pos2 < 0)
            {
            // Run to the end of both strings
            break;
            }

        if ((pos1 < 0 && pos2 >= 0) || (pos1 >= 0 && pos2 < 0))
            {
            // Run out of the other string while the other still
            // contains digits to compare
            return EFalse;
            }

        if (pos1 >= 0 && pos2 >= 0 && aNumText1[pos1] != aNumText2[pos2])
            {
            // Different digits found
            return EFalse;
            }

        if (--aNumberOfDigits == 0)
            {
            // All requested digits compared ok
            break;
            }
        }

    // Didn't find nonmatching digits
    return ETrue;
    }

EXPORT_C TInt PbkEngUtils::AppendAndReplaceChars
        (TDes& aDest, const TDesC& aSrc, 
        const TDesC& aCharsToReplace, const TDesC& aReplaceChars)
    {
    // Check that aDest has enough capacity
    __ASSERT_DEBUG((aDest.MaxLength()-aDest.Length()) >= aSrc.Length(),
        Panic(EPanicPreCond_CopyAndReplaceChars));
    // Check that a replacament is provided for all characters
    __ASSERT_DEBUG(aCharsToReplace.Length() == aReplaceChars.Length(),
        Panic(EPanicPreCond_CopyAndReplaceChars));

    TInt count, i;
    const TInt sourceLenght = aSrc.Length();
    for (count=0, i=0; i < sourceLenght; ++i)
        {
        TText ch = aSrc[i];
        const TInt replacePos = aCharsToReplace.Locate(aSrc[i]);
        if (replacePos != KErrNotFound)
            {
            ++count;
            ch = aReplaceChars[replacePos];
            }
        aDest.Append(ch);
        }

    return count;
    }

EXPORT_C TInt PbkEngUtils::CountSpaces(const TDesC& aText)
    {
    TInt result = 0;
    const TInt length = aText.Length();
    for (TInt i=0; i<length; ++i)
        {
        if (TChar(aText[i]).IsSpace())
            {
            ++result;
            }
        }
    return result;
    }

EXPORT_C void PbkEngUtils::ReplaceNonGraphicCharacters
        (TDes& aText, TText aChar)
    {
    const TInt len = aText.Length();
    for (TInt i=0; i < len; ++i)
        {
        if (!TChar(aText[i]).IsGraph() &&
            !ZWSCharacter( aText[i] ) )
            {
            // replace non-graphic character with aChar
            aText[i] = aChar;
            }
        }
    }

EXPORT_C void PbkEngUtils::AppendGraphicCharacters
        (TDes& aDest, const TDesC& aSrc, TText aChar)
    {
    const TInt len = aSrc.Length();
    for (TInt i=0; i < len; ++i)
        {
        TText ch(aSrc[i]);
        if (TChar(ch).IsGraph() || 
            ZWSCharacter(ch))
            {
            aDest.Append(ch);
            }
        else
            {
            aDest.Append(aChar);
            }
        }
    }

EXPORT_C MDesCArray* PbkEngUtils::BreakInWordsLC
        (const TDesC& aText, 
        TIsWordSeparator aWordSeparatorFunc/*=NULL*/)
    {
    return CWords::NewLC(aText,aWordSeparatorFunc);
    }

EXPORT_C HBufC* PbkEngUtils::AllocL(HBufC* aBuffer, TInt aMaxLength)
    {
    if (aBuffer)
        {
        TPtr bufPtr(aBuffer->Des());
        if (bufPtr.MaxLength() < aMaxLength)
            {
            aBuffer = aBuffer->ReAllocL(aMaxLength);
            }
        }
    else
        {
        aBuffer = HBufC::NewL(aMaxLength);
        }
    return aBuffer;
    }

EXPORT_C HBufC* PbkEngUtils::CopyL
        (HBufC* aBuffer, const TDesC& aText, TInt aMinBufLength/*=0*/)
    {
    aBuffer = AllocL(aBuffer,Max(aText.Length(),aMinBufLength));
    *aBuffer = aText;
    return aBuffer;
    }

EXPORT_C TInt PbkEngUtils::TrimAllLength(const TDesC& aText)
    {
    const TInt textLength=aText.Length();
    TInt result=0;
    TBool uncommittedSpace=EFalse;
    for (TInt i=0; i < textLength; ++i)
        {
        if (TChar(aText[i]).IsSpace())
            {
            if (result > 0)
                {
                // Not a leading space, set uncommitted space flag
                uncommittedSpace = ETrue;
                }
            }
        else
            {
            if (uncommittedSpace)
                {
                // Add previously encountered space(s)
                ++result;
                uncommittedSpace = EFalse;
                }
            // Add the character
            ++result;
            }
        }

    __ASSERT_DEBUG(result <= aText.Length(), 
        Panic(EPanicPostCond_TrimAllLength));
    return result;
    }

EXPORT_C TInt PbkEngUtils::TrimAllAppend(const TDesC& aText, TDes& aDest)
    {
#ifdef _DEBUG
    const TInt oldDestLength = aDest.Length();
    __ASSERT_DEBUG(aDest.MaxLength()-oldDestLength >= TrimAllLength(aText), 
        Panic(EPanicPreCond_TrimAllAppend));
#endif

    const TInt textLength=aText.Length();
    TInt result=0;
    TBool uncommittedSpace=EFalse;
    for (TInt i=0; i < textLength; ++i)
        {
        const TChar ch(aText[i]);
        if (ch.IsSpace() &&
            !ZWSCharacter( ch ) )
            {
            if (result > 0)
                {
                // Not a leading space, set uncommitted space flag
                uncommittedSpace = ETrue;
                }
            }
        else
            {
            if (uncommittedSpace)
                {
                // Add previously encountered space(s)
                ++result;
                aDest.Append(' ');
                uncommittedSpace = EFalse;
                }
            // Add the character
            ++result;
            aDest.Append(ch);
            }
        }

    __ASSERT_DEBUG(aDest.Length()-oldDestLength == TrimAllLength(aText), 
        Panic(EPanicPostCond_TrimAllAppend));
    return result;
    }

EXPORT_C TInt PbkEngUtils::TrimRightLength(const TDesC& aText)
    {
    const TInt textLength=aText.Length();
    TInt result=0;
    TBool uncommittedSpace=EFalse;
    TBool leadingSpace=ETrue;
    for (TInt i=0; i < textLength; ++i)
        {
        if (TChar(aText[i]).IsSpace() &&
            !ZWSCharacter( aText[i] ) )
            {
            if (leadingSpace)
                {
                // Add all leading spaces
                ++result;
                }
            else
                {
                // Not a leading space, set uncommitted space flag
                uncommittedSpace = ETrue;
                }
            }
        else
            {
            // First non-space character found, adjust leading space flag
            leadingSpace = EFalse;
            if (uncommittedSpace)
                {
                // Add previously encountered space(s)
                ++result;
                uncommittedSpace = EFalse;
                }
            // Add the character
            ++result;
            }
        }

    __ASSERT_DEBUG(result <= aText.Length(), 
        Panic(EPanicPostCond_TrimRightLength));
    return result;
    }

EXPORT_C TInt PbkEngUtils::TrimRightAppend(const TDesC& aText, TDes& aDest)
    {
#ifdef _DEBUG
    const TInt oldDestLength = aDest.Length();
    __ASSERT_DEBUG(aDest.MaxLength()-oldDestLength >= TrimRightLength(aText), 
        Panic(EPanicPreCond_TrimRightAppend));
#endif

    const TInt textLength=aText.Length();
    TInt result=0;
    TBool uncommittedSpace=EFalse;
    TBool leadingSpace=ETrue;
    for (TInt i=0; i < textLength; ++i)
        {
        const TChar ch(aText[i]);
        if (ch.IsSpace() &&
            !ZWSCharacter( ch ) )
            {
            if (leadingSpace)
                {
                // Add all leading spaces
                ++result;
                aDest.Append(' ');
                }
            else
                {
                // Not a leading space, set uncommitted space flag
                uncommittedSpace = ETrue;
                }
            }
        else
            {
            // First non-space character found, adjust leading space flag
            leadingSpace = EFalse;
            if (uncommittedSpace)
                {
                // Add previously encountered space(s)
                ++result;
                aDest.Append(' ');
                uncommittedSpace = EFalse;
                }
            // Add the character
            ++result;
            aDest.Append(ch);
            }
        }

    __ASSERT_DEBUG(aDest.Length()-oldDestLength == TrimRightLength(aText), 
        Panic(EPanicPostCond_TrimRightAppend));
    return result;
    }



// CWords
inline CWords::CWords(PbkEngUtils::TIsWordSeparator aWordSeparatorFunc)
    : iWordSeparatorFunc
        (aWordSeparatorFunc ? 
        aWordSeparatorFunc : CWords::DefaultIsWordSeparator)
    {
    }

CWords* CWords::NewLC
        (const TDesC& aText, 
        PbkEngUtils::TIsWordSeparator aWordSeparatorFunc)
    {
    CWords* self = new(ELeave) CWords(aWordSeparatorFunc);
    CleanupStack::PushL(self);
    self->ConstructL(aText);
    return self;
    }

CWords::~CWords()
    {
    iWords.Close();
    }

TInt CWords::MdcaCount() const
    {
    return iWords.Count();
    }

TPtrC16 CWords::MdcaPoint(TInt aIndex) const
    {
    return iWords[aIndex];
    }

void CWords::ConstructL(const TDesC& aText)
    {
    const TInt textLength = aText.Length();
    for (TInt beg=0; beg < textLength; ++beg)
        {
        // Skip separators before next word
        if (!iWordSeparatorFunc(aText[beg]))
            {
            // Scan the end of the word
            TInt end = beg;
            for (; end < textLength && !iWordSeparatorFunc(aText[end]); ++end)
                {
                }
            const TInt len = end-beg;
            // Append found word to the array
            User::LeaveIfError(iWords.Append(aText.Mid(beg,len)));
            // Scan for next word
            beg = end;
            }
        }

    if (iWords.Count()==0 && textLength > 0)
        {
        // aText is all word separator characters -> make a "word" out of those
        User::LeaveIfError(iWords.Append(aText));
        }
    }

TBool CWords::DefaultIsWordSeparator(TChar aChar)
    {
    return (aChar.IsSpace());
    }


// End of File
