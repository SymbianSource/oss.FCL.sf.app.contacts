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
* Description:  Phonebook 2 contact editor dialog extension to extension.
*
*/

#ifndef MPBK2CONTACTEDITOREXTENSIONEXTENSION_H_
#define MPBK2CONTACTEDITOREXTENSIONEXTENSION_H_

// INCLUDES
#include "MPbk2UIFieldFactory.h"
//  FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor UI extension interface.
 */
class MPbk2ContactEditorExtensionExtension : public MPbk2UIFieldFactory
    {
    public: // Interface

    	/**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactEditorExtensionExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2CONTACTEDITOREXTENSIONEXTENSION_H_

// End of File
