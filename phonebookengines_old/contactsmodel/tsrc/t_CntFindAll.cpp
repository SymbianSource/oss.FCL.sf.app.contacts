// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
@SYMTestCaseID PIM-T-CNTFINDALL-INC049017-0001
@SYMTestPriority High
@SYMDEF INC049017
@SYMTestCaseDependencies cnttutil
@SYMTestCaseDesc Tests that Identity fields are searched when FindAsyncL and FindLC are used with KUidContactFieldMatchAll.
@SYMTestActions Create a contact database and add four contacts.
Search the database by using identity fields: GivenName, FamilyName and CompanyName.
Search for contacts by providing a name and using KUidContactFieldMatchAll.
Perform the search synchronously and asynchronously.
@SYMTestExpectedResults Contacts are found by given name, family name or company name when using KUidContactFieldMatchAll.
*/
#include <e32std.h>
#include <e32test.h>
#include <e32math.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfldst.h>

#include "t_utils2.h"
#include "t_CntFindAll.h"


// Constants.
_LIT(KTestName,"t_CntFindAll");
_LIT(KDbFileName,"c:contacts.cdb");
_LIT(KLogFileName,"t_cntFindAll.log");
_LIT(KDummyData, "Some dummy data");

// CFindTest.
CFindTest::CFindTest()
 : iTest(KTestName),
 iLogFileName(KLogFileName),
 iDbFileName(KDbFileName)
	{
	}

CFindTest::~CFindTest()
	{
	delete iTemplate;
	delete iDb;
	delete iLog;
	iTest.Close();
	}

void CFindTest::ConstructL()
	{
	iLog=CLog::NewL(iTest,iLogFileName);
	}


void CFindTest::DoTestsL()
	{
	iTest.Title(); 
	iTest.Start(_L("@SYMTestCaseID:PIM-T-CNTFINDALL-INC049017-0001 CREATING THE DATABASE"));
	iDb=CContactDatabase::ReplaceL(iDbFileName);
	iTemplate=STATIC_CAST(CContactTemplate*,iDb->ReadContactL(iDb->TemplateId()));

	iTest.Next(_L("ADDING COMPACT CONTACTS"));
		{
		iLog->LogLine(_L("Adding The Contacts."));
	//	AddContactCompactL(aFirstName,aLastName,aCompanyName,aWorkPhone, 
	//												aHomePhone,& aWorkMobile,aHomeMobile,aWorkEmail)											
		AddContactCompactL(_L("TestRt"),_L("Jrtstn"),_L("Example Company"),_L("02079460100"), 
												_L("02079460900"), _L("07700900329"),_L("07700900300"),_L("tj2@example_company.abc"));									
		AddContactCompactL(_L("Atsta"),_L("Jrtstn"),_L("Example Company"),_L("02079460100"), 
												_L("02079460900"), _L("07700900101"),_L("07700900301"),_L("aj2@example_company.abc"));									
		AddContactCompactL(_L("Btest"),_L("Tester"),_L("High Test Example"),_L("02079460658"), 
												_L("02079460901"), _L("07700900210"),_L("07700900321"),_L("btester@hightestex.abc"));									
		AddContactCompactL(_L("Chtsts"),_L("Artestw"),_L("Testx Kick"),_L("1234567890"), 
												_L("02079460678"), _L("07700900890"),_L("07700900891"),_L("c_Artestw@testxkick.abc"));
		}

	//start of tests for defect where identity fields were not searched when using KUidMatchInAllFields
	iTest.Next(_L("FINDING CONTACTS BY NAME FROM GIVEN NAME FIELD."));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM GIVEN NAME FIELD."));
		FindNameInFirstNameFieldL(_L("Chtsts"),1);
		FindNameInFirstNameFieldL(_L("Tester"),0);
		FindNameInFirstNameFieldL(_L("e"),2);
		}

	iTest.Next(_L("FINDING CONTACTS BY NAME FROM FAMILY NAME FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM FAMILY NAME FIELD."));
		FindNameInLastNameFieldL(_L("Tester"),1);
		FindNameInLastNameFieldL(_L("Jrtstn"),2);
		FindNameInLastNameFieldL(_L("r"),4);
		FindNameInLastNameFieldL(_L("Chtsts"),0);
		}

	iTest.Next(_L("FINDING CONTACTS BY NAME FROM ALL FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM ALL FIELDS"));
		FindInAllFieldsL(_L("Btest"),1);
		FindInAllFieldsL(_L("Jrtstn"),2);
		FindInAllFieldsL(_L("r"),4);
		FindInAllFieldsL(_L("xxxxxxxxx"),0);
		}

	iTest.Next(_L("FINDING CONTACTS BY COMPANY FROM COMPANY NAME FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS COMPANY FROM COMPANY NAME FIELD"));
		FindInCompanyFieldL(_L("Btest"),0);
		FindInCompanyFieldL(_L("High Test Example"),1);
		FindInCompanyFieldL(_L("Example Company"),2);
		FindInCompanyFieldL(_L("c"),3);
		}

	iTest.Next(_L("FINDING CONTACTS BY COMPANY FROM  ALL FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY COMPANY FROM ALL FIELDS"));
		FindInAllFieldsL(_L("Kick"),1);
		FindInAllFieldsL(_L("High Test Example"),1);
		FindInAllFieldsL(_L("Example Company"),2);
		FindInAllFieldsL(_L("c"),4);
		}
	//end of tests for defect where identity fields were not searched when using KUidMatchInAllFields

	//start of Async tests for defect where identity fields were not searched when using KUidMatchInAllFields
	iTest.Next(_L("FINDING CONTACTS BY NAME FROM GIVEN NAME FIELD ASYNCHRONOUSLY"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM GIVEN NAME FIELD ASYNCHRONOUSLY."));
		FindNameInFirstNameFieldL(_L("Chtsts"),1,ETrue);
		FindNameInFirstNameFieldL(_L("Tester"),0,ETrue);
		FindNameInFirstNameFieldL(_L("e"),2,ETrue);
		}

	iTest.Next(_L("FINDING CONTACTS BY NAME FROM FAMILY NAME FIELD ASYNCHRONOUSLY"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM FAMILY NAME FIELD ASYNCHRONOUSLY."));
		FindNameInLastNameFieldL(_L("Tester"),1,ETrue);
		FindNameInLastNameFieldL(_L("Jrtstn"),2,ETrue);
		FindNameInLastNameFieldL(_L("r"),4,ETrue);
		FindNameInLastNameFieldL(_L("Chtsts"),0,ETrue);
		}

	iTest.Next(_L("FINDING CONTACTS BY NAME FROM ALL FIELDS ASYNCHRONOUSLY"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NAME FROM ALL FIELDS ASYNCHRONOUSLY"));
		FindInAllFieldsL(_L("Btest"),1,ETrue);
		FindInAllFieldsL(_L("Jrtstn"),2,ETrue);
		FindInAllFieldsL(_L("r"),4,ETrue);
		FindInAllFieldsL(_L("xxxxxxxxx"),0,ETrue);
		}

	iTest.Next(_L("FINDING CONTACTS BY COMPANY FROM COMPANY NAME FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS COMPANY FROM COMPANY NAME FIELD ASYNCHRONOUSLY"));
		FindInCompanyFieldL(_L("Btest"),0,ETrue);
		FindInCompanyFieldL(_L("High Test Example"),1,ETrue);
		FindInCompanyFieldL(_L("Example Company"),2,ETrue);
		FindInCompanyFieldL(_L("s"),2,ETrue);
		}

	iTest.Next(_L("FINDING CONTACTS BY COMPANY FROM  ALL FIELDS ASYNCHRONOUSLY"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY COMPANY FROM ALL FIELDS ASYNCHRONOUSLY"));
		FindInAllFieldsL(_L("Kick"),1,ETrue);
		FindInAllFieldsL(_L("High Test Example"),1,ETrue);
		FindInAllFieldsL(_L("Example Company"),2,ETrue);
		FindInAllFieldsL(_L("s"),4,ETrue);
		}
    iTest.Next(_L("FINDING CONTACTS BY SIP ADDRESS IN ALL FIELDS ASYNCHRONOUSLY"));
        {
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY SIP ADDRESS IN ALL FIELDS"));
		FindInSipAddressL(_L("123@a.com"),1, ETrue);
		}
	//end of Async tests for defect where identity fields were not searched when using KUidMatchInAllFields

	//the following tests were not part of the defect but included as regression testing
	iTest.Next(_L("FINDING CONTACTS BY WORK PHONE NUMBER FROM   PHONE FIELD"));
		{ 
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY WORK PHONE NUMBER FROM   PHONE FIELD"));
		FindPhoneNumberL(_L("02079460100"),2);
		FindPhoneNumberL(_L("02079460658"),1);
		FindPhoneNumberL(_L("999999999999"),0);
		}
	iTest.Next(_L("FINDING CONTACTS BY  HOME PHONE NUMBER FROM PHONE FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY HOME PHONE NUMBER FROM   PHONE FIELD"));
		FindPhoneNumberL(_L("02079460900"),2);
		FindPhoneNumberL(_L("02079460678"),1);
		FindPhoneNumberL(_L("999999999999"),0);
		}
	iTest.Next(_L("FINDING CONTACTS BY  WORK MOBILE  NUMBER FROM PHONE FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY WORK MOBILE NUMBER FROM   PHONE FIELD"));
		FindPhoneNumberL(_L("07700900329"),1);
		FindPhoneNumberL(_L("07700900101"),1);
		FindPhoneNumberL(_L("07700900210"),1);
		FindPhoneNumberL(_L("999999999999"),0);
		}
	iTest.Next(_L("FINDING CONTACTS BY HOME MOBILE  NUMBER FROM  PHONE FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY HOME MOBILE NUMBER FROM   PHONE FIELD"));
		FindPhoneNumberL(_L("07700900891"),1);
		FindPhoneNumberL(_L("07700900301"),1);
		FindPhoneNumberL(_L("07700900300"),1);
		FindPhoneNumberL(_L("999999999999"),0);
		}
	iTest.Next(_L("FINDING CONTACTS BY PHONE NUMBER FROM  ALL FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY PHONE NUMBER FROM  ALL FIELDS"));
		FindInAllFieldsL(_L("02079460100"),2);
		FindInAllFieldsL(_L("02079460900"),2);
		FindInAllFieldsL(_L("02079460658"),1);
		FindInAllFieldsL(_L("02079460678"),1);
		FindInAllFieldsL(_L("999999999999"),0);
		}
	iTest.Next(_L("FINDING CONTACTS BY EMAIL FROM  EMAIL FIELD"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY EMAIL FROM  EMAIL FIELD"));
		FindEmailAddressL(_L("@@"),0);
		FindEmailAddressL(_L("aj2@example_company.abc"),1);
		FindEmailAddressL(_L("example_company.abc"),2);
		FindEmailAddressL(_L("example"),2);
		FindEmailAddressL(_L("@"),4);
		}
	iTest.Next(_L("FINDING CONTACTS BY EMAIL FROM  ALL FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY EMAIL FROM  ALL FIELDS"));
		FindInAllFieldsL(_L("@@"),0);
		FindInAllFieldsL(_L("aj2@example_company.abc"),1);
		FindInAllFieldsL(_L("example_company.abc"),2);
		FindInAllFieldsL(_L("example"),3);
		FindInAllFieldsL(_L("@"),4);
		}
	iTest.Next(_L("FINDING CONTACTS BY NOTES FROM NOTES FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY NOTES FROM  NOTES FIELDS"));
		FindNotesL(_L("dummy"),4);
		FindNotesL(_L("dummy"),4, ETrue);
		FindNotesL(_L("nonexistant"),0);
		FindNotesL(_L("nonexistant"),0, ETrue);
		}
	iTest.Next(_L("FINDING CONTACTS BY SIP ADDRESS IN ALL FIELDS"));
        {
		iLog->LogLine(_L("\r\nFINDING CONTACTS BY SIP ADDRESS IN ALL FIELDS"));
		FindInSipAddressL(_L("123@a.com"),1);
		}
	iTest.Next(_L("FINDING NOTES FROM  ALL FIELDS"));
		{
		iLog->LogLine(_L("\r\nFINDING NOTES FROM  ALL FIELDS"));
		FindInAllFieldsL(_L("dummy"),4);
		FindInAllFieldsL(_L("dummy"),4, ETrue);
		FindInAllFieldsL(_L("nonexistant"),0);
		FindInAllFieldsL(_L("nonexistant"),0, ETrue);
		}
	iTest.Next(_L("FINDING All CONTACTS WITH NULL SEARCH STRING"));
		{
		iLog->LogLine(_L("\r\nFINDING All CONTACTS WITH NULL SEARCH STRING"));
				
		//Create a new database and check that there are no contacts on it.
		delete iDb;
		iDb=CContactDatabase::ReplaceL(iDbFileName);
		iTest(iDb->CountL() == 0);

		//Add a fixed number of contacts to the database and check if they have been added or not.
		CRandomContactGenerator* rcg = CRandomContactGenerator::NewL();
		CleanupStack::PushL(rcg);
		rcg->SetDbL(*iDb);
		const TInt KContactsCount = 50;
		for(TInt count = 0;count < KContactsCount;++count)
			{
			rcg->AddTypicalRandomContactL();
			}
		iTest(iDb->CountL() == KContactsCount);
		
		//Actual Test: This should return all 50 contacts. No less, no more!
		FindInAllFieldsL(_L(""), KContactsCount, ETrue);
		CleanupStack::PopAndDestroy();
		}
	iTest.End();
	}

void CFindTest::AddContactL(const TDesC& aFirstName,const TDesC& aLastName,const TDesC& aCompanyName,
													const TDesC& aWorkPhone,const TDesC& aHomePhone,const TDesC& aWorkMobile,
													const TDesC& aHomeMobile,const TDesC& aWorkEmail)
	{
	iLog->LogLine(_L("\r\nAdding Contact "));
	iLog->LogLine(_L("Name: %S %S"),&aFirstName,&aLastName);
	iLog->LogLine(_L("Company: %S "),&aCompanyName);
	iLog->LogLine(_L("WorkPhone: %S "),&aWorkPhone);
	iLog->LogLine(_L("WorkMob: %S"),&aWorkMobile);
	iLog->LogLine(_L("HomePhone: %S"),&aHomePhone);
	iLog->LogLine(_L("HomeMob: %S"),&aHomeMobile);
	iLog->LogLine(_L("Email: %S "),&aWorkEmail);

	CTestContact* contact=CTestContact::NewLC(*iTemplate);
	
	contact->SetFirstNameL(aFirstName);
	contact->SetLastNameL(aLastName);
	contact->SetCompanyNameL(aCompanyName);
	contact->SetWorkPhoneL(aWorkPhone);
	contact->SetHomePhoneL(aHomePhone);
	contact->SetWorkMobileL(aWorkMobile);
	contact->SetHomeMobileL(aHomeMobile);
	contact->SetWorkEmailAddressL(aWorkEmail);
	contact->SetNotesL(KDummyData);	
	iDb->AddNewContactL(contact->ContactItem());
	CleanupStack::PopAndDestroy(1);  //contact.

	}

void CFindTest::AddContactCompactL(const TDesC& aFirstName,const TDesC& aLastName,const TDesC& aCompanyName,
																	  const TDesC& aWorkPhone,const TDesC& aHomePhone,const TDesC& aWorkMobile,
																	  const TDesC& aHomeMobile,const TDesC& aWorkEmail)
	{
	AddContactL(aFirstName,aLastName,aCompanyName,aWorkPhone,aHomePhone,aWorkMobile,aHomeMobile,aWorkEmail);
	iDb->CompactL();
	}

void CFindTest::FindNameInFirstNameFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Given name field"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldGivenName);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

void CFindTest::FindNameInLastNameFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Family name field"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldFamilyName);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}	
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.	
	}

void CFindTest::FindInSipAddressL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
    {
    CContactCard *contactItem = CContactCard::NewL(iTemplate); CleanupStack::PushL(contactItem);

    CContactItemField* itemFieldPtr = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldSIPID );
    CContactTextField* textStoragePtr = itemFieldPtr->TextStorage();
    textStoragePtr->SetTextL( aTextToFind );
    contactItem->AddFieldL( *itemFieldPtr );
    CleanupStack::Pop( itemFieldPtr ); 
 
    CContactItemField* itemFieldPtr_1 = CContactItemField::NewLC( KStorageTypeStore, KUidContactFieldPicture );
    CContactStoreField* binaryStoragePtr = itemFieldPtr_1->StoreStorage();
      
    binaryStoragePtr->SetThingL(_L8("Jpeg"));
     
    contactItem->AddFieldL( *itemFieldPtr_1 ); 
    CleanupStack::Pop( itemFieldPtr_1 ); 
           
    TContactItemId id = iDb->AddNewContactL(*contactItem);
   
    CContactItemFieldDef* def = new(ELeave) CContactItemFieldDef();
    CleanupStack::PushL(def);
    def->AppendL(KUidContactFieldSIPID);
 
    if(!aAsync)
        {
        DoFindL(aTextToFind,*def,aNumExpectedFinds);
        }
    else
        {
        DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
        }

    CleanupStack::PopAndDestroy(2); //def, contactItem
    
    iDb->DeleteContactL(id);
        
    contactItem = CContactCard::NewL(iTemplate); 
    CleanupStack::PushL(contactItem);
    
    itemFieldPtr = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldFamilyName );
    textStoragePtr = itemFieldPtr->TextStorage();
    textStoragePtr->SetTextL( _L("A") );
    contactItem->AddFieldL( *itemFieldPtr );
    CleanupStack::Pop( itemFieldPtr ); 
    id = iDb->AddNewContactL(*contactItem);
    CleanupStack::PopAndDestroy( contactItem );
   
    // retrieve the contact add an image to it
    CContactItem *contactItem1 = iDb->OpenContactL( id );
    CleanupStack::PushL( contactItem1 );
    itemFieldPtr_1 = CContactItemField::NewLC( KStorageTypeStore, KUidContactFieldVCardMapJPEG );
    binaryStoragePtr = itemFieldPtr_1->StoreStorage();   
    binaryStoragePtr->SetThingL(_L8("Jpeg"));
   
    contactItem1->AddFieldL( *itemFieldPtr_1 ); 
    CleanupStack::Pop( itemFieldPtr_1 );
    iDb->CommitContactL(*contactItem1);
    iDb->CloseContactL( id );
    CleanupStack::PopAndDestroy( contactItem1 );
   
    //retrieve the contact and add voip to it
    CContactItem *contactItem2 = iDb->OpenContactL( id );
    CleanupStack::PushL( contactItem2 );
    CContactItemField* itemFieldPtr2 = CContactItemField::NewLC( KStorageTypeText, KUidContactFieldSIPID );
    CContactTextField* textStoragePtr2 = itemFieldPtr2->TextStorage();
    textStoragePtr2->SetTextL( aTextToFind );
    contactItem2->AddFieldL( *itemFieldPtr2 );
    CleanupStack::Pop( itemFieldPtr2 );
    iDb->CommitContactL( *contactItem2 );
    iDb->CloseContactL( id );
    CleanupStack::PopAndDestroy( contactItem2 );
  
    // find the contact with voip number
    def = new(ELeave) CContactItemFieldDef();
    CleanupStack::PushL(def);
    def->AppendL(KUidContactFieldSIPID);
    if(!aAsync)
        {
        DoFindL(aTextToFind,*def,aNumExpectedFinds);
        }
    else
        {
        DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
        }

    CleanupStack::PopAndDestroy(1); 
   
    iDb->DeleteContactL(id);
    }

void CFindTest::FindInAllFieldsL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in All fields"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldMatchAll);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

void CFindTest::FindInCompanyFieldL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Company name field"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldCompanyName);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

void CFindTest::FindPhoneNumberL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Phone fields"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldPhoneNumber);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}

void CFindTest::FindEmailAddressL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Email fields"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldEMail);
	if(!aAsync)
		{
		DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}
	
void CFindTest::FindNotesL(const TDesC& aTextToFind, const TInt aNumExpectedFinds,TBool aAsync)
	{
	iLog->LogLine(_L("Find %S in Email fields"),&aTextToFind);	
	CContactItemFieldDef* def=new(ELeave) CContactItemFieldDef();
	CleanupStack::PushL(def);
	def->AppendL(KUidContactFieldNote);
	
	if(!aAsync)
		{	
	    DoFindL(aTextToFind,*def,aNumExpectedFinds);
		}
	else
		{
		DoFindAsyncL(aTextToFind,*def,aNumExpectedFinds);
		}
	CleanupStack::PopAndDestroy(); // def.
	}	

void CFindTest::DoFindL(const TDesC& aTextToFind,const CContactItemFieldDef& aFieldDef, const TInt aNumExpectedFinds)
	{
	CContactIdArray* matchList=iDb->FindLC(aTextToFind,&aFieldDef);

	const TInt numIds=matchList->Count();
	iLog->LogLine(_L("Matched %d contact(s) of %d expected"),numIds,aNumExpectedFinds);
	if(numIds!=aNumExpectedFinds)
		{
		iLog->LogLine(_L("%S FAILED!"),&KTestName);
		}
	iTest(numIds==aNumExpectedFinds);
	CleanupStack::PopAndDestroy(); // matchList.
	}

void CFindTest::DoFindAsyncL(const TDesC& aTextToFind,const CContactItemFieldDef& aFieldDef, const TInt aNumExpectedFinds)
	{
	iIdleFinder=iDb->FindAsyncL(aTextToFind,&aFieldDef,this);
	CleanupStack::PushL(iIdleFinder); // Handle on cleanup stack because object is very temporary.
	CActiveScheduler::Start();

	CContactIdArray* matchList=iIdleFinder->TakeContactIds();
	CleanupStack::PushL(matchList);
	const TInt numIds=matchList->Count();
	iLog->LogLine(_L("Matched %d contact(s) of %d expected"),numIds,aNumExpectedFinds);
	if(numIds!=aNumExpectedFinds)
		{
		iLog->LogLine(_L("%S FAILED!"),&KTestName);
		}
	iTest(numIds==aNumExpectedFinds);
	CleanupStack::PopAndDestroy(2); // matchList.  iIdleFinder.
	}

void CFindTest::IdleFindCallback()
	{
	if (iIdleFinder->IsComplete())
		{
		CActiveScheduler::Stop();
		}
	}

//
// Main.
//

GLDEF_C TInt E32Main()
	{
	RDebug::Print(_L("t_cntfindall started"));
	__UHEAP_MARK;
	
	CActiveScheduler* sheduler=new CActiveScheduler;
	if (sheduler)
		{
		CActiveScheduler::Install(sheduler);
		CTrapCleanup* cleanup=CTrapCleanup::New();
		if (cleanup)
			{
			CFindTest* findtest=new CFindTest();//::NewL();
			if (findtest)
				{
				TRAPD(err, findtest->ConstructL();	findtest->DoTestsL(););
				RDebug::Print(_L("t_cntFindAll finish with %d error"),err);
				delete findtest;
				}
			delete cleanup;
			}
		delete sheduler;
		}
	__UHEAP_MARKEND;
	return KErrNone;
    }
