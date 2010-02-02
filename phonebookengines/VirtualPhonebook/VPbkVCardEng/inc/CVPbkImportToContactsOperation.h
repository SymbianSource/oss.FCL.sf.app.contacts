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
* Description:  An operation for importing the card to the contact(s)
*
*/



#ifndef CVPBKIMPORTTOCONTACTSOPERATION_H
#define CVPBKIMPORTTOCONTACTSOPERATION_H

#include "MVPbkImportOperationImpl.h"
#include <e32base.h>
#include <MVPbkContactOperation.h>

class CVPbkVCardData;
class MVPbkStoreContact;
class MVPbkContactStore;
class MVPbkSingleContactOperationObserver;

/**
 *  An operation for importing and saving a contact to the store
 */
NONSHARABLE_CLASS(CVPbkImportToContactsOperation) 
    :   public CBase,
        public MVPbkContactOperation,
        private MVPbkImportOperationObserver
    {
public:

    static CVPbkImportToContactsOperation* NewL(
        TVPbkImportCardType aType,
        CVPbkVCardData& aData,
        RPointerArray<MVPbkStoreContact>& aImportedContacts,
        MVPbkContactStore& aTargetStore,
        RReadStream &aSourceStream,
        MVPbkSingleContactOperationObserver& aObserver );

    virtual ~CVPbkImportToContactsOperation();

    
// from base class MVPbkContactOperation

    void StartL();
    void Cancel();

// from base class MVPbkImportOperationObserver
    void ContactsImported();
    void ContactImportingFailed( TInt aError );
    void ContactsImportingCompleted();
    
private:

    CVPbkImportToContactsOperation(
        MVPbkSingleContactOperationObserver& aObserver );
    void ConstructL( TVPbkImportCardType aType,
        CVPbkVCardData& aData,
        RPointerArray<MVPbkStoreContact>& aImportedContacts,
        MVPbkContactStore& aTargetStore,
        RReadStream &aSourceStream );

private: // data
        /// Ref: the client that is observing importing
        MVPbkSingleContactOperationObserver& iObserver;
        /// Own: the implementation of the import operation
        MVPbkImportOperationImpl* iOperationImpl;
    };

#endif // CVPBKIMPORTTOCONTACTSOPERATION_H
