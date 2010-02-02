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
*
*/


#ifndef __MPbkEditorBinderVisitor_H__
#define __MPbkEditorBinderVisitor_H__

//  FORWARD DECLARATIONS
class MPbkFieldEditorControl;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Abstract base class for field editor visitors. (visitor pattern)
 */
class MPbkFieldEditorVisitor
    {
    public:  // Interface
        /**
         * Visits the passed field editor.
         * @param aThis Field editor to visit.
         */
        virtual void VisitL(MPbkFieldEditorControl& aThis) = 0;
    };


#endif // __MPbkEditorBinderVisitor_H__

// End of File
