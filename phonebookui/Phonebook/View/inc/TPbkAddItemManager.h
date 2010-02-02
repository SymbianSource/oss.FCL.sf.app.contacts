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
*    Phonebook add item manager.
*
*/


#ifndef __TPbkAddItemManager_H__
#define __TPbkAddItemManager_H__

//  INCLUDES
#include <e32base.h>        // CBase

// FORWARD DECLARATIONS
class CPbkFieldInfoGroup;
class CPbkContactItem;
class CPbkFieldsInfo;
class TPbkAddItemWrapper;
class MPbkContactEditorCreateField;

// CLASS DECLARATION

/**
 * Manages the add field operation for a contact item. Uses 
 * CPbkMemoryEntrySelectAddItemDlg for querying the field(s)
 * to add.
 */
NONSHARABLE_CLASS(TPbkAddItemManager)
    {
    public:  // Constructors and destructor        
        /**
         * C++ Constructor.
         * @param aItem   Contact item, which is the target for
         *                field adding operation.
		 * @param aAddField	managerial object that does the field adding.
         */        
        TPbkAddItemManager(CPbkContactItem& aItem,
			MPbkContactEditorCreateField& aAddField);

        /**
         * Destructor.
         */
        ~TPbkAddItemManager();
    
    public:  // implementation
         /**
         * Queries for the field to add and adds the selected 
         * field(s) to the contact.
         * @param aFieldsInfo   Fieldsinfo instance.
         * @param aAddedItemsControlIds As return contains
		 *        all the control id of the controls
         *        which were added to the dialog.
         */
        void AddFieldsL(
            const CPbkFieldsInfo& aFieldsInfo,
            RArray<TInt>& aAddedItemsControlIds);

    private:  // Implementation
        TBool ContactHasGroupItems(
            const CPbkFieldInfoGroup& aGroup) const;
        void AddFieldsToContactL(
            const TPbkAddItemWrapper& aItemWrapper,
            RArray<TInt>& aAddedItemsControlIds);       
        void CreateAddItemArrayL(
            const CPbkFieldsInfo& aFieldsInfo,
            RArray<TPbkAddItemWrapper>& aAddItems) const;

    private:  // Data
        /// Ref: to the contact item where fields are added
        const CPbkContactItem&    iContactItem;
		/// Ref: the field adding managerial class
		MPbkContactEditorCreateField& iAddField;
    };

#endif // __TPbkAddItemManager_H__

// End of File
