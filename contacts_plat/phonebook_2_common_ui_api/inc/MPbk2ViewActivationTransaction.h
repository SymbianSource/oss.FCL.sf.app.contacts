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
* Description:  Phonebook 2 view activation transaction interface.
*
*/


#ifndef MPBK2VIEWACTIVATIONTRANSACTION_H
#define MPBK2VIEWACTIVATIONTRANSACTION_H

// CLASS DECLARATION

/**
 * Phonebook 2 view activation transaction interface.
 */
class MPbk2ViewActivationTransaction
    {
    public: // Interface

        /**
         * Virtual destructor. Override to roll back any changes made
         * at construction if Commit() is not called.
         */
        virtual ~MPbk2ViewActivationTransaction()
                {}

        /**
         * Commits this transaction, tells the destructor
         * to keep the changes.
         */
        virtual void Commit() = 0;

        /**
         * Rolls back changes but cannot do it leave-safely.
         * This function should not be utilized when in DoActivateL,
         * in there just deleting this object is the preferred way to go.
         * But if the view has already been switched on (DoActivateL
         * has been executed) calling this function rolls back by force
         * to the previous view.
         */
        virtual void RollbackL() = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ViewActivationTransactionExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2VIEWACTIVATIONTRANSACTION_H

// End of File
