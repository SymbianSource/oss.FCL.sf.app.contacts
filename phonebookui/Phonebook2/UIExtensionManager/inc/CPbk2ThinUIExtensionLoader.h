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
* Description:  Loads thin UI extensions
*
*/



#ifndef CPBK2THINUIEXTENSIONLOADER_H
#define CPBK2THINUIEXTENSIONLOADER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionThinPlugin;
class CImplementationInformation;

// CLASS DECLARATION

/**
*  Loads thin UI extensions
*
*/
NONSHARABLE_CLASS(CPbk2ThinUIExtensionLoader) : 
        public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * Loads all menu plugins
        * @return a new instance of this class
        */
        static CPbk2ThinUIExtensionLoader* NewL();
        
        /**
        * Destructor.
        */
        ~CPbk2ThinUIExtensionLoader();

    public: // New functions
        /**
        * Returns the number of loaded plugins
        * @return the number of loaded plugins
        */
        inline TInt PluginCount() const;
        
        /**
        * Returns the plugin in give position
        * @param aIndex the index of the plugin
        */
        inline CPbk2UIExtensionThinPlugin& PluginAt(TInt aIndex) const;
    
    private:    // Construction
        CPbk2ThinUIExtensionLoader();
        void ConstructL();

    private:    // New functions
        void LoadImplementationsL(CImplementationInformation& aImplInfo);
        CPbk2UIExtensionThinPlugin* FindImplementation(TUid& aUid);
        
    private:    // Data
        /// An array of menu plugin implementions
        RPointerArray<CPbk2UIExtensionThinPlugin> iPlugins;
    };

// INLINE FUNCTIONS
// -----------------------------------------------------------------------------
// CPbk2ThinUIExtensionLoader::PluginCount
// -----------------------------------------------------------------------------
//
inline TInt CPbk2ThinUIExtensionLoader::PluginCount() const
    {
    return iPlugins.Count();
    }

// -----------------------------------------------------------------------------
// CPbk2ThinUIExtensionLoader::PluginAt
// -----------------------------------------------------------------------------
//
inline CPbk2UIExtensionThinPlugin& CPbk2ThinUIExtensionLoader::PluginAt(
        TInt aIndex) const
    {
    return *iPlugins[aIndex];
    }

#endif      // CPBK2THINUIEXTENSIONLOADER_H
            
// End of File
