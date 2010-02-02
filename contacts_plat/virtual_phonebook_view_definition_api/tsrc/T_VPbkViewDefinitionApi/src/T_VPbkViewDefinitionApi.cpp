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
* Description:  STIF test module for testing VirtualPhonebook View Definition API
*
*/



// INCLUDE FILES
#include "T_VPbkViewDefinitionApi.h"

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::CT_VPbkViewDefinitionApi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CT_VPbkViewDefinitionApi::CT_VPbkViewDefinitionApi( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CT_VPbkViewDefinitionApi::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CT_VPbkViewDefinitionApi* CT_VPbkViewDefinitionApi::NewL( 
		CTestModuleIf& aTestModuleIf )
    {
    CT_VPbkViewDefinitionApi* self = new (ELeave) CT_VPbkViewDefinitionApi( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
//
CT_VPbkViewDefinitionApi::~CT_VPbkViewDefinitionApi()
    {
    Delete();
    }

//-----------------------------------------------------------------------------
// CT_VPbkViewDefinitionApi::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CT_VPbkViewDefinitionApi::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("T_VPbkViewDefinitionApi.dll");

	TestModuleIf().SendTestModuleVersion( moduleVersion, moduleName );
	}

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {
    return ( CScriptBase* ) CT_VPbkViewDefinitionApi::NewL( aTestModuleIf );
    }

//  End of File
