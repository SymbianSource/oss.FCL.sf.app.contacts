/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 info dialog.
*
*/


#ifndef CPBK2PHONEBOOKINFODLG_H
#define CPBK2PHONEBOOKINFODLG_H

// INCLUDES
#include <eikdialg.h>    // CEikDialog

// FORWARD DECLARATIONS
class CAknPopupList;
class CAknSinglePopupMenuStyleListBox;
class CVPbkContactManager;
class CVPbkContactStoreUriArray;
class MPbk2StoreInfoUiItem;

// CLASS DECLARATION

/**
 * Phonebook 2 info dialog.
 * Shows memory statuses of different stores.
 */
class CPbk2PhonebookInfoDlg : public CBase
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2PhonebookInfoDlg* NewL();

        /**
         * Destructor.
         */
        ~CPbk2PhonebookInfoDlg();

    public: // Interface

        /**
         * Executes the info dialog.
         *
         * @param aStoreUris    If the array contains only one store
         *                      a single store information is shown.
         *                      If there are more than one store
         *                      information of all the stores is shown.
         */
        IMPORT_C void ExecuteLD(
                CVPbkContactStoreUriArray& aStoreUris );

    private: // Implementation
        CPbk2PhonebookInfoDlg();
        void ConstructL();
        void SetTitleL(
                CVPbkContactStoreUriArray& aStoreUris );
        void SetDataL(
                CVPbkContactManager& aContactManager,
                CVPbkContactStoreUriArray& aStoreUris );

    private: // Data
        /// Own: Popup list that is the actual dialog that is shown
        CAknPopupList* iPopupList;
        /// Own: Listbox that is given to CAknPopupList constructor
        CAknSinglePopupMenuStyleListBox* iListBox;        
        /// Ref: Keeps track if destructor is already called
        TBool* iDestroyed;
        /// Own: Info item array
        CArrayPtr<MPbk2StoreInfoUiItem>* iInfoItems;
    };

#endif // CPBK2PHONEBOOKINFODLG_H

// End of File
