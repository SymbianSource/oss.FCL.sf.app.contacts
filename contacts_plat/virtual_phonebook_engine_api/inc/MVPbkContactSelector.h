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
* Description:  Virtual Phonebook contact selector interface for a 
*                filtered view, CVPbkFilteredContactView .
*
*/

 
#ifndef MVPBKCONTACTSELECTOR_H
#define MVPBKCONTACTSELECTOR_H

// INCLUDES
#include <e32cmn.h>

// FORWARD DECLARATIONS
class MVPbkBaseContact;

// CLASS DECLARATIONS

/**
 * Virtual Phonebook contact selector interface.
 * This inteface is used to create a contact selector object,
 */
class MVPbkContactSelector
    {
    public: // Interface
        /**
         * Returns true if aContact belongs to the set defined
         * by this contact selector.
         *
         * @param aContact  The contact to check.
         * @return  ETrue if contact is included.
         */
        virtual TBool IsContactIncluded(
                const MVPbkBaseContact& aContact ) = 0;
    
        /**
         * Returns an extension point for this interface or NULL.
         * @param aExtensionUid Uid of extension.
         * @return Extension point or NULL.
         */
        virtual TAny* ContactSelectorExtension(
                TUid /*aExtensionUid*/) { return NULL; }

    protected:
        virtual ~MVPbkContactSelector() { }

    };

// Use this UID to access MVPbkOptimizedSelector extension of the MVPbkContactSelector.
// Used as a parameter to ContactSelectorExtension() method.
const TUid KVPbkOptimizedSelectorExtensionUid = { 2 };

/**
 * This class is an extension to MVPbkContactSelector.
 * See documentation of MVPbkContactSelector from header MVPbkContactSelector.h
 *
 * @see MVPbkContactSelector
 *
 */
class MVPbkOptimizedSelector
    {
    protected:  // Destructor
        virtual ~MVPbkOptimizedSelector() { }

    public:

        /**
         * Vpbk stop calling MVPbkContactSelector::IsContactIncluded if this function returns false.
         * With this function client can optimize selector use in VPbk.
         */
         virtual TBool Continue() const = 0;

    };

	
#endif  // MVPBKCONTACTSELECTOR_H

//End of file
