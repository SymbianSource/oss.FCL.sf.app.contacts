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
*     Field type selection dialog for Phonebook's Create New / Edit Memory entry
*     functions.
*
*/


#ifndef __CPbkMemoryEntrySelectAddItemDlg_H__
#define __CPbkMemoryEntrySelectAddItemDlg_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CEikFormattedCellListBox;
class CAknPopupList;
class TPbkAddItemWrapper;

// CLASS DECLARATION

/**
 * Field type selection dialog for Phonebook's Create New / Edit Memory entry
 * functions.
 */
NONSHARABLE_CLASS(CPbkMemoryEntrySelectAddItemDlg) : 
        public CBase
    {
    public:  // Interface
        /**
         * Constructor.
         */
        CPbkMemoryEntrySelectAddItemDlg();

        /**
         * Sets *aSelfPtr to NULL when this dialog is destroyed.
         * @precond !aSelfPtr || *aSelfPtr==this
         */
        void ResetWhenDestroyed(CPbkMemoryEntrySelectAddItemDlg** aSelfPtr);

        /**
         * Basic ExecuteLD.
		 * @param aAddItems selectable items
         */
        TPbkAddItemWrapper* ExecuteLD(RArray<TPbkAddItemWrapper>& aAddItems);

        /**
         * Destructor.
         * Also cancels and dismisses this popup list, in other words makes
         * ExecuteLD return just like cancel was pressed.
         */
        ~CPbkMemoryEntrySelectAddItemDlg();

    private:
        /**
         * Creates list items, sets Cba and title.
         * Executed pre RunLD.
         * First part of ExecuteLD.
		 * @param aAddItems selectable items
		 * @param aCbaResource  cba buttons to use with the dialog
         */
        void PrepareLC(
            RArray<TPbkAddItemWrapper>& aAddItems, 
            TInt aCbaResource);

        /**
         * Execute the initialized dialog.
         * Second part of ExecuteLD.
		 * @return add item wrapper
         */
        TPbkAddItemWrapper* RunLD();

    private: // Data types
        class CListBoxModel;

    private: // Data members
        /// Own: list box.
        CEikFormattedCellListBox* iListBox;
        /// Own: popup listbox.
        CAknPopupList* iPopupList;
        /// Ref: Referred boolean is set to ETrue when this object is destroyed.
        TBool* iDestroyedPtr;
        /// Ref: set to NULL from destructor
        CPbkMemoryEntrySelectAddItemDlg** iSelfPtr;
    };

#endif // __CPbkMemoryEntrySelectAddItemDlg_H__
            
// End of File
