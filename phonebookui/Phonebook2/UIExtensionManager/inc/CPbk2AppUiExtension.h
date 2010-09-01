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
* Description:  An application UI extension implementation
*
*/



#ifndef CPBK2APPUIEXTENSION_H
#define CPBK2APPUIEXTENSION_H

//  INCLUDES
#include <e32base.h>
#include <MPbk2AppUiExtension.h>
#include <MPbk2StartupMonitor.h>
#include <MPbk2StartupObserver.h>

// FORWARD DECLARATIONS
class CPbk2UIExtensionLoader;
class CPbk2ThinUIExtensionLoader;
class CVPbkContactManager;

// CLASS DECLARATION

/**
 * An application UI extension implementation.
 */
NONSHARABLE_CLASS(CPbk2AppUiExtension) : 
        public CBase,
        public MPbk2AppUiExtension,
        public MPbk2StartupMonitor,
        private MPbk2StartupObserver
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         *
         * @param aContactManager the Virtual Phonebook Contact Manager
  		 * @param aExtensionLoader a UI extension loader for calling extension
		 *        start-up point
		 * @param aThinExtensionLoader a thin extension loader for calling thin 
		 *        extension start-up point.
		 * @return a new instance of this class
         */
        static CPbk2AppUiExtension* NewL(
                CVPbkContactManager& aContactManager,
                CPbk2UIExtensionLoader& aExtensionLoader,
                CPbk2ThinUIExtensionLoader& aThinExtensionLoader);
        
        /**
         * Destructor.
         */
        virtual ~CPbk2AppUiExtension();

    public: // Functions from MPbk2AppUiExtension
        void ExtensionStartupL(MPbk2StartupMonitor& aStartupMonitor);
        void ApplyDynamicPluginTabGroupContainerChangesL(
                const CPbk2ViewNode& aNode, 
                CAknTabGroup& aAknTabGroup,
                CPbk2IconInfoContainer& aTabIcons,
                TInt aViewCount);
        
    public: // Functions from MPbk2UiReleasable
        void DoRelease();
    
    public: // Functions from MPbk2StartupMonitor
        void HandleStartupComplete();
        void HandleStartupFailed( TInt aError );
        void RegisterEventsL( MPbk2StartupObserver& aObserver );
		void DeregisterEvents( MPbk2StartupObserver& aObserver );
        void NotifyViewActivationL( TUid aViewId );
        void NotifyViewActivationL(
                TUid aViewId, MVPbkContactViewBase& aContactView );
    
    private:    // Functions from MPbk2StartupObserver
        void ContactUiReadyL( 
                MPbk2StartupMonitor& aStartupMonitor );
            
    private:    // New functions
        void StopObservingMonitor();
        TBool AllExtensionsCompleted() const;
        TInt NumOfExtensions() const;
        
    private: // Construction
        CPbk2AppUiExtension(
                CVPbkContactManager& aContactManager,
                CPbk2UIExtensionLoader& aExtensionLoader,
                CPbk2ThinUIExtensionLoader& aThinExtensionLoader);
        void ConstructL();
    
    private:    // Data        
        /// Ref: a UI extension loader for calling extension start-up point
        CPbk2UIExtensionLoader& iExtensionLoader;
        /// Ref: a thin extension loader for calling thin extension start-up point.
        CPbk2ThinUIExtensionLoader& iThinExtensionLoader;
        /// Ref: The phonebook2 contact manager
        CVPbkContactManager& iContactManager;
        /// Own: An array for extensions
        RPointerArray<MPbk2AppUiExtension> iExtensions;
        /// Ref: the actual start-up monitor
        MPbk2StartupMonitor* iStartupMonitor;
        /// Own: The number of extensions that have notified start-up complete
        TInt iNumOfCompletedExtensions;
        /// Own: The number of extensions that have notified start-up failed
        TInt iNumOfFailedExtensions;
        /// Own: An array of start-up observers
        RPointerArray<MPbk2StartupObserver> iStartupObservers;
        /// Own: the state of the core pbk2 UI.
        TBool iContactUiReady;
    };

#endif      // CPBK2APPUIEXTENSION_H
            
// End of File
