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


#include "CVPbkImportToContactsOperation.h"

// Internal
#include "CVPbkVCardCompactBCardImporter.h"
#include "CVPbkVCardImporter.h"
#include "VPbkVCardEngError.h"

// From Virtual Phonebook
#include <MVPbkSingleContactOperationObserver.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CVPbkImportToContactsOperation::CVPbkImportToContactsOperation
// ---------------------------------------------------------------------------
//
CVPbkImportToContactsOperation::CVPbkImportToContactsOperation(
        MVPbkSingleContactOperationObserver& aObserver )
        :   iObserver( aObserver )
    {
    }


// ---------------------------------------------------------------------------
// CVPbkImportToContactsOperation::ConstructL
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsOperation::ConstructL(
        TVPbkImportCardType aType,
        CVPbkVCardData& aData,
        RPointerArray<MVPbkStoreContact>& aImportedContacts,
        MVPbkContactStore& aTargetStore,
        RReadStream &aSourceStream )
    {    
    if ( aType == ECompactBusinessCard )
        {
        iOperationImpl = CVPbkVCardCompactBCardImporter::NewL(
            aImportedContacts, aTargetStore, aSourceStream, aData );
        }
    else if ( aType == EVCard )
        {
        iOperationImpl = CVPbkVCardImporter::NewL(
            aImportedContacts, aSourceStream, aTargetStore, aData );
        }
    else
        {
        User::Leave( KErrArgument );
        }        
    }

// ---------------------------------------------------------------------------
// CVPbkImportToContactsOperation::NewL
// ---------------------------------------------------------------------------
//
CVPbkImportToContactsOperation* CVPbkImportToContactsOperation::NewL(
        TVPbkImportCardType aType,
        CVPbkVCardData& aData,
        RPointerArray<MVPbkStoreContact>& aImportedContacts,
        MVPbkContactStore& aTargetStore,
        RReadStream &aSourceStream,
        MVPbkSingleContactOperationObserver& aObserver )
    {
    CVPbkImportToContactsOperation* self = 
        new( ELeave ) CVPbkImportToContactsOperation( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aType, aData, aImportedContacts, aTargetStore, 
        aSourceStream );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVPbkImportToContactsOperation::~CVPbkImportToContactsOperation()
    {
    delete iOperationImpl;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation
// CVPbkImportToContactsOperation::StartL
// ---------------------------------------------------------------------------
//
void CVPbkImportToContactsOperation::StartL()
    {
    __ASSERT_DEBUG( iOperationImpl, VPbkVCardEngError::Panic( 
        VPbkVCardEngError::EPreCond_CVPbkImportToStoreOperation_StartL ) );
        
    iOperationImpl->SetObserver( *this );    
    iOperationImpl->StartL();
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation
// CVPbkImportToContactsOperation::Cancel
// ---------------------------------------------------------------------------
//    
void CVPbkImportToContactsOperation::Cancel()
    {
    delete iOperationImpl;
    iOperationImpl = NULL;
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToContactsOperation::ContactsImported
// ---------------------------------------------------------------------------
//        
void CVPbkImportToContactsOperation::ContactsImported()
    {
    // Client get contacts from the array it gave in construction
    iObserver.VPbkSingleContactOperationComplete( *this, NULL );
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToContactsOperation::ContactImportingFailed
// ---------------------------------------------------------------------------
//            
void CVPbkImportToContactsOperation::ContactImportingFailed( TInt aError )
    {
    iObserver.VPbkSingleContactOperationFailed( *this, aError );
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToContactsOperation::ContactsImportingCompleted
// ---------------------------------------------------------------------------
//      
void CVPbkImportToContactsOperation::ContactsImportingCompleted()
    {
        // Unused
    }

// End of file    
