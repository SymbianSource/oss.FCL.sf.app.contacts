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
* Description:  Phonebook 2 Group Feature ECom plug-in interface.
*
*/


#ifndef CPBK2GROUPFEATUREPLUGIN_H
#define CPBK2GROUPFEATUREPLUGIN_H

// INCLUDE FILES
#include <e32cmn.h>
#include <e32base.h>
#include <ecom/ecom.h>

// FORWARD DECLARATION
class MPbk2GroupFeatureObserver;
class MVPbkStoreContact;

// CLASS DECLARATION

/**
 * Phonebook 2 Group Feature ECom plug-in interface.
 * Responsible for providing interface for group functionality
 * needed from other Phonebook 2 UI extensions.
 */
class CPbk2GroupFeaturePlugin : public CBase

    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2GroupFeaturePlugin* NewL();

        /**
         * Destructor.
         */
        ~CPbk2GroupFeaturePlugin();

    public: // Interface

        /**
         * Get a list of groups in which contact belongs to. Asynchronous
         * operation. Results are given to observer, when ready.
         *
         * @param aContact      Contact whose groups to search for.
         * @param aObserver     Callback observer where list is returned.
         *
         */
        virtual void JoinedGroupsL(
                const MVPbkStoreContact* aContact,
                MPbk2GroupFeatureObserver* aObserver ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* CPbk2GroupFeaturePluginExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    private: // Data
        /// Own: Destructor key
        TUid iDtorIDKey;
    };

// INLINE IMPLEMENTATION
#include "cpbk2groupfeatureplugin.inl"

#endif // CPBK2GROUPFEATUREPLUGIN_H

// End of File
