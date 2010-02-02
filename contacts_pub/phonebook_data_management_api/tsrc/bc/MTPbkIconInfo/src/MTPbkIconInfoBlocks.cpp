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
#include "MTPbkIconInfo.h"
#include <PbkIconInfo.h>
#include <COEMAIN.H>
#include <barsread.h>
#include <aknsutils.h> 
#include <GULICON.H>


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
// CMTPbkIconInfo::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkIconInfo::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkIconInfo::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkIconInfo::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    
     TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Test_construction_with_CCoeEnv", CMTPbkIconInfo::RunConstructContainerWithCoeEnvTestL ),
        ENTRY( "Test_construction_without_CCoeEnv", CMTPbkIconInfo::RunConstructContainerWithoutCoeEnvTestL ),
        ENTRY( "Test_finding_TPbkIconInfo", CMTPbkIconInfo::RunFindTestL ),
        ENTRY( "Test_loading_bitmap", CMTPbkIconInfo::RunLoadBitmapLTestL ),
        ENTRY( "Test_getting_icon_id", CMTPbkIconInfo::RunPbkIconInfoIdTest ),
        ENTRY( "Test_getting_bitmap_file_name", CMTPbkIconInfo::RunMbmFileNameTest ),
        ENTRY( "Test_getting_icon_id", CMTPbkIconInfo::RunIconIdTest ),
        ENTRY( "Test_getting_icon's_mask_id", CMTPbkIconInfo::RunMaskIdTest ),
        ENTRY( "Test_getting_icon's_skin_id", CMTPbkIconInfo::RunSkinIdTest ),
        ENTRY( "Test_creating_bitmap_and_mask_for_an_icon", CMTPbkIconInfo::RunCreateIconLTestL ),
        ENTRY( "Test_creating_bitmap_and_mask_for_an_icon1", CMTPbkIconInfo::RunCreateIconLCTestL ),
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

//  METHODS

// ----------------------------------------------------------------------------
// CMTPbkIconInfo::OpenResourceFileL()
// Open resource file
// ----------------------------------------------------------------------------
RPbkViewResourceFile* CMTPbkIconInfo::OpenResourceFileL()
    {  
    //open resource file
    RPbkViewResourceFile*  resourceFile = new RPbkViewResourceFile( *CCoeEnv::Static() );
    if( !resourceFile->IsOpen() )
        {
        resourceFile->OpenL();
        }
       
    ASSERT( resourceFile->IsOpen());
    
    return resourceFile;
    }
    
// ----------------------------------------------------------------------------
// CMTPbkIconInfo::DummyMethod()
// Does nothing
// ----------------------------------------------------------------------------
void CMTPbkIconInfo::DummyMethod()
    {  
    }
    
///////////////////////////////////////////////////////////////////////////////
// CPbkIconInfoContainer test methods                                          //   
///////////////////////////////////////////////////////////////////////////////
    
// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunConstructContainerWithCoeEnvTestL()
//  Test "CPbkIconInfoContainer::NewL" with CCoeEnv
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunConstructContainerWithCoeEnvTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL( CCoeEnv::Static() );
    CleanupStack::PushL( container );
    ASSERT( container );
    
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile );
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunConstructContainerWithoutCoeEnvTestL()
//  Test "CPbkIconInfoContainer::NewL" without CCoeEnv
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunConstructContainerWithoutCoeEnvTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL( );
    CleanupStack::PushL( container );
    ASSERT( container );
    
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile );
    return KErrNone;    
    }

// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunFindTestL()
//  Run 'CPbkIconInfoContainer::Find' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunFindTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL();
    CleanupStack::PushL( container );
    
    // get null icon
    const TPbkIconInfo* iconInfo1 = container->Find( EPbkNullIconId );
    ASSERT( !iconInfo1 );
    
    // get real icon
    const TPbkIconInfo* iconInfo2 = container->Find( EPbkqgn_prop_checkbox_off );
    ASSERT( iconInfo2 );
    
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile );
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunLoadBitmapLTestL()
//  Run 'CPbkIconInfoContainer::LoadBitmapL' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunLoadBitmapLTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL();
    CleanupStack::PushL( container );
         
    CGulIcon* bmp1 = container->LoadBitmapL( EPbkNullIconId );
    ASSERT( !bmp1 );
    delete bmp1;
         
    CGulIcon* bmp2 = container->LoadBitmapL( EPbkqgn_prop_checkbox_off );
    ASSERT( bmp2 );
    delete bmp2;
    
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile );
    return KErrNone;    
    }
       
///////////////////////////////////////////////////////////////////////////////
// TPbkIconInfo test methods                                                   //   
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// CMTPbkIconInfo::SetupIconInfoL()
// Create TPbkIconInfo 
// ----------------------------------------------------------------------------
void CMTPbkIconInfo::SetupIconInfoL()
    {  
    RPbkViewResourceFile* resourceFile = OpenResourceFileL(); 
    CleanupStack::PushL( resourceFile );
    
    iContainer = CPbkIconInfoContainer::NewL( );
    
    iIconInfo = iContainer->Find( EPbkqgn_prop_checkbox_off );
    
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile );
    }
        
// ----------------------------------------------------------------------------
// CMTPbkIconInfo::TeardownIconInfo()
// Teardown TPbkIconInfo 
// ----------------------------------------------------------------------------
void CMTPbkIconInfo::TeardownIconInfo()
    {
    iIconInfo = NULL;
    delete iContainer;
    iContainer = NULL;
    }
    
// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunPbkIconInfoIdTest()
//  Run 'TPbkIconInfo::PbkIconInfoId' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunPbkIconInfoIdTest()
    {
    SetupIconInfoL();
    ASSERT( iIconInfo->PbkIconInfoId() == EPbkqgn_prop_checkbox_off );
    TeardownIconInfo();
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunMbmFileNameTest()
//  Run 'TPbkIconInfo::MbmFileName' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunMbmFileNameTest()
    {
    SetupIconInfoL();
    const TDesC& name = iIconInfo->MbmFileName();
    // just test string is not empty
    ASSERT( name.Length() > 0 );
    TeardownIconInfo();
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunIconIdTest()
//  Run 'TPbkIconInfo::IconId' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunIconIdTest()
    {
    SetupIconInfoL();
    ASSERT( iIconInfo->IconId() > 0 );
    TeardownIconInfo();
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunMaskIdTest()
//  Run 'TPbkIconInfo::MaskId' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunMaskIdTest()
    {
    SetupIconInfoL();
    ASSERT( iIconInfo->MaskId() > 0 );
    TeardownIconInfo();
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunSkinIdTest()
//  Run 'TPbkIconInfo::SkinId' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunSkinIdTest()
    {
    SetupIconInfoL();
    TAknsItemID id = iIconInfo->SkinId();
    
    ASSERT( id.iMajor > 0 );
    ASSERT( id.iMinor > 0 );
    TeardownIconInfo();
    return KErrNone;    
    }
    
///////////////////////////////////////////////////////////////////////////////
// PbkIconUtils test methods                                                 //   
///////////////////////////////////////////////////////////////////////////////

    
// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunCreateIconLTestL()
//  Run 'PbkIconUtils::CreateIconL' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunCreateIconLTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL();
    CleanupStack::PushL( container );
    
    const TPbkIconInfo* iconInfo = container->Find( EPbkqgn_prop_checkbox_off );
       
    CGulIcon* bmp = container->LoadBitmapL( EPbkqgn_prop_checkbox_off );
    CleanupStack::PushL( bmp );
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    
    PbkIconUtils::CreateIconL(
                    AknsUtils::SkinInstance(),
                    bitmap,
                    mask,
                    *iconInfo
                    );
    
    ASSERT( bitmap );
    ASSERT( mask );
    
    delete bitmap;
    delete mask;
  
    CleanupStack::PopAndDestroy( bmp );  
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile );
    return KErrNone;    
    }
    
// ----------------------------------------------------------------------------
//  CMTPbkIconInfo::RunCreateIconLCTestL()
//  Run 'PbkIconUtils::CreateIconLC' testcase
// ----------------------------------------------------------------------------
TInt CMTPbkIconInfo::RunCreateIconLCTestL()
    {
    RPbkViewResourceFile* resourceFile = OpenResourceFileL();
    CleanupStack::PushL( resourceFile );
    
    CPbkIconInfoContainer* container = CPbkIconInfoContainer::NewL();
    CleanupStack::PushL( container );
    
    const TPbkIconInfo* iconInfo = container->Find( EPbkqgn_prop_checkbox_off );
       
    CGulIcon* bmp = container->LoadBitmapL( EPbkqgn_prop_checkbox_off );
    CleanupStack::PushL( bmp );
    CFbsBitmap* bitmap;
    CFbsBitmap* mask;
    
    PbkIconUtils::CreateIconLC(
                    AknsUtils::SkinInstance(),
                    bitmap,
                    mask,
                    *iconInfo
                    );
    
    ASSERT( bitmap );
    ASSERT( mask );
    
    //CreateIconLC puts both bitmap and mask in the cleanup stack
    CleanupStack::PopAndDestroy( mask );
    CleanupStack::PopAndDestroy( bitmap );
    CleanupStack::PopAndDestroy( bmp );  
    CleanupStack::PopAndDestroy( container );
    resourceFile->Close();
    CleanupStack::PopAndDestroy( resourceFile );
	return KErrNone;    
    }    

//  [End of File] - Do not remove
