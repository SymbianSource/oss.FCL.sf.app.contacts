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
*     PbkNameFormatterFactory implementation factory for PbkEng.dll
*
*/


#ifndef __PbkNameFormatterFactory_H__
#define __PbkNameFormatterFactory_H__

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class MPbkContactNameFormat;
class CPbkSortOrderManager;

/**
 * PbkNameFormatterFactory implementation factory.
 */
NONSHARABLE_CLASS(PbkNameFormatterFactory)
    {
    public:  // Interface
        /**
         * Creates and returns a PbkNameFormatterFactory implementation.
		 * @param aUnnamedText text to use for unnamed contacts
		 * @param aEngine Engine to be used for formatting
		 * @param aSettingsVisible Can user change the name formatting method
         */
        static MPbkContactNameFormat* CreateL
            (const TDesC& aUnnamedText, 
             const CPbkSortOrderManager& aEngine,
             TBool aSettingsVisible);

    private:  // Disabled functions
        PbkNameFormatterFactory();
        PbkNameFormatterFactory(const PbkNameFormatterFactory&);
    };


#endif // __PbkNameFormatterFactory_H__
            
// End of File
