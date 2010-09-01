/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       Abstract Phonebook editor control interface.
*
*/


#ifndef __MPbkFieldEditorControl_H__
#define __MPbkFieldEditorControl_H__

//  INCLUDES
#include <PbkFields.hrh>

//  FORWARD DECLARATIONS
class CEikEdwin;
class MPbkContactEditorUiBuilder;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Abstract Phonebook editor control interface.
 */
class MPbkFieldEditorControl
    {
    public:  // Interface
        /**
         * Returns the editor control.
         */
        virtual CEikEdwin* Control() = 0;

        /**
         * Returns the field id.
         */
        virtual TPbkFieldId FieldId() = 0;
        
        /**
         * Uses aUiBuilder to load bitmap to the field.
         */
        virtual void LoadBitmapToFieldL
                (MPbkContactEditorUiBuilder& aUiBuilder) = 0;
    };


#endif // __MPbkFieldEditorControl_H__

// End of File
