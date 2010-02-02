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
* Description:  Phonebook 2 USIM Thin UI extension plugin.
*
*/


#ifndef CPSU2THINUIEXTENSIONPLUGIN_H
#define CPSU2THINUIEXTENSIONPLUGIN_H

// INCLUDE FILES
#include "TPsu2ServiceTable.h"
#include <CPbk2UIExtensionThinPlugin.h>
#include <MVPbkSimPhoneObserver.h>
#include <MPbk2StartupObserver.h>
#include <MPbk2CommandObserver.h>

// FORWARD DECLARATIONS
class CEikMenuPane;
class MPbk2ContactUiControl;
class MVPbkSimPhone;
class CPsu2CheckAvailabeSimStore;

/**
 * Phonebook 2 USIM Thin UI extension plugin.
 */
class CPsu2ThinUIExtensionPlugin : public CPbk2UIExtensionThinPlugin,
                                   private MVPbkSimPhoneObserver,
                                   private MPbk2StartupObserver,
                                   private MPbk2CommandObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPsu2ThinUIExtensionPlugin* NewL();

        /**
         * Destructor.
         */
        ~CPsu2ThinUIExtensionPlugin();

    public: // From CPbk2UIExtensionThinPlugin
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane,
                MPbk2ContactUiControl& aControl );
        void ExtensionStartupL(
                MPbk2StartupMonitor& aStartupMonitor );

    private: // From MVPbkSimPhoneObserver
        void PhoneOpened(
                MVPbkSimPhone& aPhone );
        void PhoneError(
                MVPbkSimPhone& aPhone,
                TErrorIdentifier aIdentifier,
                TInt aError );
        void ServiceTableUpdated(
                TUint32 aServiceTable );
        void FixedDiallingStatusChanged(
                TInt aFDNStatus );

    private: // From MPbk2StartupObserver
        void ContactUiReadyL(
                MPbk2StartupMonitor& aStartupMonitor );

    private: // From MPbk2CommandObserver
        void CommandFinished(
                const MPbk2Command& aCommand );

    private: // Implementation
        CPsu2ThinUIExtensionPlugin();
        void ConstructL();

    private: // Data
        /// Own: Phone connection
        MVPbkSimPhone* iPhone;
        /// Own: Service table wrapper
        TPsu2ServiceTable iServiceTable;
        /// Own: Command that copies new SIM contacts to phone
        MPbk2Command* iCopySimContactsCmd;
        /// Own:
        CPsu2CheckAvailabeSimStore* iStoreChecker;
    };

#endif // CPSU2THINUIEXTENSIONPLUGIN_H

// End of File
