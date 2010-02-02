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
* Description:  Phonebook 2 contact info data swapper utility class.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <bamdesca.h>

// FORWARD DECLARATIONS
class CEikTextListBox;
class CPbk2PresentationContactFieldCollection;
class CPbk2ThumbnailLoader;

// CLASS DECLARATION

/**
 * Phonebook 2 contact info data swapper utility class.
 * Responsible for holding data for a short period of time so that
 * contact info can change the contact to show safely and rollback
 * if an error occurs.
 */
NONSHARABLE_CLASS(CPbk2ContactInfoDataSwapper) : public CBase
    {
    public: // Construction and destruction

        /**
         * Constructor.
         */
        CPbk2ContactInfoDataSwapper();

        /**
         * Destructor.
         */
        ~CPbk2ContactInfoDataSwapper();

    public:  // Interface

        /**
         * Saves data into this object which restores list box's state
         * (model, top and current item index) in this objects destructor.
         */
        void StoreListBoxState(CEikTextListBox& aListBox);

        /**
         * Resets any data set with StoreListBoxState().
         */
        void ResetListBoxState();

        /**
         * Swaps objects.
         *
         * @param aObj1     First object to swap.
         * @param aObj2     Second object to swap.
         */
        template<class Type>
        inline void Swap( Type& aObj1, Type& aObj2 )
            {
            Type tmp = aObj1;
            aObj1 = aObj2;
            aObj2 = tmp;
            }

    public: // Data
        /// Ref: Field collection
        const CPbk2PresentationContactFieldCollection* iFieldCollection;
        /// Own: List box model
        MDesCArray* iListBoxModel;
        /// Ref: List box
        CEikTextListBox* iListBox;
        /// Ref: List box's item array
        MDesCArray* iItemTextArray;
        /// Own: Top item index to restore
        TInt iTopItemIndex;
        /// Own: Current item index to restore
        TInt iCurrentItemIndex;
        /// Own: Title pane text
        HBufC* iTitlePaneText;
        /// Own: Thumbnail loader
        CPbk2ThumbnailLoader* iThumbnailLoader;
    };

// End of File
