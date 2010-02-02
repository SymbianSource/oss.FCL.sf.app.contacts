/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        ?Description
*
*/









// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "MTRPbkViewResourceFile.h"
#include <RPbkViewResourceFile.h>
#include <coemain.h>

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMTRPbkViewResourceFile::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTRPbkViewResourceFile::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTRPbkViewResourceFile::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTRPbkViewResourceFile::RunMethodL( 
    CStifItemParser& aItem ) 
    {

     TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Open_Resource_file", CMTRPbkViewResourceFile::RunLoadResourceTestL),
        ENTRY( "Size_of_Class", CMTRPbkViewResourceFile::RunTest_ClassSizeL),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

//----------------------------------------------------------------------------
void CMTRPbkViewResourceFile::SetupL(  )
    {
    iViewResourceFile = new RPbkViewResourceFile(*CCoeEnv::Static());
    }


//----------------------------------------------------------------------------
void CMTRPbkViewResourceFile::Teardown(  )
    {
    delete iViewResourceFile;
    iViewResourceFile = NULL;
    }


//----------------------------------------------------------------------------
TInt CMTRPbkViewResourceFile::RunLoadResourceTestL()
    {
	SetupL();
    TRAPD(error1, iViewResourceFile->OpenL());
    ASSERT(!error1);

    ASSERT(iViewResourceFile->IsOpen());

    // Resource file already open, must leave
    TRAPD(error2, iViewResourceFile->OpenL());
    ASSERT(error2);

    ASSERT(iViewResourceFile->IsOpen());

    iViewResourceFile->Close();
    ASSERT(!(iViewResourceFile->IsOpen()));
	Teardown();
	return KErrNone;
    }


TInt CMTRPbkViewResourceFile::RunTest_ClassSizeL()
    {
    SetupL();
    static const TInt sizeof_class_RPbkViewResourceFile_30 = 12;
    TInt size = sizeof(class RPbkViewResourceFile);
    ASSERT( size >= sizeof_class_RPbkViewResourceFile_30 );
    Teardown();
	return KErrNone;
    }

//  [End of File] - Do not remove
