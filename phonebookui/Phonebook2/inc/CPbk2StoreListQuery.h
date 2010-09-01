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
* Description:  Phonebook 2 contact store selection list query.
*
*/


#ifndef CPBK2STORELISTQUERY_H
#define CPBK2STORELISTQUERY_H

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class CVPbkContactStoreUriArray;

// CLASS DECLARATION

/**
 * Phonebook 2 contact store selection list query.
 */
class CPbk2StoreListQuery : public CBase
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreListQuery* NewL();

        /**
         * Destructor.
         */
        virtual ~CPbk2StoreListQuery();

    public: // Interface

        /**
         * Executes the selection dialog.
         *
         * @param aTitle        Dialog title.
         * @param aUriArray     Contains the store URIs that are shown
         *                      in the query if they have a name.
         *                      After the query the array contains the
         *                      selected URIs. If "All" item text has
         *                      been set then the array is not changed
         *                      and it contains all the stores.
         * @return the dialog return value
         */
        IMPORT_C TInt ExecuteLD(
                const TDesC& aTitle,
                CVPbkContactStoreUriArray& aUriArray );

        /**
         * If the list must contain an item to select all the stores then
         * this can be used to set the item.
         *
         * @param aAllItemText      The text to display as an "All" text.
         * @param aUseTopPosition   If ETrue the item is shown at the
         *                          topmost position. If EFalse the item
         *                          is shown at the bottom.
         */
        IMPORT_C void SetAllItemTextL(
                const TDesC& aAllItemText,
                TBool aUseTopPosition );

    private: // Implementation
        CPbk2StoreListQuery();
        MDesCArray* CreateListItemsL(
                CVPbkContactStoreUriArray& aUriArray );
        void SetSelection(
                CVPbkContactStoreUriArray& aUriArray,
                TInt aSelection );
        TInt AllItemIndex(
                CVPbkContactStoreUriArray& aUriArray );

    private: // Data
        /// Own: Buffer containing the "All" item text
        HBufC* iAllItemText;
        /// Own: Indicates whether the "All" item is shown at the top
        TBool iAllItemUsesTopPosition;
    };

#endif // CPBK2STORELISTQUERY_H

// End of File
