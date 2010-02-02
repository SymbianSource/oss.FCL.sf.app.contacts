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
*     Name Order settings item for Phonebook.
*
*/


#ifndef __CPbkNameOrderSettingItem_H__
#define __CPbkNameOrderSettingItem_H__

//FORWARD DECLARATIONS
class CPbkContactEngine;

//  INCLUDES
#include <aknsettingitemlist.h>

// CLASS DECLARATION

/**
 * Name Order settings item.
 */
NONSHARABLE_CLASS(CPbkNameOrderSettingItem) : 
		public CAknEnumeratedTextPopupSettingItem
    {
    public:  // interface
        /**
         * Constructor.
         * @see CAknEnumeratedTextPopupSettingItem
         */
        CPbkNameOrderSettingItem(TInt aResourceId, TInt& aValue,
            CPbkContactEngine& aEngine);

        /**
         * Returns the value of the setting.
         */
         TInt Value();

        /**
         * Changes the value of the setting.
         * Since there are only two possible values
         * this function does not require any parameters.
         */
         void ChangeValueL();
         
         /**
         * Method called by framework to complete the construction.  Extra allocations of
         * memory or extra resource reading can happen here.	
         */
        void CompleteConstructionL();
        
    private:
        /// Ref: Phonebook contact engine
        CPbkContactEngine& iEngine;
    };

#endif // __CPbkNameOrderSettingItem_H__

// End of File
