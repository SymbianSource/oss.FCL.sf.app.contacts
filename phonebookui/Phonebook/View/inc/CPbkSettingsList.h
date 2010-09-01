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
*     Settings list for Phonebook.
*
*/


#ifndef __CPbkSettingsList_H__
#define __CPbkSettingsList_H__

// INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class CPbkNameOrderSettingItem;
class CPbkContactEngine;

// CLASS DECLARATION

/**
 * Settings list listbox.
 */
NONSHARABLE_CLASS(CPbkSettingsList) : 
		public CAknSettingItemList
    {
    public:  // interface
        /**
         * Creates a new instance of this class.
         */
		static CPbkSettingsList* NewL(CPbkContactEngine& aEngine);

		/**
         * Destructor.
         */
        ~CPbkSettingsList();

		/**
		 * Returns the value of the desired setting item aSettingId.
		 */
		TInt SettingValueL(TInt aSettingId);

		/**
		 * Changes the value of the desired setting item aSettingId.
		 */
		void ChangeValueL(TInt aSettingId);

	private: // from CAknSettingItemList
		CAknSettingItem* CreateSettingItemL(TInt aSettingId);

    private:  // from CCoeControl
        virtual void SizeChanged();
	    
	private: // implementation
        CPbkSettingsList(CPbkContactEngine& aEngine);
        void ConstructL();

    private:  // data
		/// Own: default value for name order setting
		TInt iDefaultNameOrderValue;
		/// Ref: name order setting item
		CPbkNameOrderSettingItem* iNameOrderSettingItem;
		/// Ref: phonebook engine
		CPbkContactEngine& iEngine;
    };

#endif // __CPbkSettingsList_H__

// End of File
