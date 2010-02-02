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
*    Abstract interface for making phone book UI control extensions.
*
*/


#ifndef __MPbkContactUiControlExtension_H__
#define __MPbkContactUiControlExtension_H__


//  INCLUDES
#include <cntdef.h>         // TContactItemId
#include <PbkIconId.hrh>    // TPbkIconId
#include <MPbkUiExtensionBase.h>


//  FORWARD DECLARATIONS
class CPbkContactItem;
class MPbkContactUiControlUpdate;


// CLASS DECLARATION

/**
 * Abstract interface for phone book UI control extensions.
 */
class MPbkContactUiControlExtension : public MPbkUiExtensionBase
    {
    public: // Interface
        /**
         * Returns icons info array and icons array used in extension. If 
         * return values are 0, resource id's are not defined.
         *
         * @param aArrayIconInfoId resource id for icon info array
         * @param aArrayIconId resource id for icon array used in extension
         */
        virtual void IconArrayResourceId
                (TInt& aArrayIconInfoId, TInt& aArrayIconId) =0;

        /**
         * Gets a dynamic icon id's for a contact. Icons are in right order
         * starting from 0 (notify icon/presence status, marked icon).
         *
         * @param aContactId    id of the contact to get dynamic icon for.
         * @return              array of icon id's. Count of the array is 0 if 
         *                      no icon is associated with the contact.
         */
        virtual const TArray<TPbkIconId> GetDynamicIconsL
                (TContactItemId aContactId) =0;

        /**
         * Gets a dynamic icon id's for a contact.
         *
         * @param aContact  contact to get dynamic icon for.
         * @return          array of icon id's. Count of the array is 0 if 
         *                  no icon is associated with the contact.
         */
        virtual const TArray<TPbkIconId> GetDynamicIconsL
                (const CPbkContactItem& aContact) =0;

        /**
         * Sets ui contact updator for this extension. 
         *
         * @param aContactUpdator for updating the 
         *                        contact control; if NULL, association 
         *                        is removed.
         * @see MPbkContactUiUpdate
         */
        virtual void SetContactUiControlUpdate
                (MPbkContactUiControlUpdate* aContactUpdator) =0;

    protected:
        /**
         * Destructor.
         */
        virtual ~MPbkContactUiControlExtension() { }
    };

#endif // __MPbkContactUiControlExtension_H__
            
// End of File
