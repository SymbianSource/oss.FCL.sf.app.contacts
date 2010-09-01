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
* Description:  Utility class that splits a string into words
*
*/

#ifndef __C_WORDS_H__
#define __C_WORDS_H__

#include <e32base.h>
#include <bamdesca.h>

/**
 * Utility class that splits a string into words
 */
NONSHARABLE_CLASS(CWords) : 
        public CBase, 
        public MDesCArray
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aText the text to separate
         */
        IMPORT_C static CWords* NewLC(const TDesC& aText);
        /**
         * Destructor.
         */
        IMPORT_C ~CWords();
        
    public:  // from MDesCArray
        IMPORT_C TInt MdcaCount() const;
        IMPORT_C TPtrC16 MdcaPoint(TInt aIndex) const;

    private:  // Implementation
        CWords();
        void ConstructL(const TDesC& aText);
        static TBool DefaultIsWordSeparator(TChar aChar);

    private:

        /// Own: array of words
        RArray<TPtrC> iWords;
    };
    
#endif // C_WORDS_H

    
