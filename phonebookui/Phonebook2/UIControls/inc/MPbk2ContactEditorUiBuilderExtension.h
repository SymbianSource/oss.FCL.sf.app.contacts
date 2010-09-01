/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor UI builder interface extension.
*
*/


#ifndef MPBK2CONTACTEDITORUIBUILDEREXTENSION_H
#define MPBK2CONTACTEDITORUIBUILDEREXTENSION_H

// INCLUDES
//#include <e32def.h>

// FORWARD DECLARATIONS
class CCoeControl;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor UI builder interface extension.
 */
class MPbk2ContactEditorUiBuilderExtension
    {
    public:  // Interface

        /**
         * Adds custom field to editor form.
         *
         * @param aIndex                Index.
         * @param aResourceId           Id of the corresponding resource.
         * @return                      Custom control.
         */
        virtual CCoeControl* AddCustomFieldToFormL(TInt aIndex, TInt aResourceId) = 0;

        /**
         * Handles custom field command.
         *
         * @param aCommand Custom command id.
         * @return         ETrue if command was executed.
         */
        virtual TBool HandleCustomFieldCommandL(TInt aCommand ) = 0;
        
        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactEditorUiBuilderExtensionExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2CONTACTEDITORUIBUILDEREXTENSION_H

// End of File
