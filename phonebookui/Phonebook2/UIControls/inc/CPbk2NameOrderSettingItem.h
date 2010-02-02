/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Name Order settings item for Phonebook 2.
*
*/


#ifndef CPBK2NAMEORDERSETTINGITEM_H
#define CPBK2NAMEORDERSETTINGITEM_H

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CPbk2SortOrderManager;

// CLASS DECLARATION

/**
 * Name Order settings item.
 */
NONSHARABLE_CLASS(CPbk2NameOrderSettingItem) : 
		public CAknEnumeratedTextPopupSettingItem
    {
    public:  // interface
		/**
         * Constructor.
		 * @see CAknEnumeratedTextPopupSettingItem
         */
        CPbk2NameOrderSettingItem(
                CPbk2SortOrderManager& aSortOrderManager, 
                TInt aResourceId, 
                TInt& aValue);

        /**
		 * Returns the value of the setting.
		 *
		 * @return selected value
		 */
		 TInt Value();
		 
    public: // From CAknEnumeratedTextPopupSettingItem
        void CompleteConstructionL();
        void StoreL();
    
    private: // implementation
        TPtrC Separator();
        
        
    private: // Data
        /// Ref: Sort order manager
        CPbk2SortOrderManager& iSortOrderManager;
    };

#endif // CPBK2NAMEORDERSETTINGITEM_H

// End of File
