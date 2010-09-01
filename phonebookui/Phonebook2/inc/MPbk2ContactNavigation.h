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
* Description:  Phonebook 2 contact navigation interface.
*
*/


#ifndef MPBK2CONTACTNAVIGATION_H
#define MPBK2CONTACTNAVIGATION_H

// INCLUDES
#include <w32std.h>
#include <MVPbkContactViewObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;
class MVPbkContactViewBase;

// CLASS DECLARATION

/**
 * Phonebook 2 contact navigation interface.
 */
class MPbk2ContactNavigation : public MVPbkContactViewObserver
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ContactNavigation()
            {}

       /**
         * Sets the contact view to navigate.
         *
         * @param aScrollView           The contact view to nagivate.
         * @param aOwnershipTransfered  ETrue if the ownership of scroll
         *                              view is transfered to this object.
         */
        virtual void SetScrollViewL(
                MVPbkContactViewBase* aScrollView,
                TBool aOwnershipTransfered ) = 0;

        /**
         * Updates navigation indicators.
         *
         * @param aContactLink      Contact whose position in
         *                          the view is to be indicated.
         */
        virtual void UpdateNaviIndicatorsL(
                const MVPbkContactLink& aContactLink ) const = 0;

        /**
         * Handles relevant command keys by navigating
         * contacts.
         *
         * @see CCoeControl::OfferKeyEventL
         */
        virtual TBool HandleCommandKeyL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType ) = 0;
        
        /**
         * Returns the current contact of the navigator. 
         * After MPbk2NavigationObserver::EContactDeleted the current contact
         * will be set to the next contact. If there are no more contact this
         * will return NULL in the CleanupStack.
         *
         * @return The current contact or NULL in the CleanupStack
         */
        virtual MVPbkContactLink* CurrentContactLC() const = 0;
    };

#endif // MPBK2CONTACTNAVIGATION_H

// End of File
