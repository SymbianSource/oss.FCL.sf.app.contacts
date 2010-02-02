/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Remote Contact Lookup test suite header file
*
*/



#ifndef RCL_UTILS_TESTSUITE_H
#define RCL_UTILS_TESTSUITE_H

//  SYSTEM INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// USER INCLUDES
#include "rclutilstestsuitedefs.h"

// CLASS DECLARATION

/**
*  CRclUtilsTestSuite test class for STIF Test Framework TestScripter.
*
*  @since S60 v5.2
*/

NONSHARABLE_CLASS(CRclUtilsTestSuite) : public CScriptBase
    {
public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CRclUtilsTestSuite* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CRclUtilsTestSuite();

public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * 
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
        
        TInt TestDefaultAccountIdForNullL();
        
        TInt TestDefaultAccountIdL();
        
        TInt TestGetAccountForNullL();
        
        TInt TestGetAccountL();
        
        TInt TestGetAllAccountsL();
        
        TInt TestGetSpecificAccountsL();

private:

        /**
        * C++ default constructor.
        */
        CRclUtilsTestSuite( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * 
        */
        void Delete();
    
    
      
private:  


    private:    // Data
    	
};

#endif      // RCL_UTILS_TESTSUITE_H

// End of File
