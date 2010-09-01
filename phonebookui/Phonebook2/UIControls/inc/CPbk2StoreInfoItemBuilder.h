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
* Description:  A class for building store info items
*
*/



#ifndef CPBK2STOREINFOITEMBUILDER_H
#define CPBK2STOREINFOITEMBUILDER_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h>

// FORWARD DECLARATIONS
class CVPbkContactManager;
class TVPbkContactStoreUriPtr;
class CVPbkContactStoreUriArray;
class MVPbkContactStore;
class CPbk2StorePropertyArray;
class CPbk2StoreInfoItemArray;
class MPbk2StoreInfoUiItem;
class CPbk2StoreInfoItem;
class CPbk2StoreProperty;

// CLASS DECLARATION

/**
*  A class for building store info items
*
*/
class CPbk2StoreInfoItemBuilder : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aContactManager The contact manager that has currently
		*						 valid stores open
		* @param aStoreProperties UI properties of the stores
        * @return a new instance of this class
        */
        static CPbk2StoreInfoItemBuilder* NewL(
			CVPbkContactManager& aContactManager,
			CPbk2StorePropertyArray& aStoreProperties);
        
        /**
        * Destructor.
        */
        virtual ~CPbk2StoreInfoItemBuilder();

    public: // New functions
        
        /**
        * Builds an item array for single store
        *
        * @param aUriPtr the source store URI
        * @return an array of UI info items.
        *         Caller takes the ownership of the array and its items
        *         so ResetAndDestroy must be called
        */
        CArrayPtr<MPbk2StoreInfoUiItem>* BuildSingleStoreItemsL(
            const TVPbkContactStoreUriPtr& aUriPtr);
        
        /**
        * Builds an item array for all stores
        *
        * @param aStoreUris the source store URIs
        * @return an array of UI info items.
        *         Caller takes the ownership of the array and its items
        *         so ResetAndDestroy must be called
        */
        CArrayPtr<MPbk2StoreInfoUiItem>* BuildAllStoresItemsL(
                CVPbkContactStoreUriArray& aStoreUris);
         
    private: // Construction

        /**
        * C++ constructor.
        */
        CPbk2StoreInfoItemBuilder(CVPbkContactManager& aContactManager,
	        CPbk2StorePropertyArray& aStoreProperties);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // New functions
        MPbk2StoreInfoUiItem* CreateSingleStoreUiItemLC(
            MVPbkContactStore& aStore,
            const CPbk2StoreProperty& aProperty,
            const CPbk2StoreInfoItem& aItem);
        MPbk2StoreInfoUiItem* CreateUiItemLC(
            const CPbk2StoreProperty* aProperty,
            const CPbk2StoreInfoItem& aItem,
            TInt aIntValue);
        HBufC* CreateHeadingL(
            const CPbk2StoreProperty* aProperty, 
            const CPbk2StoreInfoItem& aItem, 
            TInt aValue);
        HBufC* FormatUiItemValueL(
            const CPbk2StoreInfoItem& aItem, TInt aValue);
        
    private:    // Data
        /// Not own: Contact manager
        CVPbkContactManager& iContactManager;
        /// Not own: Store properties
	    CPbk2StorePropertyArray& iStoreProperties;
	    /// Own: Single store item array
	    CPbk2StoreInfoItemArray* iSingleStoreItemDefs;
	    /// Own: All stores item array
	    CPbk2StoreInfoItemArray* iAllStoresItemDefs;
    };

#endif      // CPBK2STOREINFOITEMBUILDER_H
            
// End of File
