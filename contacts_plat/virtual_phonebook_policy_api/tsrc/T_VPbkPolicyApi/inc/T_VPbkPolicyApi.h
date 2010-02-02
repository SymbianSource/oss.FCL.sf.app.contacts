/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  STIF test module for testing VirtualPhonebook Policy API
*
*/



#ifndef T_VPBKPOLICYAPI_H
#define T_VPBKPOLICYAPI_H

#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

// DEFINES
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// FORWARD DECLARATIONS
class CVPbkContactCopyPolicyManager;
class CVPbkContactManager;
class MVPbkContactCopyPolicy;

NONSHARABLE_CLASS(CT_VPbkPolicyApi) : 
				public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_VPbkPolicyApi* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CT_VPbkPolicyApi();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();
        
    private:

        /**
        * C++ default constructor.
        */
    	CT_VPbkPolicyApi( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Tests Virtual Phonebook Policy Api functions
        * @return Symbian OS error code.
        */
        virtual TInt TestPolicyManager();
        
        void TestPolicyManagerL();
        
        void Delete();
        
        CVPbkContactManager* iContactManager;
    };

#endif      // T_VPBKPOLICYAPI_H

// End of File
