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
* Description:  Phonebook 2 contact assigner observer interface.
*
*/


#ifndef MPBK2CONTACTASSIGNEROBSERVER_H
#define MPBK2CONTACTASSIGNEROBSERVER_H

// FORWARD DECLARATIONS
class MPbk2ContactAssigner;

// CLASS DECLARATION

/**
 * Phonebook 2 contact assigner observer interface.
 */
class MPbk2ContactAssignerObserver
    {
    public: // Interface

        /**
         * Notifies assign operation completed succesfully.
         *
         * @param aAssigner     Assigner that sends this notification.
         * @param aIndex        Index of the field the data was assigned.
         */
        virtual void AssignComplete(
                MPbk2ContactAssigner& aAssigner,
                TInt aIndex ) = 0;

        /**
         * Notifies was assign operation completed unsuccesfully.
         *
         * @param aAssigner         Assigner that sends this notification.
         * @param aErrorCode        Error code.
         */
        virtual void AssignFailed(
                MPbk2ContactAssigner& aAssigner,
                TInt aErrorCode ) = 0;

    protected: // Disabled functions
        virtual ~MPbk2ContactAssignerObserver()
            {};
    };

#endif // MPBK2CONTACTASSIGNEROBSERVER_H

// End of File
