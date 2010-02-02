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
 *
*/


#ifndef CCALAUNCHAPITESTER_H
#define CCALAUNCHAPITESTER_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

// Logging path
_LIT( KCCALaunchAPITesterLogPath, "c:\\logs\\cca\\unittest\\" );
// Log file
_LIT( KCCALaunchAPITesterLogFile, "CCALaunchAPITester.txt" );

// CLASS DECLARATION

/**
 *  CCALaunchAPITester test class for STIF Test Framework TestScripter.
 *
 *  @lib cca_launch_api.lib
 *  @since 5.0
 */
NONSHARABLE_CLASS(CCALaunchAPITester) : public CScriptBase
    {
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCALaunchAPITester* NewL( CTestModuleIf& aTestModuleIf );

    /**
     * Destructor.
     */
    virtual ~CCALaunchAPITester();

public: // Functions from base classes

    /**
     * From CScriptBase Runs a script line.
     * @since 5.0
     * @param aItem Script line containing method name and parameters
     * @return Symbian OS error code
     */
    virtual TInt RunMethodL( CStifItemParser& aItem );

private:

    /**
     * C++ default constructor.
     */
    CCALaunchAPITester( CTestModuleIf& aTestModuleIf );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

    /**
     * Frees all resources allocated from test methods.
     * @since 5.0
     */
    void Delete();

    /**
     * Test methods are listed below. 
     */

    /**
     * Basic API tests
     * @since 5.0
     * @param aItem Script line containing parameters.
     * @return Symbian OS error code.
     */
    virtual TInt RunTestsL( CStifItemParser& aItem );

private: // Data

    CStifLogger * iLog;
    CActiveScheduler* iScheduler;

    };

#endif      // CCALAUNCHAPITESTER_H
// End of File
