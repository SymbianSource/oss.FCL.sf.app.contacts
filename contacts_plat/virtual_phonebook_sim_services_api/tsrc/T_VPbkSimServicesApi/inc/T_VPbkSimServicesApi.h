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
* Description:  STIF test module for testing VirtualPhonebook SimServices API
*
*/



#ifndef T_VPBKSIMSERVICESAPI_H
#define T_VPBKSIMSERVICESAPI_H

#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

// DEFINES
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// FORWARD DECLARATIONS
class MVPbkSimPhone;
class MVPbkSimStateInformation;

NONSHARABLE_CLASS(CT_VPbkSimServicesApi) : 
				public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CT_VPbkSimServicesApi* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CT_VPbkSimServicesApi();

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
    	CT_VPbkSimServicesApi( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        virtual TInt TestSimPhone();
        
        void TestSimPhoneL();
        
        void Delete();
        
        MVPbkSimPhone* iPhone;
        MVPbkSimStateInformation* iSimState;
    };

#endif      // T_VPBKSIMSERVICESAPI_H

// End of File
