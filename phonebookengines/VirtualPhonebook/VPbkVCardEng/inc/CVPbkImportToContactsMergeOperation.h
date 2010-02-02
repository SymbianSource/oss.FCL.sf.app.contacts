/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An operation for Updating a contact in the store
*
*/


#ifndef CVPbkImportToContactsMergeOperation_H
#define CVPbkImportToContactsMergeOperation_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactObserver.h>
#include <MVPbkContactOperation.h>
#include "MVPbkImportOperationImpl.h"
#include "CVPbkGroupCardHandler.h"
#include "MVPbkContactCopyObserver.h"


// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkContactStore;
class MVPbkSingleContactOperationObserver;
class MVPbkContactLink;
class MVPbkStoreContactField;
class MVPbkFieldType;
class MVPbkSingleContactOperationObserver;
class MVPbkContactCopyObserver;
class CVPbkVCardEng;
class CVPbkVCardImporter;
class CVPbkContactManager;

/**
 *  An operation for Updating a contact in the store
 */
NONSHARABLE_CLASS(CVPbkImportToContactsMergeOperation) 
    :   public CActive,
        public MVPbkContactOperation,
        public MVPbkImportOperationObserver,
        public MVPbkSingleContactOperationObserver,
        public MVPbkContactObserver
    {
public:
     static CVPbkImportToContactsMergeOperation *NewL( 
             TVPbkImportCardType aType,
             CVPbkVCardData& aData, 
             const MVPbkContactLink& aReplaceContact, 
             MVPbkContactStore& aTargetStore, 
             RReadStream& aSourceStream, 
             MVPbkSingleContactOperationObserver& aObserver );

	/**
	* Destructor.
	*/
    ~CVPbkImportToContactsMergeOperation();
        
protected: // CActive
	/**
	* From CActive. Called when asynchronous operation completes.
	*/
    void RunL();
	/**
	* From CActive. Called when RunL leaves.
	*/    

    TInt RunError( TInt aError );
	/**
	* From CActive. Cancels operation.
	*/

    void DoCancel();        

private:

    CVPbkImportToContactsMergeOperation( 
            const MVPbkContactLink& aReplaceContact,
            MVPbkSingleContactOperationObserver& aObserver,
            MVPbkContactStore& aTargetStore,
            CVPbkVCardData & aData
            );
	/**
	* Second phase constructor.
	*/
    void ConstructL( TVPbkImportCardType aType, RReadStream& aSourceStream);

public:
    // from base class MVPbkImportOperationObserver
      /**
		* From MVPbkImportOperationObserver  
		* Called when a contact import operation has Completed.
		*/
     void ContactsImported();
      /**
		* From MVPbkImportOperationObserver  
		* Called when a contact import operation has failed.
		*/
     void ContactImportingFailed( TInt aError );
      /**
		* From MVPbkImportOperationObserver  
		* Called when a contact import operation has Completed Fully.
		*/
     void ContactsImportingCompleted();
        
public:
    // from base class MVPbkContactOperation
     /**
		* From MVPbkContactOperation  
		* Called when a contact operation has to be started
		*/
     void StartL();
      /**
		* From MVPbkContactOperation  
		* Called when a contact operation has to be cancled
		*/
     void Cancel();
     
public: // from MVPbkSingleContactOperationObserver

	/**
		* From MVPbkSingleContactOperationObserver  
		* Called when the contact operation is complete
		* Client takes Ownership in deleting returned aContact.
		*/
    void VPbkSingleContactOperationComplete(
                    MVPbkContactOperationBase& aOperation,
                    MVPbkStoreContact* aContact );
    /**
		* From MVPbkSingleContactOperationObserver  
		* Called when the contact operation fails
		*/                
    void VPbkSingleContactOperationFailed(
                    MVPbkContactOperationBase& aOperation, 
                    TInt aError );
public:

    // from MVPbkContactObserver
    /**
		* From MVPbkContactObserver  
		* Called when a contact operation has succesfully completed.
		* Client takes Ownership in deleting returned aResult.
		*/
    void ContactOperationCompleted(TContactOpResult aResult);
    /**
		* From MVPbkContactObserver  
		* Called when a contact operation has failed.
		*/
    void ContactOperationFailed
                (TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);
    
private: // types
	enum TState
		{
        EImport,
        ERetrieve,
        ELock,
        EReplaceFields,
        ECommit,
        EReRetrieve, //Reload contact to have valid lastmodified datetime
        EComplete
        };

private:    
    void NextState( TState aNextState );
	/**
	* Retrive the contact to be updated
	*/
    void RetrieveContactL();
	/**
	* Lock the contact for editing
	*/
    void LockContactL();
    /**
	* Commit the updated contact
	*/
    void CommitContactL();
	/**
	* Update imported contacts
	*/
    void UpdateContactL();
	/**
	* Retrieve the contact after updation
	*/
    void ReRetrieveContactL();
    
    void HandleError(TInt aError);
    void HandleContactReplacing(MVPbkStoreContact* aContact);
	/**
	* Replace the fields of target contact
	*/
    void ReplaceContactL( const MVPbkStoreContact& aSrc, MVPbkStoreContact& aTarget );
	/**
	* Copy the fields of source contact to target contact
	*/
    void CopyFieldL( const MVPbkStoreContactField& aSourceField,
    		const MVPbkFieldType& aType,
            MVPbkStoreContact& aTargetContact );
    
private: // data
	/// Own: state
	TState iState;
    /// Own: the imported contacts
    RPointerArray<MVPbkStoreContact> iImportedContacts;
    /// Own: the implementation of the import operation
    MVPbkImportOperationImpl* iOperationImpl;
    /// Ref: the target store of the operation
    MVPbkContactStore& iTargetStore;
    //Ref: Vcard Data
    CVPbkVCardData& iData;
	/// Ref: contact to replace
	const MVPbkContactLink& iReplaceContact;
    /// Own: current contact
    MVPbkStoreContact* iContact;
    /// Ref: client that waits importing
    MVPbkSingleContactOperationObserver& iObserver;
    /// Own: the operation for import and retrieve
    MVPbkContactOperationBase* iVPbkOperation;
    //Own: Group Card Handler
    CVPbkGroupCardHandler* iGroupcardHandler;
    };

#endif
