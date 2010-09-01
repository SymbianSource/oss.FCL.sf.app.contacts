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
* Description:    Responsible for the deletion of existing predefined contacts
*
*/

#ifndef C_PDCCONTACTDELETION_H
#define C_PDCCONTACTDELETION_H

// System includes
#include <e32base.h>                        // CBase
#include <MVPbkBatchOperationObserver.h>    // MVPbkBatchOperationObserver

// Forward declarations
class MVPbkContactOperationBase;
class CVPbkContactManager;
class MVPbkContactLinkArray;
class CVPbkContactLinkArray;

/**
 *  This class is responsible for the deletion of
 *  any existing predefined contacts.
 */
NONSHARABLE_CLASS( CPdcContactDeletion ): public CBase,
                            public MVPbkBatchOperationObserver
    {
public:
    /**
    * Symbian 1st phase constructor
    * @return Self pointer to CPdcContactDeletion
    * @param    aContactManager   contacts manager
    */
    static CPdcContactDeletion* NewL( CVPbkContactManager& aContactManager );
    
    /**
    * Destructor.
    */
    virtual ~CPdcContactDeletion();

public:
    /**
    * Deletes a list of contacts
    * @param    aLinkArray   array of links to contacts to be deleted,
    *                        takes ownership
    * @param    aCallerStatus     Caller's iStatus.
    */
    void DeleteContactsL( MVPbkContactLinkArray* aLinkArray,
                                     TRequestStatus& aCallerStatus ,TBool aDeleteStoredContact ); 
    
    /** 
     * Cancels the deletion process
     */
     void Cancel(); 
     void DeleteStoredContactsL(MVPbkContactLinkArray* aLinkArray,
                                     TRequestStatus& aCallerStatus,TBool aDeleteStoredContact  );
                                     
private: // C++ constructor and the Symbian second-phase constructor
    CPdcContactDeletion( CVPbkContactManager& aContactManager );
    void ConstructL();
    
private:
    void DeleteNextContact();
    TBool DeleteNextContactL();
    
private: // from MVPbkBatchOperationObserver
    void StepComplete( MVPbkContactOperationBase& aOperation,
                                    TInt aStepSize );
    TBool StepFailed( MVPbkContactOperationBase& aOperation,
                                    TInt aStepSize, TInt aError );
    void OperationComplete( MVPbkContactOperationBase& aOperation );
	
private: // data
    /// Ref: Contact manager
    CVPbkContactManager& iContactManager;
    /// Ref: Array of links to be deleted
    MVPbkContactLinkArray* iLinkArray;
    /// Ref: Link currently being deleted
    CVPbkContactLinkArray* iDeleteArray;
    /// Own: Async delete operation
    MVPbkContactOperationBase* iDeleteOp;
    /// Callers iStatus
    TRequestStatus* iCallerStatus;
    /// Index of contact being deleted 
    TInt iIndex;
    /// Judge the type of deletion operator
    TBool iDeleteStoredContact;
    };

#endif // C_PDCCONTACTDELETION_H
