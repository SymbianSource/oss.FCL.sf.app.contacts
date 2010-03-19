/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef DBSQLCONSTANTS_H
#define DBSQLCONSTANTS_H

#include <cntdef.hrh>

const TInt KInitialValue = -1;
const TInt KNumOfTables = 5;
const TInt KNumColInPrefTable = 6;
const TInt KNumColInContactTable = 16;
const TInt KNumColInGroupTable = 3;
const TInt KNumColInCommTable = 5;
const TInt KNumColInPredSearchTable = 4;
const TInt KNumColInPresenceTable = 5;

// tables in the contact database 
_LIT(KSqlContactTableName,"contact");
_LIT(KSqlContactGroupTableName,"groups");
_LIT(KSqlContactPrefTableName,"preferences");
_LIT(KSqlContactCommAddrTableName,"comm_addr");
_LIT(KSqlContactPredSearchTableName,"predictivesearch");
_LIT(KSqlContactPresenceTableName, "presence");

enum TDatabaseTables
	{
	EContactPrefTableName,
	EContactTableName,
	EContactGroupTableName,
	EContactCommAddressTableName,
	KContactPredSearchTableName
	};

// columns for contact table
_LIT(KContactId, "contact_id");
_LIT(KContactTemplateId, "template_id");
_LIT(KContactTypeFlags, "type_flags");
_LIT(KContactAccessCount, "access_count");
_LIT(KContactCreationDate, "creation_date");
_LIT(KContactLastModified, "last_modified");
_LIT(KContactGuidString, "guid_string");
_LIT(KContactFirstName, "first_name");
_LIT(KContactLastName, "last_name");
_LIT(KContactCompanyName, "company_name");
_LIT(KContactFirstNamePrn, "firstname_prn");
_LIT(KContactLastNamePrn, "lastname_prn");
_LIT(KContactCompanyNamePrn, "companyname_prn");
_LIT(KContactTextFieldHeader, "text_fields_header");
_LIT(KContactBinaryFieldHeader, "binary_fields_header");
_LIT(KContactTextFields, "text_fields");
_LIT(KContactBinaryFields, "binary_fields");
//end columns for contact table

// parameters for contact table
_LIT(KContactIdParam, ":contact_id");
_LIT(KContactTemplateIdParam, ":template_id");
_LIT(KContactTypeFlagsParam, ":type_flags");
_LIT(KContactTypeParam, ":contact_type");
_LIT(KAttributesAndHintParam, ":attr_and_hint");
_LIT(KContactAccessCountParam, ":access_count");
_LIT(KContactCreationDateParam, ":creation_date");
_LIT(KContactLastModifiedParam, ":last_modified");
_LIT(KContactGuidStringParam, ":guid_string");
_LIT(KContactFirstNameParam, ":first_name");
_LIT(KContactLastNameParam, ":last_name");
_LIT(KContactCompanyNameParam, ":company_name");
_LIT(KContactFirstNamePrnParam, ":firstname_prn");
_LIT(KContactLastNamePrnParam, ":lastname_prn");
_LIT(KContactCompanyNamePrnParam, ":companyname_prn");
_LIT(KContactTextFieldHeaderParam, ":text_fields_header");
_LIT(KContactBinaryFieldHeaderParam, ":binary_fields_header");
_LIT(KContactTextFieldsParam, ":text_fields");
_LIT(KContactBinaryFieldsParam, ":binary_fields");
//end parameters for contact table

// create statements
_LIT(KContactCreateStmnt, "CREATE TABLE contact (contact_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, template_id INTEGER, type_flags INTEGER, access_count INTEGER, creation_date , last_modified , guid_string VARCHAR(255) NULL, first_name VARCHAR(255) NULL, last_name VARCHAR(255) NULL, company_name VARCHAR(255) NULL, firstname_prn VARCHAR(255) NULL, lastname_prn VARCHAR(255) NULL, companyname_prn VARCHAR(255) NULL, text_fields_header BLOB, binary_fields_header BLOB, text_fields TEXT, binary_fields BLOB); CREATE INDEX contact_guid_string_idx  ON contact (guid_string);");
// end contact table

// comm-address table
// columns
_LIT(KCommAddrId, "comm_addr_id");
_LIT(KCommAddrType, "type");
_LIT(KCommAddrValue, "value");
_LIT(KCommAddrExtraValue, "extra_value");
_LIT(KCommAddrContactId, "contact_id");
// parameters
_LIT(KCommAddrIdParam, ":comm_addr_id");
_LIT(KCommAddrTypeParam, ":type");
_LIT(KCommAddrValueParam, ":value");
_LIT(KCommAddrExtraValueParam, ":extra_value");
_LIT(KCommAddrContactIdParam, ":contact_id");
// create statements
_LIT(KCommAddrCreateStmnt, "CREATE TABLE comm_addr (comm_addr_id  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, type INTEGER NOT NULL, value CHAR(255) NOT NULL, extra_value CHAR(255), contact_id INTEGER NOT NULL, FOREIGN KEY (contact_id) REFERENCES contact(contact_id)); CREATE INDEX comm_addr_val_idx  ON comm_addr (value); CREATE INDEX comm_addr_contact_id_idx  ON comm_addr (contact_id);");
// end comm_address table

// groups table
// columns
_LIT(KGroupId, "group_id");
_LIT(KGroupContactGroupId, "contact_group_id");
_LIT(KGroupContactGroupMemberId, "contact_group_member_id");
// parameters
_LIT(KGroupIdParam, ":group_id");
_LIT(KGroupContactGroupIdParam, ":contact_group_id");
_LIT(KGroupContactGroupMemberIdParam, ":contact_group_member_id");
// create statements
_LIT(KGroupsCreateStmnt, "CREATE TABLE groups (group_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, contact_group_id INTEGER NOT NULL, contact_group_member_id INTEGER NOT NULL, FOREIGN KEY (contact_group_id) REFERENCES contact(contact_id), FOREIGN KEY (contact_group_member_id) REFERENCES contact(contact_id) ); CREATE INDEX contact_group_id_idx ON groups (contact_group_id); CREATE INDEX contact_group_member_id_idx ON groups (contact_group_member_id);");
// end groups table

// preferences table
// columns
_LIT(KPrefPrefId,"preference_id");
_LIT(KPrefSchemaVersion,"data_schema_version");
_LIT(KPrefDatabaseUid,"database_uid");
_LIT(KPrefPrefCreationDate,"creation_date");
_LIT(KPrefMachineId, "machine_id");
_LIT(KPrefPreferredOrder, "preferred_sortorder");
_LIT(KPrefferredTemplateId,"preferred_cardtemplate_id");

// parameters
_LIT(KPrefIdParam, ":preference_id");
_LIT(KPrefSchemaVersionParam, ":data_schema_version");
_LIT(KPrefDatabaseUidParam, ":database_uid");
_LIT(KPrefCreationDateParam, ":creation_date");
_LIT(KPrefMachineIdParam, ":machine_id");
_LIT(KPrefPreferredOrderParam, ":preferred_sortorder");
_LIT(KPrefferredTemplateIdParam,":preferred_cardtemplate_id");

// create statements
_LIT(KPreferencesCreateStmnt,  "CREATE TABLE preferences (preference_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, data_schema_version INTEGER NOT NULL, preferred_cardtemplate_id INTEGER, machine_id , creation_date , preferred_sortorder BLOB); ");
// end preferences table

// predictive search table
// columns
_LIT(KPredSearchFirstNameAsNumber, "first_name_as_number");
_LIT(KPredSearchLastNameAsNumber, "last_name_as_number");
_LIT(KPredSearchCompanyNameAsNumber, "company_name_as_number");
_LIT(KPredSearchContactId, "contact_id");
// Do not store names. They can be looked up from contact table.
// Do not store phone number. It can be searched from comm-address table.

// parameters
_LIT(KPredSearchFirstNameAsNumberParam, ":first_name_as_number");
_LIT(KPredSearchLastNameAsNumberParam, ":last_name_as_number");
_LIT(KPredSearchCompanyNameAsNumberParam, ":company_name_as_number");
_LIT(KPredSearchContactIdParam, ":contact_id");

// create statements
_LIT(KPredSearchCreateStmnt,
"CREATE TABLE predictivesearch (first_name_as_number VARCHAR(255) NULL,\
 last_name_as_number VARCHAR(255) NULL,\
 company_name_as_number VARCHAR(255) NULL, contact_id INTEGER);");
 
// create table index
_LIT(KPredSearchCreateIndex,
"CREATE INDEX index_last_name_as_number \
on predictivesearch (last_name_as_number, first_name_as_number );");
 
_LIT(KPredSearchCreateView1,
"CREATE VIEW view1 AS SELECT contact_id, last_name_as_number, first_name_as_number  FROM predictivesearch \
WHERE (first_name_as_number LIKE \"%% 1%%\") OR (last_name_as_number LIKE \"%% 1%%\")");

_LIT(KPredSearchCreateView2,
"CREATE VIEW view2 AS SELECT contact_id, last_name_as_number, first_name_as_number FROM predictivesearch \
WHERE (first_name_as_number LIKE \"%% 2%%\") OR (last_name_as_number LIKE \"%% 2%%\")");

_LIT(KPredSearchCreateView3,
"CREATE VIEW view3 AS SELECT contact_id, last_name_as_number, first_name_as_number FROM predictivesearch \
WHERE (first_name_as_number LIKE \"%% 3%%\") OR (last_name_as_number LIKE \"%% 3%%\")");

_LIT(KPredSearchCreateView4,
"CREATE VIEW view4 AS SELECT contact_id, last_name_as_number, first_name_as_number FROM predictivesearch \
WHERE (first_name_as_number LIKE \"%% 4%%\") OR (last_name_as_number LIKE \"%% 4%%\")");

_LIT(KPredSearchCreateView5,
"CREATE VIEW view5 AS SELECT contact_id, last_name_as_number, first_name_as_number FROM predictivesearch \
WHERE (first_name_as_number LIKE \"%% 5%%\") OR (last_name_as_number LIKE \"%% 5%%\")");

_LIT(KPredSearchCreateView6,
"CREATE VIEW view6 AS SELECT contact_id, last_name_as_number, first_name_as_number FROM predictivesearch \
WHERE (first_name_as_number LIKE \"%% 6%%\") OR (last_name_as_number LIKE \"%% 6%%\")");

_LIT(KPredSearchCreateView7,
"CREATE VIEW view7 AS SELECT contact_id, last_name_as_number, first_name_as_number FROM predictivesearch \
WHERE (first_name_as_number LIKE \"%% 7%%\") OR (last_name_as_number LIKE \"%% 7%%\")");

_LIT(KPredSearchCreateView8,
"CREATE VIEW view8 AS SELECT contact_id, last_name_as_number, first_name_as_number FROM predictivesearch \
WHERE (first_name_as_number LIKE \"%% 8%%\") OR (last_name_as_number LIKE \"%% 8%%\")");

_LIT(KPredSearchCreateView9,
"CREATE VIEW view9 AS SELECT contact_id, last_name_as_number, first_name_as_number FROM predictivesearch \
WHERE (first_name_as_number LIKE \"%% 9%%\") OR (last_name_as_number LIKE \"%% 9%%\")");

_LIT(KPredSearchCreateView0,
"CREATE VIEW view0 AS SELECT contact_id, last_name_as_number, first_name_as_number FROM predictivesearch \
WHERE (first_name_as_number LIKE \"%% 0%%\") OR (last_name_as_number LIKE \"%% 0%%\")");

_LIT(KPresenceContactId, "contact_id");
_LIT(KPresenceAccountUri, "account_uri");
_LIT(KPresenceServiceName, "service_name");
_LIT(KPresenceStatus, "status");
_LIT(KPresenceStatusMsg, "status_message");
_LIT(KPresenceContactIdParam, ":contact_id");
_LIT(KPresenceAccountUriParam, ":account_uri");
_LIT(KPresenceServiceNameParam, ":service_name");
_LIT(KPresenceStatusParam, ":status");
_LIT(KPresenceStatusMsgParam, ":status_message");
_LIT(KPresenceCreateStmnt,
"CREATE TABLE presence (contact_id INTEGER NOT NULL,\
account_uri VARCHAR(255),service_name VARCHAR(255),\
status INTEGER, status_msg VARCHAR(255),\
CONSTRAINT contactid_fk FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE);"); 
// Condition strings for searching id
_LIT(KSelectCondition_SearchForEqual,   "%S == :SeekId");

// SQLITE_SEQUENCE table stuff for getting last row_id
_LIT(KSelectLastIdSqlStmnt, "SELECT seq FROM SQLITE_SEQUENCE WHERE name = 'contact';");


// SQLITE_SEQUENCE table stuff for getting last row_id
_LIT(KSqliteSequenceTableName, "SQLITE_SEQUENCE");
_LIT(KSqliteSequenceSeq, "seq");
_LIT(KSqliteSequenceName, "name");

_LIT(KSQLSelect, "SELECT ");
_LIT(KFrom, "FROM ");

_LIT(KSpace, " ");
_LIT(KComma, ",");
_LIT(KSelectTwoFields, "SELECT %S, %S FROM %S WHERE %S=%d");
_LIT(KSelectTwoFieldsWithGreater, "SELECT %S, %S FROM %S WHERE %S>=%d");
_LIT(KSelectFiveFields, "SELECT %S, %S, %S, %S, %S FROM %S");
_LIT(KCountSelect, "SELECT count(*) FROM %S");
_LIT(KCountTypeSelect, "SELECT count(*) FROM %S WHERE (%S>>%d)=%d AND ((%S>>%d)&%d)=0");
_LIT(KOneTypeField, "SELECT %S FROM %S WHERE (%S>>%d)=%d");
_LIT(KTwoTypeField, "SELECT %S, %S FROM %S WHERE (%S>>%d)=%d");
_LIT(KJoinConditionClause, "%S=%S and %S=%d");
_LIT(KConditionClause, "%S=%d");
_LIT(KTypeConditionClause, "(%S>>%d)=%d");
_LIT(KWhereStringEqualsStringFormatText, "%S = %S");
_LIT(KSelectFlagsSqlStmntFormat, "SELECT type_flags FROM contact WHERE contact_id = %d");
_LIT(KVar, ":var");
_LIT(KIntValue, "%d");
_LIT(KSelectCondition_SearchForContactId, "contact_id == :SeekId");


// COUNT function
_LIT(KCount, " COUNT(%S) ");

// NULL
_LIT(KNullText, "NULL");    //think this should be deleted

// AND
_LIT(KSqlAnd, " AND ");

// OR
_LIT(KSqlOr, " OR ");

// COUNT
_LIT(KSqlCount, "count(*)");

const TInt  KFirstParam = 0;

/* constants defined for type_flags in contact table */

// definition for contact type flags

//Bit flags in the byte of Contact Type
enum TContactTypeFlags 
	{
	EContactTypeFlags_ContactCard,   
	EContactTypeFlags_OwnCard,
	EContactTypeFlags_ICCEntry,
	EContactTypeFlags_Group,
	EContactTypeFlags_Template,
	EContactTypeFlags_CardTemplate,
	EContactTypeFlags_UnknownType
	};

const TInt KContactTypeShiftBits = 24; //ContactType byte stored in highest(left most) byte in the type flags integer

/* definition for contact attributes flags */
enum TContactAttrsFlags 
	{
	EContactAttrsFlags_System  			= 1,
	EContactAttrsFlags_Hidden  			= 2, // 1 << 1
	EContactAttrsFlags_CompressedGUID 	= 4, // 1 << 2
	EContactAttrsFlags_Deleted    		= 8, // 1 << 3
	EContactAttrsFlags_Mask  		    = 0x00FF0000
	};
const TInt KContactAttrsShiftBits = 16; //Contact Attributes byte stored in second highest byte in the type flags integer

/* definition for contact hint flags */ 
const TInt KFastAccessFieldUids[] = 
	{
	KUidContactFieldGivenNameValue, 
	KUidContactFieldFamilyNameValue,
	KUidContactFieldCompanyNameValue,
	KUidContactFieldGivenNamePronunciationValue,
	KUidContactFieldFamilyNamePronunciationValue,
	KUidContactFieldCompanyNamePronunciationValue
	};
	
/* definition for contact hint flags */
enum TContactTypeFlagsMasks 
	{
	EContactType_Mask 		= 0xFF000000,
	EContactAttributes_Mask	= 0x00FF0000,
	EContactHintFlags_Mask 	= 0x0000FFFF // Hint value put lowest 2 bytes of the type flags integer
	};

enum TContactTypeFlagsShitfs
	{
	EContactAttributes_Shift	= 16,
	EContactType_Shift 			= 24	
	};	

enum TFastAccessField
	{
	EFirstFastAccessField = 0,
	EGivenName = EFirstFastAccessField,
	EFamilyName,
	ECompanyName,
	EGivenNamePrn,
	EFamilyNamePrn,
	ECompanyNamePrn,
	ENumberOfFastAccessFields,
	ENoSuchFastAccessField = -1
	};

const TInt KFirstIndex = 0;

#endif // #ifndef DBSQLCONSTANTS_H
