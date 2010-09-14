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
#include "BCTestPbkVcardImportExport.h"

#include <BCardEng.h>
#include <CPbkContactItem.h>
#include "PbkFields.hrh"


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
// CBCTestPbkVcardImportExport::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CBCTestPbkVcardImportExport::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CBCTestPbkVcardImportExport::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CBCTestPbkVcardImportExport::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CBCTestPbkVcardImportExport::ExampleL ),
        ENTRY( "TestNewL", CBCTestPbkVcardImportExport::TestNewL ),
        ENTRY( "TestImportVCardWithMemoryL", CBCTestPbkVcardImportExport::TestImportVCardWithMemoryL ),
        ENTRY( "TestEmptyStreamL", CBCTestPbkVcardImportExport::TestEmptyStreamL ),
        ENTRY( "TestImportPicVCardL", CBCTestPbkVcardImportExport::TestImportPicVCardL ),
        ENTRY( "TestImportCCardWithMemoryL", CBCTestPbkVcardImportExport::TestImportCCardWithMemoryL ),
        ENTRY( "TestImportCCardPhoneNbrsL", CBCTestPbkVcardImportExport::TestImportCCardPhoneNbrsL ),
        ENTRY( "TestMaxSizeCompactCardL", CBCTestPbkVcardImportExport::TestMaxSizeCompactCardL ),
        ENTRY( "TestNoBusinessCompactCardL", CBCTestPbkVcardImportExport::TestNoBusinessCompactCardL ),
        ENTRY( "TestSupportsFieldTypeL", CBCTestPbkVcardImportExport::TestSupportsFieldTypeL ),
        ENTRY( "TestExportCardL", CBCTestPbkVcardImportExport::TestExportCardL ),
        ENTRY( "TestExportCard2L", CBCTestPbkVcardImportExport::TestExportCard2L ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CBCTestPbkVcardImportExport::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CBCTestPbkVcardImportExport::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KBCTestPbkVcardImportExport, "BCTestPbkVcardImportExport" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KBCTestPbkVcardImportExport, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KBCTestPbkVcardImportExport, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

TInt CBCTestPbkVcardImportExport::TestNewL( CStifItemParser& /*aItem*/ )
    {

     CBCardEngine*	testBCEngine;
     testBCEngine = CBCardEngine::NewL(iContactEngine);

		if(testBCEngine)
			{ 
					delete testBCEngine;
					testBCEngine = NULL;
			}
			
    return KErrNone;

    }
    
  TInt CBCTestPbkVcardImportExport::TestImportVCardWithMemoryL( CStifItemParser& /*aItem*/ )
  {

   CBCardEngine*	testBCEngine;
   testBCEngine = CBCardEngine::NewL(iContactEngine);
   RFs	iFs;
   
   
   	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iReadStream.Open(iFs,KCardBasic03, EFileRead));
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    TRAPD(err, testBCEngine->ImportBusinessCardL(*iContactItem, iReadStream));
    
    if(testBCEngine)
				{ 
					delete testBCEngine;
					testBCEngine = NULL;
				}	
		
		iFs.Close();
			
    return err;
		 	
  }
  
  TInt CBCTestPbkVcardImportExport::TestEmptyStreamL( CStifItemParser& /*aItem*/ )
  {

   CBCardEngine*	testBCEngine;
   testBCEngine = CBCardEngine::NewL(iContactEngine);
   RFs	iFs;
   
   
   	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iReadStream.Open(iFs,KCardDummy, EFileRead));
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    TRAPD(err, testBCEngine->ImportBusinessCardL(*iContactItem, iReadStream));
    
    if(testBCEngine)
				{ 
					delete testBCEngine;
					testBCEngine = NULL;
				}	
		iFs.Close();
				
    return err;

  }
  
 
 TInt CBCTestPbkVcardImportExport::TestImportPicVCardL( CStifItemParser& /*aItem*/ )
  {

   CBCardEngine*	testBCEngine;
   testBCEngine = CBCardEngine::NewL(iContactEngine);
   RFs	iFs;
   
   
   	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iReadStream.Open(iFs,KCardPicture2, EFileRead));
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    TRAPD(err, testBCEngine->ImportBusinessCardL(*iContactItem, iReadStream));
    
   	if(testBCEngine)
				{ 
					delete testBCEngine;
					testBCEngine = NULL;
				}	
		iFs.Close();
				
    return err;

  }
  
  TInt CBCTestPbkVcardImportExport::TestImportCCardWithMemoryL( CStifItemParser& /*aItem*/ )
  {

   CBCardEngine*	testBCEngine;
   testBCEngine = CBCardEngine::NewL(iContactEngine);
   RFs	iFs;
   
   
   	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iReadStream.Open(iFs,KCardBasicCC01, EFileRead));
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    TRAPD(err, testBCEngine->ImportCompactBusinessCardL(*iContactItem, iReadStream));
    
    if(testBCEngine)
				{ 
					delete testBCEngine;
					testBCEngine = NULL;
				}	
		
		iFs.Close();
				
    return err;
	
  }
  
  TInt CBCTestPbkVcardImportExport::TestImportCCardPhoneNbrsL( CStifItemParser& /*aItem*/ )
  {

   CBCardEngine*	testBCEngine;
   testBCEngine = CBCardEngine::NewL(iContactEngine);
   RFs	iFs;
   
   
   	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iReadStream.Open(iFs,KCardBasicCC02, EFileRead));
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    TRAPD(err, testBCEngine->ImportCompactBusinessCardL(*iContactItem, iReadStream));
    
    if(testBCEngine)
				{ 
					delete testBCEngine;
					testBCEngine = NULL;
				}	
		iFs.Close();
				
    return err;
	
  }
  
  TInt CBCTestPbkVcardImportExport::TestMaxSizeCompactCardL( CStifItemParser& /*aItem*/ )
  {

   CBCardEngine*	testBCEngine;
   testBCEngine = CBCardEngine::NewL(iContactEngine);
   RFs	iFs;
   
   
   	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iReadStream.Open(iFs,KCardBasicMaxSize, EFileRead));
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    TRAPD(err, testBCEngine->ImportCompactBusinessCardL(*iContactItem, iReadStream));
    
    if(testBCEngine)
				{ 
					delete testBCEngine;
					testBCEngine = NULL;
				}	
		iFs.Close();
				
    return err;
	
  }
  
  TInt CBCTestPbkVcardImportExport::TestNoBusinessCompactCardL( CStifItemParser& /*aItem*/ )
  {

   CBCardEngine*	testBCEngine;
   testBCEngine = CBCardEngine::NewL(iContactEngine);
   RFs	iFs;
   
   
   	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iReadStream.Open(iFs,KCardBasicNoBusiness, EFileRead));
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    TRAPD(err, testBCEngine->ImportCompactBusinessCardL(*iContactItem, iReadStream));
    
    if(testBCEngine)
				{ 
					delete testBCEngine;
					testBCEngine = NULL;
				}	
		iFs.Close();
				
    return err;
  }
  
  TInt CBCTestPbkVcardImportExport::TestSupportsFieldTypeL( CStifItemParser& /*aItem*/ )
  {

   CBCardEngine*	testBCEngine;
   testBCEngine = CBCardEngine::NewL(iContactEngine);
   
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdLastName));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdFirstName));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPhoneNumberGeneral));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPhoneNumberStandard));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPhoneNumberHome));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPhoneNumberWork));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPhoneNumberMobile));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdFaxNumber));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPagerNumber));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdEmailAddress));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPostalAddress));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdURL));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdJobTitle));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdCompanyName));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdCompanyAddress));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdDate));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdNote));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdThumbnailImage));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPOBox));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdExtendedAddress));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdStreetAddress));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPostalCode));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdCity));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdState));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdCountry));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdSecondName));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPhoneNumberVideo));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdVOIP));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdShareView));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdSIPID));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdPrefix));
   ASSERT(testBCEngine->SupportsFieldType(EPbkFieldIdSuffix));
   
   if(testBCEngine)
				{ 
					delete testBCEngine;
					testBCEngine = NULL;
				}	
				
    
	 return KErrNone;
  }
  
TInt CBCTestPbkVcardImportExport::TestExportCardL( CStifItemParser& /*aItem*/ )
  {

   CBCardEngine*	testBCEngine;
   testBCEngine = CBCardEngine::NewL(iContactEngine);
   RFs	iFs;
   
   
   	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iWriteStream.Replace(iFs,KCardOut01, EFileRead));
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    // actually export the data:    
    TRAPD(err,testBCEngine->ExportBusinessCardL(iWriteStream, *iContactItem));
    
    if(testBCEngine)
				{ 
					delete testBCEngine;
					testBCEngine = NULL;
				}	
		iFs.Close();
				
    return err;
  }
  
  TInt CBCTestPbkVcardImportExport::TestExportCard2L( CStifItemParser& /*aItem*/ )
  {

   CBCardEngine*	testBCEngine;
   testBCEngine = CBCardEngine::NewL(iContactEngine);
   RFs	iFs;
   
   
   	User::LeaveIfError(iFs.Connect());
    User::LeaveIfError(iWriteStream.Replace(iFs,KCardOut02, EFileRead));
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    
    // actually export the data:    
    TRAPD(err,testBCEngine->ExportBusinessCardL(iWriteStream, *iContactItem));
    
    if(testBCEngine)
				{ 
					delete testBCEngine;
					testBCEngine = NULL;
				}	
		iFs.Close();
				
    return err;
  }
// -----------------------------------------------------------------------------
// CBCTestPbkVcardImportExport::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CBCTestPbkVcardImportExport::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
