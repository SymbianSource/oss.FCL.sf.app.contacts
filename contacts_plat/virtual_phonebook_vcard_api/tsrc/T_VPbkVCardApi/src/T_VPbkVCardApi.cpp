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
* Description:  STIF test module for testing VirtualPhonebook Vcard API
*
*/



// INCLUDE FILES
#include "T_VPbkVCardApi.h"
#include <MVPbkContactOperationBase.h>

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::CT_VPbkVCardApi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CT_VPbkVCardApi::CT_VPbkVCardApi( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    iEikEnv = NULL;
    User::LeaveIfError( iFs.Connect() );
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CT_VPbkVCardApi::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CT_VPbkVCardApi* CT_VPbkVCardApi::NewL( 
		CTestModuleIf& aTestModuleIf )
    {
    CT_VPbkVCardApi* self = new (ELeave) CT_VPbkVCardApi( aTestModuleIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
//
CT_VPbkVCardApi::~CT_VPbkVCardApi()
    {
    Delete();
    }

//-----------------------------------------------------------------------------
// CT_VPbkVCardApi::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CT_VPbkVCardApi::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("T_VPbkVCardApi.dll");

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
    return ( CScriptBase* ) CT_VPbkVCardApi::NewL( aTestModuleIf );
    }

//  End of File
