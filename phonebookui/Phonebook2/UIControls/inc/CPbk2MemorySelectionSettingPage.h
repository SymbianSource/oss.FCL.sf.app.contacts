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
*     Memory selection settings page for Phonebook 2.
*
*/


#ifndef CPBK2MEMORYSELECTIONSETTINGPAGE_H
#define CPBK2MEMORYSELECTIONSETTINGPAGE_H

//  INCLUDES
#include <akncheckboxsettingpage.h>

// CLASS DECLARATION

/**
 * Memory selection setting page
 */
NONSHARABLE_CLASS(CPbk2MemorySelectionSettingPage) : 
		public CAknCheckBoxSettingPage
    {
    public:  // interface
		/**
         * Constructor.
		 * @see CAknCheckBoxSettingPage
         */
        CPbk2MemorySelectionSettingPage(TInt aResourceID, 
                                         CSelectionItemList* aItemArray);

        ~CPbk2MemorySelectionSettingPage();
               
    private: // From CAknCheckBoxSettingPage
        void UpdateSettingL();
        void UpdateCbaL();                        

        void HandleControlEventL( CCoeControl* aControl,  
            TCoeEvent aEventType );
            
        void ProcessCommandL(TInt aCommandId);
    private: // Data
        //Ref: Pages item array
        CSelectionItemList* iItemsArray; 

    };

#endif // CPBK2MEMORYSELECTIONSETTINGPAGE_H

// End of File
