/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 presentation utilities.
*
*/


#ifndef PBK2PRESENTATIONUTILS_H
#define PBK2PRESENTATIONUTILS_H

#include <e32std.h>

class CFbsBitmap;

/**
 * Util class with static text formatting functions
 */
class Pbk2PresentationUtils
    {
    public: // Interface

        /**
         * Appends given text to given descriptor. Ignores all leading
         * and trailing spaces in the given text and converts consencutive
         * space characters to a single space character.
         *
         * @precond aDest.MaxLength()-aDest.Length() >= TrimAllLength(aText)
         * @postcond aDest.Length()-old(aDest.Length()) == TrimAllLength(aText)
         * @param aText     Text to append as a trimmed version.
         * @param aDest     Buffer where thed trimmed text is appended.
         *                  Must have enough room for the copied text.
         * @return  Number of characters appended.
         */
        static TInt TrimAllAppend(
                const TDesC& aText,
                TDes& aDest );

        /**
         * Appends given text to given descriptor. Ignores all trailing
         * spaces in the given text and converts consencutive space
         * characters to a single space character. Leading spaces remain
         * unaffected.
         *
         * @precond aDest.MaxLength()-aDest.Length() >= TrimRightLength(aText)
         * @postcond aDest.Length()-old(aDest.Length())==TrimRightLength(aText)
         * @param aText     Text to append as a trimmed version.
         * @param aDest     Buffer where the trimmed text is appended.
         *                  Must have enough room for the copied text.
         * @return  Number of characters appended.
         */
        IMPORT_C static TInt TrimRightAppend(
                const TDesC& aText,
                TDes& aDest );

        /**
         * Appends a descriptor to another and replaces characters while
         * appending.
         *
         * @precond (aDest.MaxLength()-aDest.Length()) >= aSrc.Length()
         * @precond aCharsToReplace.Length() == aReplaceChars.Length()
         * @param aDest             Destination descriptor.
         * @param aSrc              Source descriptor.
         * @param aCharsToReplace   Characters to replace.
         * @param aReplaceChars     Replace characters. The replace position
         *                          maps to the position in aCharsToReplace.
         * @return  Number of characters replaced.
         */
        IMPORT_C static TInt AppendAndReplaceChars(
                TDes& aDest,
                const TDesC& aSrc,
                const TDesC& aCharsToReplace,
                const TDesC& aReplaceChars );

        /**
         * Replaces all non-graphic characters (!TChar::IsGraph()) in
         * a string with the specified character.
         *
         * @param aText     The string where to replace
         *                  non-graphics characters.
         * @param aChar     The character used to replace non-graphic
         *                  characters.
         */
        IMPORT_C static void ReplaceNonGraphicCharacters(
                TDes& aText,
                TText aChar );
                
        /**
         * Replaces all CR, LF or CRLF characters with unicode Paragraph
         * Separator (0x2029) character that for example Avkon controls
         * understand. Copies all other text as-is. Copies only as many
         * characters from the beginning of aText as aBuffer has space for.
         *
         * @param aBuffer  Destination buffer where aText is appended with
         *                 newlines translated to Paragraph Separators.
         * @param aText    Source text to be translated.
         */
        IMPORT_C static void AppendWithNewlineTranslationL(
                TDes& aBuffer,
                const TDesC& aText );
                
        /**
        * Solves the correct language specific resource filename.
        * With all resource files the same pbk2presentation.rsg can be used.
        *
        * @return A reference to resource file
        */    
        IMPORT_C static const TDesC& PresentationResourceFile();
    
    private: // Disabled functions
        Pbk2PresentationUtils();
    };

class Pbk2PresentationImageUtils
    {
public:
    enum TCroppingMode
        {
        /// Image is cropped to square, target size is not used
        ECropping                  = 0x0,
        /**
         * Image is cropped to square and optimized to target size.
         * Cropped bitmap minimum width/height is aTargetSize width/height. 
         */
        EOptimizedCropping
        };
    
    IMPORT_C static void CropImageL( 
            CFbsBitmap& aBitmap, 
            TCroppingMode aCroppingMode, 
            const TSize& aTargetSize );
    
private: // Disabled functions
    Pbk2PresentationImageUtils();
    };


#endif // PBK2PRESENTATIONUTILS_H

// End of File
