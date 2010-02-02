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
* Description:  Load UI extensions
*
*/


#ifndef CPBK2UIEXTENSIONLOADER_H
#define CPBK2UIEXTENSIONLOADER_H

// INCLUDE FILES
#include <e32base.h>
#include <RPbk2LocalizedResourceFile.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionPlugin;
class CPbk2UIExtensionInformation;
/**
 * Load UI extensions
 */
NONSHARABLE_CLASS(CPbk2UIExtensionLoader) : public CBase
    {
    public: // Construction and destruction

        /**
         * Returns a new instance of this class.
         * @return A newly created object of this class.
         */
        static CPbk2UIExtensionLoader* NewL();

        /**
         * Destructor.
         */
        ~CPbk2UIExtensionLoader();

    public: // Interface
        /**
         * Checks if the given plug-in implementation is loaded.
         * @aImplementationUid Plug-in whose loading status to check
         * @return ETrue if a plug-in with aImplementationUid is loaded,
         *         EFalse otherwise.
         */
        TBool IsLoaded(TUid aImplementationUid);

        /**
         * Tries to load a plug-in with implementation UID aImplementationUid
         * if it is not already loaded.
         */
        void EnsureLoadedL(TUid aImplementationUid);

        /**
         * Returns a loaded plug-in with implementation UID aImplementationUid
         * if it exists, NULL otherwise.
         * @param aImplementationUid Implementation UID of the plug-in that is searched.
         * @return A oaded plug-in with implementation UID aImplementationUid
         *         if it exists, NULL otherwise.
         */
        CPbk2UIExtensionPlugin* LoadedPlugin(TUid aImplementationUid) const;
        
        /**
         * Returns an array of extension plugin information instances
         * @return an array of extension plugin information instances
         */
        TArray<CPbk2UIExtensionInformation*> PluginInformation() const;
        
        /**
         * Returns an array of loaded plugins.
         * @return an array of loaded plugins
         */
        TArray<CPbk2UIExtensionPlugin*> Plugins() const;

    private: // Implementation
        CPbk2UIExtensionLoader();
        void ConstructL();
        void LoadExtensionL(
                TUid aImplementationUid );
        void LoadStartUpExtensionsL();
        TBool IsOkToLoadL(TUid aImplementationUid ); 

    private: // Data
        /// Own: Common extension resource file
        RPbk2LocalizedResourceFile iCommonResourceFile;
        /// Own: Array of loaded plug-ins.
        RPointerArray<CPbk2UIExtensionPlugin> iPluginArray;
        /// Own: Array of plug-in resources.
        RPointerArray<CPbk2UIExtensionInformation> iPluginInformation;
	};

#endif // CPBK2UIEXTENSIONLOADER_H
//End of file
