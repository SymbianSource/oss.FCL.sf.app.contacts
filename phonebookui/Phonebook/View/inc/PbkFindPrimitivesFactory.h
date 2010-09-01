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
*      MPbkFindPrimitives implementation factory for PbkView.dll
*
*/


#ifndef __PbkFindPrimitivesFactory_H__
#define __PbkFindPrimitivesFactory_H__

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MPbkFindPrimitives;
class MPbkContactNameFormat;

// CLASS DECLARATION

/**
 * MPbkFindPrimitives implementation factory.
 */
NONSHARABLE_CLASS(PbkFindPrimitivesFactory)
    {
    public:  // Interface
        /**
         * Creates and returns a MPbkFindPrimitives implementation.
		 * @param aContactNameFormatter contact name formatter
         */
        static MPbkFindPrimitives* CreateL
            (MPbkContactNameFormat& aContactNameFormatter);

    private:  // Disabled functions
        PbkFindPrimitivesFactory();
        PbkFindPrimitivesFactory(const PbkFindPrimitivesFactory&);
    };

#endif // __PbkFindPrimitivesFactory_H__
            
// End of File
