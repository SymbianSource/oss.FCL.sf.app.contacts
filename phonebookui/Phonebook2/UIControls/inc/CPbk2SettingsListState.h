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
* Description:  Phonebook 2 settings list state.
*
*/


#ifndef CPBK2SETTINGSLISTSTATE_H
#define CPBK2SETTINGSLISTSTATE_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CVPbkContactStoreUriArray;
class CSelectionItemList;

// CLASS DECLARATION

/**
 * Phonebook 2 settings list state.
 */
NONSHARABLE_CLASS(CPbk2SettingsListState) : public CBase
    {
    public: // Constructor and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2SettingsListState* NewL();

        /**
         * Destructor.
         */
        ~CPbk2SettingsListState();

    public: // Interface

        /**
         * Returns the selection item list.
         *
         * @return  Selection item list.
         */
        CSelectionItemList& SelectionItemList();
        
        /**
         * Returns a copy of selection item list.
         *
         * @return old item list state.
         */
        CSelectionItemList& ItemListState();

        /**
         * Returns supported store configuration.
         *
         * @return  Array of supported contact store URIs.
         */
        CVPbkContactStoreUriArray& SupportedStores();

        /**
         * Stores selection item list state.
         */
        void StoreItemStateL();

        /**
         * Restores selection item list state.
         */
        void RestoreItemStateL();

        /**
         * Checks is selection item list changed.
         *
         * @return  ETrue if it is changed, EFalse otherwise.
         */
        TBool IsChanged();

    private: // Implementation
        CPbk2SettingsListState();
        void ConstructL();
        void PopulateSelectionItemListL();

    private: // Data
        /// All the following lists must be of same size
        /// Own: Array of supported contact store URIs
        CVPbkContactStoreUriArray* iSupportedStores;
        /// Own: Selection item list
        CSelectionItemList* iSelectionItemList;
        /// Own: Selection item list. A copy of iSelectionItemList,
        /// which is used to restore old state
        CSelectionItemList* iItemListState;
    };

#endif // CPBK2SETTINGSLISTSTATE_H

// End of File
