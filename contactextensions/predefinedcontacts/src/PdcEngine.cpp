/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Predefined contacts engine (state machine)
 *
*/

// System includes
#include <CVPbkContactManager.h>        // CVPbkContactManager
#include <MVPbkContactStoreList.h>      // MVPbkContactStoreList
#include <MVPbkContactStore.h>          // MVPbkContactStore
#include <CVPbkContactStoreUriArray.h>  // CVPbkContactStoreUriArray
#include <TVPbkContactStoreUriPtr.h>    // TVPbkContactStoreUriPtr
#include <CVPbkContactLinkArray.h>      // CVPbkContactLinkArray
#include <VPbkContactStoreUris.h>       // VPbkContactStoreUris
#include <AknGlobalNote.h>              // CAknGlobalNote
#include <stringresourcereader.h>       // CStringResourceReader
#include <predefinedcontacts.rsg>

// User includes
#include "PdcEngine.h"          // CPdcEngine
#include "PdcVCardImporter.h"   // CPdcVCardImporter
#include "PdcXMLImporter.h"     // CPdcXmlImporter
#include "pdcdata.h"            // CPdcData
#include "pdccontactdeletion.h" // CPdcContactDeletion
#include "pdclogger.h"

// Constants
_LIT( KErrorResourceFile, "z:\\resource\\predefinedcontacts.rsc" );

#ifdef USE_FILE_LOGGER 
_LIT(KLogsPath,"c:\\logs\\predefinedcontacts\\");
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPdcEngine::NewLC
// Symbian 1st phase constructor
// @return Self pointer to CPdcEngine pushed to
// the cleanup stack.
// ---------------------------------------------------------------------------
//
CPdcEngine* CPdcEngine::NewLC()
    {
    CPdcEngine* self = new( ELeave ) CPdcEngine();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CPdcEngine::~CPdcEngine
// Destructor
// ---------------------------------------------------------------------------
//
CPdcEngine::~CPdcEngine()
    {
    Cancel();
    delete iLinkArray;
    delete iVCardImporter;
    delete iXmlImporter;
    delete iFileDirectory;
    delete iPdcData;
    delete iContactDeleter;

    if (iContactStore)
        {
        iContactStore->Close( *this);
        }

    delete iContactManager;

    iFs.Close();
    }

// ---------------------------------------------------------------------------
// CPdcEngine::CPdcEngine
// C++ constructor
// ---------------------------------------------------------------------------
//
CPdcEngine::CPdcEngine() :
    CActive(EPriorityNormal)
    {
    }

// ---------------------------------------------------------------------------
//  CPdcEngine::ConstructL
//  Second-phase constructor
// ---------------------------------------------------------------------------
//
void CPdcEngine::ConstructL()
    {
    // Connect to the file system
    User::LeaveIfError(iFs.Connect() );
    
#ifdef USE_FILE_LOGGER 
    iFs.MkDirAll(KLogsPath); // create file logger path
#endif
    LOGTEXT( _L("CPdcEngine::ConstructL") );
    
    // Create the array of links
    iLinkArray = CVPbkContactLinkArray::NewL();
    // Create data checker
    iPdcData = CPdcData::NewL(iFs, *iLinkArray);

    // Add to the activeScheduler
    CActiveScheduler::Add( this);
    }

// ---------------------------------------------------------------------------
// CPdcEngine::PredefinedContactsNeedAddingL 
// Checks if the predefined contacts need to be added.
// @return ETrue if the predefined contacts need to be added
// ---------------------------------------------------------------------------
//
TBool CPdcEngine::PredefinedContactsNeedAddingL()
    {
    LOGTEXT( _L("CPdcEngine::PredefinedContactsNeedAddingL") );
    return iPdcData->ContactsUpToDateL();
    }

// ---------------------------------------------------------------------------
// CPdcEngine::AddPredefinedContactsL
// Starts the asyncronous process of reading and adding the
// predefined contacts to the contact model
// ---------------------------------------------------------------------------
//  
void CPdcEngine::AddPredefinedContactsL()
    {
    LOGTEXT( _L("CPdcEngine::AddPredefinedContactsL") );
    // Begin the process of adding of the prefined contacts creating the
    // contact manager
    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();

    // get the default Contacts Model database store URI.
    const TDesC& cntDbUri = VPbkContactStoreUris::DefaultCntDbUri();
    uriArray->AppendL(cntDbUri);

    // Create the contact manager
    iContactManager = CVPbkContactManager::NewL( *uriArray);
    CleanupStack::PopAndDestroy(uriArray);

    // Get the store list
    MVPbkContactStoreList& storeList = iContactManager->ContactStoresL();

    // Get the store
    iContactStore = storeList.Find(cntDbUri);
    ASSERT( iContactStore );

    // Open the store
    iContactStore->OpenL( *this);

    // Set the engine state to opening the contacts database, and await the
    // the store ready callback.
    iEngineState = EOpeningContacts;
    }

// ---------------------------------------------------------------------------
// CPdcEngine::AddPredefinedContactsL
// Starts the asyncronous process of reading and adding the
// predefined contacts to the contact model
//  @param aStatus  TRequestStatus of caller (called by ECom
//                  plugin starting mechanism.
// ---------------------------------------------------------------------------
//  
void CPdcEngine::AddPredefinedContactsL(TRequestStatus& aCallerStatus)
    {
    iCallerStatus = &aCallerStatus;
    aCallerStatus = KRequestPending;

    AddPredefinedContactsL();
    }

// ---------------------------------------------------------------------------
// CPdcEngine::SelfComplete
// Signals that the AO should be run again by completing the outstanding
// TRequestStatus.
// ---------------------------------------------------------------------------
//  
void CPdcEngine::SelfComplete()
    {
    // Begin the process of adding of the prefined contacts to contacts
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// ---------------------------------------------------------------------------
// CPdcEngine::DoCancel
// From CActive
// Cancels any outstanding async operations. 
// ---------------------------------------------------------------------------
//   
void CPdcEngine::DoCancel()
    {
    LOGTEXT( _L("CPdcEngine::Cancel") );
    
    if( iVCardImporter )
        {
        iVCardImporter->Cancel();
        }
    if( iXmlImporter )
        {
        iXmlImporter->Cancel();
        }
    if( iContactDeleter )
        {
        iContactDeleter->Cancel();
        }
    if( iCallerStatus )
    	{
    	User::RequestComplete( iCallerStatus, KErrCancel);
    	}
    }

// ---------------------------------------------------------------------------
// CPdcEngine::RunL
// From CActive
// Handles the objects request completion event by performing the relevent
// action for the current state. 
// ---------------------------------------------------------------------------
//
void CPdcEngine::RunL()
    {

    //Reserve error info before going into next step
    if (iStatus!= KErrNone)
        {
        LOGTEXT2(_L("error happend iStatus = %d"), iStatus.Int() );
        User::LeaveIfError(iStatus.Int() );
        }

    switch (iEngineState)
        {
        case EDeleteOldContacts:
            {
            LOGTEXT(_L("EDeleteOldContacts"));
            DeleteContactsL();

            // Set the next state
            iEngineState = EGettingFileLocation;
            }
            break;
        case EGettingFileLocation:
            {
            // Reset the link array
            LOGTEXT(_L("EGettingFileLocation"));
            GetFileLocationL();

            // Set the next state
            iEngineState = EReadingVCards;
            }
            break;
        case EReadingVCards:
            {
            LOGTEXT(_L("EReadingVCards"));
            ReadVCardsL();

            // Set the next state
            iEngineState = EReadingXML;
            }
            break;
        case EReadingXML:
            {
            LOGTEXT(_L("EReadingXML"));
            ReadXmlL();

            // Set the next state
            iEngineState = EUpdateData;
            }
            break;
        case EUpdateData:
            {
            LOGTEXT(_L("EUpdateData"));
            TRAPD( error, UpdateDataL() )
            ;
            if (error)
                {
                TRAP_IGNORE( DisplayErrorNoteL( error, iEngineState, EFalse ) );
                }

            // Set the next state
            iEngineState = EFinish;
            }
            break;
        case EDeleteStoredContact:
            {
            LOGTEXT(_L("EDeleteStoredContact"));
            iDeletedStoredContact = EFalse;
            DeleteStoredContactsL();

            // Set the next state                        
            iEngineState = EFinish;
            }
            break;
        case EFinish:
            {
            LOGTEXT(_L("EFinish"));
            Finish(KErrNone);
            }
            break;
        default:
            {
            // Should never reach this point
            ASSERT( 0 );
            }
            break;
        }
    }

// ---------------------------------------------------------------------------
// CPdcEngine::RunError
// From CActive
// Called when RunL leaves.
// ---------------------------------------------------------------------------
//
TInt CPdcEngine::RunError(TInt aError)
    {
    LOGTEXT(_L("RunError"));
    //flag for represent XML or vCard error       
    TBool needDelete = EFalse;

    //Display the error message.
    TInt iErrPosition = iEngineState - 1;

    TRAP_IGNORE( DisplayErrorNoteL( aError, iErrPosition, EFalse ) )

    if (EReadingVCards == iErrPosition || EReadingXML == iErrPosition)
        {
        needDelete = ETrue;
        }

    //XML or vCard error, so return to RunL and delete the stored contacts.
    if (needDelete)
        {
        iEngineState = EDeleteStoredContact;
        SelfComplete();
        }
    else
        {
        // Stop the code executing.
        Finish(aError);
        }

    // Don't propagate the error to the active scheduler, (otherwise the 
    // active scheduler by default will panic).
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CPdcEngine::DeleteOldContactsL
// Checks to see if there are any exisitng contacts that need to be
// deleted, if so starts the asycnronous deletion of the contacts.
// ---------------------------------------------------------------------------
//    
void CPdcEngine::DeleteContactsL()
    {
    // Get old contacts
    HBufC8* linkBuffer = iPdcData->LinkArrayBuffer();
    if (linkBuffer)
        {
        // Create the contacts deletion object
        iContactDeleter = CPdcContactDeletion::NewL( *iContactManager);
        iDeletedStoredContact = ETrue;
        // Get the links array
        MVPbkContactLinkArray* linkArray =
                iContactManager->CreateLinksLC( *linkBuffer);

        // Start deleting the contacts, takes ownership of the link
        // array.
        iContactDeleter->DeleteContactsL(linkArray, iStatus,
                iDeletedStoredContact);
        CleanupStack::Pop(); // linkArray

        SetActive();
        }
    else
        {
        // Queue a request for the next state
        SelfComplete();
        }
    }

// ---------------------------------------------------------------------------
// CPdcEngine::GetFileLocationL
// Gets the file location of any vCards and xml scripts
// ---------------------------------------------------------------------------
//    
void CPdcEngine::GetFileLocationL()
    {
    // Get the location of the predefined contacts vCards and/or XML
    // script
    iFileDirectory = iPdcData->GetFileLocationL();

    // Queue a request for the next state
    SelfComplete();
    }

// ---------------------------------------------------------------------------
// CPdcEngine::ReadVCardsL
// Starts the asyncrous importation of contacts from vCards
// ---------------------------------------------------------------------------
//    
void CPdcEngine::ReadVCardsL()
    {
    // Create the vCard importer
    iVCardImporter = CPdcVCardImporter::NewL(iFs, *iContactManager,
            *iContactStore, *iLinkArray);
    // Read in any vCards asyncronously
    iVCardImporter->GetVCardsL( *iFileDirectory, iStatus);
    SetActive();
    }

// ---------------------------------------------------------------------------
// CPdcEngine::ReadXmlL
// Starts the asycnronous importation of contacts from the XML script
// ---------------------------------------------------------------------------
//    
void CPdcEngine::ReadXmlL()
    {
    // Create the XML importer
    iXmlImporter = CPdcXmlImporter::NewL(iFs, *iContactStore, *iLinkArray);

    //transfer the pointer of ContactManager to Xmlimporter
    iXmlImporter->SetContactManager(iContactManager);
    // Read any contacts from XML asyncronously
    iXmlImporter->GetXmlContactsL( *iFileDirectory, iStatus);

    SetActive();
    }

// ---------------------------------------------------------------------------
// CPdcEngine::UpdateDataL
// Persists the internal data to file ( including links to any contacts
// that have been added )
// ---------------------------------------------------------------------------
//    
void CPdcEngine::UpdateDataL()
    {
    // Update the data
    iPdcData->StoreL();

    // Queue a request for the next state
    SelfComplete();
    }

// ---------------------------------------------------------------------------
// CPdcEngine::Finish
// Signals the completion of the state machine, either by completing the
// caller's TRequestStatus or be stopping the active scheduler loop.
// @param aError    Any error on completion or KErrNone otherwise
// ---------------------------------------------------------------------------
//    
void CPdcEngine::Finish(TInt /*aError*/)
    {
    // The process of adding predefined contacts has finished.
    CActiveScheduler::Stop();
    }

// ---------------------------------------------------------------------------
// CPdcEngine::DisplayErrorNoteL
// Displays an error message based upon the current state of the engine.
// @param aError    error that has occurred
// ---------------------------------------------------------------------------
//    
void CPdcEngine::DisplayErrorNoteL(TInt aError, TInt aErrorPosition,
        TBool errorCannotReserve)
    {
    LOGTEXT(_L("DisplayErrorNoteL"));
    TInt resId = -1;

    if (errorCannotReserve)
        {
        resId = R_PDC_ERROR_RESERVE_ERROR;
        }

    switch (aErrorPosition)
        {
        case EDeleteOldContacts:
            {
            // Error deleting old contacts
            resId = R_CONTACT_DELETION_ERROR;
            }
            break;
        case EGettingFileLocation:
            {
            // Error getting the file location
            resId = R_FILELOCATION_ERROR;
            }
            break;
        case EReadingVCards:
            {
            // Error reading the vCards
            resId = R_VCARD_ERROR;
            }
            break;
        case EReadingXML:
            {
            // Error read the XML file
            resId = R_XML_ERROR;
            }
            break;
        case EUpdateData:
            {
            // Error persisting the data
            resId = R_STORE_ERROR;
            }
            break;
        case EFinish:
        default:
            {
            // Should never reach this point
            ASSERT( 0 );
            }
        }

    TFileName fileName(KErrorResourceFile);
    CStringResourceReader* strings = CStringResourceReader::NewLC(fileName);

    TPtrC noteFormat;
    noteFormat.Set(strings->ReadResourceString(resId));

    TBuf<128> noteText;
    noteText.Format(noteFormat, aError);

    // Create and display the error note
    CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
    globalNote->ShowNoteL(EAknGlobalErrorNote, noteText);
    CleanupStack::PopAndDestroy(globalNote);
    CleanupStack::PopAndDestroy(strings);
    }

// ---------------------------------------------------------------------------
// CPdcEngine::StoreReady
// from MVPbkContactStoreObserver
// Called when the contact store is ready to be used, signals
// the next engine state.
// @param aContactStore The store that is ready.
// ---------------------------------------------------------------------------
//
void CPdcEngine::StoreReady(MVPbkContactStore& /*aContactStore*/)
    {
    LOGTEXT(_L("StoreReady"));
    // If we are waiting for the contacts to be opened, self
    // complete when the store ready callback is received.
    if (iEngineState == EOpeningContacts)
        {
        // Set the next state
        iEngineState = EDeleteOldContacts;
        iDeletedStoredContact = EFalse;
        SelfComplete();
        }
    }

// ---------------------------------------------------------------------------
// CPdcEngine::StoreUnavailable
// from MVPbkContactStoreObserver
// Called when a contact store becomes unavailable.
// @param aContactStore The store that became unavailable.
// @param aReason The reason why the store is unavailable.
//                This is one of the system wide error codes.
// ---------------------------------------------------------------------------
//
void CPdcEngine::StoreUnavailable(MVPbkContactStore& /*aContactStore*/,
        TInt aReason)
    {

    // Self complete with the error reason.
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, aReason);
    SetActive();
    }

// ---------------------------------------------------------------------------
// CPdcEngine::HandleStoreEventL
// from MVPbkContactStoreObserver
// Called when changes occur in the contact store.
// IGNORED.
// @param aContactStore A store whose event it is.
// @param aStoreEvent The event that has occurred.
// ---------------------------------------------------------------------------
//
void CPdcEngine::HandleStoreEventL(MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent /*aStoreEvent*/)
    {
    }

// ---------------------------------------------------------------------------
// CPdcEngine::DeleteStoredContactsL
// Called when error occur iwhile reading XML or vcard file.
// IGNORED.
// ---------------------------------------------------------------------------
//    
void CPdcEngine::DeleteStoredContactsL()
    {
    LOGTEXT(_L("DeleteStoredContactsL"));
    // Create the contacts deletion object
    delete iContactDeleter;
    iContactDeleter = NULL;

    iContactDeleter = CPdcContactDeletion::NewL( *iContactManager);
    iContactDeleter->DeleteStoredContactsL(iLinkArray, iStatus,
            iDeletedStoredContact);
    SetActive();
    }

