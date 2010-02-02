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
*     Entry loader for CPbkEntryCache
*
*/


#ifndef __MPbkContactEntryLoader_H__
#define __MPbkContactEntryLoader_H__

//  INCLUDES
#include <e32std.h>
 
// FORWARD DECLARATIONS
class CPbkContactEngine;
class TPbkContactEntry;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Entry loader for CPbkEntryCache.
 * @see CPbkEntryCache
 */
class MPbkContactEntryLoader
    {
    public: // interface
        /**
         * Loads aEntry with data from a contact.
         *
         * @param aEntry contact cache entry to fill
         * @param aEngine  contact database where to load aContactId
         * @param aContactId id of the contact to read
         */
        virtual void LoadEntryL
            (TPbkContactEntry& aEntry, 
            CPbkContactEngine& aEngine, 
            TContactItemId aContactId) = 0;

    protected:
        /**
         * Proteced destructor. Concrete instances are not destroyed through 
         * this interface.
         */
        virtual ~MPbkContactEntryLoader() { }
    };


#endif // __MPbkContactEntryLoader_H__
            
// End of File
