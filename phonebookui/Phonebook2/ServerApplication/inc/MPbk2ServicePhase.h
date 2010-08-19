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
* Description:  Phonebook 2 phased service interface.
*
*/


#ifndef MPBK2SERVICEPHASE_H
#define MPBK2SERVICEPHASE_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkContactLinkArray;
class MVPbkStoreContact;

// CLASS DECLARATION

/**
 * Phonebook 2 phased service interface.
 */
class MPbk2ServicePhase
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ServicePhase() {};

        /**
         * Launches the phased service.
         */
        virtual void LaunchServicePhaseL() = 0;

        /**
         * Cancels the phased service by force.
         */
        virtual void CancelServicePhase() = 0;

        /**
         * Request the phased service to cancel itself.
         *
         * @param aExitCommandId    Cancel command id.
         */
        virtual void RequestCancelL(
                TInt aExitCommandId ) = 0;

        /**
         * Notifies of delayed acception.
         *
         * @param aContactLinkBuffer    Buffer containing accepted
         *                              contact links.
         */
        virtual void AcceptDelayedL(
                const TDesC8& aContactLinkBuffer ) = 0;

        /**
         * Notifies of delayed acception.
         *
         * Purpose: Sometimes client maybe deny the selected contact, 
         * the function is used to deliver the client's feedback to phonebook.
         * 
         * @param aContactLinkBuffer    Buffer containing denied
         *                              contact links.
         */
        virtual void DenyDelayedL(
                const TDesC8& aContactLinkBuffer ) = 0;
        
        /**
         * Returns the results.
         *
         * @return  Results.
         */
        virtual MVPbkContactLinkArray* Results() const = 0;

        /**
         * Returns additional result data if supported.
         *
         * @return TInt result data or KErrNotSupported.
         */
        virtual TInt ExtraResultData() const = 0;

        /**
         * Takes store contact from the phase.
         *
         * @return  Results.
         */
        virtual MVPbkStoreContact* TakeStoreContact() = 0;
        
        /**
         * Returns content of the selected field if supported.
         *
         * @return field content or NULL.
         */
        virtual HBufC* FieldContent() const = 0;
    };

#endif // MPBK2SERVICEPHASE_H

// End of File
