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
*       Composite AppUi extension.
*
*/


#ifndef __CPbkMultiAppUiExtension_H__
#define __CPbkMultiAppUiExtension_H__

// INCLUDES
#include <e32base.h>
#include <Mpbkappuiextension.h>

// FORWARD DECLARATIONS
class CDigViewGraph;
class MPbkExtensionStartupObserver;

/**
 * Composite of AppUi extensions.	
 */
class CPbkMultiAppUiExtension : public CBase,
                                public MPbkAppUiExtension,
                                public MPbkExtensionStartupObserver
    {
    public:
        /**
         * Creates new composite AppUI extension object with empty
         * set of contained AppUIs.
         */
        static CPbkMultiAppUiExtension* NewL();
        
        /**
         * Appends an extension AppUI to this composite AppUI.
         * Ownership is transferred to this object.
         *
         * @param aAppUi AppUI to append. 
         */
        void AppendL(MPbkAppUiExtension* aAppUi);

        /**
         * Destructor.	
         */
        ~CPbkMultiAppUiExtension();
    public: // From MPbkAppUiExtension
        /**
         * Modifies already created view graph.
         *
         * @param aViewGraph view graph to be modified
         */
        void ApplyExtensionViewGraphChangesL(CDigViewGraph& aViewGraph);

        /**
         * Creates a Phonebook startup extension.
         * @param aObserver the startup observer
         * @param aEngine Phonebook contact engine
         */
        void ExtensionStartupL
            (MPbkExtensionStartupObserver& aObserver,
            CPbkContactEngine& aEngine);

        /**
         * Returns the startupstatus of the application UI extensions.
         * @return The startupstatus of the application UI extensions.
         */
        TStartupStatus StartupStatus() const;

    public: // From MPbkExtensionStartupObserver
        /**
         * Called when the startup completes succesfully
         */
        void HandleStartupComplete();

        /**
         * Called if there is an error during startup.
         */
        void HandleStartupFailedL(TInt aError);

    private: // From MPbkAppUiExtension
        void DoRelease();

    private: // Implementation
        CPbkMultiAppUiExtension();
    private: // Data
        /// Array of AppUi extensions
        CArrayPtrFlat<MPbkAppUiExtension> iAppUis;
        /// Ref: Startup observer from client.
        MPbkExtensionStartupObserver* iStartupObserver;
        /// Number of extensions that have succeeded in startup
        TInt iStartedExtensions;
        /// Number of extensions that have failed in startup
        TInt iFailedExtensions;
        /// Startup status of AppUi extensions
        TStartupStatus iStartupStatus;
    };



#endif // __CPbkMultiAppUiExtension_H__

// End of File
