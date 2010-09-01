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
* Description:  CVersitParser wrapper class
*
*/


#ifndef __CVPBKVCARDPARSER_H__
#define __CVPBKVCARDPARSER_H__

//  INCLUDES
#include <versit.h>

// CLASS DECLARATION

/**
 * Class deriving from CVersitParser to access its protected methods.
 */
NONSHARABLE_CLASS( CVPbkVCardParser ) : 
        public CVersitParser
	{
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
         */
		static CVPbkVCardParser* NewL();

    public: // Interface
        CDesCArray* MakePropertyValueCDesCArrayL(TPtr16 aStringValue);

	private: // Implementation
        CVPbkVCardParser();
    };

#endif // __CVPBKVCARDPARSER_H__
            
// End of File
