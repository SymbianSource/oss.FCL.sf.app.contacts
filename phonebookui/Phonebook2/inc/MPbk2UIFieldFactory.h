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
* Description:  Phonebook 2 contact editor dialog field factory.
*
*/

#ifndef MPBK2UIFIELDFACTORY_H_
#define MPBK2UIFIELDFACTORY_H_

// INCLUDE FILES
#include <e32def.h>

// FORWARD DECLARATIONS
class MPbk2ContactEditorUIField;
class MPbk2UIField;
class MPbk2ContactEditorUiBuilder;
class CPbk2IconInfoContainer;

/**
 * Phonebook 2 UI field factory interface.
 *
 */
class MPbk2UIFieldFactory
    {
    public: // Interface

    	/**
         * Creates a new contact editor field.
         *
         * @param aField             Phonebook2 UI field.
         * @param aUiBuilder         Builder for adding the field into dialog.
         * @param aIconInfoContainer An icon container for setting field icon.
         * @param aCustomPosition    Needed only for custom fields to indicate desired
         *                           postion in form. For other fields can be NULL.
         * @param aCustomText	     If not empty, text will be added into field.                           
         * @return  A new contact editor UI field.
         */
        virtual MPbk2ContactEditorUIField* CreateFieldLC( 
        		MPbk2UIField& aField,
        		TInt aCustomPosition, 
                MPbk2ContactEditorUiBuilder& aUiBuilder,
                const TDesC& aCustomText, CPbk2IconInfoContainer& aIconInfoContainer ) = 0;

        
    };

#endif /*MPBK2UIFIELDFACTORY_H_*/

// End of File
