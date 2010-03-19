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
* Description:  Unit test class for testing CPcsKeyMap
*
*/

//  INTERNAL INCLUDES
#include "t_cpplpredictivesearchtable.h"
#include "pltables.h"
#include "dbsqlconstants.h"

//  SYSTEM INCLUDES
#include <digia/eunit/eunitmacros.h>

// Database file
_LIT(KDBFile, "c:\\unittest.db");

const TContactItemId KTestContactId = 20;
const TContactItemId KTestContactId2 = 85;
const TContactItemId KTestContactId3 = 7001;
const TContactItemId KTestContactId4 = 56030;

_LIT(KTestFirstName, "123");
_LIT(KTestLastName, "45678");
_LIT(KTestCompanyName, "90");
    

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::NewL
// -----------------------------------------------------------------------------
//
UT_CPplPredictiveSearchTable* UT_CPplPredictiveSearchTable::NewL()
    {
    UT_CPplPredictiveSearchTable* self = UT_CPplPredictiveSearchTable::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::NewLC
// -----------------------------------------------------------------------------
//
UT_CPplPredictiveSearchTable* UT_CPplPredictiveSearchTable::NewLC()
    {
    UT_CPplPredictiveSearchTable* self = new( ELeave ) UT_CPplPredictiveSearchTable();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::~UT_CPplPredictiveSearchTable
// -----------------------------------------------------------------------------
//
UT_CPplPredictiveSearchTable::~UT_CPplPredictiveSearchTable()
    {
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CPplPredictiveSearchTable
// -----------------------------------------------------------------------------
//
UT_CPplPredictiveSearchTable::UT_CPplPredictiveSearchTable()
    {
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::ConstructL
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    
// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::SetupL
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::SetupL()
    {
    // Ignore error
	RSqlDatabase::Delete(KDBFile);
    // If this causes KErrAlreadyExists, iDB won't be fully constructed
	iDB.Create(KDBFile);

	iTable = CPplPredictiveSearchTable::NewL(iDB);
	iTable->CreateTableL();
    }
    
// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::Teardown
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::Teardown()
    {
    delete iTable;
	iTable = NULL;
	
	iDB.Close(); // Must close DB before it can be deleted
	RSqlDatabase::Delete(KDBFile);
    }


// TEST CASES

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CreateInDbLL
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_CreateInDbLL()
    {
    CheckItemCountL(); // all empty

    AddContactL(KTestFirstName, KTestLastName, KTestCompanyName, KTestContactId);

    CheckItemCountL(0, 1, 0, 0, 1); // table 1 and 4 have one entry
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CreateInDbManyContactsL
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_CreateInDbManyContactsL()
    {
	AddContactL(_L(" 1 22 333 4"), _L("  505 6768"), _L("7 8 9"), KTestContactId);
	CheckItemCountL(0, 1, 1, 1, 1, 1, 1);

	AddContactL(_L(" 1 22 333 4"), _L("777  505 6768"), _L("123"), KTestContactId2);
	CheckItemCountL(0, 2, 2, 2, 2, 2, 2, 1);

	AddContactL(_L(" 1 22 333 4"), _L("  5 6"), KNullDesC, KTestContactId3);
	CheckItemCountL(0, 3, 3, 3, 3, 3, 3, 1);

	AddContactL(_L("858585"), _L("88"), KNullDesC, KTestContactId4);
	CheckItemCountL(0, 3, 3, 3, 3, 3, 3, 1, 1);

	TContactItemId id = KTestContactId4;
	AddContactL(KNullDesC, KNullDesC, _L("1 2 3 4 5 6"), ++id);
	CheckItemCountL(0, 3, 3, 3, 3, 3, 3, 1, 1);

	AddContactL(_L("1 1 1 1 1 1 1"), KNullDesC, _L("1 2 3 4 5 6"), ++id);
	CheckItemCountL(0, 4, 3, 3, 3, 3, 3, 1, 1);

	AddContactL(KNullDesC, _L("  22 22 2 2 222222"), KNullDesC, ++id);
	CheckItemCountL(0, 4, 4, 3, 3, 3, 3, 1, 1);
	}

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_UpdateLL
// Update an existing contact
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_UpdateLL()
    {
    _LIT(KTestUpdatedName, "0581");
    
    // Add one contact to DB
    AddContactL(KTestFirstName, KTestLastName, KTestCompanyName, KTestContactId);
    
    // Update some field
    TInt id = KUidContactCardValue;
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);
    
    // Only first name is changed, but also the last name must be filled to
    // CContactItemField.
    CContactItemField* field =
        CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(KTestUpdatedName);
    contact->AddFieldL(*field); // Takes ownership
    field = NULL;
    
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(KTestLastName);  
    contact->AddFieldL(*field); // Takes ownership
    
    contact->SetId(KTestContactId);

    iTable->UpdateL(*contact);
    CleanupStack::PopAndDestroy(contact);

    CheckItemCountL(1, 0, 0, 0, 1); // tables 0 and 4 have one entry
    
    RArray<TInt32> results = DoPredictiveSearchL(KTestUpdatedName);
    CleanupClosePushL(results);
    EUNIT_ASSERT_EQUALS(1, results.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results[0]);
    CleanupStack::PopAndDestroy(&results);
    

    RArray<TInt32> results2 = DoPredictiveSearchL(KTestFirstName);
    CleanupClosePushL(results2);
    EUNIT_ASSERT_EQUALS(0, results.Count());
    CleanupStack::PopAndDestroy(&results2);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_UpdateLL
// Update an existing contact
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_UpdateLBothFieldsL()
    {
    _LIT(KTestUpdatedFirstName, "777");
    _LIT(KTestUpdatedLastName, "012345");
       
    // Add some contacts
    AddContactL(KTestFirstName, KTestLastName, KTestCompanyName, KTestContactId);
    AddContactL(_L("9876"), _L("888"), KTestCompanyName, KTestContactId2);
    AddContactL(_L("5050"), _L("2"), KTestCompanyName, KTestContactId3);
    CheckItemCountL(0, 1, 1, 0, 1, 1, 0, 0, 1, 1);

    // Update FN and LN of second contact
    TInt id = KUidContactCardValue;
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);
    
    // Only first name is changed, but also the last name must be filled to
    // CContactItemField.
    CContactItemField* field =
       CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(KTestUpdatedFirstName);
    contact->AddFieldL(*field); // Takes ownership
    field = NULL;
    
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(KTestUpdatedLastName);  
    contact->AddFieldL(*field); // Takes ownership
    
    contact->SetId(KTestContactId2);
    
    iTable->UpdateL(*contact);
    CleanupStack::PopAndDestroy(contact);
    
    CheckItemCountL(1, 1, 1, 0, 1, 1, 0, 1, 0, 0);
    
    RArray<TInt32> results = DoPredictiveSearchL(KTestUpdatedFirstName);
    CleanupClosePushL(results);
    EUNIT_ASSERT_EQUALS(1, results.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId2, results[0]);
    CleanupStack::PopAndDestroy(&results);
    
    
    RArray<TInt32> results2 = DoPredictiveSearchL(KTestUpdatedLastName);
    CleanupClosePushL(results2);
    EUNIT_ASSERT_EQUALS(1, results2.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId2, results2[0]);
    CleanupStack::PopAndDestroy(&results2);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_SearchL
// Search contacts from the table 
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_SearchL()
    {
    // Create some contacts
    AddContactL(_L("123704"), _L("39"), KNullDesC, KTestContactId);
    AddContactL(_L("908"), KNullDesC, _L("00588"), KTestContactId2);
    AddContactL(KNullDesC, _L("967"), _L("2345"), KTestContactId3);
    // Tables 0 and 2 are empty, since company name is not used
    CheckItemCountL(0, 1, 0, 1, 0, 0, 0, 0, 0, 2); 
    
    // One contact matches
    RArray<TContactItemId> results = DoPredictiveSearchL(_L("1"));
    CleanupClosePushL(results);
    EUNIT_ASSERT_EQUALS(1, results.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results[0]);
    CleanupStack::PopAndDestroy(&results);
  
    // No matches
    RArray<TContactItemId> results2 = DoPredictiveSearchL(_L("124"));
    CleanupClosePushL(results2);
    EUNIT_ASSERT_EQUALS(0, results2.Count());
    CleanupStack::PopAndDestroy(&results2);


    // Two matches
    RArray<TContactItemId> results3 = DoPredictiveSearchL(_L("9"));
    CleanupClosePushL(results3);
    EUNIT_ASSERT_EQUALS(2, results3.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId2, results3[0]);
    EUNIT_ASSERT_EQUALS(KTestContactId3, results3[1]);
    CleanupStack::PopAndDestroy(&results3);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_SearchWithSpacesL
// Search contacts having spaces from the table 
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_SearchWithSpacesL()
    {
    // Create some contacts
    AddContactL(_L(" 8075 4702 380"), _L(" 470 038"), _L(" 0804 08"), KTestContactId);
    AddContactL(_L(" 0 00"), _L(" 38 038  470"), KNullDesC, KTestContactId2);
    AddContactL(KNullDesC, _L(" 123 47  "), KNullDesC, KTestContactId3);
    CheckItemCountL(2, 1, 0, 2, 3, 0, 0, 0, 1, 0);
    
    // No matches
    RArray<TContactItemId> results = DoPredictiveSearchL(_L("2"));
    TInt count = results.Count();
    results.Close();
    EUNIT_ASSERT_EQUALS(0, count);
    
    
    RArray<TContactItemId> results2 = DoPredictiveSearchL(_L("07"));
    count = results2.Count();
    results2.Close();
    EUNIT_ASSERT_EQUALS(0, count);

    
    RArray<TContactItemId> results3 = DoPredictiveSearchL(_L("1230"));
    count = results3.Count();
    results3.Close();
    EUNIT_ASSERT_EQUALS(0, count);

    
    RArray<TContactItemId> results4 = DoPredictiveSearchL(_L("5047"));
    count = results4.Count();
    results4.Close();
    EUNIT_ASSERT_EQUALS(0, count);
    
    
    RArray<TContactItemId> results5 = DoPredictiveSearchL(_L("4700"));
    count = results5.Count();
    results5.Close();
    EUNIT_ASSERT_EQUALS(0, count);
    
    
    // One match
    RArray<TContactItemId> results6 = DoPredictiveSearchL(_L("00"));
    CleanupClosePushL(results6);
    EUNIT_ASSERT_EQUALS(1, results6.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId2, results6[0]);
    CleanupStack::PopAndDestroy(&results6);
    
    
    RArray<TContactItemId> results7 = DoPredictiveSearchL(_L("4702"));
    CleanupClosePushL(results7);
    EUNIT_ASSERT_EQUALS(1, results7.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results7[0]);
    CleanupStack::PopAndDestroy(&results7);

 
    // Two matches
    RArray<TContactItemId> results8 = DoPredictiveSearchL(_L("470"));
    CleanupClosePushL(results8);
    EUNIT_ASSERT_EQUALS(2, results8.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results8[0]);
    EUNIT_ASSERT_EQUALS(KTestContactId2, results8[1]);
    CleanupStack::PopAndDestroy(&results8);
    
    
    RArray<TContactItemId> results9 = DoPredictiveSearchL(_L("0"));
    CleanupClosePushL(results9);
    EUNIT_ASSERT_EQUALS(2, results9.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results9[0]);
    EUNIT_ASSERT_EQUALS(KTestContactId2, results9[1]);
    CleanupStack::PopAndDestroy(&results9);
    

    RArray<TContactItemId> results10 = DoPredictiveSearchL(_L("038"));
    CleanupClosePushL(results10);
    EUNIT_ASSERT_EQUALS(2, results10.Count());
    EUNIT_ASSERT_EQUALS(KTestContactId, results10[0]);
    EUNIT_ASSERT_EQUALS(KTestContactId2, results10[1]);
    CleanupStack::PopAndDestroy(&results10);
    
    
    // Three matches
    RArray<TContactItemId> results11 =  DoPredictiveSearchL(_L("47"));
    CleanupClosePushL(results11);
    EUNIT_ASSERT_EQUALS(3, results11.Count());
    CleanupStack::PopAndDestroy(&results11);
    }
    
// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_DeleteLL
// Delete a the only contact from the table 
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_DeleteLL()
    {
    AddContactL(KTestFirstName, KTestLastName, KTestCompanyName, KTestContactId);
    CheckItemCountL(0, 1, 0, 0, 1, 0, 0, 0, 0, 0);
    
    TInt id = KUidContactCardValue;
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId);

    // Set FN & LN as they are needed to determine affected tables
    CContactItemField* field =
        CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    CContactTextField* textfield = field->TextStorage();
    textfield->SetTextL(KTestFirstName);
    contact->AddFieldL(*field); // Takes ownership
    field = NULL;
        
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(KTestLastName);
    contact->AddFieldL(*field); // Takes ownership

    TBool lowDiskErrorOccurred(EFalse);
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    CheckItemCountL();
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_DeleteLL
// Delete contacts one-by-one from the table that has multiple contacts
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_DeleteContactsL()
    {
    _LIT(KTestFN, "7 8 ");
    _LIT(KTestLN, "  4  56  1  ");
    TContactItemId contactId = KTestContactId4 + 1;
    AddContactL(KTestFirstName, KTestLastName, KTestCompanyName, KTestContactId);
    AddContactL(KTestFirstName, KTestLastName, KTestCompanyName, KTestContactId2);
    AddContactL(KNullDesC, KTestLastName, KTestCompanyName, KTestContactId3);
    AddContactL(KNullDesC, KNullDesC, KTestCompanyName, KTestContactId4);
    AddContactL(KTestFN, KTestLN, KTestCompanyName, contactId);
    CheckItemCountL(0, 3, 0, 0, 4, 1, 0, 1, 1, 0);    
    
    TInt id = KUidContactCardValue;
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId3);

    CContactItemField* field =
        CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(KNullDesC);
    contact->AddFieldL(*field); // Takes ownership
    field = NULL;
        
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(KTestLastName);
    contact->AddFieldL(*field); // Takes ownership

    TBool lowDiskErrorOccurred(EFalse);
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    contact = NULL;
    CheckItemCountL(0, 3, 0, 0, 3, 1, 0, 1, 1, 0);
    
    
    contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId4);
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(KNullDesC);
    contact->AddFieldL(*field); // Takes ownership
    field = NULL;
        
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(KNullDesC);
    contact->AddFieldL(*field); // Takes ownership


    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    contact = NULL;
    CheckItemCountL(0, 3, 0, 0, 3, 1, 0, 1, 1, 0);
    
    
    contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId);
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(KTestFirstName);
    contact->AddFieldL(*field); // Takes ownership
    field = NULL;
        
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(KTestLastName);
    contact->AddFieldL(*field); // Takes ownership

    
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    contact = NULL;
    CheckItemCountL(0, 2, 0, 0, 2, 1, 0, 1, 1, 0);
    
    
    contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId2);
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(KTestFirstName);
    contact->AddFieldL(*field); // Takes ownership
    field = NULL;
        
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(KTestLastName);
    contact->AddFieldL(*field); // Takes ownership

    
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    contact = NULL;
    CheckItemCountL(0, 1, 0, 0, 1, 1, 0, 1, 1, 0);
    
    
    contact = CContactItem::NewLC(uid);
    contact->SetId(contactId);
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
    field->TextStorage()->SetTextL(KTestFN);
    contact->AddFieldL(*field); // Takes ownership
    field = NULL;
        
    field = CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
    field->TextStorage()->SetTextL(KTestLN);
    contact->AddFieldL(*field); // Takes ownership

    
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    CheckItemCountL(); // all empty
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_DeleteNonexistingContactL
// Try to delete a contact that does not existing in the table
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_DeleteNonexistingContactL()
    {
    AddContactL(KTestFirstName, KTestLastName, KTestCompanyName, KTestContactId);
    CheckItemCountL(0, 1, 0, 0, 1, 0, 0, 0, 0, 0);
    
    TInt id = KUidContactCardValue;
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);
    contact->SetId(KTestContactId2);
    
    TBool lowDiskErrorOccurred(EFalse);
    // Note: DeleteL does not leave if the contact could not be deleted
    iTable->DeleteL(*contact, lowDiskErrorOccurred);
    CleanupStack::PopAndDestroy(contact);
    CheckItemCountL(0, 1, 0, 0, 1, 0, 0, 0, 0, 0);
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CheckIfTableExistsL
// Predictive search table does not exist in the database.
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_CheckIfTableExistsL()
    {
    // Delete DB that was created in setup()
    iDB.Close(); // Must close DB before it can be deleted
    RSqlDatabase::Delete(KDBFile);
	delete iTable;
	iTable = NULL;

    // Create DB, but do not create pred search table in it
    iDB.Create(KDBFile);
    iTable = CPplPredictiveSearchTable::NewL(iDB);

	// SQLite does not have SHOW TABLES command, so try to search from
    // predictive search table.
    _LIT(KCheckIfTableExistsFormat, "SELECT %S FROM %S;");
    TInt bufSize = KCheckIfTableExistsFormat().Length() +
                   KPredSearchContactId().Length() +
                   KSqlContactPredSearchTable0().Length();
    HBufC* sqlStatement = HBufC::NewLC(bufSize);
    sqlStatement->Des().AppendFormat(KCheckIfTableExistsFormat,
        &KPredSearchContactId,
        &KSqlContactPredSearchTable0);

    RSqlStatement stmnt;
    CleanupClosePushL(stmnt);
    // If table does not exist, leaves with -311
    // If table exists, does not leave
    TRAPD(err, stmnt.PrepareL(iDB, *sqlStatement));
	EUNIT_ASSERT_EQUALS(-311, err);
    
    CleanupStack::PopAndDestroy(&stmnt);
    CleanupStack::PopAndDestroy(sqlStatement);
    
    // Delete iTable here, otherwise it is treated as a resource leak
    delete iTable;
    iTable = NULL;
    }

// -----------------------------------------------------------------------------
// UT_CPplPredictiveSearchTable::UT_CheckIfTableExists2L
// Predictive search table exists in the database.
// -----------------------------------------------------------------------------
//
void UT_CPplPredictiveSearchTable::UT_CheckIfTableExists2L()
    {
    _LIT(KCheckIfTableExistsFormat, "SELECT %S FROM %S;");
    TInt bufSize = KCheckIfTableExistsFormat().Length() +
                   KPredSearchContactId().Length() +
                   KSqlContactPredSearchTable0().Length();
    HBufC* sqlStatement = HBufC::NewLC(bufSize);
    sqlStatement->Des().AppendFormat(KCheckIfTableExistsFormat,
        &KPredSearchContactId,
        &KSqlContactPredSearchTable0);

    RSqlStatement stmnt;
    CleanupClosePushL(stmnt);
    // If table exists, does not leave
    TRAPD(err, stmnt.PrepareL(iDB, *sqlStatement));
	EUNIT_ASSERT_EQUALS(KErrNone, err);
    
    CleanupStack::PopAndDestroy(&stmnt);
    CleanupStack::PopAndDestroy(sqlStatement);
    }

/* Create DB and other tables, except pred search table. Add some contacts to DB
   but do not write them to (non-existing) pred search table.
   
   Or just create DB and all tables, then add contacts, and delete pred.search
   tables.
   Close database and open it, to see that pred search tables are created. */
void UT_CPplPredictiveSearchTable::UT_SynchronizeTableL()
    {
/////// This test case is not ready /////////
    
    // Delete DB that was created in setup()
    iDB.Close(); // Must close DB before it can be deleted
    RSqlDatabase::Delete(KDBFile);

	delete iTable;
	iTable = NULL;
    iTable = CPplPredictiveSearchTable::NewL(iDB);

    // Create DB and create other tables except pred search table
    iDB.Create(KDBFile);
    
    
    // would it be easier to write to contacts table directly using RSqlStatement
    // instead of using CPplContactTable?
    
    // Create contacts table
    User::LeaveIfError(iDB.Exec(KContactCreateStmnt));
    
    //TODO: write new entries to contact table
    // or maybe create db using emulator's phonebook etc

    //iContactTable = CPplContactTable::NewL(iDB, iContactProperties);
    
//    iContactTable->CreateTableL();  
     
	// Delete iTable here, otherwise it is treated as a resource leak
    delete iTable;
    iTable = NULL;
    }

void UT_CPplPredictiveSearchTable::AddContactL(const TDesC& aFirstName,
                                               const TDesC& aLastName,
                                               const TDesC& aCompanyName,
                                               TContactItemId aContactId)
    {
    TInt id = KUidContactCardValue; // Defined by macro, so lacks type
    TUid uid;
    uid.iUid = id;
    CContactItem* contact = CContactItem::NewLC(uid);

    // Fill contact fields before writing to DB
    if (aFirstName.Length() > 0)
        {
        CContactItemField* field =
            CContactItemField::NewL(KStorageTypeText, KUidContactFieldGivenName);
        CContactTextField* textfield = field->TextStorage();
        textfield->SetTextL(aFirstName);
        contact->AddFieldL(*field); // Takes ownership
        }
    
    if (aLastName.Length() > 0)
        {
        CContactItemField* field =
            CContactItemField::NewL(KStorageTypeText, KUidContactFieldFamilyName);
        field->TextStorage()->SetTextL(aLastName);
        contact->AddFieldL(*field); // Takes ownership
        }
    
    if (aCompanyName.Length() > 0)
        {
        CContactItemField* field =
            CContactItemField::NewL(KStorageTypeText, KUidContactFieldCompanyName);
        field->TextStorage()->SetTextL(aCompanyName);
        contact->AddFieldL(*field); // Takes ownership
        }
   
    contact->SetId(aContactId);
    
    iTable->CreateInDbL(*contact);
    CleanupStack::PopAndDestroy(contact);
    }

void UT_CPplPredictiveSearchTable::CheckItemCountL(
    TInt aCountInTable0,
    TInt aCountInTable1,
    TInt aCountInTable2,
    TInt aCountInTable3,
    TInt aCountInTable4,
    TInt aCountInTable5,
    TInt aCountInTable6,
    TInt aCountInTable7,
    TInt aCountInTable8,
    TInt aCountInTable9)
    {
    const TInt KTableCount = 10;
    TPtrC tableNames[KTableCount] =
        {
        KSqlContactPredSearchTable0,
        KSqlContactPredSearchTable1,
        KSqlContactPredSearchTable2,
        KSqlContactPredSearchTable3,
        KSqlContactPredSearchTable4,
        KSqlContactPredSearchTable5,
        KSqlContactPredSearchTable6,
        KSqlContactPredSearchTable7,
        KSqlContactPredSearchTable8,
        KSqlContactPredSearchTable9
        };
    TInt rowCounts[KTableCount] = {0};
    
    for (TInt i = 0; i < KTableCount; ++i)
        {
        HBufC* s = HBufC::NewLC(KCountSelect().Length() + 
            // All table names are same length
            KSqlContactPredSearchTable0().Length());
        TPtr ptr = s->Des();
        ptr.Format(KCountSelect, &tableNames[i]);

        TSqlScalarFullSelectQuery scalarQuery(iDB);
    
        rowCounts[i] = scalarQuery.SelectIntL(ptr);
        CleanupStack::PopAndDestroy(s);
        }

    EUNIT_ASSERT_EQUALS(aCountInTable0, rowCounts[0]);
    EUNIT_ASSERT_EQUALS(aCountInTable1, rowCounts[1]);
    EUNIT_ASSERT_EQUALS(aCountInTable2, rowCounts[2]);
    EUNIT_ASSERT_EQUALS(aCountInTable3, rowCounts[3]);
    EUNIT_ASSERT_EQUALS(aCountInTable4, rowCounts[4]);
    EUNIT_ASSERT_EQUALS(aCountInTable5, rowCounts[5]);
    EUNIT_ASSERT_EQUALS(aCountInTable6, rowCounts[6]);
    EUNIT_ASSERT_EQUALS(aCountInTable7, rowCounts[7]);
    EUNIT_ASSERT_EQUALS(aCountInTable8, rowCounts[8]);
    EUNIT_ASSERT_EQUALS(aCountInTable9, rowCounts[9]);
    }

RArray<TContactItemId>
UT_CPplPredictiveSearchTable::DoPredictiveSearchL(const TDesC& aSearchString)
    {
	RArray<TContactItemId> foundContactIds;
	if (aSearchString.Length() == 0)
		{
		return foundContactIds;
		}

    // There is only need to search from one table (unless search string begins
    // with a zero).
    // E.g. if search string is "102", the results are both in tables 1 and 2
    // and it does not matter to which one the search is made.
    // E.g. "0102", search from table 0 (for exact match "0102",and from 
    // table 1 or 2 (for something that begins with 1 and 2)
    
    // NOTE: a single SELECT can search from multiple tables
    // Find the table(s) to search from
    const TPtrC& tableName = DetermineTableName(aSearchString);
    
//TODO: continue, add support for zero in the beginning (search also from table0)
// and zero in the middle (add the 3rd OR-block to WHERE part)


    // Searching contact id by the numeric representation of names.
    // SELECT contact_id
    // FROM predictivesearch
    // WHERE (first_name_as_number LIKE :first_name_as_number) OR
    //  (last_name_as_number LIKE :last_name_as_number);
    //
    // Even though the company name is still stored in the table, it is not
    // used for searching.
    //
    // The parameter values be like "% <search-string>%"
//TODO: update the SELECT to support zero (e.g. "102")
    _LIT(KConditionLikeFormat, "(%S LIKE %S) OR (%S LIKE %S)");
    // Size required by condition clause
    const TInt KWhereLikeClauseSize(KConditionLikeFormat().Size() +
        KPredSearchFirstNameAsNumber().Size() +
        KPredSearchFirstNameAsNumberParam().Size() +
        KPredSearchLastNameAsNumber().Size() +
        KPredSearchLastNameAsNumberParam().Size());
    HBufC* whereLikeClause = HBufC::NewLC(KWhereLikeClauseSize);
    whereLikeClause->Des().AppendFormat(KConditionLikeFormat,
        &KPredSearchFirstNameAsNumber,
        &KPredSearchFirstNameAsNumberParam,
        &KPredSearchLastNameAsNumber,
        &KPredSearchLastNameAsNumberParam);

    TCntSqlStatementType selectType(ESelect, tableName);
    CCntSqlStatement* selectStmnt = TSqlProvider::GetSqlStatementL(selectType);
    CleanupStack::PushL(selectStmnt);

    // Add parameters for SELECT
    selectStmnt->SetParamL(KPredSearchContactId, KNullDesC);
    const TInt KContactIdIndex(selectStmnt->ParameterIndex(KPredSearchContactId));

    // Condition contains parameters for WHERE
    selectStmnt->SetConditionL(*whereLikeClause);
    CleanupStack::Pop(selectStmnt);
    CleanupStack::PopAndDestroy(whereLikeClause);
    CleanupStack::PushL(selectStmnt);


    RSqlStatement stmnt;
    CleanupClosePushL(stmnt);
    stmnt.PrepareL(iDB, selectStmnt->SqlStringL()); //TODO:leaves if e.g. table is "predsearch0,predsearch5"
    CleanupStack::Pop(&stmnt);
    CleanupStack::PopAndDestroy(selectStmnt);
    CleanupClosePushL(stmnt);
    
    _LIT(KWildCard, "%");
    _LIT(KSeparator, " ");
    // Space required for prefix (% ) and postfix (%) ("% <search-string>%")
    const TInt KAdditionalSizeNeeded = 3;
    HBufC* pattern = HBufC::NewLC(aSearchString.Length() + KAdditionalSizeNeeded);
    TPtr searchPattern = pattern->Des();
    searchPattern.Append(KWildCard);
    searchPattern.Append(KSeparator);
    searchPattern.Append(aSearchString);
    searchPattern.Append(KWildCard);
    
    // These could be hardcoded for more speed (values 0, 1)
    const TInt KFirstNameIndex = stmnt.ParameterIndex(KPredSearchFirstNameAsNumberParam);
    User::LeaveIfError(stmnt.BindText(KFirstNameIndex, *pattern));
    const TInt KLastNameIndex = stmnt.ParameterIndex(KPredSearchLastNameAsNumberParam);
    User::LeaveIfError(stmnt.BindText(KLastNameIndex, *pattern));
    CleanupStack::PopAndDestroy(pattern);


    // Fetch the list of any matching contact ids
    TInt err(KErrNone);
    while ((err = stmnt.Next() ) == KSqlAtRow)
        {
        foundContactIds.AppendL(stmnt.ColumnInt(KContactIdIndex));
        }

    // Leave if we didn't complete going through the results properly
    if (err != KSqlAtEnd)
        {
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(&stmnt);
    return foundContactIds;

#if 0 // old code, used for 1 table implementation
	// Searching contact id by the numeric representation of names.
	// SELECT contact_id
	// FROM predictivesearch
	// WHERE (first_name_as_number LIKE :first_name_as_number) OR
	//	(last_name_as_number LIKE :last_name_as_number);
	//
	// Even though the company name is still stored in the table, it is not
	// used for searching.
	//
	// The parameter values be like "% <search-string>%"
	_LIT(KConditionLikeFormat, "(%S LIKE %S) OR (%S LIKE %S)");
	// Size required by condition clause
	const TInt KWhereLikeClauseSize(KConditionLikeFormat().Size() +
		KPredSearchFirstNameAsNumber().Size() +
		KPredSearchFirstNameAsNumberParam().Size() +
		KPredSearchLastNameAsNumber().Size() +
		KPredSearchLastNameAsNumberParam().Size());
	HBufC* whereLikeClause = HBufC::NewLC(KWhereLikeClauseSize);
	whereLikeClause->Des().AppendFormat(KConditionLikeFormat,
		&KPredSearchFirstNameAsNumber,
		&KPredSearchFirstNameAsNumberParam,
		&KPredSearchLastNameAsNumber,
		&KPredSearchLastNameAsNumberParam);

	TCntSqlStatementType selectType( ESelect,
	    KSqlContactPredSearchTableName );
	CCntSqlStatement* selectStmnt = TSqlProvider::GetSqlStatementL(selectType);
	CleanupStack::PushL(selectStmnt);

	// Add parameters for SELECT
	selectStmnt->SetParamL(KPredSearchContactId, KNullDesC);
	const TInt KContactIdIndex(selectStmnt->ParameterIndex(KPredSearchContactId));

	// Condition contains parameters for WHERE
	selectStmnt->SetConditionL(*whereLikeClause);
	CleanupStack::Pop(selectStmnt);
	CleanupStack::PopAndDestroy(whereLikeClause);
	CleanupStack::PushL(selectStmnt);


    RSqlStatement stmnt;
    CleanupClosePushL(stmnt);
	stmnt.PrepareL(iDB, selectStmnt->SqlStringL());
	CleanupStack::Pop(&stmnt);
	CleanupStack::PopAndDestroy(selectStmnt);
	CleanupClosePushL(stmnt);
    
    _LIT(KWildCard, "%");
    _LIT(KSeparator, " ");
    // Space required for prefix (% ) and postfix (%) ("% <search-string>%")
    const TInt KAdditionalSizeNeeded = 3;
    HBufC* pattern = HBufC::NewLC(aSearchString.Length() + KAdditionalSizeNeeded);
    TPtr searchPattern = pattern->Des();
    searchPattern.Append(KWildCard);
    searchPattern.Append(KSeparator);
    searchPattern.Append(aSearchString);
    searchPattern.Append(KWildCard);
    
    // These could be hardcoded for more speed (values 0, 1)
    const TInt KFirstNameIndex = stmnt.ParameterIndex(KPredSearchFirstNameAsNumberParam);
    User::LeaveIfError(stmnt.BindText(KFirstNameIndex, *pattern));
    const TInt KLastNameIndex = stmnt.ParameterIndex(KPredSearchLastNameAsNumberParam);
    User::LeaveIfError(stmnt.BindText(KLastNameIndex, *pattern));
    CleanupStack::PopAndDestroy(pattern);


    // Fetch the list of any matching contact ids
	TInt err(KErrNone);
    RArray<TContactItemId> foundContactIds;
    while ((err = stmnt.Next() ) == KSqlAtRow)
        {
        foundContactIds.AppendL(stmnt.ColumnInt(KContactIdIndex));
        }

    // Leave if we didn't complete going through the results properly
    if (err != KSqlAtEnd)
        {
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(&stmnt);
    return foundContactIds;
#endif
    }

const TDesC& UT_CPplPredictiveSearchTable::DetermineTableName(
	const TDesC& aSearchString)
	{
	switch (aSearchString[0])
		{
		case '0':
		    return KSqlContactPredSearchTable0;
		case '1':
			return KSqlContactPredSearchTable1;
		case '2':
			return KSqlContactPredSearchTable2;
		case '3':
			return KSqlContactPredSearchTable3;
		case '4':
			return KSqlContactPredSearchTable4;
		case '5':
			return KSqlContactPredSearchTable5;
		case '6':
			return KSqlContactPredSearchTable6;
		case '7':
			return KSqlContactPredSearchTable7;	
		case '8':
			return KSqlContactPredSearchTable8;
		case '9':
			return KSqlContactPredSearchTable9;
		default:
			return KNullDesC;
		}
	}


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_CPplPredictiveSearchTable,
    "UT_CPplPredictiveSearchTable",
    "UNIT" )
    
EUNIT_TEST(
    "CreateInDbL - test",
    "UT_CPplPredictiveSearchTable",
    "CreateInDbL",
    "FUNCTIONALITY",
    SetupL, UT_CreateInDbLL, Teardown )

EUNIT_TEST(
    "CreateInDbL - test add many contacts",
    "UT_CPplPredictiveSearchTable",
    "CreateInDbL",
    "FUNCTIONALITY",
    SetupL, UT_CreateInDbManyContactsL, Teardown )

EUNIT_TEST(
    "UpdateL - test updating FN",
    "UT_CPplPredictiveSearchTable",
    "UpdateL",
    "FUNCTIONALITY",
    SetupL, UT_UpdateLL, Teardown )

EUNIT_TEST(
    "UpdateL - test updating FN and LN",
    "UT_CPplPredictiveSearchTable",
    "UpdateL",
    "FUNCTIONALITY",
    SetupL, UT_UpdateLBothFieldsL, Teardown )

EUNIT_TEST(
    "Search - test",
    "UT_CPplPredictiveSearchTable",
    "predictive search",
    "FUNCTIONALITY",
    SetupL, UT_SearchL, Teardown )

EUNIT_TEST(
    "Search with spaces - test",
    "UT_CPplPredictiveSearchTable",
    "predictive search",
    "FUNCTIONALITY",
    SetupL, UT_SearchWithSpacesL, Teardown )

EUNIT_TEST(
    "Delete the only contact - test",
    "UT_CPplPredictiveSearchTable",
    "DeleteL",
    "FUNCTIONALITY",
    SetupL, UT_DeleteLL, Teardown )

EUNIT_TEST(
    "Delete many contacts - test",
    "UT_CPplPredictiveSearchTable",
    "DeleteL",
    "FUNCTIONALITY",
    SetupL, UT_DeleteContactsL, Teardown )

EUNIT_TEST(
    "Delete non-existing contact - test",
    "UT_CPplPredictiveSearchTable",
    "DeleteL",
    "FUNCTIONALITY",
    SetupL, UT_DeleteNonexistingContactL, Teardown )

EUNIT_TEST(
    "Check if predictive search table exists (table does not exist)",
    "UT_CPplPredictiveSearchTable",
    "",
    "FUNCTIONALITY",
    SetupL, UT_CheckIfTableExistsL, Teardown )

EUNIT_TEST(
    "Check if predictive search table exists (table exists)",
    "UT_CPplPredictiveSearchTable",
    "",
    "FUNCTIONALITY",
    SetupL, UT_CheckIfTableExists2L, Teardown )

EUNIT_TEST(
    "Synchronize table",
    "UT_CPplPredictiveSearchTable",
    "",
    "FUNCTIONALITY",
    SetupL, UT_SynchronizeTableL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
