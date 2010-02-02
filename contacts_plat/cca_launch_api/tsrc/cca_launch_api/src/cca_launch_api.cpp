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


// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "cca_launch_api.h"
#include <s32mem.h>

//CCACLIENT
#include "mccaparameter.h"
#include "ccafactory.h"
#include "mccaconnection.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCALaunchAPITester::CCALaunchAPITester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCALaunchAPITester::CCALaunchAPITester(CTestModuleIf& aTestModuleIf) :
    CScriptBase(aTestModuleIf)
    {
    }

// -----------------------------------------------------------------------------
// CCALaunchAPITester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCALaunchAPITester::ConstructL()
    {
    iLog = CStifLogger::NewL(KCCALaunchAPITesterLogPath,
            KCCALaunchAPITesterLogFile, CStifLogger::ETxt,
            CStifLogger::EFile, EFalse, ETrue);
    }

// -----------------------------------------------------------------------------
// CCALaunchAPITester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCALaunchAPITester* CCALaunchAPITester::NewL(CTestModuleIf& aTestModuleIf)
    {
    CCALaunchAPITester* self = new (ELeave) CCALaunchAPITester( aTestModuleIf );

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CCALaunchAPITester::~CCALaunchAPITester()
    {
    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL(
        CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework

    {
    return ( CScriptBase* ) CCALaunchAPITester::NewL( aTestModuleIf );
    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
        TDllReason /*aReason*/) // Reason code

    {
    return(KErrNone);

    }
#endif // EKA2
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCALaunchAPITester::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CCALaunchAPITester::Delete()
    {
    }

// -----------------------------------------------------------------------------
// CCALaunchAPITester::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCALaunchAPITester::RunMethodL(CStifItemParser& aItem)
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
                // First string is the function name used in TestScripter script file.
                // Second is the actual implementation member function. 
                ENTRY( "RunTests", CCALaunchAPITester::RunTestsL )
        };

    const TInt count = sizeof(KFunctions ) / sizeof(TStifFunctionInfo);

    return RunInternalL(KFunctions, count, aItem);

    }

// -----------------------------------------------------------------------------
// CCALaunchAPITester::RunTests
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCALaunchAPITester::RunTestsL(CStifItemParser& /*aItem*/)
    {
    TInt iErrorStatus = KErrNone;
    // Print to UI
    _LIT( KCCALaunchAPITester, "CCALaunchAPITester" );
    _LIT( KLaunchingApplication, "In RunTests" );
    TestModuleIf().Printf( 0, KCCALaunchAPITester, KLaunchingApplication);
    // Print to log file
    iLog->Log(KLaunchingApplication);

    MCCAParameter* parameter = TCCAFactory::NewParameterL();
    CleanupClosePushL( *parameter );
    TAL( parameter );
    MCCAConnection* connection = TCCAFactory::NewConnectionL();
    CleanupClosePushL( *connection );
    TAL( connection );

    iLog->Log(_L("Starting testing:"));
    parameter->SetConnectionFlag(MCCAParameter::ENormal);
    parameter->SetContactDataFlag(MCCAParameter::EContactId);
    parameter->SetContactDataL(_L("blaah"));
    parameter->SetLaunchedViewUid(TUid::Uid(0x00000));

    if (parameter->ConnectionFlag()!=MCCAParameter::ENormal)
        {
        iLog->Log(_L("FAILED: ConnectionFlag()!=MCCAParameter::ENormal"));
        return KErrGeneral;
        }
    if (parameter->ContactDataFlag()!=MCCAParameter::EContactId)
        {
        iLog->Log(_L("FAILED: ContactDataFlag()!=MCCAParameter::EContactId"));
        return KErrGeneral;
        }
    const HBufC* apu = &parameter->ContactDataL();
    if (apu==NULL)
        {
        iLog->Log(_L("FAILED: ContactDataL()==NULL"));
        return KErrGeneral;
        }
    if (parameter->LaunchedViewUid()!=TUid::Uid(0x00000))
        {
        iLog->Log(_L("FAILED: LaunchedViewUid()!=TUid::Uid(0x00000)"));
        return KErrGeneral;
        }
    if (parameter->Version()!=KErrNotFound)
        {
        iLog->Log(_L("FAILED: Version()!=KErrNotFound"));
        return KErrGeneral;
        }

    CleanupStack::PopAndDestroy( 2 ); //parameter, connection    

    iLog->Log(_L("Starting testing: Done."));

    return iErrorStatus;
    }

//  End of File
