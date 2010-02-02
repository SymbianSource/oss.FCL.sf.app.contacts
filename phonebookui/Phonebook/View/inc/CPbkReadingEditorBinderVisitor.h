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
*       Implements MPbkFieldEditorVisitor for Japanese reading fields.
*
*/


#ifndef __CPbkReadingEditorBinderVisitor_H__
#define __CPbkReadingEditorBinderVisitor_H__

//  INCLUDES
#include <e32base.h>
#include <PbkFields.hrh>
#include "MPbkFieldEditorVisitor.h"


// FORWARD DECLARATIONS
class CReadingConverter;

// CLASS DECLARATION

/**
 * Implements MPbkEditorBinderVisitor for Japanese reading fields.
 */
NONSHARABLE_CLASS(CPbkReadingEditorBinderVisitor) : 
        public CBase,
        public MPbkFieldEditorVisitor
    {
    public: // Construction
        /**
         * Creates a new instance of this class.
         */
        static CPbkReadingEditorBinderVisitor* NewL();

        /**
         * Destructor.
         */
		~CPbkReadingEditorBinderVisitor();
		
    public:  // New methods
    
        /**
         * Prepares the binder for destruction of an editor
         */
        void NotifyEditorDeletion( TPbkFieldId aFieldId );

    public:  // from MPbkFieldEditorVisitor
        void VisitL(MPbkFieldEditorControl& aThis);

    private: // implementation
        CPbkReadingEditorBinderVisitor();

    private: // data
        ///Own: converter for first name field
        CReadingConverter* iFirstNameConverter;
        ///Own: converter for last name field
        CReadingConverter* iLastNameConverter;
    };

#endif // __CPbkReadingEditorBinderVisitor_H__
            
// End of File
