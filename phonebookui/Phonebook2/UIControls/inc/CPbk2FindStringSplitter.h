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
* Description:   Utility class for splitting find string into separate words
*
*/


#ifndef CPBK2FINDSTRINGSPLITTER_H
#define CPBK2FINDSTRINGSPLITTER_H

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class MPbk2ContactNameFormatter;


NONSHARABLE_CLASS ( CPbk2FindStringSplitter ) : public CBase
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aText             The text to separate into words.
         * @param aNameFormatter    Name formatter.
         * @return A new instance of this class.
         */
        static CPbk2FindStringSplitter* NewL(
            MPbk2ContactNameFormatter& aNameFormatter );
        /**
         * Destructor.
         */
        ~CPbk2FindStringSplitter();
        
        MDesCArray* SplitTextIntoArrayL( const TDesC& aText );

    private: // Implementation
        CPbk2FindStringSplitter(
            MPbk2ContactNameFormatter& aNameFormatter );
        void ConstructL();

    private: // Data
        /// Ref: Name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
    };

#endif // CPBK2FINDSTRINGSPLITTER_H
