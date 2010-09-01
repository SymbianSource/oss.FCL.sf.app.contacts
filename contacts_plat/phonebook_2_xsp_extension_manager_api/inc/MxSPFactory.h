/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*    Abstract interface for creating Phonebook xSP extensions.
*
*/


#ifndef __MXSPFACTORY_H__
#define __MXSPFACTORY_H__


//  INCLUDES
#include <e32std.h>  // for TUid


//  FORWARD DECLARATIONS
class MxSPView;
class MxSPViewMapper;
class MxSPContactManager;
class CxSPBaseView;
class CPbk2UIExtensionView;
class CVPbkContactManager;
class CPbk2StorePropertyArray;
class MxSPViewInfo;

// CLASS DECLARATION

/**
 * Abstract factory interface for creating Phonebook xSP extensions. Every xSP
 * extension should have a factory class derived from the factory base CxSPFactory,
 * which is derived from this. Instance of this factory should be kept as long as 
 * extensions are used, if resources are needed.
 */
class MxSPFactory
    {        
    public:  // Destructor
        /**
         * Destructor.
         */
        virtual ~MxSPFactory() {}

    public: // Interface

        /**
         * Creates a Phonebook view extension for "Names List" view.
         * Default implementation is empty.
         * @return Phonebook view extension object.
         */
        virtual MxSPView* CreatePbkViewExtensionL();

        /**
         * Creates a new extension-implemented Phonebook view.
         *
         * @param aViewId   id of the extension view to create.
         * @param aView     Host view fot this extension view
         * @return the extension view object.
         */
        virtual CxSPBaseView* CreateViewL( TUid aViewId, CPbk2UIExtensionView& aView ) =0;

        /**
         * Registers view id mapper. This method is called automatically
         * by Extension Manager framework at initialization phase.
         *
         * @param aMapper View id mapper
         */  
        virtual void RegisterViewMapper( MxSPViewMapper& aMapper ) = 0;
                
        /**
         * Gets XSP_VIEW_INFO_ARRAY resource. Resource should be loaded.
         * This is alternative A for providing view information.
         * Alternative B is to use ViewInfoCount and ViewInfo methods. If
         * this alternative A is used, ViewInfoCount method must return 0
         * and ViewInfo method must return NULL
         *
         * @return Resource id or KErrNotFound
         */        
        virtual TInt ViewInfoResource() const = 0;
        
        /**
         * Gets extension id
         *
         * @return Extension implementation id (ECom implementation id)
         */
        virtual TUint32 Id() const = 0;

        /**
         * Gets extension name
         *
         * @return The name of the extension
         */
        virtual const TDesC& Name() const = 0;
        
        /**
         * Register contact manager/mapper. This method is called automatically
         * by Extension Manager framework at initialization phase.
         *
         * @param aManager Contact manager
         */
        virtual void RegisterContactManager( MxSPContactManager& aManager ) = 0;

        /**
         * Sets virtual pbk contact manager
         *
         * @param aVPbkContactManager virtual pbk contact manager
         */
        virtual void SetVPbkContactManager( CVPbkContactManager& aVPbkContactManager ) = 0;
        
        /**
         * Called in an application start-up and when the store
         * configuration changes. The extension can update the store
         * property array by adding or removing properties.
         *
         * @param aPropertyArray    Phonebook 2 store property array.
         */
        virtual void UpdateStorePropertiesL( CPbk2StorePropertyArray& aPropertyArray ) = 0;
        
        /**
         * Gets view count of this extension.
         * This is alternative B for providing view information.
         * Alternative A is to use ViewInfoResource method. If
         * this alternative B is used, ViewInfoResource method
         * must return KErrNotFound
         *
         * @return view count
         */  
        virtual TInt ViewInfoCount() const = 0;
        
        /**
         * Gets view info at specified index.
         * This is alternative B for providing view information.
         * Alternative A is to use ViewInfoResource method. If
         * this alternative B is used, ViewInfoResource method
         * must return KErrNotFound
         *
         * @param aIndex index
         * @return view info at specified index
         */ 
        virtual const MxSPViewInfo* ViewInfo( TInt aIndex ) const = 0;
    };
        
    
inline MxSPView* MxSPFactory::CreatePbkViewExtensionL()
    {
    return NULL;
    }

#endif // __MXSPFACTORY_H__

// End of File
