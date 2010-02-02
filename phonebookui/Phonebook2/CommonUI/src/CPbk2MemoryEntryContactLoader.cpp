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
* Description:  Memory entry contact loader.
*
*/


// INCLUDE FILES
#include <CPbk2MemoryEntryContactLoader.h>

// Phonebook 2
#include <MPbk2MemoryEntryView.h>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactStore.h>

// System includes
#include <coemain.h>

// --------------------------------------------------------------------------
// CPbk2MemoryEntryContactLoader::CPbk2MemoryEntryContactLoader
// --------------------------------------------------------------------------
//
CPbk2MemoryEntryContactLoader::CPbk2MemoryEntryContactLoader
        ( MPbk2MemoryEntryView& aMemoryEntryView ) :
            iMemoryEntryView( aMemoryEntryView )
    {
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryContactLoader::~CPbk2MemoryEntryContactLoader
// --------------------------------------------------------------------------
//
CPbk2MemoryEntryContactLoader::~CPbk2MemoryEntryContactLoader()
    {
    delete iRetrieveOperation;
    delete iContactLink;

    if ( iContactStore )
        {
        iContactStore->Close( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryContactLoader::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2MemoryEntryContactLoader* CPbk2MemoryEntryContactLoader::NewL
        ( MPbk2MemoryEntryView& aMemoryEntryView )
    {
    CPbk2MemoryEntryContactLoader* self =
        new( ELeave ) CPbk2MemoryEntryContactLoader( aMemoryEntryView );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryContactLoader::ChangeContactL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2MemoryEntryContactLoader::ChangeContactL
        (  const MVPbkContactLink& aContactLink )
    {
    MVPbkContactLink* temp = aContactLink.CloneLC();
    iMemoryEntryView.PrepareForContactChangeL();
    CleanupStack::Pop(); // temp
    delete iContactLink;
    iContactLink = temp;        

    if ( !iContactStore )
        {
        iContactStore = &iContactLink->ContactStore();
        iContactStore->OpenL( *this );
        }
    else if ( &iContactLink->ContactStore() != iContactStore )
        {
        iContactStore->Close( *this );
        iContactStore = &iContactLink->ContactStore();
        iContactStore->OpenL( *this );
        }
    else
        {
        RetrieveContactL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryContactLoader::ContactLinkLC
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactLink*
        CPbk2MemoryEntryContactLoader::ContactLinkLC() const
    {
    MVPbkContactLink* link = NULL;
    const MVPbkStoreContact* contact = iMemoryEntryView.Contact();
    if (contact)
        {
        link = contact->CreateLinkLC();
        }
    else
        {
        // LC function semantics
        CleanupDeletePushL(link);
        }

    return link;
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryContactLoader::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryContactLoader::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& aOperation,
          MVPbkStoreContact* aContact )
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // The aContact ownership is taken care of in ContactChangedL
        TRAPD( err, iMemoryEntryView.ContactChangedL( aContact ) );
        // Do not anything, if KErrNone or KErrDied        
        if (err != KErrNone && err != KErrDied )
            {
            CCoeEnv::Static()->HandleError( err );
            iMemoryEntryView.ContactChangeFailed();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryContactLoader::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryContactLoader::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& aOperation, TInt aError )
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // Display standard error message
        CCoeEnv::Static()->HandleError(aError);

        iMemoryEntryView.ContactChangeFailed();
        }
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryContactLoader::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryContactLoader::StoreReady
        ( MVPbkContactStore& /*aContactStore*/ )
    {
    TRAPD( err, RetrieveContactL() );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );

        iMemoryEntryView.ContactChangeFailed();
        }
    }

// --------------------------------------------------------------------------
// CPbk2NavigationBase::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryContactLoader::StoreUnavailable
        ( MVPbkContactStore& /*aContactStore*/, TInt aReason )
    {
    //special fix for BT SAP case. When BT SAP is activate, this will show "Feature Not Supported", which is not
    //wanted.
    if( aReason != KErrNotSupported )
		{
	    CCoeEnv::Static()->HandleError( aReason );
		}

    iMemoryEntryView.ContactChangeFailed();
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryContactLoader::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryContactLoader::HandleStoreEventL
        ( MVPbkContactStore& /*aContactStore*/,
          TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2MemoryEntryContactLoader::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2MemoryEntryContactLoader::RetrieveContactL()
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    // Retrieve the actual store contact from the given link
    iRetrieveOperation = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ContactManager().RetrieveContactL( *iContactLink, *this );
    }

// End of File
