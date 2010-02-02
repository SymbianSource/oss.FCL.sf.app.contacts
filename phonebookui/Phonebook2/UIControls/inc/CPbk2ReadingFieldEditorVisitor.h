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
* Description:  Phonebook 2 contact editor reading field visitor.
*
*/


#ifndef CPBK2READINGFIELDEDITORVISITOR_H
#define CPBK2READINGFIELDEDITORVISITOR_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ContactEditorFieldVisitor.h"

// FORWARD DECLARATIONS
class CReadingConverter;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * Implements MPbk2ContactEditorFieldVisitor for Japanese reading fields
 * in contact editor.
 */
NONSHARABLE_CLASS(CPbk2ReadingFieldEditorVisitor) : 
        public CBase,
        public MPbk2ContactEditorFieldVisitor
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2ReadingFieldEditorVisitor* NewL(
                const CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         */
		~CPbk2ReadingFieldEditorVisitor();

    public:  // From MPbk2ContactEditorFieldVisitor
        void VisitL(
                MPbk2ContactEditorField& aThis );

    private: // Implementation
        CPbk2ReadingFieldEditorVisitor(
                const CVPbkContactManager& aContactManager );
        void SetEditorsL(
                MPbk2ContactEditorField& aThis );

    private: // Data
        /// Own: Converter for first name field
        CReadingConverter* iFirstNameConverter;
        /// Own: Converter for last name field
        CReadingConverter* iLastNameConverter;
        /// Ref: Virtual Phonebook Contact Manager
        const CVPbkContactManager& iContactManager;
    };

#endif // CPBK2READINGFIELDEDITORVISITOR_H
            
// End of File
