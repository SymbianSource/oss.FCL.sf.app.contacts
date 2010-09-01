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
*       Class deriving from CVersitParser to access its 
*       protected methods.
*
*/


#ifndef __CBCARDPARSER_H__
#define __CBCARDPARSER_H__

//  INCLUDES
#include <versit.h>

// CLASS DECLARATION

/**
 * Class deriving from CVersitParser to access its protected methods.
 */
NONSHARABLE_CLASS(CBCardParser) : 
        public CVersitParser
	{
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
         */
		static CBCardParser* NewL();

    public: // Interface
        CDesCArray* MakePropertyValueCDesCArrayL(TPtr16 aStringValue);

	private: // Implementation
        CBCardParser();
    };

#endif // __CBCARDPARSER_H__
            
// End of File
