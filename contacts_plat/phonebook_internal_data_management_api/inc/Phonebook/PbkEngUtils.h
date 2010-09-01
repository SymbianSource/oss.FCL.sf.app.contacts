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
*    Namespace structure for Phonebook engine helper functions
*
*/


#ifndef __PbkEngUtils_H__
#define __PbkEngUtils_H__

//  INCLUDES
#include <e32std.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class RResourceFile;
class RFs;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Namespace structure for Phonebook engine helper functions.
 */
struct PbkEngUtils 
    {
    /**
     * Searches and opens a resource file.
     *
     * @param   aFs             open file server handle.
     * @param   aResFileName    resource file name.
     * @param   aResFilePath    path to search the resource file from. 
     * @param   aResFileDrive   Drive the resource file is on.
     * @param   aResFile        resource file to open. If open is succesful, resource file
     *                          close operation is pushed on to the cleanup stack.
     *
     * @exception KErrNoMemory  if out of memory.
     * @exception KErrNotFound  if the resource file is not found.
	 * @exception KErrBadName	if an empty file name (with path) was supplied
     * @exception other         error searching or opening the resource file.
     */
    IMPORT_C static void FindAndOpenResourceFileLC
                    (RFs& aFs, 
                    const TDesC& aResFileName, 
                    const TDesC& aResFilePath, 
                    const TDesC& aResFileDrive,
                    RResourceFile& aResFile);

    /**
     * Searches and opens the DLL default resource file. 
     * Calls FindAndOpenResourceFileLC.
     *
     * @param   aFs         open file server handle.
     * @param   aResFile    resource file to open. If open is succesful, resource file
     *                      close operation is pushed on to the cleanup stack.
     *
     * @see PbkEngUtils::FindAndOpenResourceFileLC
     */
    IMPORT_C static void FindAndOpenDefaultResourceFileLC
                    (RFs& aFs, 
                    RResourceFile& aResFile);

    /**
     * Copies specified number of digit characters (c is a digit if 
     * TChar(c).IsDigit() is true) from the end of a descriptor to another
     * descriptor. Used in phone number matching.
     *
     * @param aNumText  the text containing digits to copy, (i.e. a phone 
     *                  number).
     * @param aNumberOfDigits   maximum number of digits to copy.
     * @param aDigitsText   descriptor where digits are copied.
     * @return number of digits actually copied.
     */
    IMPORT_C static TInt CopyDigitsFromEnd
        (const TDesC& aNumText, 
        TInt aNumberOfDigits,
        TDes& aDigitsText);

    /**
     * Returns true if two descriptors contain the same digits in the same 
     * order. Skips all non-digit characters when doing the comparison.
     *
     * @param aNumText1 first descriptor to compare.
     * @param aNumText2 second descriptor to compare.
     * @param aNumberOfDigits   number of digits to compare from the end of
     *                          the descriptors, if 0 all digits in both 
     *                          descriptors are compared.
     * @return  true if both descriptors contain the same numbers
	 *
     *          Boundary conditions:
     *          <ul>
     *          <li>if both descriptors are empty the result is true</li>
     *          <li>if both descriptors contain no digits the result is true</li>
     *          <li>if both descriptors contain digits, but the other less than
     *              aNumberOfDigits the result is false</li>
     *          <li>if both descriptors contain digits, but the other less than
     *              aNumberOfDigits the result is false</li>
     *          <li>if both descriptors contain the same digits in the same order 
     *              the result is always true regardless of the value of 
     *              aNumberOfDigits</li>
     *          </ul>
     */
    IMPORT_C static TBool ContainSameDigits
        (const TDesC& aNumText1, 
        const TDesC& aNumText2, 
        TInt aNumberOfDigits=0);

    /**
     * Appends a descriptor to another and replaces characters while appending.
     *
     * @param aDest destination descriptor.
     * @param aSrc  source descriptor.
     * @param aCharsToReplace   characters to replace from aSrc.
     * @param aReplaceChars     characters to replace to from the same 
     *                          position in aCharsToReplace.
     * @return number of characters replaced.
     * @precond (aDest.MaxLength()-aDest.Length()) >= aSrc.Length()
     * @precond aCharsToReplace.Length() == aReplaceChars.Length()
     */
    IMPORT_C static TInt AppendAndReplaceChars
        (TDes& aDest, const TDesC& aSrc, 
        const TDesC& aCharsToReplace, const TDesC& aReplaceChars);

    /**
     * Returns the number of white space characters in aText.
     */
    IMPORT_C static TInt CountSpaces(const TDesC& aText);

    /**
     * Returns ETrue if aText is either empty or consists of only white space
     * characters.
     */
    static TBool IsEmptyOrAllSpaces(const TDesC& aText);

    /**
     * Replaces all non-graphic characters (!TChar::IsGraph()) in a string 
     * with the specified character.
     *
     * @param aText the string where to replace non-graphics characters.
     * @param aChar the character used to replace non-graphic characters in aText.
     */
    IMPORT_C static void ReplaceNonGraphicCharacters(TDes& aText, TText aChar);

    /**
     * Appends all graphic characters (TChar::IsGraph()) in a string to 
     * another string. Non-graphic characters are replaced with a specified
     * character.
     *
     * @param aDest the string where to append graphic characters.
     * @param aSrc  the string where from chracters are appended to aDest.
     * @param aChar the character used to replace non-graphic characters in aSrc.
     */
    IMPORT_C static void AppendGraphicCharacters
        (TDes& aDest, const TDesC& aSrc, TText aChar);

    /**
     * Definition for a function that returns true if aChar is a word separator
     * character.
     * @see BreakInWordsL
     */
    typedef TBool (*TIsWordSeparator)(TChar aChar);

    /**
     * Breaks text into words array.
     *
     * @param aText text to break into words.
     * @param aWordSeparatorFunc    function for detecting word separator
     *                              characters. Default implementation uses
     *                              TChar::IsSpace().
     * @return  the words found in text in an array. The ownership of the array
     *          is tranferred to caller.
     */
    IMPORT_C static MDesCArray* BreakInWordsLC
        (const TDesC& aText, TIsWordSeparator aWordSeparatorFunc=NULL);

    /**
     * Allocates or reallocates aBuffer to be able to hold at least aMaxLength 
     * characters.
     *
     * @param aBuffer   buffer to (re)allocate. Can be NULL.
     * @param aMaxLength    size for the buffer in characters
     * @return the (re)allocated buffer. Never NULL.
     */
    IMPORT_C static HBufC* AllocL(HBufC* aBuffer, TInt aMaxLength);

    /**
     * Copies aText into aBuffer. The buffer is reallocated if necessary.
     * 
     * @param aBuffer   the buffer where to copy aText. Can be NULL.
     * @param aText     text to be copied into aBuffer.
     * @param aMinBufLength minimum length to allocate for aBuffer.
     * @return the (re)allocated buffer. Never NULL.
     */
    IMPORT_C static HBufC* CopyL(HBufC* aBuffer, const TDesC& aText,
		TInt aMinBufLength=0);

    /**
     * Calculates and returns the trimmed length of aText. Ignores all leading 
     * and trailing spaces in aText and calculates consencutive space 
     * characters as a single character.
     *
     * @param aText the text whose trimmed length is to be calculated.
	 * @return trimmed length of aText
     * @postcond TrimAllLength(aText) <= aText.Length()
     */
    IMPORT_C static TInt TrimAllLength(const TDesC& aText);

    /**
     * Appends aText to aDest. Ignores all leading and trailing spaces in aText
     * and converts consencutive space characters to a single space character.
     *
     * @param aText text to append as a trimmed version to aDest.
     * @param aDest buffer where trimmed aText is appended. aDest must have 
     *              enough room for the copied text.
     * @return number of characters appended from aText to aDest.
     * @precond aDest.MaxLength()-aDest.Length() >= TrimAllLength(aText)
     * @postcond aDest.Length()-old(aDest.Length()) == TrimAllLength(aText)
     */
    IMPORT_C static TInt TrimAllAppend(const TDesC& aText, TDes& aDest);

    /**
     * Calculates and returns the trimmed length of aText. Ignores all 
     * trailing spaces in aText and calculates consencutive space 
     * characters as a single character.
     * Leading spaces remain unaffected.
     *
     * @param aText the text whose trimmed length is to be calculated.
	 * @return trimmed length of aText
     * @postcond TrimRightLength(aText) <= aText.Length()
     */
    IMPORT_C static TInt TrimRightLength(const TDesC& aText);

    /**
     * Appends aText to aDest. Ignores all trailing spaces in aText
     * and converts consencutive space characters to a single space character.
     * Leading spaces remain unaffected.
     *
     * @param aText text to append as a trimmed version to aDest.
     * @param aDest buffer where trimmed aText is appended. aDest must have 
     *              enough room for the copied text.
     * @return number of characters appended from aText to aDest.
     * @precond aDest.MaxLength()-aDest.Length() >= TrimRightLength(aText)
     * @postcond aDest.Length()-old(aDest.Length()) == TrimRightLength(aText)
     */
    IMPORT_C static TInt TrimRightAppend(const TDesC& aText, TDes& aDest);

    private:  // Implementation
        /**
         * Hidden, unimplemented constructor.
         */
        PbkEngUtils();
    };


// INLINE FUNCTIONS
inline TBool PbkEngUtils::IsEmptyOrAllSpaces(const TDesC& aText)
    {
    return (CountSpaces(aText) == aText.Length());
    }


#endif // __PbkEngUtils_H__

// End of File
