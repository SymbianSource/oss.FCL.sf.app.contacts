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
#include "MTPbkIdleFinder.h"
#include <CPbkIdleFinder.h>
#include <PbkFields.hrh>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>    

#include <CPbkFieldsInfo.h>

#include <cntdb.h>

//  INTERNAL INCLUDES


// CONSTANTS

_LIT(KTestDatabaseName, "c:CMTPbkIdleFinder.cdb");
_LIT(KPanicText, "MT_CPbkContactEngine");

_LIT(KFindText, "Find this");

static const TInt KTestContactCount = 3;

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
// CMTPbkIdleFinder::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkIdleFinder::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkIdleFinder::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkIdleFinder::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "IdleFind-found", CMTPbkIdleFinder::MT_CPbkIdleFinder_FindL ),
        ENTRY( "IdleFind-notfound", CMTPbkIdleFinder::MT_CPbkIdleFinder_Find1L ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

//----------------------------------------------------------------------------
/**
 * Helper class for idle find observer check.
 */
class CMTPbkIdleFinder::CFindObserver : public CBase, public MIdleFindObserver
    {
    public: // constructor & destructor
        CFindObserver();
        ~CFindObserver();

    public: // from idlefindobserver
	    void IdleFindCallback();

    public: // interface
        CContactIdArray* FindSyncL(CPbkContactEngine& aEngine,
                                   const TDesC& aText,
                                   const CPbkFieldIdArray* aFieldTypes=NULL);
        
    private: // members
        CPbkIdleFinder* iIdleFinder; // owns
        CContactIdArray* iFoundContacts;
        TBool iRunning;
        TInt iFieldDefCount;
    };


CMTPbkIdleFinder::CFindObserver::CFindObserver()
    {
    } 


CMTPbkIdleFinder::CFindObserver::~CFindObserver()
    {
    delete iIdleFinder;
    }


void CMTPbkIdleFinder::CFindObserver::IdleFindCallback()
    {
    ASSERT(iIdleFinder);

	// Get Symbian cntmodel idle finder object, saved into idle finder object
    //   at construct time
    CIdleFinder* symbIdleFinder = iIdleFinder->IdleFinder();
    ASSERT( symbIdleFinder);
    // Following methods must return same values regardless of the object used
    // to get values. S60 CPbk classes delegates final work to Symbian cntmodel
    // classes. Pointers to instantiated cntmodel based objects are saved to
    // idle finder object at construct time. 
    ASSERT(symbIdleFinder->IsComplete()     == iIdleFinder->IsComplete());
    ASSERT(symbIdleFinder->Error()          == iIdleFinder->Error());

    // Get Symbian cntmodel field definition (array of field types)
    CContactItemFieldDef* symbFieldDef = iIdleFinder->FieldDef();
    ASSERT( symbFieldDef);

    // If you can get some info using base class fieldDef pointer, the pointer probably is valid
    // Now we get Count.  It must be equal to value saved earlier. 
    ASSERT( (symbFieldDef->Count() == iFieldDefCount));


    if (iIdleFinder->IsComplete())
        {
        if (iIdleFinder->Error() != KErrNone)
            {
            User::Panic(KPanicText, 0);
            }
        else
            {
            iFoundContacts = iIdleFinder->TakeContactIds();
            ASSERT( iFoundContacts );
            if (iRunning)
                {
                CActiveScheduler::Stop();
                }
            }
        delete iIdleFinder;
        iIdleFinder = NULL;
        }
    } 


CContactIdArray* CMTPbkIdleFinder::CFindObserver::FindSyncL(
        CPbkContactEngine& aEngine,
        const TDesC& aText,
        const CPbkFieldIdArray* aFieldTypes)
    {    
    delete iIdleFinder;
    iIdleFinder = NULL;

    // Create CContactItemFieldDef. We need to get some reference info
    // to be used later for comparison in CFindObserver callback
    CContactItemFieldDef*
      fieldDef = aEngine.FieldsInfo().CreateContactItemFieldDefLC(aFieldTypes);
    ASSERT( fieldDef );

    iFieldDefCount = fieldDef->Count();
    CleanupStack::PopAndDestroy(fieldDef);


    iIdleFinder = aEngine.FindAsyncL(aText, aFieldTypes, this); 
    ASSERT( iIdleFinder );

    iRunning = ETrue;
    CActiveScheduler::Start();
    
    return iFoundContacts;
    } 
/**
 * Fills contact item text fields with given value.
 * However does not fill Phonenumberfields!
 */
void CMTPbkIdleFinder::SetTextFieldValueL(CPbkContactItem& aItem, 
                                           const TDesC& aTextValue)
    {
    for (TInt i = 0; i < aItem.CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = aItem.CardFields()[i];
        if ((!field.FieldInfo().IsPhoneNumberField()) && 
                (field.StorageType() == KStorageTypeText))
            {
            field.TextStorage()->SetTextL(aTextValue);
            }
        }   
    }


/**
 * Adds requested number of contacts,
 */
void CMTPbkIdleFinder::AddTestContactItemsL( TInt aTestContactCount)
    {
    // Add some valid items to the DB
    CContactIdArray* okContacts = CContactIdArray::NewLC();
    ASSERT(okContacts);

    CPbkContactItem* okContact = iContactEngine->CreateEmptyContactL();
    ASSERT(okContact);
    CleanupStack::PushL(okContact);

    SetTextFieldValueL(*okContact, KFindText);
    
    TContactItemId cid;
    TInt i;
    for (i = 0; i < aTestContactCount; ++i)
        {
        cid = iContactEngine->AddNewContactL(*okContact);
        ASSERT(cid);
        okContacts->AddL(cid);        
        }
    iTestContactCount = aTestContactCount;

    CleanupStack::PopAndDestroy(okContact);
    CleanupStack::PopAndDestroy(okContacts);
    }


void CMTPbkIdleFinder::SetupL(  )
    {
    // create engine
    iContactEngine = CPbkContactEngine::NewL(KTestDatabaseName, ETrue);

    AddTestContactItemsL(KTestContactCount);
    }


void CMTPbkIdleFinder::SetupEmptyL(  )
    {
    // create engine
    iContactEngine = CPbkContactEngine::NewL(KTestDatabaseName, ETrue);
    iTestContactCount = 0;
    }


void CMTPbkIdleFinder::Teardown(  )
    {
	delete iContactEngine;
    iContactEngine = NULL;
    }


/**
 * Testcase: Tries to find specific number of contacts.
 * Using CPbkIdleFinder functions and implemented MIdleFindObserver
 */
TInt CMTPbkIdleFinder::MT_CPbkIdleFinder_FindL(  )
    {
    SetupL();
    CFindObserver* observer = new (ELeave) CFindObserver;
    ASSERT( observer );
    CleanupStack::PushL(observer);

    CPbkFieldIdArray* fields = new (ELeave) CPbkFieldIdArray; 
    ASSERT( fields );
    CleanupStack::PushL(fields);

    // Add desired fields to the field id array
    fields->AppendL(EPbkFieldIdPhoneNumberStandard);
    fields->AppendL(EPbkFieldIdPhoneNumberHome);
    fields->AppendL(EPbkFieldIdPhoneNumberWork);

    CContactIdArray*
      foundContacts = observer->FindSyncL(*iContactEngine, KFindText, fields);
    ASSERT( foundContacts );
    CleanupStack::PushL(foundContacts);

    ASSERT(foundContacts->Count() == iTestContactCount);

    CleanupStack::PopAndDestroy(foundContacts);
    CleanupStack::PopAndDestroy(fields);
    CleanupStack::PopAndDestroy(observer);
    Teardown();
    return KErrNone;
    }

/**
 * Testcase: Tries to find specific number of contacts.
 * Using CPbkIdleFinder functions and implemented MIdleFindObserver
 */
TInt CMTPbkIdleFinder::MT_CPbkIdleFinder_Find1L(  )
    {
    SetupEmptyL();
    CFindObserver* observer = new (ELeave) CFindObserver;
    ASSERT( observer );
    CleanupStack::PushL(observer);

    CPbkFieldIdArray* fields = new (ELeave) CPbkFieldIdArray; 
    ASSERT( fields );
    CleanupStack::PushL(fields);

    // Add desired fields to the field id array
    fields->AppendL(EPbkFieldIdPhoneNumberStandard);
    fields->AppendL(EPbkFieldIdPhoneNumberHome);
    fields->AppendL(EPbkFieldIdPhoneNumberWork);

    CContactIdArray*
      foundContacts = observer->FindSyncL(*iContactEngine, KFindText, fields);
    ASSERT( foundContacts );
    CleanupStack::PushL(foundContacts);

    ASSERT(foundContacts->Count() == iTestContactCount);

    CleanupStack::PopAndDestroy(foundContacts);
    CleanupStack::PopAndDestroy(fields);
    CleanupStack::PopAndDestroy(observer);
    Teardown();
    return KErrNone;
    }

//  [End of File] - Do not remove
