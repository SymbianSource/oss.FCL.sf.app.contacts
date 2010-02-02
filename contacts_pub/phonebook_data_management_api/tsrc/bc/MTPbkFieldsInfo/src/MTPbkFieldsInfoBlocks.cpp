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
#include "MTPbkFieldsInfo.h"
#include <e32debug.h>

#include <CPbkContactItem.h>    
#include <CPbkContactEngine.h>
#include <CPbkFieldsInfo.h>
#include <CPbkFieldInfo.h>

// CONSTANTS
_LIT(KTestDatabaseName, "c:CMTPbkFieldsInfo.cdb");

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
// CMTPbkFieldsInfo::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkFieldsInfo::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkFieldsInfo::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkFieldsInfo::RunMethodL( 
    CStifItemParser& aItem ) 
    {

     TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Field_data", CMTPbkFieldsInfo::RunTestFieldData_CPbkFieldsInfoL ),
        ENTRY( "Match_by_FieldId", CMTPbkFieldsInfo::RunTestMatchByFieldIdL ),
        ENTRY( "Test_IsEqualTo", CMTPbkFieldsInfo::RunTestIsEqualToL ),
        ENTRY( "ContactItemViewDef", CMTPbkFieldsInfo::RunTestContactItemViewDefL ),
        ENTRY( "ContactItemFieldDef", CMTPbkFieldsInfo::RunTestContactItemFieldDefL ),
        ENTRY( "Test_that_fields_are_uniquely_identified_by_IsSame", CMTPbkFieldsInfo::RunTestIsSameL ),
        ENTRY( "Test_that_<FieldId(),Location()>_pair_identifies_each_field_type_uniquely", CMTPbkFieldsInfo::RunTestIdsUniqueL ),
        ENTRY( "Every_CPbkFieldInfo_must_match_its_DB_export_type", CMTPbkFieldsInfo::RunTestMatchesDbExportTypeL ),
        ENTRY( "No_match_should_be_found_for_a_field_with_incorrect_storage_type", CMTPbkFieldsInfo::RunTestDoesNotMatchWrongStorageTypeL ),
        ENTRY( "Field_info_ordering", CMTPbkFieldsInfo::RunTestFieldOrderingL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }
    
inline const CPbkFieldsInfo& CMTPbkFieldsInfo::FieldsInfo() const
    {
    return iEngine->FieldsInfo();
    }
    
//----------------------------------------------------------------------------

void CMTPbkFieldsInfo::SetupL(  )
    {
    iEngine = CPbkContactEngine::NewL(KTestDatabaseName, 1);
    }


void CMTPbkFieldsInfo::Teardown()
    {
    delete iEngine;
    iEngine  = NULL;
    }


static void UniqueIsSameL
        (const CPbkFieldsInfo& aFieldsInfo, const CPbkFieldInfo& aFieldInfo)
    {
    const TInt count = aFieldsInfo.Count();
    for (TInt i=0; i < count; ++i)
        {
        const CPbkFieldInfo& each = *aFieldsInfo[i];
        if (each.IsSame(aFieldInfo))
            {
            // Can not do pointer compare because there are some fieldtypes
            // that have same Pbk field id but differs in Symbian's contenttype.
            ASSERT( each.FieldId() == aFieldInfo.FieldId() );
            ASSERT( each.FieldStorageType() == 
                aFieldInfo.FieldStorageType() );
            }
        }
    }


static void MatchByFieldIdL
        (const CPbkFieldsInfo& aFieldsInfo, const CPbkFieldInfo& aFieldInfo)
    {
    const TInt count = aFieldsInfo.Count();
    TPbkFieldId fieldId = aFieldInfo.FieldId(); 
    for (TInt i=0; i < count; ++i)
        {
        const CPbkFieldInfo& field = *aFieldsInfo[i];
        if (field.FieldId() == fieldId)
            {
            ASSERT((field.FieldId() == fieldId) && field.Match(fieldId));
            }
        else
            {
            ASSERT((field.FieldId() != fieldId) && !field.Match(fieldId));
            }
        }
    return;
    }




/**
 * TestFieldDataL compares reference data against to data collected in environment to be tested.
 */
static void TestFieldDataL(const CPbkFieldsInfo& /*aFieldsInfo*/, const CPbkFieldInfo& aFieldInfo)
{

    ASSERT(!(aFieldInfo.FieldId() & ~KPbkFieldIdMask));

    ASSERT(aFieldInfo.ContentType().FieldTypeCount() >= 0);

    ASSERT(!(aFieldInfo.Category() & ~0xF));

    ASSERT(!(aFieldInfo.FieldName().Length() & ~0xFFFF));  // Values within 16 bit

    ASSERT(!(aFieldInfo.AddItemText().Length() & ~0xFFFF));

    ASSERT(!(aFieldInfo.MaxLength() & ~0xFFFF));

    ASSERT((aFieldInfo.IconId() == EPbkNullIconId) || !(aFieldInfo.IconId() & ~0xFF));

    ASSERT((aFieldInfo.FieldStorageType() == KStorageTypeText) ||
                 (aFieldInfo.FieldStorageType() == KStorageTypeStore) ||
                 (aFieldInfo.FieldStorageType() == KStorageTypeContactItemId) ||
                 (aFieldInfo.FieldStorageType() == KStorageTypeDateTime));

    ASSERT((aFieldInfo.Multiplicity() == EPbkFieldMultiplicityOne) ||
                 (aFieldInfo.Multiplicity() == EPbkFieldMultiplicityMany));

    ASSERT( aFieldInfo.EditMode() >= EPbkFieldEditModeAlpha );

    ASSERT( aFieldInfo.DefaultCase() >= 0 );

    ASSERT( aFieldInfo.CtrlType() >= 0 );

    ASSERT( aFieldInfo.AddItemOrdering() >= 0 );

    ASSERT( aFieldInfo.Location() >= EPbkFieldLocationNone );

    ASSERT((aFieldInfo.VersitStorageType() >= 0 ) &&
                 (aFieldInfo.VersitStorageType() <= EPbkVersitPropertyInt) );

    // Flags tests
	// Return values of Flags checking functions are boolean, but
	//   return values are based on corresponding flag bits of flags integer variable 
	// Return values can not be tested by equality to ETrue or EFalse.  
    TUint flags = aFieldInfo.Flags();
    ASSERT((aFieldInfo.IsHidden()      == (flags & EContactFieldFlagHidden)) );
    ASSERT((aFieldInfo.IsReadOnly()    == (flags & EContactFieldFlagReadOnly)) );
    ASSERT((aFieldInfo.DoSynchronize() == (flags & EContactFieldFlagSynchronize)) );
    ASSERT((aFieldInfo.IsDisabled()    == (flags & EContactFieldFlagDisabled)) );

    // AddFlags tests
	// Return values of AddFlags checking functions are boolean, but
	//   return values are based on corresponding flag bits of flags integer variable.
	//   Return values can not be tested by equality to ETrue or EFalse.  
    TUint addFlags = aFieldInfo.AddFlags();
    ASSERT((aFieldInfo.TemplateField()   == (addFlags & KPbkFieldFlagDefTemplate)) );
    ASSERT((aFieldInfo.UserCanAddField() == (addFlags & KPbkFieldFlagUserCanAddField)) );
    ASSERT((aFieldInfo.IsEditable()      == !(addFlags & KPbkFieldFlagDisableEdit)) );

    ASSERT((aFieldInfo.NumericField() == 0) || (aFieldInfo.NumericField() == 1) );

    // FieldId subset tests
	// Return values of FieldId subset tests are constant values ETrue (0) or EFalse (1)
    ASSERT((aFieldInfo.NameField() == 1) ||
                 (aFieldInfo.NameField() == 0) );
    ASSERT((aFieldInfo.IsPhoneNumberField() == 1) ||
                 (aFieldInfo.IsPhoneNumberField() == 0) );
    ASSERT((aFieldInfo.IsEmailOverSmsField() == 1) ||
                 (aFieldInfo.IsEmailOverSmsField() == 0) );
    ASSERT((aFieldInfo.IsEmailField() == 1) ||
                 (aFieldInfo.IsEmailField() == 0) );
    ASSERT((aFieldInfo.IsMmsField() == 1) ||
                 (aFieldInfo.IsMmsField() == 0) );
    ASSERT((aFieldInfo.IsImageField() == 1) ||
                 (aFieldInfo.IsImageField() == 0) );
    ASSERT((aFieldInfo.IsPocField() == 1) ||
                 (aFieldInfo.IsPocField() == 0) );
    ASSERT((aFieldInfo.IsVoipField() == 1) ||
                 (aFieldInfo.IsVoipField() == 0) );
    ASSERT((aFieldInfo.IsReadingField() == 1) ||
                 (aFieldInfo.IsReadingField() == 0) );

    return;
}



	
/**
 * Tests that <FieldId(),Location()> pair identifies each CPbkFieldInfo object
 * uniquely.
 */
static void UniqueFieldIdL
        (const CPbkFieldsInfo& aFieldsInfo, const CPbkFieldInfo& aFieldInfo)
    {
    const CPbkFieldInfo* match = aFieldsInfo.Find(aFieldInfo.FieldId(), 
        aFieldInfo.Location());
    // Can not do pointer compare because there are some fieldtypes
    // that have same Pbk field id but differs in Symbian's contenttype.
    ASSERT( match );
    ASSERT( match->FieldId() == aFieldInfo.FieldId() );
    ASSERT( match->FieldStorageType() == 
        aFieldInfo.FieldStorageType() );   
    }


static void MatchesDbExportTypeL
        (const CPbkFieldsInfo& aFieldsInfo, const CPbkFieldInfo& aFieldInfo)
    {
    CContactItemField* field = aFieldInfo.CreateFieldL();
    CleanupStack::PushL(field);
    ASSERT(aFieldsInfo.Find(*field) == &aFieldInfo);
    CleanupStack::PopAndDestroy(field);
    }


static TStorageType ChangeStorageType(TStorageType aStorageType)
    {
    switch (aStorageType)
        {
        case KStorageTypeText:
            return KStorageTypeStore;

        case KStorageTypeStore:
            return KStorageTypeText;

        case KStorageTypeDateTime:
            return KStorageTypeText;

        case KStorageTypeContactItemId:
            return KStorageTypeText;

        default:
            ASSERT(EFalse);
            return KStorageTypeText;
        }
    }



static CContactItemField* CreateFieldWithWrongStorageTypeL
        (const CPbkFieldInfo& aFieldInfo)
    {
    CContactItemField* field =
                       CContactItemField::NewL(ChangeStorageType(aFieldInfo.FieldStorageType()),
                                                                 aFieldInfo.ContentType());
    CleanupStack::PushL(field);
	field->SetUserFlags(aFieldInfo.Category());
    field->SetLabelL(aFieldInfo.FieldName());
	if (aFieldInfo.IsHidden())
        {
		field->SetHidden(1);
        }
	if (aFieldInfo.IsReadOnly())
        {
		field->SetReadOnly(1);
        }
	if (aFieldInfo.IsDisabled())
        {
		field->SetDisabled(1);
        }
    CleanupStack::Pop(field);
    return field;
    }


static void DoesNotMatchWrongStorageTypeL
        (const CPbkFieldsInfo& aFieldsInfo, const CPbkFieldInfo& aFieldInfo)
    {
    CContactItemField* field = CreateFieldWithWrongStorageTypeL(aFieldInfo);
    CleanupStack::PushL(field);
    ASSERT(!aFieldsInfo.Find(*field));
    CleanupStack::PopAndDestroy(field);
    }


static void IsEqualToWithRightAndWrongStorageTypeL
        (const CPbkFieldsInfo& aFieldsInfo, const CPbkFieldInfo& aFieldInfo)
    {
    CContactItemField* badField = CreateFieldWithWrongStorageTypeL(aFieldInfo);
    CleanupStack::PushL(badField);
    ASSERT(!aFieldInfo.IsEqualTo(*badField));
    CleanupStack::PopAndDestroy(badField);

    CContactItemField* okField = aFieldInfo.CreateFieldL();
    CleanupStack::PushL(okField);
    ASSERT(aFieldInfo.IsEqualTo(*okField));
    CleanupStack::PopAndDestroy(okField);

    // Test also CPbkFieldsInfo.Find(by CContactItemField)
    CContactItemField* contactItemField = aFieldInfo.CreateFieldL();
    CleanupStack::PushL(contactItemField);
    ASSERT(aFieldsInfo.Find(*contactItemField));
    CleanupStack::PopAndDestroy(contactItemField);

    }


void CMTPbkFieldsInfo::ForEachL(TFieldInfoTest aTest) 
    {
    const TInt count = FieldsInfo().Count();
    for (TInt i=0; i < count; ++i)
        {
        (*aTest)(FieldsInfo(), *(FieldsInfo()[i]));
        }
    }


TInt CMTPbkFieldsInfo::RunTestFieldData_CPbkFieldsInfoL()
    {
    SetupL();
    ForEachL(TestFieldDataL);
    Teardown();
    return KErrNone;
    }


TInt CMTPbkFieldsInfo::RunTestIsSameL()
    {
    SetupL();
    ForEachL(UniqueIsSameL);
    Teardown();
    return KErrNone;
    }


TInt CMTPbkFieldsInfo::RunTestIdsUniqueL()
    {
    SetupL();
    ForEachL(UniqueFieldIdL);
    Teardown();
    return KErrNone;
    }


TInt CMTPbkFieldsInfo::RunTestMatchesDbExportTypeL()
    {
    SetupL();
    ForEachL(MatchesDbExportTypeL);
    Teardown();
    return KErrNone;
    }


TInt CMTPbkFieldsInfo::RunTestDoesNotMatchWrongStorageTypeL()
    {
    SetupL();
    ForEachL(DoesNotMatchWrongStorageTypeL);
    Teardown();
    return KErrNone;
    }


TInt CMTPbkFieldsInfo::RunTestFieldOrderingL()
    {
    SetupL();
    const TInt last = FieldsInfo().Count() - 1;
    for (TInt i=0; i < last; ++i)
        {
        const CPbkFieldInfo& current = *(FieldsInfo()[i]);
        const CPbkFieldInfo& next = *(FieldsInfo()[i+1]);
        ASSERT( current.CompareOrdering(current) == 0 );
        ASSERT( (current.CompareOrdering(next) * next.CompareOrdering(current)) < 0 );
        }
    Teardown();
    return KErrNone;
    }


TInt CMTPbkFieldsInfo::RunTestMatchByFieldIdL()
    {
    SetupL();
    ForEachL(MatchByFieldIdL);
        Teardown();
    return KErrNone;}


TInt CMTPbkFieldsInfo::RunTestIsEqualToL()
    {
    SetupL();
    ForEachL(IsEqualToWithRightAndWrongStorageTypeL);
    Teardown();
    return KErrNone;
    }

    
TInt CMTPbkFieldsInfo::RunTestContactItemViewDefL()
    {
    SetupL();
    CPbkFieldIdArray* fields = new (ELeave) CPbkFieldIdArray; 
    CleanupStack::PushL(fields);
    ASSERT(fields);

    // Add desired fields to the field id array
    fields->AppendL(EPbkFieldIdLastName);
    fields->AppendL(EPbkFieldIdFirstName);

    CContactItemViewDef* viewDef = FieldsInfo().CreateContactItemViewDefLC(*fields);
    ASSERT(viewDef);

    TInt fieldTypeCountInitial = viewDef->Count();
    CPbkFieldInfo* fieldInfo;

    // LastName allready added at time of createContactItemViewDef
    fieldInfo = FieldsInfo().Find(EPbkFieldIdLastName, EPbkFieldLocationNone);
    fieldInfo->AddToViewDefL(*viewDef);

    // FirstName allready added at time of createContactItemViewDef
    fieldInfo = FieldsInfo().Find(EPbkFieldIdFirstName, EPbkFieldLocationNone);
    fieldInfo->AddToViewDefL(*viewDef);

    // At this point, no new field types added, so count must not have increased
    TInt fieldTypeCountAfterRedundantAdds = viewDef->Count();
    ASSERT(fieldTypeCountAfterRedundantAdds == fieldTypeCountInitial);

    // PhoneNumberGeneral not yet added, add now
    fieldInfo = FieldsInfo().Find(EPbkFieldIdPhoneNumberGeneral, EPbkFieldLocationNone);
    fieldInfo->AddToViewDefL(*viewDef);

    // At this point a new field type has been added, so count must have increased
    TInt fieldTypeCountAfterUniqueAdd = viewDef->Count();
    ASSERT(fieldTypeCountAfterUniqueAdd > fieldTypeCountInitial);

    CleanupStack::PopAndDestroy(viewDef);
    CleanupStack::PopAndDestroy(fields);
    Teardown();
    return KErrNone;
    }
    

TInt CMTPbkFieldsInfo::RunTestContactItemFieldDefL()
    {
    SetupL();
    // First, test with all fields
    CContactItemFieldDef* allFieldDef = FieldsInfo().CreateContactItemFieldDefLC(NULL);
    ASSERT(allFieldDef);
    CleanupStack::PopAndDestroy(allFieldDef);

    // Next, test with specified fields
    CPbkFieldIdArray* fields = new (ELeave) CPbkFieldIdArray; 
    CleanupStack::PushL(fields);
    ASSERT(fields);

    // Add desired fields to the field id array
    fields->AppendL(EPbkFieldIdLastName);
    fields->AppendL(EPbkFieldIdFirstName);

    CContactItemFieldDef* fieldDef = FieldsInfo().CreateContactItemFieldDefLC(fields);
    ASSERT(fieldDef);

    TInt fieldTypeCountInitial = fieldDef->Count();
    CPbkFieldInfo* fieldInfo;

    // LastName allready added at time of createContactItemViewDef
    fieldInfo = FieldsInfo().Find(EPbkFieldIdLastName, EPbkFieldLocationNone);
    fieldInfo->AddToFieldDefL(*fieldDef);

    // FirstName allready added at time of createContactItemViewDef
    fieldInfo = FieldsInfo().Find(EPbkFieldIdFirstName, EPbkFieldLocationNone);
    fieldInfo->AddToFieldDefL(*fieldDef);

    // At this point, no new field types added, so count must not have increased
    TInt fieldTypeCountAfterRedundantAdds = fieldDef->Count();
    ASSERT(fieldTypeCountAfterRedundantAdds == fieldTypeCountInitial);

    // PhoneNumberGeneral not yet added, add now
    fieldInfo = FieldsInfo().Find(EPbkFieldIdPhoneNumberGeneral, EPbkFieldLocationNone);
    fieldInfo->AddToFieldDefL(*fieldDef);

    // At this point a new field type has been added, so count must have increased
    TInt fieldTypeCountAfterUniqueAdd = fieldDef->Count();
    ASSERT(fieldTypeCountAfterUniqueAdd > fieldTypeCountInitial);

    CleanupStack::PopAndDestroy(fieldDef);
    CleanupStack::PopAndDestroy(fields);
    Teardown();
    return KErrNone;
    }
    
//  [End of File] - Do not remove
