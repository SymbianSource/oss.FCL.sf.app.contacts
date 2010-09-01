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
#include "MTPbkContactChangeNotifier.h"
#include <CPbkContactItem.h>    
#include <CPbkContactEngine.h>
#include <MPbkContactDbObserver.h>
#include <CPbkContactChangeNotifier.h>
#include <CPbkFieldsInfo.h>

//  INTERNAL INCLUDES
#include "CContactEngineEventQueue.h"



// CONSTANTS
_LIT(KTestDatabaseName, "c:CMTPbkContactChangeNotifier.cdb");
_LIT(KPanicText, "MT_CPbkContactEngine");

static const TInt KPanicUnspportedFieldType = 1;

const TInt KEventListenTimeout = 60;
const TInt KEventConsumeTimeout = 10;
const TInt KEventConsumeMaxCount = 100;


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
// CMTPbkContactChangeNotifier::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkContactChangeNotifier::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkContactChangeNotifier::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkContactChangeNotifier::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CreateNotifier", CMTPbkContactChangeNotifier::RunCreateContactChangeNotifierL  ),
        ENTRY( "Contact_added_and_deleted_events", CMTPbkContactChangeNotifier::RunAddAndDeleteContactEventsL  ),
        ENTRY( "Contact_changed_event", CMTPbkContactChangeNotifier::RunContactChangedEventL  ),
        ENTRY( "Group_added_and_deleted_events", CMTPbkContactChangeNotifier::RunAddAndDeleteGroupEventsL  ),
        ENTRY( "Group_changed_event", CMTPbkContactChangeNotifier::RunGroupChangedEventL  ),
        ENTRY( "Unknown_changes_event", CMTPbkContactChangeNotifier::RunDeleteContactsEventL  )
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

void CMTPbkContactChangeNotifier::FillContactFieldsL(
        CPbkContactItem& aItem, 
        const TInt aBasePhoneNumber,
        const TBool aConstantNumber)
    {
    _LIT(KFillText, "TESTDATA");

    for (TInt i = 0; i < aItem.CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = aItem.CardFields()[i];
        
        if (field.FieldInfo().IsPhoneNumberField())
            {
            // different number for each phonenumber;
            // just simply set the loop counter
            TInt number = aBasePhoneNumber;
            if (!aConstantNumber)
                {
                number = number+ i;
                }

            TBuf<10> phoneNumber;
            phoneNumber.Num(number);
            field.TextStorage()->SetTextL(phoneNumber);
            }
        else
            {
            switch (field.StorageType())
                {

                case KStorageTypeText:
                    field.TextStorage()->SetTextL(KFillText);
                    break;

                case KStorageTypeDateTime:
                    {
                    TTime now;
                    now.UniversalTime();
                    field.DateTimeStorage()->SetTime(now);
                    break;
                    }

                default:
                    User::Panic(KPanicText, KPanicUnspportedFieldType);
                    break;
                }
            }
        }
    }

/**
 * create an empty contact.
 */ 
void CMTPbkContactChangeNotifier::SetEmptyContactL()
    {
    if (!iContactEngine)
        {
        iContactEngine = CPbkContactEngine::NewL(KTestDatabaseName,ETrue);        
        }
    
    iContactItem = iContactEngine->CreateEmptyContactL();
    }

/**
 * create a filled contact.
 */ 
void CMTPbkContactChangeNotifier::SetFilledContactL()
    {
    SetEmptyContactL();
    FillContactFieldsL(*iContactItem,0);
    }


/**
 * Set the engine, filled contact and event observer.
 */ 
void CMTPbkContactChangeNotifier::SetupEventObserverL()
    {
    SetFilledContactL();
    iEventQueue = CContactEngineEventQueue::NewL(iContactEngine);
    }


/**
 * Teardown the contact engine, contact, and event observer
 */ 
void CMTPbkContactChangeNotifier::TeardownEventObserver()
    {
    delete iEventQueue;
    iEventQueue = NULL;

    delete iContactItem;
    iContactItem = NULL;

    delete iContactEngine;
    iContactEngine = NULL;
    }


void CMTPbkContactChangeNotifier::SetupL(  )
    {
    iContactEngine = CPbkContactEngine::NewL(KTestDatabaseName, ETrue);
    }


void CMTPbkContactChangeNotifier::Teardown(  )
    {
    delete iContactEngine;
    iContactEngine = NULL;
    }



/**
 * Consumes the defined events.
 */ 
void CMTPbkContactChangeNotifier::ConsumeEventsL()
    {
    if (iEventQueue)
        {
        TContactDbObserverEvent event;    
        iEventQueue->ConsumeEvent(KEventConsumeTimeout, event, KEventConsumeMaxCount);   
        }
    }


/**
 * Listens for the defined event.
 */ 
TBool CMTPbkContactChangeNotifier::ListenForEventL(TContactDbObserverEvent& aEvent)
    {
    TContactDbObserverEvent event;
    TBool found = EFalse;
    do
        {
        found  = iEventQueue->ListenForEvent(KEventListenTimeout,event);        
        if (found)
            {
            if (event.iType == aEvent.iType)
                { 
                if ((event.iType == EContactDbObserverEventUnknownChanges) ||
                    (event.iContactId == aEvent.iContactId) )
                    {
                    return ETrue;
                    }
                }
            }
        }
    while (found);
    
    // Event not found 
    return EFalse;
    }


/**
 * Run 'Create contact change notifier' test case
 */ 
TInt CMTPbkContactChangeNotifier::RunCreateContactChangeNotifierL()
    {    
    SetupL();
    CContactEngineEventQueue* observer = CContactEngineEventQueue::NewL(iContactEngine);
    ASSERT(observer);
    CleanupStack::PushL(observer);    

    CPbkContactChangeNotifier* notifier = iContactEngine->CreateContactChangeNotifierL(observer);
    ASSERT(notifier);
    CleanupStack::PushL(notifier);    

    CleanupStack::PopAndDestroy(notifier);
    CleanupStack::PopAndDestroy(observer);
    Teardown();
    return KErrNone;
    }


/**
 * Run 'add and delete contact events' test case.
 */ 
TInt CMTPbkContactChangeNotifier::RunAddAndDeleteContactEventsL()
    {
    SetupEventObserverL();
    // event struct used for filter ContactChange notification messages
    TContactDbObserverEvent event;

    // Add contact for creating the event
    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);
    
    // Catch notification messages, set filter for 'ContactAdded' event
    event.iType = EContactDbObserverEventContactAdded;
    event.iContactId = cid;
    // Catch notification delivered by calling HandleDatabaseEventL
    ASSERT(ListenForEventL(event));
    // Catch notification delivered by calling DatabaseEventHandledL
    ASSERT(ListenForEventL(event));


    // delete contact for creating the event
    iContactEngine->DeleteContactL(cid);
    
    // Catch notification messages, set filter for 'ContactDeleted' event
    event.iType = EContactDbObserverEventContactDeleted;
    event.iContactId = cid;
    // Catch notification delivered by calling HandleDatabaseEventL
    ASSERT(ListenForEventL(event));
    // Catch notification delivered by calling DatabaseEventHandledL
    ASSERT(ListenForEventL(event));
	TeardownEventObserver();
	return KErrNone;
    }

/**
 * Run 'add and delete group event' test case.
 */ 
TInt CMTPbkContactChangeNotifier::RunAddAndDeleteGroupEventsL()
    {
    SetupEventObserverL();
    // event struct used for filter ContactChange notification messages
    TContactDbObserverEvent event;

    // Add contact group
    _LIT(KGroupName,"TestGroup");
    CContactGroup* cgroup = iContactEngine->CreateContactGroupL(KGroupName);
    CleanupStack::PushL(cgroup);
    
    // get id of the group
    TInt cid = cgroup->Id();

    // Catch notification messages, set filter for 'GroupAdded' event
    event.iType = EContactDbObserverEventGroupAdded;
    event.iContactId = cid;
    // Catch notification delivered by calling HandleDatabaseEventL
    ASSERT(ListenForEventL(event));
    // Catch notification delivered by calling DatabaseEventHandledL
    ASSERT(ListenForEventL(event));
    
    // delete contact for creating the event
    iContactEngine->DeleteContactGroupL(cid);
    
    // Catch notification messages, set filter for 'ContactDeleted' event
    event.iType = EContactDbObserverEventGroupDeleted;
    event.iContactId = cid;
    // Catch notification delivered by calling HandleDatabaseEventL
    ASSERT(ListenForEventL(event));
    // Catch notification delivered by calling DatabaseEventHandledL
    ASSERT(ListenForEventL(event));

    CleanupStack::PopAndDestroy(cgroup);
    TeardownEventObserver();
    return KErrNone;
    }

/**
 * Run 'contact changed event' test case.
 */ 
TInt CMTPbkContactChangeNotifier::RunContactChangedEventL()
    {
    SetupEventObserverL();
    // event struct used for filter ContactChange notification messages
    TContactDbObserverEvent event;

    TContactItemId cid = iContactEngine->AddNewContactL(*iContactItem);

    // Catch notification messages, set filter for 'ContactAdded' event
    event.iType = EContactDbObserverEventContactAdded;
    event.iContactId = cid;
    // Catch notification delivered by calling HandleDatabaseEventL
    ASSERT(ListenForEventL(event));
    // Catch notification delivered by calling DatabaseEventHandledL
    ASSERT(ListenForEventL(event));

    // Open contact just added
    CPbkContactItem* item = iContactEngine->OpenContactLCX(cid);
    // Finally do the actual commit test, commit contact    
    iContactEngine->CommitContactL(*item);

    // Catch notification messages, set filter for 'ContactChanged' event
    event.iType = EContactDbObserverEventContactChanged;
    event.iContactId = cid;
    // Catch notification delivered by calling HandleDatabaseEventL
    ASSERT(ListenForEventL(event));
    // Catch notification delivered by calling DatabaseEventHandledL
    ASSERT(ListenForEventL(event));

    // OpenContactLCX lefts 2 objects in the cleanup stack
    CleanupStack::PopAndDestroy(2);
    TeardownEventObserver();
    return KErrNone;
    }

/**
 * Run 'group changed event' test case.
 */ 
TInt CMTPbkContactChangeNotifier::RunGroupChangedEventL()
    {    
    SetupEventObserverL();
    // event struct used for filter ContactChange notification messages
    TContactDbObserverEvent event;

    // Add contact group
    _LIT(KGroupName,"TestGroup");
    CContactGroup* cgroup = iContactEngine->CreateContactGroupL(KGroupName);
    CleanupStack::PushL(cgroup);
    
    // listen away uninterest events
    TContactDbObserverEvent tempEvent;
    ASSERT(iEventQueue->ConsumeEvent(KEventConsumeTimeout,
                                           tempEvent, KEventConsumeMaxCount));

    // get the id of the group
    TInt cid = cgroup->Id();
    CContactGroup* cgroup2 = iContactEngine->OpenContactGroupLCX(cid); 

    
    iContactEngine->CommitContactGroupL(*cgroup2);
        
    // Catch notification messages, set filter for 'GroupChanged' event
    event.iType = EContactDbObserverEventGroupChanged;
    event.iContactId = cid;
    // Catch notification delivered by calling HandleDatabaseEventL
    ASSERT(ListenForEventL(event) );
    // Catch notification delivered by calling DatabaseEventHandledL
    ASSERT(ListenForEventL(event) );


    // OpenContactGroupLCX lefts 2 objects in the cleanup stack
    CleanupStack::PopAndDestroy(2);
    CleanupStack::PopAndDestroy(cgroup);

    // delete contact
    iContactEngine->DeleteContactGroupL(cid);
    TeardownEventObserver();
    return KErrNone;
    }

/**
 * Run 'Unknown event' test case.
 */ 
TInt CMTPbkContactChangeNotifier::RunDeleteContactsEventL()
    {    
    SetupEventObserverL();
    // event struct used for filter ContactChange notification messages
    TContactDbObserverEvent event;
    
    TContactItemId cid;
    CContactIdArray* contacts = CContactIdArray::NewLC();    


    // Add new contact, so we can delete it soon
    cid = iContactEngine->AddNewContactL(*iContactItem);
    contacts->AddL(cid);

    // Catch notification messages, set filter for 'ContactAdded' event
    event.iType = EContactDbObserverEventContactAdded;
    event.iContactId = cid;
    // Catch notification delivered by calling HandleDatabaseEventL
    ASSERT(ListenForEventL(event));
    // Catch notification delivered by calling DatabaseEventHandledL
    ASSERT(ListenForEventL(event));


    // Add second new contact, so we can delete it soon
    cid = iContactEngine->AddNewContactL(*iContactItem);
    contacts->AddL(cid);

    // Catch notification messages, set filter for 'ContactAdded' event
    event.iType = EContactDbObserverEventContactAdded;
    event.iContactId = cid;
    // Catch notification delivered by calling HandleDatabaseEventL
    ASSERT(ListenForEventL(event));
    // Catch notification delivered by calling DatabaseEventHandledL
    ASSERT(ListenForEventL(event));


    // Delete just added contacts
    iContactEngine->DeleteContactsL(*contacts);

    // Catch notification messages, set filter for 'UnknownChanged' event
    event.iType = EContactDbObserverEventUnknownChanges;
    event.iContactId = KNullContactId;
    // Catch notification delivered by calling HandleDatabaseEventL
    ASSERT(ListenForEventL(event));
    // Catch notification delivered by calling DatabaseEventHandledL
    ASSERT(ListenForEventL(event));

    CleanupStack::PopAndDestroy(contacts);
    TeardownEventObserver();
    return KErrNone;
    }

//  [End of File] - Do not remove
