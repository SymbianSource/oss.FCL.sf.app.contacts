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
* Description:  Phonebook 2 UI control UI extension interface.
*
*/


#ifndef MPBK2CONTACTUICONTROLEXTENSION_H
#define MPBK2CONTACTUICONTROLEXTENSION_H

// INCLUDES
#include <tpbk2iconid.h>
#include <mpbk2uireleasable.h>

//  FORWARD DECLARATIONS
class MVPbkViewContact;
class MPbk2ContactUiControlUpdate;
//class CPbk2IconArray;

//Use this UID to access extension 2 for this interface. Used as a parameter
//to  ContactUiControlExtensionExtension() method.
const TUid KMPbk2ContactUiControlExtensionExtension2Uid = { 2 };

// CLASS DECLARATION
/**
 * Phonebook 2 UI control UI extension interface.
 */
class MPbk2ContactUiControlExtension : public MPbk2UiReleasable
    {
    public: // Interface

        /**
         * Gets dynamic icon ids for a contact.
         *
         * @param aContactHandle    Handle of the contact to get
         *                          dynamic icons
         * @return  An array of icon ids. Count of the array is zero if
         *          no icon is associated with the contact.
         */
        virtual const TArray<TPbk2IconId> GetDynamicIconsL(
                const MVPbkViewContact* aContactHandle ) = 0;

        /**
         * Sets contact UI control updater for this extension.
         *
         * @param aContactUpdator   UI control updater.
         *                          If NULL, association  is removed.
         */
        virtual void SetContactUiControlUpdate(
                MPbk2ContactUiControlUpdate* aContactUpdator ) = 0;
        
        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactUiControlExtensionExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2CONTACTUICONTROLEXTENSION_H

// End of File
