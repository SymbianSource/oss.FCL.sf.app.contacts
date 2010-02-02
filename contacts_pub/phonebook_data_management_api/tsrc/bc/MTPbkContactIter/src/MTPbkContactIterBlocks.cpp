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
#include "MTPbkContactIter.h"
//  INTERNAL INCLUDES
#include <CPbkContactEngine.h>
#include <CPbkContactIter.h>
#include <CPbkContactItem.h>
#include <TPbkContactItemField.h>

// CONSTANTS
_LIT(KTestDatabaseName, "c:CMTPbkContactIter.cdb");

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
// CMTPbkContactIter::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkContactIter::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkContactIter::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkContactIter::RunMethodL( 
    CStifItemParser& aItem ) 
    {

     TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Test_creation", CMTPbkContactIter::TestCreation ),        
        ENTRY( "Test_create_and_reset", CMTPbkContactIter::TestCreateAndReset ),
        ENTRY( "Test_with_empty_db", CMTPbkContactIter::TestEmptyDbL ),        
        ENTRY( "Test_with_empty_db_minimal_read", CMTPbkContactIter::TestEmptyDbMinimalReadL ),
        ENTRY( "Test_FirstL_with_one_contact_db", CMTPbkContactIter::TestIterOnDbWithOneContactL ),        
        ENTRY( "Test_NextL_with_one_contact_db", CMTPbkContactIter::TestIterOnDbWithOneContact2L ),
        ENTRY( "Test_CurrentL_with_one_contact_db", CMTPbkContactIter::TestIterOnDbWithOneContact3L ),        
        ENTRY( "Test_FirstLCurrentL_with_one_contact_db", CMTPbkContactIter::TestIterOnDbWithOneContact4L ),
        ENTRY( "Test_GetCurrentL_with_one_contact_db", CMTPbkContactIter::TestIterOnDbWithOneContact5L ),        
        ENTRY( "Test_FirstLGetCurrentL_with_one_contact_db", CMTPbkContactIter::TestIterOnDbWithOneContact6L ),
        ENTRY( "Test_FirstL_with_one_contact_db_minimal_read", CMTPbkContactIter::TestIterOnDbWithOneContactMinimalReadL ),        
        ENTRY( "Test_NextL_with_one_contact_db_minimal-read", CMTPbkContactIter::TestIterOnDbWithOneContactMinimalRead2L ),
        ENTRY( "Test_CurrentL_with_one_contact_dbminimal_read", CMTPbkContactIter::TestIterOnDbWithOneContactMinimalRead3L ),        
        ENTRY( "Test_FirstLCurrentL_with_one_contact_db_minimal_read", CMTPbkContactIter::TestIterOnDbWithOneContactMinimalRead4L ),                
        ENTRY( "Test_GetCurrentL_with_one_contact_db_minimal_read", CMTPbkContactIter::TestIterOnDbWithOneContactMinimalRead5L ),        
        ENTRY( "Test_FirstLGetCurrentL_with_one_contact_db_minimal_read", CMTPbkContactIter::TestIterOnDbWithOneContactMinimalRead6L ),                
        ENTRY( "Test_iterator_with_two_contact_db", CMTPbkContactIter::TestIterOnDbWith2ContactsL ),        
        ENTRY( "Test_iterator_with_two_contact_db_minimal_read", CMTPbkContactIter::TestIterOnDbWith2ContactsMinimalReadL ),                
        

        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void CMTPbkContactIter::SetupL()
	{
    // create engine
    iEngine = CPbkContactEngine::NewL(KTestDatabaseName, ETrue);
	} 

void CMTPbkContactIter::SetupWithContactL()
	{
    SetupL();
    // create Contact into db
    CPbkContactItem* item = iEngine->CreateEmptyContactL();
    CleanupStack::PushL(item);
    iAddedContactId = iEngine->AddNewContactL(*item);
    CleanupStack::PopAndDestroy(item);
	} 

void CMTPbkContactIter::SetupWith2ContactL()
	{
    SetupL();
    // create Contact into db
    CPbkContactItem* item = iEngine->CreateEmptyContactL();
    CleanupStack::PushL(item);
    iAddedContactId = iEngine->AddNewContactL(*item);
    CleanupStack::PopAndDestroy(item);

    item = iEngine->CreateEmptyContactL();
    CleanupStack::PushL(item);
    iAddedContactId2 = iEngine->AddNewContactL(*item);
    CleanupStack::PopAndDestroy(item);
	} 

void CMTPbkContactIter::Teardown()
	{
	delete iEngine;
    iAddedContactId = KNullContactId;
    iAddedContactId2 = KNullContactId;
	}

TInt CMTPbkContactIter::TestCreation()
	{
	SetupL();
    CPbkContactIter* iter = CPbkContactIter::NewL(*iEngine);
    ASSERT(iter);
    delete iter;
    iter = NULL;

    iter = CPbkContactIter::NewL(*iEngine, ETrue);
    ASSERT(iter);
    delete iter;
    iter = NULL;

    iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    CleanupStack::PopAndDestroy(iter);

    iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    CleanupStack::PopAndDestroy(iter);
	Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestCreateAndReset()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewL(*iEngine);
    ASSERT(iter);

    TContactItemId id = iter->FirstL();
    ASSERT(id == iAddedContactId);

    iter->Reset();
    ASSERT(iter->CurrentL() == NULL);

    id = NULL;
    id = iter->FirstL();
    ASSERT(id == iAddedContactId);

    delete iter;
    iter = NULL;
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestEmptyDbL()
    {
    SetupL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    TContactItemId id = iter->FirstL();
    ASSERT(id == KNullContactId);
    CleanupStack::PopAndDestroy(iter);

    iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    id = iter->NextL();
    ASSERT(id == KNullContactId);
    CleanupStack::PopAndDestroy(iter);

    iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    CPbkContactItem* item = iter->CurrentL();
    ASSERT(!item);
    CleanupStack::PopAndDestroy(iter);

    iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    item = iter->GetCurrentL();
    ASSERT(!item);
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestEmptyDbMinimalReadL()
    {
    SetupL();
    // minimal read
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    TContactItemId id = iter->FirstL();
    ASSERT(id == KNullContactId);
    CleanupStack::PopAndDestroy(iter);

    iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    id = iter->NextL();
    ASSERT(id == KNullContactId);
    CleanupStack::PopAndDestroy(iter);

    iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    CPbkContactItem* item = iter->CurrentL();
    ASSERT(!item);
    CleanupStack::PopAndDestroy(iter);

    iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    item = iter->GetCurrentL();
    ASSERT(!item);
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContactL()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    TContactItemId id = iter->FirstL();
    ASSERT(id == iAddedContactId);
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContact2L()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    TContactItemId id = iter->NextL();
    ASSERT(id == iAddedContactId);
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContact3L()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    CPbkContactItem* item = iter->CurrentL();
    ASSERT(!item);
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContact4L()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    TContactItemId id = iter->FirstL();
    ASSERT(id == iAddedContactId);
    CPbkContactItem* item = iter->CurrentL();
    ASSERT(item);
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContact5L()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    CPbkContactItem* item = iter->GetCurrentL();
    ASSERT(!item);
    delete item;
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContact6L()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter);
    TContactItemId id = iter->FirstL();
    ASSERT(id == iAddedContactId);
    CPbkContactItem* item = iter->GetCurrentL();
    ASSERT(item);
    delete item;
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContactMinimalReadL()
    {
    SetupWithContactL();
    // read minimal
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    TContactItemId id = iter->FirstL();
    ASSERT(id == iAddedContactId);
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContactMinimalRead2L()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    TContactItemId id = iter->NextL();
    ASSERT(id == iAddedContactId);
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContactMinimalRead3L()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    CPbkContactItem* item = iter->CurrentL();
    ASSERT(!item);
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContactMinimalRead4L()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    TContactItemId id = iter->FirstL();
    ASSERT(id == iAddedContactId);
    CPbkContactItem* item = iter->CurrentL();
    ASSERT(item);
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContactMinimalRead5L()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    CPbkContactItem* item = iter->GetCurrentL();
    ASSERT(!item);
    delete item;
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWithOneContactMinimalRead6L()
    {
    SetupWithContactL();
    CPbkContactIter* iter = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter);
    TContactItemId id = iter->FirstL();
    ASSERT(id == iAddedContactId);
    CPbkContactItem* item = iter->GetCurrentL();
    ASSERT(item);
    delete item;
    CleanupStack::PopAndDestroy(iter);
    Teardown();
    return KErrNone;
    }

TInt CMTPbkContactIter::TestIterOnDbWith2ContactsL()
    {
    SetupWith2ContactL();
    CPbkContactIter* iter1 = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter1);
    TContactItemId id = iter1->FirstL();
    ASSERT(id == iAddedContactId);
    CleanupStack::PopAndDestroy(iter1);

    CPbkContactIter* iter2 = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter2);
    id = iter2->NextL();
    ASSERT(id == iAddedContactId);
    CleanupStack::PopAndDestroy(iter2);

    CPbkContactIter* iter3 = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter3);
    id = iter3->FirstL();
    ASSERT(id == iAddedContactId);
    id = iter3->NextL();
    ASSERT(id == iAddedContactId2);
    CleanupStack::PopAndDestroy(iter3);

    CPbkContactIter* iter4 = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter4);
    CPbkContactItem* item = iter4->CurrentL();
    ASSERT(!item);
    CleanupStack::PopAndDestroy(iter4);

    CPbkContactIter* iter5 = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter5);
    id = iter5->FirstL();
    ASSERT(id == iAddedContactId);
    item = iter5->CurrentL();
    ASSERT(item);
    ASSERT(item->Id() == iAddedContactId);
    id = iter5->NextL();
    ASSERT(id == iAddedContactId2);
    item = iter5->CurrentL();
    ASSERT(item);
    ASSERT(item->Id() == iAddedContactId2);
    CleanupStack::PopAndDestroy(iter5);

    CPbkContactIter* iter6 = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter6);
    item = iter6->GetCurrentL();
    ASSERT(!item);
    delete item;
    CleanupStack::PopAndDestroy(iter6);

    CPbkContactIter* iter7 = CPbkContactIter::NewLC(*iEngine);
    ASSERT(iter7);
    id = iter7->FirstL();
    ASSERT(id == iAddedContactId);
    item = iter7->GetCurrentL();
    ASSERT(item);
    ASSERT(item->Id() == iAddedContactId);
    delete item;
    id = iter7->NextL();
    ASSERT(id == iAddedContactId2);
    item = iter7->GetCurrentL();
    ASSERT(item);
    ASSERT(item->Id() == iAddedContactId2);
    delete item;
    CleanupStack::PopAndDestroy(iter7);
    Teardown();
    return KErrNone;}

TInt CMTPbkContactIter::TestIterOnDbWith2ContactsMinimalReadL()
    {
    SetupWith2ContactL();
// read minimal
    CPbkContactIter* iter1 = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter1);
    TContactItemId id = iter1->FirstL();
    ASSERT(id == iAddedContactId);
    CleanupStack::PopAndDestroy(iter1);

    CPbkContactIter* iter2 = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter2);
    id = iter2->NextL();
    ASSERT(id == iAddedContactId);
    CleanupStack::PopAndDestroy(iter2);

    CPbkContactIter* iter3 = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter3);
    id = iter3->FirstL();
    ASSERT(id == iAddedContactId);
    id = iter3->NextL();
    ASSERT(id == iAddedContactId2);
    CleanupStack::PopAndDestroy(iter3);

    CPbkContactIter* iter4 = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter4);
    CPbkContactItem* item = iter4->CurrentL();
    ASSERT(!item);
    CleanupStack::PopAndDestroy(iter4);

    CPbkContactIter* iter5 = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter5);
    id = iter5->FirstL();
    ASSERT(id == iAddedContactId);
    item = iter5->CurrentL();
    ASSERT(item);
    ASSERT(item->Id() == iAddedContactId);
    id = iter5->NextL();
    ASSERT(id == iAddedContactId2);
    item = iter5->CurrentL();
    ASSERT(item);
    ASSERT(item->Id() == iAddedContactId2);
    CleanupStack::PopAndDestroy(iter5);

    CPbkContactIter* iter6 = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter6);
    item = iter6->GetCurrentL();
    ASSERT(!item);
    delete item;
    CleanupStack::PopAndDestroy(iter6);

    CPbkContactIter* iter7 = CPbkContactIter::NewLC(*iEngine, ETrue);
    ASSERT(iter7);
    id = iter7->FirstL();
    ASSERT(id == iAddedContactId);
    item = iter7->GetCurrentL();
    ASSERT(item);
    ASSERT(item->Id() == iAddedContactId);
    delete item;
    id = iter7->NextL();
    ASSERT(id == iAddedContactId2);
    item = iter7->GetCurrentL();
    ASSERT(item);
    ASSERT(item->Id() == iAddedContactId2);
    delete item;
    CleanupStack::PopAndDestroy(iter7);
    Teardown();
    return KErrNone;
    }


//  [End of File] - Do not remove
