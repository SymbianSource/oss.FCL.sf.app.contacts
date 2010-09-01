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
* Description:  Phonebook 2 memory selection setting item.
*
*/


#ifndef CPBK2MEMORYSELECTIONSETTINGITEM_H
#define CPBK2MEMORYSELECTIONSETTINGITEM_H

//  INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CVPbkContactStoreUriArray;
class CSelectionItemList;

// CLASS DECLARATION

/**
 * Phonebook 2 memory selection setting item.
 */
NONSHARABLE_CLASS(CPbk2MemorySelectionSettingItem) : public CAknSettingItem
    {
    public:  // Construction and destruction

        /**
         * Constructor.
         *
         * @param aSettingId            Setting id.
         * @param aSupportedStores      Array of supported
         *                              contact store URIs.
         * @param aSelectionItemList    Selection item list.
         * @see CAknSettingItem
         */
        CPbk2MemorySelectionSettingItem(
                TInt aSettingId,
                CVPbkContactStoreUriArray& aSupportedStores,
                CSelectionItemList& aSelectionItemList );

        /**
         * Destructor
         */
        ~CPbk2MemorySelectionSettingItem();

    public: // Interface

        /**
         * Launches setting page.
         *
         * @return ETrue if OK pressed, EFalse if Cancel pressed.
         */
        TBool EditItemL();

        /**
         * Restores UI state.
         */
        void RestoreStateL();

    private: // From CAknSettingItem
        void CompleteConstructionL();
        void EditItemL(
                TBool aCalledFromMenu );
        void HandleSettingPageEventL(
                CAknSettingPage* aSettingPage,
                TAknSettingPageEvent aEventType );
        void StoreL();
        void LoadL();
        const TDesC& SettingTextL();

    private: // Implementation
        void UpdateSettingTextL();

    private: // Data
        /// Ref: Array of supported contact store URIs
        CVPbkContactStoreUriArray& iSupportedStores;
        /// Ref: Selection item list
        CSelectionItemList& iSelectionItemList;
        /// Own: Setting text
        HBufC* iSettingText;
        /// Own: Setting text for several memories
        HBufC* iSeveralMemories;
    };

#endif // CPBK2MEMORYSELECTIONSETTINGITEM_H

// End of File
