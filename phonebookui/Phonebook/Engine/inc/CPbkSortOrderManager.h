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
*    Phonebook contact Sort order manager.
*
*/


#ifndef __CPBKSORTORDERMANAGER_H__
#define __CPBKSORTORDERMANAGER_H__

//  INCLUDES
#include <e32base.h>     // CBase
#include <cntviewbase.h>
#include "CPbkContactEngine.h"
#include <MPbkGlobalSetting.h>


// CLASS DECLARATION

/**
 * Phonebook Sort order manager. 
 * Owns the sorting order array for Phonebook.
 */
NONSHARABLE_CLASS(CPbkSortOrderManager) : 
        public CBase,
        public MPbkGlobalSettingObserver
	{
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aFs file server session
         * @param aNameOrder Name sort order 
         */
        static CPbkSortOrderManager* NewL(RFs& aFs, TBool aSettingsVisible);

	    /**
         * Destructor.
         */
        ~CPbkSortOrderManager();

    public:  // interface
        /**
         * Returns the current sort order.
         */
        const RContactViewSortOrder& SortOrder() const;

		/**
		 * Sets the Sort order according to aNameOrder.
		 */
		void SetNameDisplayOrderL(CPbkContactEngine::TPbkNameOrder aNameOrder);

		/**
		 * Return the current name ordering.
		 */
		CPbkContactEngine::TPbkNameOrder NameDisplayOrder() const;

		/**
		 * Set aContact as the view to be sorted.
		 */
		void SetContactView(CContactNamedRemoteView& aContactView);
		
		/**
		 * Sets the separator char to be used between last- and first names.
		 * If value of aSeparator= 0, then separator should not be used between names.
		 */		
        void SetNameSeparatorL(TChar aNameSeparator);

		/**
		 * Returns the separator char to be used between last- and first names.
		 * If value = 0, then separator should not be used between names.
		 */	
        TChar NameSeparator() const;
        
    public:  // From MPbkGlobalSettingObserver
		void SettingChangedL(MPbkGlobalSetting::TPbkGlobalSetting aKey);

    private:  // Implementation
	    CPbkSortOrderManager(RFs& aFs, TBool aSettingsVisible);
        void ConstructL();
		static RContactViewSortOrder DoCreateSortOrderL(
            CPbkContactEngine::TPbkNameOrder aNameOrder, TBool aSettingVisible);
		CPbkContactEngine::TPbkNameOrder PersistentNameDisplayOrderL();
		TChar PersistentNameSeparatorCharL();
		void SetPersistentNameDisplayOrderL(
            CPbkContactEngine::TPbkNameOrder aNameOrder);
		void SetPersistentNameSeparatorCharL(TChar aNameSeparatorChar);            

    private:  // Data
		/// Ref: sort order
        RContactViewSortOrder iSortOrder;
		/// Own: shared data client
		MPbkGlobalSetting* iPersistentSetting;
		/// Ref: sorted contact view 
		CContactNamedRemoteView* iContactView;
		/// Separator to be used between first and last names
		TChar iNameSeparator;		
		/// Own: Name order setting
		HBufC* iNameOrderSetting;
		/// Own: Setting for name separator usage
        HBufC* iNameSeparatorUsed;
        /// Own: Initial name separator char when not yet defined
        HBufC* iInitialNameSeparatorChar;
        /// Ref: file server connection
        RFs& iFs;        
        /// Settings menu item visibility
        TBool iSettingsVisible;
	};


#endif // __CPBKSORTORDERMANAGER_H__

// End of File
