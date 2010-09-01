/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 command item updater
*
*/


#ifndef MPBK2COMMANDITEMUPDATER_H
#define MPBK2COMMANDITEMUPDATER_H

// CLASS DECLARATION

/**
 * Phonebook 2 contact UI control update 2 interface.
 */
class MPbk2CommandItemUpdater
    {
    public: // Interface

        /**
         * Updates command item presented in the UI.
         * In other words redraws the corresponding list box line.
         *
         * @param aCommandId      Command identifier
         */
        virtual void UpdateCommandItem( TInt aCommandId ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* MPbk2CommandItemUpdaterExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Protected functions
        virtual ~MPbk2CommandItemUpdater()
            {}
    };

#endif // MPBK2COMMANDITEMUPDATER_H

// End of File
