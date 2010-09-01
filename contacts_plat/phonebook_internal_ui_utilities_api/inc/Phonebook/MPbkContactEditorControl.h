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
*        Abstract interface for contact editor.
*
*/


#ifndef __MPbkContactEditorControl_H__
#define __MPbkContactEditorControl_H__

//  INCLUDES
#include <cntdef.h>     // TContactItemId


// FORWARD DECLATIONS
class CContactIdArray;
class TPbkContactItemField;
class MPbkContactEditorField;


// CLASS DECLARATION

/**
 * Abstract interface for contact editor.
 */
class MPbkContactEditorControl
    {
    protected: // Destructor
        virtual ~MPbkContactEditorControl() {}

    public: // Interface
        /**
         * Returns currently focused editor field.
         */
        virtual MPbkContactEditorField* FocusedField() =0;
        /**
         * Returns the amount of editors.
         */
        virtual TInt EditorCount() =0;
        /**
         * Returns editor field according to aFieldIndex.
         * @param aFieldIndex field index
         */
        virtual const MPbkContactEditorField& FieldAt(TInt aFieldIndex) =0;
        /**
         * Sets editor state been modified.
         */
        virtual void SetStateModified() =0;        
    };

#endif // __MPbkContactEditorControl_H__

// End of File
