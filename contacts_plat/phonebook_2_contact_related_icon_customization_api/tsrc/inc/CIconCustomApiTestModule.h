/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef CICONCUSTOMAPITESTMODULE_H
#define CICONCUSTOMAPITESTMODULE_H

//  INCLUDES
#include <fbs.h>

#include <StifLogger.h>
#include <StifTestModule.h>
#include <TestScripterInternal.h>
#include <MCustomIconChangeObserver.h>

// FORWARD DECLARATIONS
class CContactCustomIconPluginBase;
class CCustomIconIdMap;
class CGulIcon;

// CONSTANTS
// Logging
_LIT( KTestModuleLogPath, "\\logs\\testframework\\" );
_LIT( KTestModuleLogFile, "IconCustomizationApiTest.log" );


// CLASS DECLARATION

/**
*  CIconCustomApiTestModule test class for STIF Test Framework TestScripter.
*
*  @lib ?library
*  @since ?Series60_version
*/
class CIconCustomApiTestModule : public CScriptBase,
                                 public MCustomIconChangeObserver
    {
    public:  // Public construction and destruction

        /**
        * Two-phased constructor.
        */
        static CIconCustomApiTestModule* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CIconCustomApiTestModule();

    public: // Functions from base CScriptBase

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    public: // Functions from MCustomIconChangeObserver

        /**
         * Icon change notification handler
         */
        void CustomIconEvent( CContactCustomIconPluginBase& aPlugin,
            TEventType aEventType,
            const TArray<TCustomIconId>& aIconIds );

    private: // Private construction

        /**
        * C++ default constructor.
        */
        CIconCustomApiTestModule( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Test methods

        TInt LoadTestPluginL( CStifItemParser& aItem );
        TInt LoadIconsL( CStifItemParser& aItem );
        TInt LoadIconL( CStifItemParser& aItem );
        TInt UpdateIconsWithLastIconIdsL( CStifItemParser& aItem );
        TInt DeleteIconsL( CStifItemParser& aItem );
        TInt DeleteIconL( CStifItemParser& aItem );
        TInt IconChoiceL( CStifItemParser& aItem );
        TInt SetPsProperty( CStifItemParser& aItem );
        TInt DeletePsProperty( CStifItemParser& aItem );
        TInt SetTestCaseAsObserverL( CStifItemParser& aItem );
        TInt CheckIfHasExtensionL( CStifItemParser& aItem );

    private: // Helpers
        void CleanupPsKeys();

    private: // Members
        CContactCustomIconPluginBase* iTestPlugin;
        CCustomIconIdMap* iIcons;
        CGulIcon* iIcon;
        RFbsSession iFbsSession; // For CFbsBitmap loading/handling support in the plugin        
        RArray<TInt> iPublishedPsKeys;
        MCustomIconChangeObserver::TEventType iLastEventType;
        RArray<TCustomIconId> iLastIconIds;
    };

#endif // CICONCUSTOMAPITESTMODULE_H

// End of File
