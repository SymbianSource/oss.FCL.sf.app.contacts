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
* Description:  A store configuration observer
*
*/



#ifndef CPBK2UIEXTENSIONCONFIGURATIONOBSERVER_H
#define CPBK2UIEXTENSIONCONFIGURATIONOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2StoreConfigurationObserver.h>

// FORWARD DECLARATIONS
class CPbk2StorePropertyArray;
class CPbk2UIExtensionLoader;
class CPbk2StoreConfiguration;

// CLASS DECLARATION

/**
 *  A store configuration observer
 */
NONSHARABLE_CLASS(CPbk2UIExtensionConfigurationObserver) : 
        public CBase,
		public MPbk2StoreConfigurationObserver
    {
    public:  // Constructors and destructor
        static CPbk2UIExtensionConfigurationObserver* NewL(
            CPbk2StorePropertyArray& aPropertyArray,
            CPbk2UIExtensionLoader& aExtensionLoader,
            CPbk2StoreConfiguration& aStoreConfiguration);
            
        /**
         * Destructor.
         */
        ~CPbk2UIExtensionConfigurationObserver();

    public: // from MPbk2StoreConfigurationObserver
        void ConfigurationChanged();
        void ConfigurationChangedComplete();
    
    private:    // Construction
        CPbk2UIExtensionConfigurationObserver(
            CPbk2StorePropertyArray& aPropertyArray,
            CPbk2UIExtensionLoader& aExtensionLoader,
            CPbk2StoreConfiguration& aStoreConfiguration);
        void ConstructL();
        void AddPropertiesFromExtensionsL();
        
    private:    // Data
        /// Ref: Store property array
        CPbk2StorePropertyArray& iPropertyArray;
        /// Ref: UI extension loader
        CPbk2UIExtensionLoader& iExtensionLoader;
        /// Ref: Store configuration
        CPbk2StoreConfiguration& iStoreConfiguration;
    };

#endif      // CPBK2UIEXTENSIONCONFIGURATIONOBSERVER_H
            
// End of File
