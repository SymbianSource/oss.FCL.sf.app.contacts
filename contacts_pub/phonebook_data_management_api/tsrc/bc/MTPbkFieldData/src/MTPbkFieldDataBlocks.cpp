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
#include "MTPbkFieldData.h"


// Classes under test
#include <CPbkContactItem.h>    // Implements pure virtual methods for class to be tested
#include <CPbkContactEngine.h>
#include <CPbkFieldInfo.h>
#include <CPbkFieldsInfo.h>


_LIT(KFillText, "TESTDATA");
static const TInt KTestDataLen = 8;
// panic text
_LIT(KPanicText,"CMTPbkFieldData");

static const TInt KPanicUnsupportedFieldType = 1;

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
// CMTPbkFieldData::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkFieldData::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkFieldData::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkFieldData::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "MPbkFieldDataArray", CMTPbkFieldData::RunMPbkFieldDataArrayL ),
        ENTRY( "MPbkFieldData", CMTPbkFieldData::RunMPbkFieldDataL )
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void CMTPbkFieldData::SetupL(  )
    {
    if (!iContactEngine)
        {
        iContactEngine = CPbkContactEngine::NewL();
        }
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    }


void CMTPbkFieldData::Teardown(  )
    {
    delete iContactItem;
    iContactItem = NULL;

    delete iContactEngine;
    iContactEngine = NULL;
    }

/**
 * Run tests for methods PbkFieldCount(), PbkFieldAt() and PbkFieldAt() const;
 */
TInt CMTPbkFieldData::RunMPbkFieldDataArrayL()
    {
    SetupL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    const CPbkContactItem* itemC = item;

    CleanupStack::PushL(item);

    ASSERT(item->CardFields().Count() == item->PbkFieldCount());

    for (TInt i = 0; i < item->CardFields().Count(); ++i)
        {

        TPbkContactItemField& field = item->CardFields()[i];
        CPbkFieldInfo& fieldInfo = field.FieldInfo();

        MPbkFieldData& fieldData = item->PbkFieldAt(i);
        
        ASSERT(field.StorageType() == fieldData.PbkFieldType());
        ASSERT(fieldInfo.FieldId() == fieldData.PbkFieldId());

        const MPbkFieldData& fieldDataC = itemC->PbkFieldAt(i);

        ASSERT(field.StorageType() == fieldDataC.PbkFieldType());
        ASSERT(fieldInfo.FieldId() == fieldDataC.PbkFieldId());

        }

    CleanupStack::PopAndDestroy(item);
	Teardown();
	return KErrNone;
    }
/**
 * Run tests for PbkFieldType, PbkFieldId, PbkFieldText and PbkFieldTime
 * Functions must be callable and return relevant info of empty contact
 */
TInt CMTPbkFieldData::RunMPbkFieldDataL()
    {
    SetupL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();

    CleanupStack::PushL(item);
    
    TInt homeIdx = KErrNotFound;
    TInt lastNameIdx = KErrNotFound;
    TInt emailIdx = KErrNotFound;

    ASSERT(item->CardFields().Count() == item->PbkFieldCount());

    for (TInt i = 0; i < item->CardFields().Count(); ++i)
        {

        TPbkContactItemField& field = item->CardFields()[i];
        CPbkFieldInfo& fieldInfo = field.FieldInfo();
        const MPbkFieldData& fieldData = item->PbkFieldAt(i);
        
        ASSERT(field.StorageType() == fieldData.PbkFieldType());
        ASSERT(fieldInfo.FieldId() == fieldData.PbkFieldId());

        if (field.FieldInfo().IsPhoneNumberField())
            {            
            if (field.FieldInfo().FieldId() == EPbkFieldIdPhoneNumberMobile)
                {
                homeIdx = i;
                }
            }
        else
            {
            switch (field.StorageType())
                {

                case KStorageTypeText:
                    {
                    field.TextStorage()->SetTextL(KFillText);

                    ASSERT(fieldData.PbkFieldText().Length() == KTestDataLen);
                    
                    if (field.FieldInfo().FieldId() == EPbkFieldIdLastName)
                        {
                        lastNameIdx = i;
                        }
                    
                    if (field.FieldInfo().FieldId() == EPbkFieldIdEmailAddress)
                        {
                        emailIdx = i;
                        }
                    
                    break;
                    }

                case KStorageTypeDateTime:
                    {
                    field.DateTimeStorage()->SetTime(iNow);
                    ASSERT(fieldData.PbkFieldTime() == iNow);
                    break;
                    }

                default:
                    {
                    User::Panic(KPanicText, KPanicUnsupportedFieldType);
                    break;
                    }
                }
            }
        }

    ASSERT(item->PbkFieldAt(homeIdx).PbkFieldId() == EPbkFieldIdPhoneNumberMobile);
    ASSERT(item->PbkFieldAt(lastNameIdx).PbkFieldId() == EPbkFieldIdLastName);
    ASSERT(item->PbkFieldAt(emailIdx).PbkFieldId() == EPbkFieldIdEmailAddress);

    CleanupStack::PopAndDestroy(item);
	Teardown();
	return KErrNone;
    }

//  [End of File] - Do not remove
