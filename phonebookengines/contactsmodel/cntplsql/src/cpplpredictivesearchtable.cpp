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
#include <QStringList>


// How many characters from the beginning of the first name and last name are
// stored. This only affects how precisely the results are put in alphabetic order.
const TInt KCharactersFromName = 16;

// Max amount of tokens stored from contact
const TInt KMaxTokens = 4;

// How many digits are stored at most in the numeric field
// Since BIGINT is a signed 64-bit integer, store only 15 hexadecimal characters
// to prevent overflow when comparing upper and lower limits.
const TInt KMaxDigits = 15;

const QChar KPadChar = 'a'; // Pad with hex-digit 0xA

const quint64 KConversionError = 0xeeeeeeeeeeeeeee;



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
	WriteToDbL(aItem);
	RDebug::Print(_L("CPplPredictiveSearchTable::CreateInDbL ends"));
	}


/**
Update is done in two steps: delete contact from all predictive search tables,
then insert it into relevant tables.

@param aItem A contact item whose data is updated in the database.
*/
void CPplPredictiveSearchTable::UpdateL( const CContactItem& aItem )
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::UpdateL"));

	TBool lowDiskErrorOccurred(EFalse);
	DeleteFromAllTablesL(aItem.Id(), lowDiskErrorOccurred);
	if (lowDiskErrorOccurred)
	    {
        User::Leave(KErrGeneral);
	    }
	WriteToDbL(aItem);

	RDebug::Print(_L("CPplPredictiveSearchTable::UpdateL ends"));
	}


/**
Deletes a contact item from predictive search tables.

@param aItem The contact item to be deleted. It contains contact id, but not
             first name or last name fields.
*/
void CPplPredictiveSearchTable::DeleteL(const CContactItem& aItem,
										TBool& aLowDiskErrorOccurred)
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::DeleteL"));

	DeleteFromAllTablesL(aItem.Id(), aLowDiskErrorOccurred); 

	RDebug::Print(_L("CPplPredictiveSearchTable::DeleteL ends"));
	}


/**
Creates the comm_addr table and its indexes in the database.
*/
void CPplPredictiveSearchTable::CreateTableL()
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::CreateTableL"));

	RDebug::Print(_L("Create 10 tables"));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable0Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable1Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable2Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable3Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable4Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable5Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable6Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable7Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable8Stmnt));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateTable9Stmnt));


	RDebug::Print(_L("Create indexes"));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable0));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable0));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable0));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable0));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable1));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable2));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable3));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable4));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable5));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable6));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable7));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable8));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbrIndexTable9));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr2IndexTable9));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr3IndexTable9));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateNbr4IndexTable9));

	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable0));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable0));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable1));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable2));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable3));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable4));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable5));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable6));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable7));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable8));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateFNIndexInTable9));
	User::LeaveIfError(iDatabase.Exec(KPredSearchCreateLNIndexInTable9));

	RDebug::Print(_L("CPplPredictiveSearchTable::CreateTableL ends"));
	}


/**
Set up the CCntSqlStatement objects held by the class.
*/
void CPplPredictiveSearchTable::ConstructL()
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::ConstructL"));

	// Using dummy table names here
	TCntSqlStatementType insertType( EInsert, KSqlContactPredSearchTable0 );
	TCntSqlStatementType deleteType( EDelete, KSqlContactPredSearchTable0 );

	// Insert new record
	// INSERT INTO predictivesearchX (X=0..9)
	//   (contact_id, nbr, nbr2, nbr3, nbr4, first_name, last_name)
	//   VALUES (contact_id value, nbr value, nbr2 value, nbr3 value, nbr4 value,
	//			 first_name value, last_name value);
	iInsertStmnt = TSqlProvider::GetSqlStatementL(insertType);
	iInsertStmnt->SetParamL( KPredSearchContactId,
	                         KPredSearchContactIdParam );
	iInsertStmnt->SetParamL( KPredSearchNameAsNumber,
							 KPredSearchNameAsNumberParam );
	iInsertStmnt->SetParamL( KPredSearchNameAsNumber2,
							 KPredSearchNameAsNumber2Param );
	iInsertStmnt->SetParamL( KPredSearchNameAsNumber3,
							 KPredSearchNameAsNumber3Param );
	iInsertStmnt->SetParamL( KPredSearchNameAsNumber4,
							 KPredSearchNameAsNumber4Param );
	iInsertStmnt->SetParamL( KPredSearchFirstName,
							 KPredSearchFirstNameParam );
	iInsertStmnt->SetParamL( KPredSearchLastName,
							 KPredSearchLastNameParam );

	const TInt KWhereContactIdBufSize(
		KWhereStringEqualsStringFormatText().Size() +
		KPredSearchContactId().Size() +
		KPredSearchContactIdParam().Size() );
	HBufC* whereContactIdClause = HBufC::NewLC(KWhereContactIdBufSize);
	// for WHERE contact_id = [contact id value]
	whereContactIdClause->Des().AppendFormat(KWhereStringEqualsStringFormatText,
		&KPredSearchContactId, &KPredSearchContactIdParam);

	// Delete information of a particular contact item
	// 	DELETE FROM predictivesearchX (X=0..9)
	//	WHERE contact_id = [contact id value];
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


// Insert a contact to predictive search tables.
// Write contact's all tokens to each associate pred.search table.
// E.g. if FN="11 22" LN="2 333", write "11","22","2" and "333" to tables 1, 2 and 3.
void CPplPredictiveSearchTable::WriteToDbL(const CContactItem& aItem)
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL"));

	HBufC* firstNameAsNbr(NULL); // owned
    HBufC* lastNameAsNbr(NULL);  // owned
	HBufC* firstName(NULL); // owned
    HBufC* lastName(NULL);  // owned
	GetFieldsLC(aItem, &firstNameAsNbr, &lastNameAsNbr, &firstName, &lastName);

	QStringList numericTokens = GetNumericTokens(firstNameAsNbr, lastNameAsNbr);
	QList<TChar> tables = DetermineTables(numericTokens);
	HBufC* tableName(NULL);
	while ((tableName = GetTableNameL(tables)) != NULL)
		{
		// Takes ownership. Clears also earlier SQL statement.
		iInsertStmnt->SetTableName(tableName);
		RSqlStatement stmnt;
		CleanupClosePushL( stmnt ); 
		RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL SQL='%S'"),
					  &iInsertStmnt->SqlStringL());
		stmnt.PrepareL(iDatabase, iInsertStmnt->SqlStringL());

		const TDesC* paramNames[] = {
			&KPredSearchNameAsNumberParam,
			&KPredSearchNameAsNumber2Param,
			&KPredSearchNameAsNumber3Param,
			&KPredSearchNameAsNumber4Param};
		for (TInt i = 0; i < numericTokens.count(); ++i)
			{
			quint64 hex = ConvertToHex(numericTokens[i]);
			if (hex == KConversionError)
				{
				User::Leave(KErrArgument);
				}
			User::LeaveIfError(stmnt.BindInt64(
				User::LeaveIfError(stmnt.ParameterIndex(*paramNames[i])), hex));
			}

		User::LeaveIfError(stmnt.BindInt(
			User::LeaveIfError(stmnt.ParameterIndex(KPredSearchContactIdParam)),
			aItem.Id()));

		if (firstName)
			{
			User::LeaveIfError( stmnt.BindText(
				User::LeaveIfError( stmnt.ParameterIndex( KPredSearchFirstNameParam ) ),
				*firstName ) );
			}
		if (lastName)
			{
			User::LeaveIfError( stmnt.BindText(
				User::LeaveIfError( stmnt.ParameterIndex( KPredSearchLastNameParam ) ),
				*lastName ) );
			}

		RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL execute SQL statement"));
		// Execute the SQL statement
		User::LeaveIfError( stmnt.Exec() );
		CleanupStack::PopAndDestroy( &stmnt );
		}

	CleanupStack::PopAndDestroy( lastNameAsNbr );
	CleanupStack::PopAndDestroy( lastName );
	CleanupStack::PopAndDestroy( firstNameAsNbr );
	CleanupStack::PopAndDestroy( firstName );

	RDebug::Print(_L("CPplPredictiveSearchTable::WriteToDbL ends"));
	}

void CPplPredictiveSearchTable::GetFieldsLC(const CContactItem& aItem,
											HBufC** aFirstNameAsNbr,
											HBufC** aLastNameAsNbr,
											HBufC** aFirstName,
											HBufC** aLastName) const
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::GetFieldsLC"));
	__ASSERT_ALWAYS(aFirstNameAsNbr != NULL && *aFirstNameAsNbr == NULL,
				    User::Leave(KErrArgument));
	__ASSERT_ALWAYS(aLastNameAsNbr != NULL && *aLastNameAsNbr == NULL,
				    User::Leave(KErrArgument));
	__ASSERT_ALWAYS(aFirstName != NULL && *aFirstName == NULL,
					User::Leave(KErrArgument));
	__ASSERT_ALWAYS(aLastName != NULL && *aLastName == NULL,
					User::Leave(KErrArgument));

	CContactItemFieldSet& fieldset = aItem.CardFields();
    TInt pos = fieldset.Find(KUidContactFieldGivenName);
    if (pos != KErrNotFound)
        {
        CContactTextField* textfield = fieldset[pos].TextStorage();
        if (textfield)
            {
            TPtrC firstName = textfield->Text();
			*aFirstName = firstName.Left(KCharactersFromName).AllocLC();
			*aFirstNameAsNbr = iKeyMap->GetNumericKeyStringL(firstName, EFalse);
            }
        }
	// If aFirstName was not pushed to cleanupstack above, do it now
	if (*aFirstName == NULL)
		{
		CleanupStack::PushL(*aFirstName);
		}
	CleanupStack::PushL(*aFirstNameAsNbr);

    pos = fieldset.Find(KUidContactFieldFamilyName);
    if (pos != KErrNotFound)
        {
        CContactTextField* textfield = fieldset[pos].TextStorage();
        if (textfield)
            {
            TPtrC lastName = textfield->Text();
			*aLastName = lastName.Left(KCharactersFromName).AllocLC();
			*aLastNameAsNbr = iKeyMap->GetNumericKeyStringL(lastName, EFalse);
            }
        }
	// If aLastName was not pushed to cleanupstack above, do it now
	if (*aLastName == NULL)
		{
		CleanupStack::PushL(*aLastName);
		}
	CleanupStack::PushL(*aLastNameAsNbr);

	RDebug::Print(_L("CPplPredictiveSearchTable::GetFieldsLC id=%d FNnbr='%S' LNnbr='%S' FN='%S' LN='%S'"),
		aItem.Id(),
	    *aFirstNameAsNbr ? *aFirstNameAsNbr : &KNullDesC,
	    *aLastNameAsNbr ? *aLastNameAsNbr : &KNullDesC,
		*aFirstName ? *aFirstName : &KNullDesC,
	    *aLastName ? *aLastName: &KNullDesC);
	}

// Find out which tables the contact belongs to.
// e.g. FN(first name)="123 456", LN(last name)=" 89 15" -> belongs to tables 1,4 and 8
QList<TChar>
CPplPredictiveSearchTable::DetermineTables(HBufC* aFirstName,
										   HBufC* aLastName) const
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::DetermineTables FN='%S',LN='%S'"),
		aFirstName ? aFirstName : &KNullDesC,
		aLastName ? aLastName : &KNullDesC);

	QList<TChar> tables;
	AddBeginningCharacters(aFirstName, tables);
	AddBeginningCharacters(aLastName, tables);

    RDebug::Print(_L("CPplPredictiveSearchTable::DetermineTables belongs to %d tables"),
                  tables.count());
	return tables;
	}

QList<TChar> CPplPredictiveSearchTable::DetermineTables(QStringList aTokens) const
	{
	QList<TChar> tables;
	for (TInt i = aTokens.count() - 1; i >= 0; --i)
		{
		TChar ch(aTokens[i][0].unicode());
		__ASSERT_ALWAYS(IsValidChar(ch),
						User::Panic(_L("DetermineTables"), KErrArgument));
		if (!tables.contains(ch))
			{
			tables.append(ch);
			}
		}
	return tables;
	}

// Ignore spaces when inspecting the first digits of (sub)strings within FN/LN.
// If FN and LN don't begin with any of digit (meaning the first char in the
// original name did not map into any digit, but was written as is, the contact
// is not added to any of the 10 tables).
void CPplPredictiveSearchTable::AddBeginningCharacters(HBufC* aString,
													   QList<TChar>& aTables) const
	{
	if (aString)
		{
		QString s((QChar*)aString->Ptr(), aString->Length());
#if defined(USE_ORBIT_KEYMAP)
		QStringList subStrings = s.split(iKeyMap->Separator(), QString::SkipEmptyParts);
#else
		QStringList subStrings = s.split(' ', QString::SkipEmptyParts);
#endif
	
		for (TInt i = subStrings.count() - 1; i >= 0; --i)
			{
			if (subStrings[i].length() > 0)
				{
				TChar ch(subStrings[i][0].unicode());
				if (IsValidChar(ch) && !aTables.contains(ch))
					{
					aTables.append(ch);
					}
				}
			}
		}
	}

// Ignore tokens beginning with something else than '0'..'9'.
// Keep duplicate tokens to support e.g. search "202" when both FN and LN are "23".
void
CPplPredictiveSearchTable::AddTokens(HBufC* aString, QStringList& aTokens) const
	{
	if (aString)
		{
		QString s((QChar*)aString->Ptr(), aString->Length());
#if defined(USE_ORBIT_KEYMAP)
		QStringList tokens = s.split(iKeyMap->Separator(), QString::SkipEmptyParts);
#else
		QStringList tokens = s.split(' ', QString::SkipEmptyParts);
#endif

		// Select tokens in the same order they are in original aString
		for (TInt i = 0; i < tokens.count(); ++i)
			{
			if (IsValidChar(tokens[i][0]))
				{
				aTokens.append(tokens[i]);
				}
			}
		}
	}

TBool CPplPredictiveSearchTable::IsValidChar(TInt aChar) const
	{
	return (aChar >= '0' && aChar <= '9');
	}

TBool CPplPredictiveSearchTable::IsValidChar(QChar aChar) const
	{
	return (aChar >= '0' && aChar <= '9');
	}

// 1. get first token of LN
// 2. get first token of FN
// 3. get second token of LN
// 4. get second token of FN
// :
// :
// If LN or FN runs out of tokens before KMaxTokens have been found,
// keep getting tokens from the other field.
QStringList CPplPredictiveSearchTable::GetNumericTokens(HBufC* aFirstName,
													    HBufC* aLastName) const
	{
	RDebug::Print(_L("CPplPredictiveSearchTable::GetNumericTokens FN='%S',LN='%S'"),
		aFirstName ? aFirstName : &KNullDesC,
		aLastName ? aLastName : &KNullDesC);

	QStringList firstNameTokens;
	QStringList lastNameTokens;
	AddTokens(aFirstName, firstNameTokens);
	AddTokens(aLastName, lastNameTokens);

	QStringList tokens;
	while (tokens.count() < KMaxTokens &&
		   (!firstNameTokens.isEmpty() || !lastNameTokens.isEmpty()))
		{
		GetNextToken(lastNameTokens, tokens);
		GetNextToken(firstNameTokens, tokens);
		}
	RDebug::Print(_L("CPplPredictiveSearchTable::GetNumericTokens found %d tokens"),
                  tokens.count());
	return tokens;
	}

void CPplPredictiveSearchTable::GetNextToken(QStringList& aSource,
											 QStringList& aDestination) const
	{
	if (!aSource.isEmpty() && aDestination.count() < KMaxTokens)
		{
        QString padded = aSource[0].left(KMaxDigits);
		aDestination.append(padded);
		aSource.removeFirst();
		}
	}

void
CPplPredictiveSearchTable::DeleteFromAllTablesL(TContactItemId aContactId,
											    TBool& aLowDiskErrorOccurred) const
	{
	QList<TChar> tables;
	const TInt KLargestKey = '9';
	for (TInt i = '0'; i <= KLargestKey; ++i)
		{
		TChar ch = i;
		tables << ch;
		}

	HBufC* tableName(NULL);
	while ((tableName = GetTableNameL(tables)) != NULL)
		{
		iDeleteStmnt->SetTableName(tableName); // Clears also earlier SQL statement

		RSqlStatement stmnt;
		CleanupClosePushL(stmnt);

		RDebug::Print(_L("CPplPredictiveSearchTable::DeleteFromAllTablesL SQL='%S'"),
					  &iDeleteStmnt->SqlStringL());
		stmnt.PrepareL(iDatabase, iDeleteStmnt->SqlStringL());
	
		// Contact id was not added with iDeleteStmnt->SetParamL() so it can not be
		// accessed with iDeleteStmnt->ParameterIndex().
		// It is the first and only parameter in query
		const TInt KContactIdParamIndex(KFirstIndex);
		User::LeaveIfError(stmnt.BindInt(KContactIdParamIndex, aContactId));
		RDebug::Print(_L("CPplPredictiveSearchTable::DeleteFromAllTablesL execute statement"));
		// Returns the amount of rows that were changed/inserted/deleted.
		// Since contact is not present in all tables, some operations return 0,
		// it is not an error.
		TInt status = stmnt.Exec();
		RDebug::Print(_L("CPplPredictiveSearchTable::DeleteFromAllTablesL rows deleted=%d"), status);
		CleanupStack::PopAndDestroy(&stmnt);

		if (status == KErrDiskFull)
			{
			RDebug::Print(_L("CPplPredictiveSearchTable::DeleteFromAllTablesL disk full"));
			aLowDiskErrorOccurred = ETrue;
			}
		else
			{
			RDebug::Print(_L("CPplPredictiveSearchTable::DeleteFromAllTablesL status=%d"), status);
			User::LeaveIfError(status);
			}
		}
	}

HBufC* CPplPredictiveSearchTable::GetTableNameL(QList<TChar>& aTables) const
	{
	HBufC* tableName(NULL);
	if (aTables.count() > 0)
		{
		TChar ch = aTables[0];
		__ASSERT_ALWAYS(IsValidChar(ch), User::Leave(KErrArgument));

        // Each table's name has same length, replace last char with the correct one
        tableName = HBufC::NewL(KSqlContactPredSearchTable0().Length());
        TPtr ptr = tableName->Des();
        ptr.Append(KSqlContactPredSearchTable0);
        ptr[ptr.Length() - 1] = ch;
		aTables.removeFirst();
		RDebug::Print(_L("CPplPredictiveSearchTable::GetTableNameL '%S'"), tableName);
        }
	return tableName;
	}

// E.g. aToken = "01230" -> append 'a' until has KMaxDigits characters
// -> "01230aaaaaaaaaa" -> convert to hexadecimal number -> 0x01230aaaaaaaaaa.
// Leaving from this function causes panic, perhaps because of QString
// parameter? So rather return an error code if conversion fails.
quint64 CPplPredictiveSearchTable::ConvertToHex(QString aToken) const
	{
	if (aToken.length() > KMaxDigits)
		{
		return KConversionError;
		}
    QString padded = aToken.leftJustified(KMaxDigits, KPadChar);

    TBuf<KMaxDigits> log(padded.utf16());
    RDebug::Print(_L("CPplPredictiveSearchTable::ConvertToHex padded '%S'"), &log);

	// Replace unmapped char and the following characters with KPadChar.
    QString replaced = padded;
    bool done(false);
    for (TInt i = 0; i < KMaxDigits && !done; ++i)
        {
        if (!IsValidChar(padded[i]))
            {
            // replace() does not work, it puts just one 'a' at end
            // replaced = padded.replace(i, KMaxDigits - i, KPadChar);

            padded.remove(i, KMaxDigits - i);
            replaced = padded.leftJustified(KMaxDigits, KPadChar);
            done = true;

			TBuf<KMaxDigits> log2(replaced.utf16());
			RDebug::Print(_L("After replacing '%S'"), &log2);
            }
        }

    const TInt KHexadecimalBase = 16;
    bool ok(true);
    quint64 hex = replaced.toULongLong(&ok, KHexadecimalBase);
    if (!ok)
        {
		RDebug::Print(_L("conv to hex failed"));
		return KConversionError;
        }
    
    RDebug::Print(_L("CPplPredictiveSearchTable::ConvertToHex result 0x%lx"), hex);
    return hex;
	}
