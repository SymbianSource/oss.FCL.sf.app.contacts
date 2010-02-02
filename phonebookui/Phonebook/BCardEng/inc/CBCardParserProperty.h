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
*       Class deriving from CParserProperty to access its 
*       protected iArrayOfParams member.
*
*/


#ifndef __CBCARDPARSERPROPERTY_H__
#define __CBCARDPARSERPROPERTY_H__

//  INCLUDES
#include <vprop.h>

// CLASS DECLARATION

/**
 * Class deriving from CParserProperty to access its protected iArrayOfParams
 * member.
 */
NONSHARABLE_CLASS(CBCardParserProperty) : 
        public CParserProperty 
	{
    public: // Interface
        /**
         * Converts aProperty to an instance of this class.
         */
        static const CBCardParserProperty& Cast(const CParserProperty& aProperty);

        /**
         * Returns this property's array of parameters.
         */
        const CArrayPtr<CParserParam>* ArrayOfParams() const;

	private:
		CBCardParserProperty(); // Not implemented (just suppress warnings)

    };

#endif // __CBCARDPARSERPROPERTY_H__
            
// End of File
