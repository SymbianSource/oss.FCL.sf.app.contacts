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
*       Class for storing pointers to CParserParam objects. The reason for
*       this class is the usage of cleanupstack; destructor deletes the
*       instances of the pointers.     
*
*/


#ifndef __CBCARDPARSERPARAMARRAY_H__
#define __CBCARDPARSERPARAMARRAY_H__

//  INCLUDES
#include    <e32base.h>

// FORWARD DECLARATIONS
class CParserParam;

// CLASS DECLARATION

NONSHARABLE_CLASS(CBCardParserParamArray) : public CArrayPtrFlat<CParserParam> 
	{
    public: // constructor and destructor
        
        /**
         * Constructor.
		 * @param aGranurality array granularity
         */
        CBCardParserParamArray(TInt aGranurality);
        /**
         * Destructor.
         */
        ~CBCardParserParamArray();
    };

#endif // __CBCARDPARSERPARAMARRAY_H__
            
// End of File
