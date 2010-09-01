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
* Description:  Phonebook 2 contact view supplier interface.
*
*/


#ifndef MPBK2CONTACTVIEWSUPPLIER_H
#define MPBK2CONTACTVIEWSUPPLIER_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkContactViewBase;

//Use this UID to access view supplier extension 2. Used as a parameter
//to MPbk2ContactViewSupplierExtension() method.
const TUid KMPbk2ContactViewSupplierExtension2Uid = { 2 };

// CLASS DECLARATION
/**
 * Phonebook 2 contact view supplier interface.
 */
class MPbk2ContactViewSupplier
    {
    public:  // Interface

        /**
         * Returns the shared all contacts view.
         *
         * @return  Shared all contacts view.
         */
        virtual MVPbkContactViewBase* AllContactsViewL() = 0;

        /**
         * Returns the shared all groups view, if any.
         *
         * @return  Shared all groups view.
         */
        virtual MVPbkContactViewBase* AllGroupsViewL() = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* MPbk2ContactViewSupplierExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Protected destructor
        ~MPbk2ContactViewSupplier()
            {}
    };

#endif // MPBK2CONTACTVIEWSUPPLIER_H

// End of File
