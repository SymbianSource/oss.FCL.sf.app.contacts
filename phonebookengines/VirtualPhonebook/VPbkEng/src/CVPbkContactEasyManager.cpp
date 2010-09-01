/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Easy management of contacts.
*
*/


// INCLUDES

#include "CVPbkContactEasyManager.h"

#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkOperationObserver.h>

CVPbkContactEasyManager* CVPbkContactEasyManager::NewL(
    CVPbkContactManager& aContactManager )
    {
    CVPbkContactEasyManager* self =
        new (ELeave) CVPbkContactEasyManager(aContactManager);
    return self;
    }

CVPbkContactEasyManager::~CVPbkContactEasyManager()
    {
    delete iOperation;
    delete iContactLinks;
    }

void CVPbkContactEasyManager::RetrieveAndLockContactsL(
    const MVPbkContactLinkArray& aContactLinks,            
    RPointerArray<MVPbkStoreContact>& aRetrievedContacts,
    MVPbkOperationObserver& aObserver,
    MVPbkOperationErrorObserver& aErrorObserver )
    {
    iCancelled = EFalse;
    iObserver = & aObserver;
    iErrorObserver = &aErrorObserver;
    iContacts = &aRetrievedContacts; // owned externally
    delete iContactLinks;
    iContactLinks = NULL;
    iContactLinks = CloneLinkArrayL( aContactLinks );
    delete iOperation;
    iOperation = NULL;
    // Retrieve the first contact.
    // The next contacts are retrieved when the previous one completes.     
    if ( iContactLinks->Count() > 0 )
        {
        iOperation = iContactManager.RetrieveContactL( iContactLinks->At( 0 ), *this );
        iContactLinks->Delete( 0 );
        // Now waiting for callback.
        }
    }

void CVPbkContactEasyManager::Cancel()
    {
    iCancelled = ETrue;
    delete iOperation;
    iOperation = NULL;
    delete iContactLinks;
    iContactLinks = NULL;
    }

// ---------------------------------------------------------------
// These are from MVPbkSingleContactOperationObserver
// They are related to the contact retrieving.

void CVPbkContactEasyManager::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& /*aOperation*/,
    MVPbkStoreContact* aContact )
    {
    TRAPD( error, DoSingleContactOperationCompleteL( aContact ) );
    if ( error != KErrNone )
        {
        delete iOperation;
        iOperation = NULL;
        iErrorObserver->VPbkOperationFailed( this, error );
        }
    }

void CVPbkContactEasyManager::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& /*aOperation*/, 
    TInt aError )
    {
    iErrorObserver->VPbkOperationFailed( this, aError );
    delete iOperation;
    iOperation = NULL;
    }

// ----------------------------------------------------------------
// These are from from MVPbkContactObserver
// They are related to the contact locking.

void CVPbkContactEasyManager::ContactOperationCompleted(
    MVPbkContactObserver::TContactOpResult /*aResult*/ )
    {
    // Locking of a contact has completed.
    TRAPD( error, DoContactOperationCompletedL() );
    if ( error != KErrNone )
        {
        iErrorObserver->VPbkOperationFailed( this, error );
        }
    }

void CVPbkContactEasyManager::ContactOperationFailed(
    TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/)
    {
    if ( !iCancelled )
        {
        iErrorObserver->VPbkOperationFailed( this, aErrorCode );
        // No more processing after this.
        }
    }

// ------------------------------------------------------------------


/// Implementation which is called from VPbkSingleContactOperationComplete().
void CVPbkContactEasyManager::DoSingleContactOperationCompleteL(
    MVPbkStoreContact* aContact )
    {
    // Retrieving of a contact has completed.
    
    iContacts->AppendL( aContact ); // Ownership is given to array.
    delete iOperation;
    iOperation = NULL;

    if ( iContactLinks->Count() > 0 )
        {
        // Start to retrieve contacts one by one. The rest of the contact
        // retrievals are started from the callback.

        iOperation = iContactManager.RetrieveContactL(
            iContactLinks->At( 0 ), *this );
        iContactLinks->Delete( 0 );
        // Now waiting for callback.
        }
    else
        {
        // No more contacts to retrieve.
        // Now lock the retrieved contacts one by one.
        iNextContactToLock = 0;
        (*iContacts)[ iNextContactToLock ]->LockL( *this );        
        }
    }

/// Implementation which is called from ContactOperationCompleted().
void CVPbkContactEasyManager::DoContactOperationCompletedL()
    {
    // Locking of a contact has completed.
    if ( !iCancelled )
        {
        ++iNextContactToLock;
        if ( iNextContactToLock < iContacts->Count() )
            {
            // Still some contacts that need to be locked.
            (*iContacts)[ iNextContactToLock ]->LockL( *this );
            }
        else
            {
            // All contacts have been retrieved & locked.
            delete iContactLinks;
            iContactLinks = NULL;
            iObserver->VPbkOperationCompleted( this );
            }
        }
    }

CVPbkContactLinkArray* CVPbkContactEasyManager::CloneLinkArrayL(
    const MVPbkContactLinkArray& aArray )
    {
    CVPbkContactLinkArray* newArray = CVPbkContactLinkArray::NewLC();
    const TInt count = aArray.Count();
    for ( TInt n = 0; n < count; ++n )
        {
        MVPbkContactLink* link = aArray.At(n).CloneLC();
        newArray->AppendL( link );
        CleanupStack::Pop(); // link
        }
    CleanupStack::Pop( newArray );
    return newArray;
    }

CVPbkContactEasyManager::CVPbkContactEasyManager(
    CVPbkContactManager& aContactManager )
    : iContactManager( aContactManager )
    {
    }

// end of file
