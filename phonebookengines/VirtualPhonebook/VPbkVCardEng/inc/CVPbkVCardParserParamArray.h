/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef CVPBKVCARDPARSERPARAMARRAY_H
#define CVPBKVCARDPARSERPARAMARRAY_H

//  INCLUDES
#include    <e32base.h>

// FORWARD DECLARATIONS
class CParserParam;

// CLASS DECLARATION

NONSHARABLE_CLASS(CVPbkVCardParserParamArray) 
: public CArrayPtrFlat<CParserParam> 
	{
    public: // constructor and destructor
        
        /**
         * Constructor.
		 * @param aGranurality array granularity
         */
        CVPbkVCardParserParamArray(TInt aGranularity);
        /**
         * Destructor.
         */
        ~CVPbkVCardParserParamArray();
    };

#endif // CVPBKVCARDPARSERPARAMARRAY_H
            
// End of File
