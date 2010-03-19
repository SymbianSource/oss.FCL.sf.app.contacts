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
* Description: 
*
*/

#include "pltables.h"
#include "dbsqlconstants.h"
#include "cntitem.h"
#include "cpcskeymap.h"


/**
@param aDatabase A handle to the database.
@param aProperties A contact properties object.

@return A pointer to a new CPplPredictiveSearchTable object.
*/
CPplPredictiveSearchTable*
CPplPredictiveSearchTable::NewL(RSqlDatabase& aDatabase)
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::NewL"));
	CPplPredictiveSearchTable* self = CPplPredictiveSearchTable::NewLC(aDatabase);
	CleanupStack::Pop(self);
	RDebug::Print(_L("CPplPredictiveSearchTable::NewL ends"));
	return self;
	}


/**
@param aDatabase A handle to the database.
@param aProperties A contact properties object.

@return A pointer to a new CPplPredictiveSearchTable object.
*/
CPplPredictiveSearchTable*
CPplPredictiveSearchTable::NewLC(RSqlDatabase& aDatabase)
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::NewLC"));
	CPplPredictiveSearchTable* self =
		new (ELeave) CPplPredictiveSearchTable(aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	RDebug::Print(_L("CPplPredictiveSearchTable::NewLC ends"));
	return self;
	}

/**
Destructor
*/
CPplPredictiveSearchTable::~CPplPredictiveSearchTable()
	{
	RDebug::Print(_L("CPplPredictiveSearchTable dtor"));
	delete iInsertStmnt;
	delete iUpdateStmnt;
	delete iDeleteStmnt;
	delete iKeyMap;
	RDebug::Print(_L("CPplPredictiveSearchTable dtor ends"));
	}


/**
@param aItem A contact item whose data are added to the table.
*/
void CPplPredictiveSearchTable::CreateInDbL( CContactItem& aItem )
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::CreateInDbL"));
	WriteToDbL( aItem, iInsertStmnt );
	RDebug::Print(_L("CPplPredictiveSearchTable::CreateInDbL ends"));
	}


/**
@param aItem A contact item whose data is updated in the database.
*/
void CPplPredictiveSearchTable::UpdateL( const CContactItem& aItem )
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::UpdateL"));
	WriteToDbL( aItem, iUpdateStmnt );
	RDebug::Print(_L("CPplPredictiveSearchTable::UpdateL ends"));
	}


/**
Deletes all the communication addresses for a particular contact item. Should be used when
deleting a contact item from the database altogether.

@param aItem The contact item whose communcation addresses are to be deleted.
*/
void CPplPredictiveSearchTable::DeleteL(const CContactItem& aItem, TBool& aLowDiskErrorOccurred)
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::DeleteL"));
	RSqlStatement stmnt;
	CleanupClosePushL(stmnt);
	stmnt.PrepareL(iDatabase, iDeleteStmnt->SqlStringL());
	
	// Contact id was not added with iDeleteStmnt->SetParamL() so it can not be
	// accessed with iDeleteStmnt->ParameterIndex().
	// It is the first and only parameter in query
	const TInt KContactIdParamIndex(KFirstIndex);
	User::LeaveIfError(stmnt.BindInt(KContactIdParamIndex, aItem.Id()));
	RDebug::Print(_L("CPplPredictiveSearchTable::DeleteL execute SQL statement"));
	// Returns the amount of rows that were changed/inserted/deleted
	TInt status = stmnt.Exec();
	RDebug::Print(_L("CPplPredictiveSearchTable::DeleteL rows deleted=%d"), status);
	CleanupStack::PopAndDestroy(&stmnt);

	if (status == KErrDiskFull)
		{
		RDebug::Print(_L("CPplPredictiveSearchTable::DeleteL disk full"));
		aLowDiskErrorOccurred = ETrue;
		}
	else
		{
		RDebug::Print(_L("CPplPredictiveSearchTable::DeleteL status=%d"), status);
		User::LeaveIfError(status);
		}
	RDebug::Print(_L("CPplPredictiveSearchTable::DeleteL"));
	}


/**
Creates the comm_addr table and its indexes in the database.
*/
void CPplPredictiveSearchTable::CreateTableL()
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::CreateTableL"));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateStmnt));
	
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateIndex));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateView1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateView2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateView3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateView4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateView5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateView6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateView7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateView8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateView9));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateView0));

	RDebug::Print(_L("CPplPredictiveSearchTable::CreateTableL ends"));
	}


/**
Set up the CCntSqlStatement objects held by the class.
*/
void CPplPredictiveSearchTable::ConstructL()
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::ConstructL"));
	// Statement types
	TCntSqlStatementType insertType( EInsert, KSqlContactPredSearchTableName() );
	TCntSqlStatementType updateType( EUpdate, KSqlContactPredSearchTableName() );
	TCntSqlStatementType deleteType( EDelete, KSqlContactPredSearchTableName() );


	// Insert new record
	// INSERT INTO predictivesearch
	//   (first_name_as_number, last_name_as_number, company_name_as_number, contact_id)
	//   VALUES (first_name_as_number value, last_name_as_number value,
	//	   		 company_name_as_number value, contact_id value);
	//
	iInsertStmnt = TSqlProvider::GetSqlStatementL(insertType);
	iInsertStmnt->SetParamL( KPredSearchFirstNameAsNumber,
							 KPredSearchFirstNameAsNumberParam );
	iInsertStmnt->SetParamL( KPredSearchLastNameAsNumber,
							 KPredSearchLastNameAsNumberParam );
	iInsertStmnt->SetParamL( KPredSearchCompanyNameAsNumber,
							 KPredSearchCompanyNameAsNumberParam );
	iInsertStmnt->SetParamL( KPredSearchContactId,
							 KPredSearchContactIdParam );


	// Update predictivesearch record (done when contact info or key map changes)
	// For a statement in the following format:
	// 	UPDATE predictivesearch SET
	//		first_name_as_number = [first_name_as_number value],
	//		last_name_as_number = [last_name_as_number value],
	//		company_name_as_number = [company_name_as_number value]
	//		WHERE contact_id = [contact_id value];
	//
	iUpdateStmnt = TSqlProvider::GetSqlStatementL(updateType);
	iUpdateStmnt->SetParamL( KPredSearchFirstNameAsNumber,
						     KPredSearchFirstNameAsNumberParam );
	iUpdateStmnt->SetParamL( KPredSearchLastNameAsNumber,
						     KPredSearchLastNameAsNumberParam );
	iUpdateStmnt->SetParamL( KPredSearchCompanyNameAsNumber,
						     KPredSearchCompanyNameAsNumberParam );
	iUpdateStmnt->SetParamL( KPredSearchContactId,
						     KPredSearchContactIdParam );

	const TInt KWhereContactIdBufSize(
		KWhereStringEqualsStringFormatText().Size() +
		KPredSearchContactId().Size() +
		KPredSearchContactIdParam().Size() );
	HBufC* whereContactIdClause = HBufC::NewLC(KWhereContactIdBufSize);
	// for WHERE contact_id = [contact id value]
	whereContactIdClause->Des().AppendFormat(KWhereStringEqualsStringFormatText,
		&KPredSearchContactId, &KPredSearchContactIdParam);
	iUpdateStmnt->SetConditionL(*whereContactIdClause);


	// Delete information of a particular contact item
	// 	DELETE FROM predictivesearch WHERE contact_id = [contact id value];
	//
	iDeleteStmnt = TSqlProvider::GetSqlStatementL(deleteType);
	iDeleteStmnt->SetConditionL(*whereContactIdClause);

	CleanupStack::PopAndDestroy(whereContactIdClause);
		
	RDebug::Print(_L("CPplPredictiveSearchTable::ConstructL create key map"));
	iKeyMap = CPcsKeyMap::NewL();

	RDebug::Print(_L("CPplPredictiveSearchTable::ConstructL ends"));
	}


/**
Constructor
*/
CPplPredictiveSearchTable::CPplPredictiveSearchTable(RSqlDatabase& aDatabase) :
	iDatabase(aDatabase)
	{
	}


// This function does the common parts of both insert and update operations
void CPplPredictiveSearchTable::WriteToDbL( const CContactItem& aItem,
										    CCntSqlStatement* aStatement )
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL"));
	RSqlStatement stmnt;
	CleanupClosePushL( stmnt );
 	stmnt.PrepareL( iDatabase, aStatement->SqlStringL() );
 	
 	// Fill parameters and their values 
 	CContactItemFieldSet& fieldset = aItem.CardFields();

 	TInt pos = fieldset.Find( KUidContactFieldGivenName );
 	if ( pos != KErrNotFound )
 	    {
		RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL first name"));
        CContactTextField* textfield = fieldset[ pos ].TextStorage();
        if ( textfield )
            {
            TPtrC firstName = textfield->Text();
            HBufC* firstNameAsNbr = ConvertToNumericRepresentationLC( firstName );
            User::LeaveIfError( stmnt.BindText(
                User::LeaveIfError( stmnt.ParameterIndex( KPredSearchFirstNameAsNumberParam ) ),
                *firstNameAsNbr ) );
            CleanupStack::PopAndDestroy( firstNameAsNbr );
            }
 	    }

	pos = fieldset.Find( KUidContactFieldFamilyName );
	if ( pos != KErrNotFound )
	    {
		RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL last name"));
        CContactTextField* textfield = fieldset[ pos ].TextStorage();
        if ( textfield )
            {
            TPtrC lastName = textfield->Text();
            HBufC* lastNameAsNbr = ConvertToNumericRepresentationLC( lastName );
            User::LeaveIfError( stmnt.BindText(
                User::LeaveIfError( stmnt.ParameterIndex( KPredSearchLastNameAsNumberParam ) ),
                *lastNameAsNbr ) );
            CleanupStack::PopAndDestroy( lastNameAsNbr );
            }
	    }

	pos = fieldset.Find( KUidContactFieldCompanyName );
	if ( pos != KErrNotFound )
	    {
		RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL company name"));
        CContactTextField* textfield = fieldset[ pos ].TextStorage();
        if ( textfield )
            {
            TPtrC companyName = textfield->Text();
            HBufC* companyNameAsNbr = ConvertToNumericRepresentationLC( companyName );
            User::LeaveIfError( stmnt.BindText(
                User::LeaveIfError( stmnt.ParameterIndex( KPredSearchCompanyNameAsNumberParam ) ),
                *companyNameAsNbr ) );
            CleanupStack::PopAndDestroy( companyNameAsNbr );
            }
	    }

	RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL set contact id"));
	// When inserting, contact id is written.
	// When updating, contact id is the search key.
	User::LeaveIfError( stmnt.BindInt(
		User::LeaveIfError( stmnt.ParameterIndex( KPredSearchContactIdParam ) ),
		aItem.Id() ) );

	RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL execute SQL statement"));
	// Execute the SQL statement
	User::LeaveIfError( stmnt.Exec() );
	CleanupStack::PopAndDestroy( &stmnt );

	RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL ends"));
	}

HBufC* CPplPredictiveSearchTable::ConvertToNumericRepresentationLC( const TDesC& aString )
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::ConvertToNumericRepresentationLC"));
	HBufC* numericBuf = iKeyMap->GetNumericKeyStringL( aString, EFalse );
	CleanupStack::PushL( numericBuf );
	RDebug::Print(_L("CPplPredictiveSearchTable::ConvertToNumericRepresentationLC result='%S'"),
				  numericBuf);
	return numericBuf;
	}
