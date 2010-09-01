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
*    Abstract interface for phone book app ui extensions.
*
*
*/


#ifndef __MPbkAppUiExtension_H__
#define __MPbkAppUiExtension_H__


//  INCLUDES
#include <MPbkUiExtensionBase.h>


//  FORWARD DECLARATIONS
class CDigViewGraph;
class MPbkStartupExtension;
class MPbkStartupObserver;
class CPbkContactEngine;


// CLASS DECLARATION
/**
 * Startup observer
 */
class MPbkExtensionStartupObserver
    {
    public: // interface
        /**
         * Called when the startup completes succesfully
         */
        virtual void HandleStartupComplete() =0;

        /**
         * Called if there is an error during startup.
         */
        virtual void HandleStartupFailedL(TInt aError) =0;

    };


/**
 * Abstract interface for phone book app ui extensions.
 */
class MPbkAppUiExtension : public MPbkUiExtensionBase
    {
    public: // enumerations
        enum TStartupStatus
            {
            EStartupNotStarted,
            EStartupInProgress,
            EStartupFailed,
            EStartupCompleted
            };
    public:
        /**
         * Modifies already created view graph.
         *
         * @param aViewGraph view graph to be modified
         */
        virtual void ApplyExtensionViewGraphChangesL
            (CDigViewGraph& aViewGraph) =0;

        /**
         * Creates a Phonebook startup extension. This is called only 
         * when Phonebook is started normally, not for example when  
         * a Phonebook view is activated from another application.
         * @param aObserver the startup observer
         * @param aEngine Phonebook contact engine
         */
        virtual void ExtensionStartupL
            (MPbkExtensionStartupObserver& aObserver,
            CPbkContactEngine& aEngine) =0;

        /**
         * Returns the startupstatus of the application UI extensions.
         * Note: This should not be implemented by extensions as this is not
         * called by Extension Manager. The Extension Manager decides the 
         * startup status itself.
         * @return The startupstatus of the application UI extensions.
         */
        virtual TStartupStatus StartupStatus() const
            { return EStartupNotStarted; }
    };

#endif // __MPbkAppUiExtension_H__

// End of File
