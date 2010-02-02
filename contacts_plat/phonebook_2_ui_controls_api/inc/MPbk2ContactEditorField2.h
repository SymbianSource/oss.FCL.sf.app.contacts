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
* Description:  Phonebook 2 contact editor field interface.
*
*/


#ifndef MPBK2CONTACTEDITORFIELD2_H
#define MPBK2CONTACTEDITORFIELD2_H



// CLASS DECLARATION

/**
 * This class is an extension to MPbk2ContactEditorField.
 * See documentation of MVPbkStoreContact from header MPbk2ContactEditorField.h 
 *
 * 
 */
class MPbk2ContactEditorField2
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ContactEditorField2()
                {}

        /**
         * Handles custom field command.
         *
         * @param aCommand Custom command id.
         * @return  ETrue if command was executed.
         */
        virtual TBool HandleCustomFieldCommandL(TInt aCommand) = 0;

    };

#endif // MPBK2CONTACTEDITORFIELD2_H

// End of File
