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
* Description:    XML importer
*
*/

// System includes
#include <CVPbkContactManager.h>        // CVPbkContactManager
#include <MVPbkContactOperationBase.h>  // MVPbkContactOperationBase
#include <MVPbkContactLinkArray.h>      // MVPbkContactLinkArray
#include <CVPbkContactLinkArray.h>      // CVPbkContactLinkArray
#include <MVPbkContactLink.h>           // MVPbkContactLink
#include <MVPbkContactOperation.h>      // MVPbkContactOperation
#include <e32def.h>

// User includes
#include "pdccontactdeletion.h"         // CPdcContactDeletion
#include "pdclogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPdcContactDeletion::NewL
// Symbian 1st phase constructor
// @param    aFs     file system
// @param    aContactDatabase   contacts database
// @return Self pointer to CPdcContactDeletion pushed to
// the cleanup stack.
// ---------------------------------------------------------------------------
//
CPdcContactDeletion* CPdcContactDeletion::NewL(
        CVPbkContactManager& aContactManager )
    {
    CPdcContactDeletion* self = new( ELeave )
        CPdcContactDeletion( aContactManager);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPdcContactDeletion::~CPdcContactDeletion
// Destructor
// ---------------------------------------------------------------------------
//
CPdcContactDeletion::~CPdcContactDeletion()
    {
    delete iDeleteOp;
    delete iLinkArray;
    delete iDeleteArray;
    }

// ---------------------------------------------------------------------------
// CPdcContactDeletion::CPdcContactDeletion
// C++ constructor
// ---------------------------------------------------------------------------
//
CPdcContactDeletion::CPdcContactDeletion(
        CVPbkContactManager& aContactManager )
    : iContactManager( aContactManager )
    {
    }
    
// ---------------------------------------------------------------------------
//  CPdcContactDeletion::ConstructL
//  Second-phase constructor
// ---------------------------------------------------------------------------
//
void CPdcContactDeletion::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// CPdcContactDeletion::GetXmlContactsL 
// Starts the asyncronous deletion of existing contacts
// @param   aLinkArray   array of links to contacts to be deleted,
//                        takes ownership
// @param   aCallerStatus       iStatus of caller
// ---------------------------------------------------------------------------
//
void CPdcContactDeletion::DeleteContactsL( MVPbkContactLinkArray* aLinkArray,
                                     TRequestStatus& aCallerStatus,TBool aDeleteStoredContact  )
    {
    LOGTEXT(_L("CPdcContactDeletion::DeleteContactsL"));
    iDeleteStoredContact = aDeleteStoredContact;
    
    // Create the array of links
    iDeleteArray = CVPbkContactLinkArray::NewL();
    
    // Set the index to the number of contacts in the link array.
    iLinkArray = aLinkArray;
    iIndex = iLinkArray->Count();
    
    // Store the caller's request status
    iCallerStatus = &aCallerStatus;
    aCallerStatus = KRequestPending;
    
    // Delete the contacts
    DeleteNextContact();    
    }
      
// ---------------------------------------------------------------------------
// CPdcContactDeletion::Cancel
// Cancels the deletion process
// ---------------------------------------------------------------------------
//
void CPdcContactDeletion::Cancel()
    {
    LOGTEXT(_L("CPdcContactDeletion::Cancel"));
    
    if( iDeleteOp )
    	{
    	// The delete operation is cancelled by deleting the operation.
        MVPbkContactOperation* contactOp =
            STATIC_CAST( MVPbkContactOperation*, iDeleteOp );
        contactOp->Cancel();
    	}
    }
    
// ---------------------------------------------------------------------------
// CPdcContactDeletion::StepComplete
// From MVPbkBatchOperationObserver.
// Called when one step of the delete operation is completed.
// IGNORED
// ---------------------------------------------------------------------------
//
void CPdcContactDeletion::StepComplete( 
        MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/ )
    {
    }

// ---------------------------------------------------------------------------
// CPdcContactDeletion::StepFailed
// From MVPbkBatchOperationObserver.
// Called when one step of the delete operation fails.
//
// @return ETrue if the batch operation should continue, 
//               EFalse otherwise
// ---------------------------------------------------------------------------
//
TBool CPdcContactDeletion::StepFailed( 
        MVPbkContactOperationBase& /*aOperation*/,
        TInt /*aStepSize*/, TInt /*aError*/ )
    {
    LOGTEXT(_L("CPdcContactDeletion::StepFailed"));
    // Attempt to delete the next contact if it exists
    if( iDeleteStoredContact)
        {
        DeleteNextContact();
        }
    else
    	{
    	User::RequestComplete( iCallerStatus, KErrNone );	
    	}
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CPdcContactDeletion::OperationComplete
// From MVPbkBatchOperationObserver.
// Called when delete operation is completed.
// This is called when all steps are executed.
//
// @param aOperation    the completed operation
// ---------------------------------------------------------------------------
//
void CPdcContactDeletion::OperationComplete(
        MVPbkContactOperationBase& /*aOperation*/ )
    {
    LOGTEXT(_L("CPdcContactDeletion::OperationComplete"));    
    // Attempt to delete the next contact if it exists
    if( iDeleteStoredContact)
        {
        DeleteNextContact();
        }
    else
        {
        User::RequestComplete( iCallerStatus, KErrNone );   
        }
    }
 
// ---------------------------------------------------------------------------
// CPdcContactDeletion::DeleteNextContact()
// Traps the leaving function DeleteNextContactL()
// 
// ---------------------------------------------------------------------------
//
void CPdcContactDeletion::DeleteNextContact()
    {
    LOGTEXT(_L("CPdcContactDeletion::DeleteNextContact"));
    TBool nextContact = EFalse;
    
    // Attempt to delete the next contact, if there is no
    // more contacts complete the caller's TRequestStatus
    TRAP_IGNORE( ( nextContact = DeleteNextContactL()  ))
    if ( !nextContact )
        {
        User::RequestComplete( iCallerStatus, KErrNone );
        }
    }
    
// ---------------------------------------------------------------------------
// CPdcContactDeletion::DeleteNextContactL()
// Deletes first contact in the link array, and removes it from the list.
//
// IMPORTANT NOTE:
// Each contact in the link array needs to be deleted individually. Although 
// CVPbkContactManager::DeleteContactsL can delete an array of contacts if the
// deletion process fails for some reason such as the contact has already been 
// deleted, it does not continue to delete the rest of the elements. ( The 
// in-code description  for the MVPbkBatchOperationObserver callback appear to
// be incorrect ).
// 
// @return ETrue if there is a contact to be deleted.
// ---------------------------------------------------------------------------
//
TBool CPdcContactDeletion::DeleteNextContactL()
    {
    TBool returnValue = EFalse;
    
    // Decrement the index
    --iIndex;
    if ( iIndex >= 0 )
        {
        // There is a contact to be deleted
        returnValue = ETrue;
        
        // Remove any entries in the delete array
        if ( iDeleteArray->Count() != 0 )
            {
            iDeleteArray->Delete( 0 );
            }
        
        // Get the link to be deleted
        const MVPbkContactLink& link = iLinkArray->At( iIndex );
        
        // Create a copy of the link and add it to
        // the delete array
        MVPbkContactLink* copiedLink = link.CloneLC();
        iDeleteArray->AppendL( copiedLink );
        CleanupStack::Pop(); // copiedLink 
        
        // delete the existing operation
        delete iDeleteOp;
        iDeleteOp = NULL;

        // Create the delete operation
        iDeleteOp = iContactManager.DeleteContactsL( *iDeleteArray, *this );
        }
        
    return returnValue;
    }
    
// ---------------------------------------------------------------------------
// CPdcContactDeletion::DeleteStoredContactsL 
// Starts the asyncronous deletion of existing contacts
// @param   aLinkArray   array of links to contacts to be deleted,
//                        takes ownership
// @param   aCallerStatus       iStatus of caller
// ---------------------------------------------------------------------------
//    
void CPdcContactDeletion::DeleteStoredContactsL( 
        MVPbkContactLinkArray* aLinkArray,
        TRequestStatus& aCallerStatus,
        TBool aDeleteStoredContact )
    {
    LOGTEXT(_L("CPdcContactDeletion::DeleteStoredContactsL"));
    iDeleteStoredContact = aDeleteStoredContact;
    
    // Create the array of links
    delete iDeleteArray;
    iDeleteArray = NULL;    

    iDeleteArray = CVPbkContactLinkArray::NewL();
         
    // Set the index to the number of contacts in the link array.
    MVPbkContactLinkArray* linkArray = aLinkArray;
    
    iIndex = linkArray->Count();  
      
    while(iIndex)
        {
        const MVPbkContactLink& link = linkArray->At( iIndex-1 );
        
        // Create a copy of the link and add it to
        // the delete array
        MVPbkContactLink* copiedLink = link.CloneLC();
        iDeleteArray->AppendL( copiedLink );
        CleanupStack::Pop(); // copiedLin
        iIndex--;
        }       
    
    delete iDeleteOp;
    iDeleteOp = NULL;  
     
    iDeleteOp = iContactManager.DeleteContactsL( *iDeleteArray, *this );

    // Store the caller's request status
    iCallerStatus = &aCallerStatus;
    aCallerStatus = KRequestPending;  
    }

// End of File
