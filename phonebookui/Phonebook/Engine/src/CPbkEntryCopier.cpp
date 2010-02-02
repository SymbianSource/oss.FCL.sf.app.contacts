/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*		Offers interface for adding entries to Phonebook.
*
*/


// INCLUDE FILES
#include    <CPbkEntryCopier.h>
#include    <cntdb.h>

// Phonebook debug
#include    <PbkDebug.h>

// Phonebook engine
#include    "CPbkContactEngine.h"
#include    "CPbkIdleFinder.h"
#include    "CPbkContactItem.h"

// Phonebook copy
#include    "CPbkNameLookup.h"
#include    "CPbkEntryCopyAddToExisting.h"

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_FindExistingNameL = 0,
    EPanicLogic_ProceedToNextStateL,
    EPanicPreCond_FindAsyncNameL,
    EPanicPreCond_IdleFindCallback
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkEntryCopier");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace


// MODULE DATA STRUCTURES
enum KPbkAddNewDataState
    {
    EPbkAsyncFindState = 0,
    EPbkNameLookupState,
    EPbkInsertToPbkState,
    EPbkCompressDbState,
    EPbkFinishedState
    };

/**
 * Interface for states that perform adding new data to Phonebook.
 */
class MPbkAddNewDataState
    {
    public: // interface
        /**
         * Virtual destructor.
         */
        virtual ~MPbkAddNewDataState() {};
        /**
         * Initializes the state.
         */
        virtual void Initialize() {};
        /**
         * Searches for the contact item asynchronously.
         * @param aName The contact item to search for.
         */
        virtual void FindAsyncNameL(CPbkContactItem& /*aName*/) {};
        /**
         * Returns the contact id array of matched names.
         * Ownership is transferred to caller.
         * @return Contact id array of matched names.
         */
        virtual CContactIdArray* GetContactIdArray() { return NULL; };
        /**
         * Lookup the given name from the array of contact ids.
         * @param aContactIds array of contact ids to serach for the name.
         * @param aName The contact to search for.
         */
        virtual void NameLookupL
            (CContactIdArray* /*aContactIds*/, CPbkContactItem& /*aName*/) {};
        /**
         * Returns the contact id that matched the search.
         */
        virtual TContactItemId GetContactId() const { return KNullContactId; };
        /**
         * Inserts the entry to Phonebook.
         * @param aContactId If a contact id is provided here, an existing
         *                   entry will be updated.
         * @param aEntry Entry that will be added to Phonebook.
         */
        virtual void InsertToPbkL(TContactItemId /*aContactId*/, 
                CPbkContactItem& /*aEntry*/) { };

        /**
         * Performs Phonebook database compression.
         */
        virtual void CompressL() {};

    };

/**
 * Base class for states that add new data to Phonebook.
 */
NONSHARABLE_CLASS(CPbkAddNewDataBaseState) :
        public CBase, 
        public MPbkAddNewDataState
    {
    protected: // Construction / destruction
        CPbkAddNewDataBaseState(CPbkEntryCopier& aProcess);
        ~CPbkAddNewDataBaseState();

    protected: // utility function
        void AdvanceToNextState(TInt aState, TInt aError);

    private: // data members
        /// Ref: state advancement
        CPbkEntryCopier& iProcess;
    };

/**
 * State for finding data asynchronously from Phonebook.
 */
NONSHARABLE_CLASS(CPbkAsyncFindState) : 
        public CPbkAddNewDataBaseState, 
        private MIdleFindObserver
    {
    public: // construction / destruction
        static CPbkAsyncFindState* NewL(CPbkContactEngine& aEngine, 
                CPbkEntryCopier& aProcess);
        ~CPbkAsyncFindState();

    public: // From CPbkAddNewDataBaseState
        void FindAsyncNameL(CPbkContactItem& aEntry);
        CContactIdArray* GetContactIdArray();
        
    private:    // from MIdleFindObserver
        void IdleFindCallback();
    
    private: // implementation
        CPbkAsyncFindState(CPbkContactEngine& aEngine,
                CPbkEntryCopier& aProcess);
        void ConstructL();

    private: // data members
        /// Ref: contact engine
        CPbkContactEngine& iEngine;
        /// Own: find id array for finding
        CPbkFieldIdArray* iFindFrom;
        /// Own: contact Id array
        CContactIdArray* iContactIds;
        /// Own: idle finder active object
        CPbkIdleFinder* iIdleFinder;
    };


/**
 * State for finding data asynchronously from Phonebook.
 */
NONSHARABLE_CLASS(CPbkNameLookupState) : 
        public CPbkAddNewDataBaseState, 
        private MPbkNameLookupObserver
    {
    public: // construction / destruction
        static CPbkNameLookupState* NewL(CPbkContactEngine& aEngine,
                CPbkEntryCopier& aProcess);
        ~CPbkNameLookupState();

    public: // From CPbkAddNewDataBaseState
        void NameLookupL(CContactIdArray* aContactIds, CPbkContactItem& aName);
        TContactItemId GetContactId() const;
        void Initialize();

    private:    // from MPbkNameLookupObserver
        void NameLookupCompleted(TContactItemId aContactId);
        void NameLookupFailed(TInt aError);
    
    private: // implementation
        CPbkNameLookupState(CPbkContactEngine& aEngine,
                CPbkEntryCopier& aProcess);
        void ConstructL();

    private: // data members
        /// Ref: phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: name lookup active object
        CPbkNameLookup* iNameLookup;
        /// Own: Contact id of the contact that was found
        TContactItemId iContactId;
    };

/**
 * State for inserting data to Phonebook.
 */
NONSHARABLE_CLASS(CPbkInsertToPhonebookState) : 
        public CActive, 
        public MPbkAddNewDataState
    {
    public: // construction / destruction
        static CPbkInsertToPhonebookState* NewL(CPbkContactEngine& aEngine,
                CPbkEntryCopier& aProcess);
        ~CPbkInsertToPhonebookState();

    public: // From MPbkAddNewDataState
        void InsertToPbkL(TContactItemId aContactId, CPbkContactItem& aEntry);        
        TContactItemId GetContactId() const;

    private: // from CActive
	    void DoCancel();
	    void RunL();
	    TInt RunError(TInt aError);

    private: // implementation
        CPbkInsertToPhonebookState(CPbkContactEngine& aEngine,
                CPbkEntryCopier& aProcess);        
        void AdvanceToNextState(TInt aState, TInt aError);
    
    private: // data members
        /// Ref: phonebook engine
        CPbkContactEngine& iEngine;
        /// Ref: state advancement
        CPbkEntryCopier& iProcess;
        /// Own: contact id of the inserted contact
        TContactItemId iContactId;
        /// Ref: entry reference
        CPbkContactItem* iEntry;
    };

/**
 * State for performing the Phonebook DB compression.
 */
NONSHARABLE_CLASS(CPbkDbCompressState) : 
        public CPbkAddNewDataBaseState, 
        private MContactUiCompactObserver
    {
    public: // interface
        static CPbkDbCompressState* NewL(CPbkContactEngine& aEngine,
                CPbkEntryCopier& aProcess);        
        ~CPbkDbCompressState();

    public: // From CPbkAddNewDataBaseState
        void CompressL();

    private: // from MContactUiCompactObserver
    	void Step(TInt aStep);
	    void HandleError(TInt aError);

    private: // implementation
        CPbkDbCompressState(CPbkContactEngine& aEngine,
                CPbkEntryCopier& aProcess);
        void ConstructL();

    private: // data members
        /// Ref: contact engine
        CPbkContactEngine& iEngine;
        /// Own: compressor active object
        CContactActiveCompress* iCompressor;
    };

// ================= MEMBER FUNCTIONS =======================

CPbkAddNewDataBaseState::CPbkAddNewDataBaseState
        (CPbkEntryCopier& aProcess) :
    iProcess(aProcess)
    {
    }

CPbkAddNewDataBaseState::~CPbkAddNewDataBaseState()
    {
    }

void CPbkAddNewDataBaseState::AdvanceToNextState
        (TInt aState, 
        TInt aError)
    {
    iProcess.ProceedToNextState(aState, aError);
    }

///////////////////////////////////////////////////////////////////////////////
//// CPbkAsyncFindState

inline CPbkAsyncFindState::CPbkAsyncFindState
        (CPbkContactEngine& aEngine,
        CPbkEntryCopier& aProcess) :
    CPbkAddNewDataBaseState(aProcess),
    iEngine(aEngine)
    {
    }

inline void CPbkAsyncFindState::ConstructL()
    {
    // Build a CPbkFieldArray specifying the field types to search
    iFindFrom = new(ELeave) CPbkFieldIdArray;
    iFindFrom->AppendL(EPbkFieldIdLastName);
    }

CPbkAsyncFindState* CPbkAsyncFindState::NewL
        (CPbkContactEngine& aEngine,
        CPbkEntryCopier& aProcess)
    {
    CPbkAsyncFindState* self = new(ELeave) CPbkAsyncFindState(aEngine, aProcess);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkAsyncFindState::~CPbkAsyncFindState()
    {
    delete iIdleFinder;
    delete iContactIds;
    delete iFindFrom;
    }

void CPbkAsyncFindState::IdleFindCallback()
    {
    __ASSERT_DEBUG(iIdleFinder, Panic(EPanicPreCond_IdleFindCallback));

    TInt error = iIdleFinder->Error();
    if (error)
        {
        AdvanceToNextState(EPbkFinishedState, error);
        }
    if (iIdleFinder->IsComplete())
        {
        if (iContactIds)
            {
            delete iContactIds;
            iContactIds = NULL;
            }
        // take ownership of contact id array
        iContactIds = iIdleFinder->TakeContactIds();
        AdvanceToNextState(EPbkNameLookupState, error);
        }
    }

CContactIdArray* CPbkAsyncFindState::GetContactIdArray()
    {
    CContactIdArray* idArray = iContactIds;
    iContactIds = NULL;
    return idArray;
    }

void CPbkAsyncFindState::FindAsyncNameL(CPbkContactItem& aEntry)
    {
    if (iIdleFinder)
        {
        iIdleFinder->IdleFinder()->Cancel();
        }

    TPbkContactItemField* lastNameField = aEntry.FindField(EPbkFieldIdLastName);
    __ASSERT_DEBUG(lastNameField, Panic(EPanicPreCond_FindAsyncNameL));
    CPbkIdleFinder* idleFinder = 
        iEngine.FindAsyncL(lastNameField->Text(), iFindFrom, this);
    if (iIdleFinder)
        {
        delete iIdleFinder;
        iIdleFinder = NULL;
        }
    iIdleFinder = idleFinder;
    }

///////////////////////////////////////////////////////////////////////////////
//// CPbkNameLookupState

inline CPbkNameLookupState::CPbkNameLookupState
        (CPbkContactEngine& aEngine,
        CPbkEntryCopier& aProcess) :
    CPbkAddNewDataBaseState(aProcess),
    iEngine(aEngine),
    iContactId(KNullContactId)
    {
    }

inline void CPbkNameLookupState::ConstructL()
    {
    iNameLookup = CPbkNameLookup::NewL(iEngine, *this);
    }

CPbkNameLookupState* CPbkNameLookupState::NewL
        (CPbkContactEngine& aEngine,
        CPbkEntryCopier& aProcess)
    {
    CPbkNameLookupState* self = new(ELeave) CPbkNameLookupState(aEngine, aProcess);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkNameLookupState::~CPbkNameLookupState()
    {
    delete iNameLookup;
    }

void CPbkNameLookupState::NameLookupCompleted
        (TContactItemId aContactId)
    {
    // Proceed to next state
    iContactId = aContactId;
    AdvanceToNextState(EPbkInsertToPbkState, KErrNone);
    }

void CPbkNameLookupState::NameLookupFailed
        (TInt aError)
    {
    AdvanceToNextState(EPbkFinishedState, aError);
    }

TContactItemId CPbkNameLookupState::GetContactId() const
    {
    return iContactId;
    }

void CPbkNameLookupState::NameLookupL(CContactIdArray* aContactIds, 
                                      CPbkContactItem& aEntry)
    {
    iNameLookup->Cancel();
    iNameLookup->IssueNameLookupL(aContactIds, aEntry);
    }

void CPbkNameLookupState::Initialize()
    {
    iContactId = KNullContactId;
    };

///////////////////////////////////////////////////////////////////////////////
//// CPbkInsertToPhonebookState

inline CPbkInsertToPhonebookState::CPbkInsertToPhonebookState
        (CPbkContactEngine& aEngine,
        CPbkEntryCopier& aProcess) :
    CActive(EPriorityIdle),
    iEngine(aEngine),
    iProcess(aProcess)
    {
    CActiveScheduler::Add(this);
    }

CPbkInsertToPhonebookState* CPbkInsertToPhonebookState::NewL
        (CPbkContactEngine& aEngine,
        CPbkEntryCopier& aProcess)
    {
    return new(ELeave) CPbkInsertToPhonebookState(aEngine, aProcess);    
    }

CPbkInsertToPhonebookState::~CPbkInsertToPhonebookState()
    {
    Cancel();
    }

void CPbkInsertToPhonebookState::InsertToPbkL
        (TContactItemId aContactId, 
        CPbkContactItem& aEntry)
    {
    Cancel();
    iContactId = aContactId;
    iEntry = &aEntry;

    TRequestStatus* rs = &iStatus;
    User::RequestComplete(rs, KErrNone);
    SetActive();
    }

TContactItemId CPbkInsertToPhonebookState::GetContactId() const
    { 
    return iContactId; 
    }

void CPbkInsertToPhonebookState::DoCancel()
    {
    }

void CPbkInsertToPhonebookState::RunL()
    {
    if (iContactId == KNullContactId)
        {
        // add new entry if it is not empty
        CPbkContactItem* emptyItem = iEngine.CreateEmptyContactL();
        CleanupStack::PushL(emptyItem);
        if (*emptyItem != *iEntry)
            {
            iContactId = iEngine.AddNewContactL(*iEntry);
            }
        CleanupStack::PopAndDestroy(emptyItem);
        }
    else
        {
        // Add to the existing phonebook entry
        CPbkEntryCopyAddToExisting* entryHandler = 
            CPbkEntryCopyAddToExisting::NewLC(*iEntry, iEngine, iContactId);
        iContactId = entryHandler->CopyL();
        CleanupStack::PopAndDestroy(entryHandler);
        }
    AdvanceToNextState(EPbkFinishedState, KErrNone); // skip the compress here.
    }

TInt CPbkInsertToPhonebookState::RunError
        (TInt aError)
    {
    if (aError)
        {
        AdvanceToNextState(EPbkFinishedState, aError);
        }
    return KErrNone;
    }

void CPbkInsertToPhonebookState::AdvanceToNextState
        (TInt aState, 
        TInt aError)
    {
    iProcess.ProceedToNextState(aState, aError);
    }

///////////////////////////////////////////////////////////////////////////////
//// CPbkDbCompressState

inline CPbkDbCompressState::CPbkDbCompressState
        (CPbkContactEngine& aEngine,
        CPbkEntryCopier& aProcess) : 
    CPbkAddNewDataBaseState(aProcess),
    iEngine(aEngine)
    {
    }

CPbkDbCompressState* CPbkDbCompressState::NewL
        (CPbkContactEngine& aEngine,
        CPbkEntryCopier& aProcess)
    {
    return new(ELeave) CPbkDbCompressState(aEngine, aProcess);    
    }

CPbkDbCompressState::~CPbkDbCompressState()
    {
    delete iCompressor;
    }

void CPbkDbCompressState::Step
        (TInt /*aStep*/)
    {
    if (iCompressor->StepsTogo() <= 0)
        {
        AdvanceToNextState(EPbkFinishedState, KErrNone);
        }
    }

void CPbkDbCompressState::HandleError
        (TInt aError)
    {
    AdvanceToNextState(EPbkFinishedState, aError);
    }

void CPbkDbCompressState::CompressL()
    {
    CContactActiveCompress* compressor = iEngine.Database().CreateCompressorLC();
    compressor->SetObserver(this);
    CleanupStack::Pop(compressor);
    delete iCompressor;
    iCompressor = compressor;
    }

///////////////////////////////////////////////////////////////////////////////
/// CPbkEntryCopier

inline CPbkEntryCopier::CPbkEntryCopier
        (CPbkContactEngine& aEngine, MPbkEntryCopyObserver& aObserver) : 
    iEngine(aEngine),
    iObserver(aObserver),
    iStateArray(4) // array granularity
    {
    }

inline void CPbkEntryCopier::ConstructL()
    {
    // create the states
    CPbkAsyncFindState* findState = CPbkAsyncFindState::NewL(iEngine, *this);
    CleanupStack::PushL(findState);
    User::LeaveIfError(iStateArray.Append(findState));
    CleanupStack::Pop(findState);

    CPbkNameLookupState* lookupState = 
        CPbkNameLookupState::NewL(iEngine, *this);
    CleanupStack::PushL(lookupState);
    User::LeaveIfError(iStateArray.Append(lookupState));
    CleanupStack::Pop(lookupState);

    CPbkInsertToPhonebookState* insertState = 
        CPbkInsertToPhonebookState::NewL(iEngine, *this);
    CleanupStack::PushL(insertState);
    User::LeaveIfError(iStateArray.Append(insertState));
    CleanupStack::Pop(insertState);

    CPbkDbCompressState* compressState = 
        CPbkDbCompressState::NewL(iEngine, *this);
    CleanupStack::PushL(compressState);
    User::LeaveIfError(iStateArray.Append(compressState));
    CleanupStack::Pop(compressState);
    }

EXPORT_C CPbkEntryCopier* CPbkEntryCopier::NewL
        (CPbkContactEngine& aEngine, MPbkEntryCopyObserver& aObserver)
    {
    CPbkEntryCopier* self = new(ELeave) CPbkEntryCopier(aEngine, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C void CPbkEntryCopier::AddToPhonebookL(CPbkContactItem& aEntry)
    {
    iEntry = &aEntry;
    iNewContactId = KNullContactId;

    TInt state = EPbkAsyncFindState;
    iStateArray[EPbkNameLookupState]->Initialize();
    HBufC* title = iEntry->GetContactTitleOrNullL();
    if (!title)
        {
        // if no name insert to phonebook
        state = EPbkInsertToPbkState;
        }
    delete title;
    ProceedToNextState(state, KErrNone);
    }

CPbkEntryCopier::~CPbkEntryCopier()
    {
    iStateArray.ResetAndDestroy();
    iStateArray.Close();
    }

void CPbkEntryCopier::ProceedToNextState(TInt aState, TInt aError)
    {
    // check if error
    if (aError != KErrNone)
        {
        iObserver.CopyError(aError);
        }
    else
        {
        TRAPD(error, DoProceedToNextStateL(aState));
        if (error)
            {
            iObserver.CopyError(error);
            }
        }
    }

void CPbkEntryCopier::DoProceedToNextStateL(TInt aState)
    {
    switch (aState)
        {
        case EPbkAsyncFindState:
            {
            iStateArray[EPbkAsyncFindState]->FindAsyncNameL(*iEntry);
            break;
            }
        case EPbkNameLookupState:
            {
            CContactIdArray* idArray = 
                iStateArray[EPbkAsyncFindState]->GetContactIdArray();
            iStateArray[EPbkNameLookupState]->NameLookupL(idArray, *iEntry);
            break;
            }
        case EPbkInsertToPbkState:
            {
            const TContactItemId id = 
                iStateArray[EPbkNameLookupState]->GetContactId();
            iStateArray[EPbkInsertToPbkState]->InsertToPbkL(id, *iEntry);
            break;
            }
        case EPbkCompressDbState:
            {
            iStateArray[EPbkCompressDbState]->CompressL();
            break;
            }
        case EPbkFinishedState:
            {
            iNewContactId = iStateArray[EPbkInsertToPbkState]->GetContactId();
            iObserver.CopySuccess(iNewContactId);
            break;
            }
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(EPanicLogic_ProceedToNextStateL));
            }
        }
    }

//  End of File  
