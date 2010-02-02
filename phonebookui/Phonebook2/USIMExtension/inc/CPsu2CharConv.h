/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class for converting characters and checking if the data is
*                too long.
*
*/


#ifndef CPSU2CHARCONV_H
#define CPSU2CHARCONV_H

// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <charconv.h>

// FORWARD DECLARATIONS
class RFs;
class CCnvCharacterSetConverter;

// CLASS DECLARATION
class CPsu2CharConv : public CBase
    {
    public:

        /**
        * Two-phased constructor.
        *
        * @param aFs an open file system session
        * @param aCharacterSet The character set to use. See charconv.h.
        * @return a new instance of this class
        */
        static CPsu2CharConv* NewL( RFs& aFs, const TUint aChacterSet );
        
        /**
        * Two-phased constructor.
        *
        * @param aFs an open file system session
        * @param aCharacterSet The character set to use. See charconv.h.
        * @return a new instance of this class
        */
        static CPsu2CharConv* NewLC( RFs& aFs, const TUint aChacterSet );
        
        /**
         * Destructor.
         */
        ~CPsu2CharConv();

    public: // Data structures

        /// Used for indicating contact field validity
        /// from (U)SIM store point of view
        enum TPsu2FieldValidity
            {
            /// It's OK to add the field to the entry
            EOk,
            /// The data is too long for this field type
            ETooLong
            };

    public: // Interface

        void SetDownGradeLf( CCnvCharacterSetConverter::
                TDowngradeForExoticLineTerminatingCharacters aDowngrade );

        /**
         * Checks contact field validity.
         *
         * @param aData         The data to be saved to the field.
         * @param aMaxLength    Maximum length of the field.
         * @param aValidity     Contains the validity value afterwards.
         * @return  A pointer to the given data. The length depends
         *          on aValidity. If ETooLong, the pointer points to
         *          the aData but the length is cut. Can be used to add
         *          the data to the entry without problems. In other cases
         *          the pointer points to the given aData and
         *          the length is same.
         */
        TPtrC CheckFieldValidityL(
                const TDesC& aData, 
                TInt aMaxLength,
                TPsu2FieldValidity& aValidity );

        /**
         * Checks unicode descriptor data length with the current character set
         * so that conversion from unicode to 8-bit descriptor will not exceed
         * the given maximum length. Returns either the given descriptor or a
         * truncated descriptor that can be converted to an 8-bit descriptor
         * with the given maximum length.
         *
         * @param aData         The data to be saved to the field.
         * @param aMaxLength    Maximum allowed length of the field for the
         *                      converted data.
         * @param aUnconvertedCount On return, contains the count of
         *          characters that could not be converted.
         * @return  A pointer to the given data. The pointer points to the
         *          aData but the length is truncated to the length that allows
         *          making the conversion so that the result length does not
         *          exceed aMaxLength that was given as a parameter.
         */
        TPtrC CheckFieldLengthL(
                const TDesC& aData, 
                TInt aMaxLength,
                TInt& aUnconvertedCount );

        /**
         * To check if the text includes unicode characters.
         * @param aText The text to be checked.
         * @return ETrue if the text included unicode characters.
         *          EFalse otherwise.
         */
        TBool IsUnicodeL( const TDesC& aText );

    private:    // Construction
        
        /**
        * Constructor.
        */
        CPsu2CharConv( RFs& aFs, const TUint aChacterSet );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
         * Checks some special cases to need if extra space needs to be
         * reserved for the data when saving to (U)SIM.
         * @param aData The data to be saved to (U)SIM.
         * @param aConvertedLength The length of the conversion. The function
         *          increases this if any extra space needs to be reserved.
         */
        void ReviseLength(
                const TDesC& aData, TInt& aConvertedLength );

    private:    // Data
        /// A file system session for characted converter
        RFs iFs;
        /// Own: Symbian's character converter
        CCnvCharacterSetConverter* iConverter;
        /// Character set identier as given to the constructors
        TUint iCharacterSetIdentifier;
	};

#endif // CPSU2CHARCONV_H

// End of File
