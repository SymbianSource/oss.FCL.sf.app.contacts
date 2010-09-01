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


#ifndef MPBK2CONTACTVIEWSUPPLIER2_H
#define MPBK2CONTACTVIEWSUPPLIER2_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkContactViewBase;

// CLASS DECLARATION
/**
 * This class is an extension to MPbk2ContactViewSupplier.
 * See documentation of MPbk2ContactViewSupplier from header
 * MPbk2ContactViewSupplier.h 
 * 
 * You can access this extension by calling
 * MPbk2ContactViewSupplier->MPbk2ContactViewSupplierExtension()
 */
class MPbk2ContactViewSupplier2
    {
    public:  // Interface

        /**
         * Returns the shared top contacts view.
         * Returns NULL if AllContactsView doesn't include
         * top contacts subview (e.g. when phone memory is not selected).
         *
         * @return  shared top contacts view or NULL.
         */
        virtual MVPbkContactViewBase* TopContactsViewL() = 0;

    protected: // Protected destructor
        ~MPbk2ContactViewSupplier2()
            {}
    };

#endif // MPBK2CONTACTVIEWSUPPLIER2_H

// End of File
