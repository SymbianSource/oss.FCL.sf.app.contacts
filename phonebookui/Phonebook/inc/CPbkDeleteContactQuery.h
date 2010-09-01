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
*     Phonebook contact deletion query.
*
*/


#ifndef __CPbkDeleteContactQuery_H__
#define __CPbkDeleteContactQuery_H__

//  INCLUDES
#include <AknQueryDialog.h> // CAknQueryDialog
#include <cntdef.h>         // TContactItemId

// FORWARD DECLARATIONS
class CPbkContactItem;
class CPbkContactEngine;


// CLASS DECLARATION

/**
 * Phonebook contact deletion query.
 */
class CPbkDeleteContactQuery : public CAknQueryDialog
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         */
        IMPORT_C static CPbkDeleteContactQuery* NewLC();
        
        /**
         * Destructor.
         */
        ~CPbkDeleteContactQuery();

    public: // New functions
        /**
         * Executes this dialog to confirm deletion of a single contact.
         *
         * @param   aContact    contact to delete.
         * @return  !=0 if the user responded that the contact should be 
         *          deleted.
         */
        IMPORT_C TInt RunLD(const CPbkContactItem& aContact);

        /**
         * Executes this dialog to confirm deletion of a single contact.
         *
         * @param   aEngine     Phonebook database.
         * @param   aContactId  id of the contact to delete.
         * @return  !=0 if the user responded that the contact should be 
         *          deleted.
         */
        IMPORT_C TInt RunLD(CPbkContactEngine& aEngine, TContactItemId aContactId);

        /**
         * Executes this dialog to confirm deletion of a multiple contacts.
         *
         * @param   aEngine     Phonebook database.
         * @param   aContactIds ids of the contacts to delete.
         * @return  !=0 if the user responded that the contacts should be 
         *          deleted.
         */
        IMPORT_C TInt RunLD
            (CPbkContactEngine& aEngine, const CContactIdArray& aContactIds);

    private:  // Implementation
        CPbkDeleteContactQuery();
        void CreatePromptL(const CPbkContactItem& aContact);
        void CreatePromptL(const CContactIdArray& aContactIds);
    };

#endif // __CPbkDeleteContactQuery_H__
            
// End of File
