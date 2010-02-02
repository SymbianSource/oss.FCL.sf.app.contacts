/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An operation for importing and saving a contact to the store
*
*/



#ifndef CVPBKIMPORTTOSTOREOPERATION_H
#define CVPBKIMPORTTOSTOREOPERATION_H

#include <e32base.h>
#include <MVPbkContactOperation.h>
#include <MVPbkContactCopyObserver.h>
#include <MVPbkContactFindObserver.h>
#include "CVPbkOwnCardHandler.h"
#include "CVPbkGroupCardHandler.h"
#include "MVPbkImportOperationImpl.h"

class MVPbkImportOperationImpl;
class MVPbkStoreContact;
class MVPbkContactStore;
class CVPbkVCardData;
class MVPbkContactCopyObserver;
class CVPbkContactLinkArray;
class MVPbkContactOperationBase;
class MVPbkContactCopyPolicy;
class MVPbkContactDuplicatePolicy;

/**
 *  An operation for importing and saving a contact to the store
 */
NONSHARABLE_CLASS(CVPbkImportToStoreOperation) 
    :   public CBase,
        public MVPbkContactOperation,
        private MVPbkImportOperationObserver,
        private MVPbkContactCopyObserver,
        private MVPbkContactFindObserver
    {
public:
        
    static CVPbkImportToStoreOperation* NewL( 
        TVPbkImportCardType aType,
        MVPbkContactStore& aTargetStore, 
        RReadStream &aSourceStream,
        CVPbkVCardData& aData,
        MVPbkContactCopyObserver& aObserver,
        TBool aSync );

    virtual ~CVPbkImportToStoreOperation();

    
// from base class MVPbkContactOperation
    void StartL();
    void Cancel();

// from base class MVPbkImportOperationObserver
    void ContactsImported();
    void ContactImportingFailed( TInt aError );
    void ContactsImportingCompleted();

// from base class MVPbkContactCopyObserver
    void ContactsSaved(MVPbkContactOperationBase& aOperation,
            MVPbkContactLinkArray* aResults);
    void ContactsSavingFailed(MVPbkContactOperationBase& aOperation,
            TInt aError);
            
// from base class MVPbkContactFindObserver
    void FindCompleteL( MVPbkContactLinkArray* aResults );
    void FindFailed( TInt aError );

private:

    CVPbkImportToStoreOperation( MVPbkContactCopyObserver& aObserver,
        MVPbkContactStore& aTargetStore );
    void ConstructL( 
        TVPbkImportCardType aType, 
        RReadStream &aSourceStream,
        CVPbkVCardData& aData,
        TBool aSync  );
    
    void CommitNextContactL();
    void HandleContactSavedL( MVPbkContactLinkArray* aResults );
    void ResetOperation();
    
private: // data
        /// Own: the imported contacts
        RPointerArray<MVPbkStoreContact> iImportedContacts;
        /// Ref: the target store of the operation
        MVPbkContactStore& iTargetStore;
        /// Ref: client that waits importing
        MVPbkContactCopyObserver& iObserver;
        /// Own: the implementation of the import operation
        MVPbkImportOperationImpl* iOperationImpl;
        /// Own; the operation that commits the contact
        MVPbkContactOperationBase* iVPbkOperation;
        /// Own: an array of links of saved contacts
        CVPbkContactLinkArray* iSavedContacts;
        /// Ref: a copy policy for target store
        MVPbkContactCopyPolicy* iCopyPolicy;
        /// Ref: a duplicate policy for finding duplicate before copy
        MVPbkContactDuplicatePolicy* iDuplicatePolicy;
        /// Own: owns the contacts
        RPointerArray<MVPbkStoreContact> iDuplicates;
         CVPbkOwnCardHandler* iOwncardHandler;
        //Own: GroupCrad Handler
        CVPbkGroupCardHandler* iGroupcardHandler;
    };

#endif // CVPBKIMPORTTOSTOREOPERATION_H
