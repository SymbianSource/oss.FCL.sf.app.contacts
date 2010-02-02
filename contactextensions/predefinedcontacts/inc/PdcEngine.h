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

#ifndef C_PDCENGINE_H
#define C_PDCENGINE_H

// System includes
#include <e32std.h>						//for iErrorArray
#include <e32base.h>                    // CActive
#include <f32file.h>                    // RFs 
#include <MVPbkContactStoreObserver.h>  // MVPbkContactStoreObserver
#include <flogger.h>

// Forward declarations
class CVPbkContactManager;
class CPdcVCardImporter;
class CPdcXmlImporter;
class CPdcData;
class MVPbkContactStore;
class CVPbkContactLinkArray;
class CPdcContactDeletion;

/**
 *  Predefined Contacts engine
 *  This class as a state machine for the reading of the
 *  predefined contacts from file, and adding the
 *  contacts to the contacts database by the delegation of
 *  the tasks to the relevent classes.
 */ 
NONSHARABLE_CLASS( CPdcEngine ): public CActive,
    public MVPbkContactStoreObserver
    {
public:
    /**
     * Current state of the engine is
     * represented by an enumeration.
     */
    enum TPdcEngineState
        {
        EOpeningContacts,
        EDeleteOldContacts,
        EGettingFileLocation,
        EReadingVCards,
        EReadingXML,
        EUpdateData,
        EDeleteStoredContact,
        EFinish
        };

public:    
    /**
    * Symbian 1st phase constructor
    * @return Self pointer to CPdcEngine pushed to
    * the cleanup stack.
    */
    static CPdcEngine* NewLC();

    /**
    * Destructor.
    */
    virtual ~CPdcEngine();

public:
    /**
    * Checks if the predefined contacts need to be added.
    * @return ETrue if the predefined contacts need to be added
    */
    TBool PredefinedContactsNeedAddingL();
    
    /**
    * Starts the asyncronous process of reading and adding the
    * predefined contacts to the contact model
    */
    void AddPredefinedContactsL();
    
    /**
    * Starts the asyncronous process of reading and adding the
    * predefined contacts to the contact model
    * @param aStatus  TRequestStatus of caller (called by ECom
    *                 plugin starting mechanism.
    */
    void AddPredefinedContactsL( TRequestStatus& aStatus );

private: // C++ constructor and the Symbian second-phase constructor
    CPdcEngine();
    void ConstructL();

private: // private member functions
    void SelfComplete();
    
    void DeleteContactsL();
    void GetFileLocationL();
    void ReadVCardsL();
    void ReadXmlL();
    void UpdateDataL();
    void Finish( TInt aError );
    
    void DisplayErrorNoteL( TInt aError, TInt aErrorPosition,
            TBool errorCannotReserve );
    
    /**
    *CPdcEngine::DeleteStoredContactsL
    * Called when error occur iwhile reading XML or vcard file.
    */
	  void DeleteStoredContactsL();
	  
private: // From CActive
    void DoCancel();
	void RunL();
	TInt RunError( TInt aError );
	
private: // from MVPbkContactStoreObserver
    void StoreReady( MVPbkContactStore& aContactStore );
    void StoreUnavailable( MVPbkContactStore& aContactStore, TInt aReason );
    void HandleStoreEventL( MVPbkContactStore& aContactStore, 
                                      TVPbkContactStoreEvent aStoreEvent );

private: // data
    /// Current state of engine
    TPdcEngineState iEngineState;
    /// File system
    RFs iFs;
    /// Own: Contact manager
    CVPbkContactManager* iContactManager;
    /// Own: Contact store
    MVPbkContactStore* iContactStore;
    /// Own: directory containing predefined contacts
    HBufC* iFileDirectory;
    /// Own: vCard importer
    CPdcVCardImporter* iVCardImporter;
    /// Own: Xml importer 
    CPdcXmlImporter* iXmlImporter;
    /// Own: Data checker
    CPdcData* iPdcData;
    /// Own: Ids of the contacts that have been added
    CVPbkContactLinkArray* iLinkArray;
    /// Own: Contact bulk deleter 
    CPdcContactDeletion* iContactDeleter;
    /// Ref: TRequestStatus of caller
    TRequestStatus* iCallerStatus;
    /// Judge the type of deletion operator
    TBool iDeletedStoredContact;
    };

#endif // C_PDCENGINE_H
