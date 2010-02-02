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
*     predictive search setting item for Phonebook 2.
*
*/


#ifndef CPBK2PREDICTIVESEARCHSETTINGITEM_H
#define CPBK2PREDICTIVESEARCHSETTINGITEM_H

// INCLUDES
#include <aknsettingitemlist.h>


/**
 * Predictive Search settings item.
 */
NONSHARABLE_CLASS(CPbk2PredictiveSearchSettingItem) : 
		public CAknBinaryPopupSettingItem
    {
    public:  // interface
		/**
         * Constructor.
		 * @see CAknEnumeratedTextPopupSettingItem
		 * @param aResourceId, resource identifier
		 * @param aValue, initial value shows to the setting
         */
        CPbk2PredictiveSearchSettingItem( 
                TInt aResourceId, 
                TInt& aValue);

		private: // From CAknSettingItem
		
			void StoreL();
  
		private:
		    // save values to predictive search settings cenrep
		    void SetPredictiveSearchModeL( TBool aPredictiveMode );
       
    };

#endif // CPBK2PREDICTIVESEARCHSETTINGITEM_H

// End of File
