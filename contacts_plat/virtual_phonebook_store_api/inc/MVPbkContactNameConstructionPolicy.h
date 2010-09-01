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
* Description:  Interface for contact name construction 
*                used by the virtual phonebook
*
*/


#ifndef MVPBKCONTACTNAMECONSTRUCTIONPOLICY_H
#define MVPBKCONTACTNAMECONSTRUCTIONPOLICY_H

// INCLUDES
#include <e32cmn.h>
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkBaseContactFieldIterator;
class MVPbkBaseContactFieldCollection;
class CVPbkFieldTypeRefsList;

/**
 * Virtual Phonebook contact name construction policy interface.
 */
class MVPbkContactNameConstructionPolicy
    {
    public:
        /**
         * Virtual destructor.
         */
        virtual ~MVPbkContactNameConstructionPolicy() {}
        
        /**
         * Create contact field iterator 
         *
         * @param aFieldCollection Contact's field collection
         * @param aFieldTypeRefsList
         * @return Contact field iterator
         */
        virtual MVPbkBaseContactFieldIterator* NameConstructionFieldsLC( 
                const MVPbkBaseContactFieldCollection& aFieldCollection,
                CVPbkFieldTypeRefsList& aFieldTypeRefsList ) = 0;        
        
        /**
         * Returns an extension point for this interface or NULL.
         */
        virtual TAny* ContactNameConstructionPolicyExtension(
                TUid /*aExtensionUid*/ ){ return NULL; }
        
    };

#endif // MVPBKCONTACTNAMECONSTRUCTIONPOLICY_H

// End of File
