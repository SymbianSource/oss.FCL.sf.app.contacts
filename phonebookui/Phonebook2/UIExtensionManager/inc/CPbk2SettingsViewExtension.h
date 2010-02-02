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
* Description:  Phonebook 2 settings view composite UI extension.
*
*/


#ifndef CPBK2SETTINGSVIEWEXTENSION_H
#define CPBK2SETTINGSVIEWEXTENSION_H

// INCLUDES
#include <e32base.h>
#include <MPbk2SettingsViewExtension.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionLoader;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * Phonebook 2 settings view composite UI extension.
 * Responsible for managing real settings UI extensions and
 * delegating calls from core Phonebook 2 to them.
 */
NONSHARABLE_CLASS(CPbk2SettingsViewExtension) : 
        public CBase,
        public MPbk2SettingsViewExtension
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager       Virtual Phonebook contact manager.
         * @param aExtensionLoader      UI extension loader.
         * @return  A new instance of this class.
         */
        static CPbk2SettingsViewExtension* NewL(
                CVPbkContactManager& aContactManager,
                CPbk2UIExtensionLoader& aExtensionLoader);
        
        /**
         * Destructor.
         */
        ~CPbk2SettingsViewExtension();
       
    public: // From MPbk2SettingsViewExtension
        void DoRelease();
        MPbk2SettingsControlExtension*
            CreatePbk2SettingsControlExtensionL();
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane );
        TBool HandleCommandL(
                TInt aCommandId );
    
    private: // Implementation
        CPbk2SettingsViewExtension(
                CVPbkContactManager& aContactManager,
                CPbk2UIExtensionLoader& aExtensionLoader );
        void ConstructL();
        void LoadExtensionsL();
                        
    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Extension loader
        CPbk2UIExtensionLoader& iExtensionLoader;
        /// Own: An array for setting extensions
        RPointerArray<MPbk2SettingsViewExtension> iSettingExtensions; 
    };

#endif // CPBK2SETTINGSVIEWEXTENSION_H
            
// End of File
