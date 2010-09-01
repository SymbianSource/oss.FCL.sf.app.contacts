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
* Description:  Phonebook 2 contact UI control update interface.
*
*/


#ifndef MPBK2CONTACTUICONTROLUPDATE_H
#define MPBK2CONTACTUICONTROLUPDATE_H

// FORWARD DECLARATIONS
class MVPbkContactLink;

// CLASS DECLARATION

/**
 * Phonebook 2 contact UI control update interface.
 */
class MPbk2ContactUiControlUpdate
    {
    public: // Interface

        /**
         * Updates contact information presented in the UI.
         * In other words redraws the corresponding list box line.
         *
         * @param aContactLink      Link to the contact to be updated.
         */
        virtual void UpdateContact(
                const MVPbkContactLink& aContactLink ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactUiControlUpdateExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }

    protected: // Protected functions
        virtual ~MPbk2ContactUiControlUpdate()
            {}
    };

#endif // MPBK2CONTACTUICONTROLUPDATE_H

// End of File
