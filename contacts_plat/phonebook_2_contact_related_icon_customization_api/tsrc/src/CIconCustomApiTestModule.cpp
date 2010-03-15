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

// INCLUDE FILES
#include <ecom\ecom.h>
#include <StifTestInterface.h>
#include <CContactCustomIconPluginBase.h>
#include <CCustomIconIdMap.h>
#include "CIconCustomApiTestModule.h"

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::CIconCustomApiTestModule
// ----------------------------------------------------------------------------
//
CIconCustomApiTestModule::CIconCustomApiTestModule
        ( CTestModuleIf& aTestModuleIf ):
            CScriptBase( aTestModuleIf )
    {
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::ConstructL
// ----------------------------------------------------------------------------
//
void CIconCustomApiTestModule::ConstructL()
    {
    User::LeaveIfError( iFbsSession.Connect() );    
    iLog = CStifLogger::NewL( KTestModuleLogPath,
                          KTestModuleLogFile,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CIconCustomApiTestModule* CIconCustomApiTestModule::NewL(
    CTestModuleIf& aTestModuleIf )
    {
    CIconCustomApiTestModule* self =
        new (ELeave) CIconCustomApiTestModule( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------------------------------
// CIconCustomApiTestModule::~CIconCustomApiTestModule
// ----------------------------------------------------------------------------
//
CIconCustomApiTestModule::~CIconCustomApiTestModule()
    {
    delete iTestPlugin;
    delete iIcons;
    delete iIcon;
    delete iLog;

    CleanupPsKeys();

    iPublishedPsKeys.Close();
    iLastIconIds.Close();

    iFbsSession.Disconnect();
    
    REComSession::FinalClose(); // Just in case to unload plug-ins and free
                                // ECom stuff
    }

// ========================== OTHER EXPORTED FUNCTIONS ========================

// ----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
//          Backpointer to STIF Test Framework
// ----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( CTestModuleIf& aTestModuleIf )
    {
    return ( CScriptBase* ) CIconCustomApiTestModule::NewL( aTestModuleIf );
    }

//  End of File

