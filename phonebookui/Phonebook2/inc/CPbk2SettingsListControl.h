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
* Description:  Phonebook 2 settings list control.
*
*/


#ifndef CPBK2SETTINGSLISTCONTROL_H
#define CPBK2SETTINGSLISTCONTROL_H

//  INCLUDES
#include <aknsettingitemlist.h>

// FORWARD DECLARATIONS
class MPbk2SettingsControlExtension;
class CPbk2SortOrderManager;
class CPbk2DefaultSavingStorageSettingItem;
class CPbk2MemorySelectionSettingItem;
class CPbk2SettingsListState;
class MPbk2SettingsControlExtension;
class CPbk2PredictiveSearchSettingItem;

/**
 * Phonebook 2 settings list control.
 */
class CPbk2SettingsListControl : public CAknSettingItemList
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContainer            Control container.
         * @param aSortOrderManager     Sort order manager.
         * @param aControlExtension     Control extension.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2SettingsListControl* NewL(
            const CCoeControl* aContainer,
            CPbk2SortOrderManager& aSortOrderManager,
            MPbk2SettingsControlExtension& aControlExtension);

        /**
         * Destructor.
         */
        ~CPbk2SettingsListControl();

    public: // Interface

        /**
         * Commits settings.
         */
        IMPORT_C void CommitSettingsL();
        
    public: // From CCoeControl
        void SizeChanged();        

    public: // From CAknSettingItemList
        void EditItemL(
                TInt aIndex,
                TBool aCalledFromMenu );
        void HandleResourceChange( TInt aType );

    	void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);
    	
    protected: // From CAknSettingItemList
        CAknSettingItem* CreateSettingItemL(
                TInt aSettingId );

    private: // Implementation
        CPbk2SettingsListControl(
                CPbk2SortOrderManager& aSortOrderManager,
                MPbk2SettingsControlExtension& aControlExtension );
        void ConstructL(
                const CCoeControl* aContainer );
        void LaunchMemorySelectionSettingPageL();
        void PopulateSelectionItemListL();
        void StoreSettings();
        
        /**
        * Checks name ordering visibility from resource file
        */
        TBool ShowNameOrderingSettingL();
        
        /**
         * get predictive search enable flag from central repository,
         * save it to iPredictiveSearchSettingsValue
         */
        void PredictiveSearchEnableInfoFromCenrepL();
        
        /**
         * @return ETrue if there is physical keyboard
         */
        TBool PhysicalKeyboardExistL();
        
        /**
         * reset the cenrep for predictive search, so that predictive search is disabled.
         */
        void DisablePredictiveSearchInCenrepL();

    private: // Data
        /// Ref: This control's container
        const CCoeControl* iContainer;
        /// Ref: Sort order manager
        CPbk2SortOrderManager& iSortOrderManager;
        /// Ref: Settings control extension
        MPbk2SettingsControlExtension& iControlExtension;
        /// Own: Settings list state
        CPbk2SettingsListState* iSettingListState;
        /// Ref: Default saving store setting item
        CPbk2DefaultSavingStorageSettingItem* iDefaultSavingStorageItem;
        /// Ref: Selection setting item
        CPbk2MemorySelectionSettingItem* iSelectionSettingItem;
        /// Ref: predictive search setting item
        CPbk2PredictiveSearchSettingItem* iPredictiveSearchItem;
        
        /// Own: Default name display order
        TInt iDefaultNameDisplayOrder;
        /// Own: Default saving store
        TInt iDefaultSavingStorage;
        /// own, settings value for predictive search 
        TBool iPredictiveSearchSettingsValue;
    };

#endif // CPBK2SETTINGSLISTCONTROL_H

// End of File
