/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 document base.
*
*/


#ifndef CPBK2DOCUMENTBASE_H
#define CPBK2DOCUMENTBASE_H

//  INCLUDES
#include <AknDoc.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionManager;

// CLASS DECLARATION

/**
 * Phonebook 2 document base.
 */
class CPbk2DocumentBase : public CAknDocument
    {
    public: // Interface

        /**
         * Returns the Phonebook 2 UI extension manager.
         *
         * @return  Phonebook 2 UI extension manager.
         */
        virtual CPbk2UIExtensionManager& ExtensionManager() const = 0;

    protected: // Implementation
        IMPORT_C CPbk2DocumentBase(
                CEikApplication& aApp );
        IMPORT_C ~CPbk2DocumentBase();
    };

#endif // CPBK2DOCUMENTBASE_H

// End of File
