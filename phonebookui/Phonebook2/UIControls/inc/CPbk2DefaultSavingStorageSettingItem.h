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
* Description:  Phonebook 2 default saving storage setting item.
*
*/


#ifndef CPBK2DEFAULTSAVINGSTORAGESETTINGITEM_H
#define CPBK2DEFAULTSAVINGSTORAGESETTINGITEM_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CVPbkContactStoreUriArray;
class CSelectionItemList;
class CAknSingleStyleListBox;
class CRepository;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * Phonebook 2 default saving storage setting item.
 */
NONSHARABLE_CLASS( CPbk2DefaultSavingStorageSettingItem ) :
            public CAknEnumeratedTextPopupSettingItem
    {
    public: // Construction and destruction

        /**
         * Constructor.
         *
         * @param aResourceId       Resource id.
         * @param aValue            Value.
         * @param aSupportedStores  Array of supported stores
         * @see CAknEnumeratedTextPopupSettingItem
         */
        CPbk2DefaultSavingStorageSettingItem(
                TInt aResourceId,
                TInt& aValue,
                CVPbkContactStoreUriArray& aSupportedStores,
                CSelectionItemList& aSelectionItemList );

        /**
         * Destructor.
         */
        ~CPbk2DefaultSavingStorageSettingItem();

    public: // Interface

        /**
         * Returns the value of the setting.
         *
         * @return  Value of the setting.
         */
         TInt Value();

        /**
         * Updates the setting item.
         *
         * @return  ETrue if update successful, EFalse otherwise.
         */
        TBool UpdateL();

        /**
         * Restores array states.
         */
        void RestoreStateL();

    private: // From CAknSettingItem
        void CompleteConstructionL();
        void StoreL();
        void LoadL();

    private: // Implementation
        void CreatePopupItemListL();
        void CreateEnumeratedItemListL();
        void ShowInformationNoteL();
        TBool LaunchDefaultSavingMemoryQueryL();
        TBool IsDefaultSavingStorageL(
                HBufC& aStoreName );
        void SetSelectedIndex(
                TInt aNewIndex );
        void SetSelectedIndex(
                const TDesC& aStoreUri );
        TBool CheckSelectedItems();

    private: // Data
        /// Ref: Array of supported contact store URIs
        CVPbkContactStoreUriArray& iSupportedStores;
        /// Ref: Selection item list
        CSelectionItemList& iSelectionItemList;
        /// Own: Repository
        CRepository* iRepository;
        /// Ref: Array of text enumerations
        CArrayPtr<CAknEnumeratedText>* iEnumeratedTextArray;
        /// Ref: Array of texts for popup items
        CArrayPtr<HBufC>* iPopupTextArray;
        /// Own: Array of contact store URIs
        CArrayPtr<HBufC>* iStoreUriArray;
    };

#endif // CPBK2DEFAULTSAVINGSTORAGESETTINGITEM_H

// End of File
