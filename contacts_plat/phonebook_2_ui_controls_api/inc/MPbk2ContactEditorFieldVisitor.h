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
* Description:  Phonebook 2 contact editor field visitor interface.
*
*/


#ifndef MPBK2CONTACTEDITORFIELDVISITOR_H
#define MPBK2CONTACTEDITORFIELDVISITOR_H

// FORWARD DECLARATIONS
class MPbk2ContactEditorField;

// CLASS DECLARATION

/**
 * Phonebook 2 contact editor field visitor interface.
 */
class MPbk2ContactEditorFieldVisitor
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ContactEditorFieldVisitor()
            {}

        /**
         * Visits the passed field editor.
         *
         * @param aThis     Field editor to visit.
         */
        virtual void VisitL(
                MPbk2ContactEditorField& aThis ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactEditorFieldVisitorExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2CONTACTEDITORFIELDVISITOR_H

// End of File
