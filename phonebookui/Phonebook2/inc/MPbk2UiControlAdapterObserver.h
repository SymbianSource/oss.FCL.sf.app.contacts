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
* Description:  Phonebook 2 interface for observing commands in a simple way.
*
*/


#ifndef MPBK2UICONTROLADAPTEROBSERVER_H
#define MPBK2UICONTROLADAPTEROBSERVER_H

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class CVPbkContactLinkArray;

// CLASS DECLARATION

/**
 * Phonebook 2 interface for observing commands in a simple way.
 */
class MPbk2UiControlAdapterObserver
    {
    public: // Interface

        /**
         * The command has relocated the contact.
         * @param aRelocatedContact The new contact.
         */
        virtual void ContactContextChanged(
            const MVPbkBaseContact& aRelocatedContact ) = 0;

        /**
         * The command has finished executing.
         *
         * Any relocated contacts have been sent to the observer with
         * ContactRelocatedByPbk2Command() function.
         */
        virtual void Pbk2CommandFinished() = 0;

    protected: // Disabled functions
        ~MPbk2UiControlAdapterObserver()
            {}
    };

#endif // MPBK2UICONTROLADAPTEROBSERVER_H

// End of File
