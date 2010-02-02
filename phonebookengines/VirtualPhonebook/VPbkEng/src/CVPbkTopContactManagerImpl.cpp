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
* Description:  Top Contact management implementation
*
*/


// INCLUDES
#include "CVPbkTopContactManagerImpl.h"

#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>

#include "CVPbkTopContactOperation.h"


CVPbkTopContactManagerImpl* CVPbkTopContactManagerImpl::NewL(
    CVPbkContactManager& aContactManager )
    {
    CVPbkTopContactManagerImpl* self =
        new (ELeave) CVPbkTopContactManagerImpl();
    CleanupStack::PushL( self );
    self->iContactManager = &aContactManager;
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CVPbkTopContactManagerImpl* CVPbkTopContactManagerImpl::NewL()
	{
    CVPbkTopContactManagerImpl* self =
        new (ELeave) CVPbkTopContactManagerImpl();
	CleanupStack::PushL( self );
    self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

void CVPbkTopContactManagerImpl::ConstructL()
    {
    if ( !iContactManager )
        {
        // If a contact manager was not given, we need to create one.
        CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();
        iOurContactManager = CVPbkContactManager::NewL(*uriArray);
        CleanupStack::PopAndDestroy( uriArray );
        iContactManager = iOurContactManager;
        }
    }

CVPbkTopContactManagerImpl::CVPbkTopContactManagerImpl()
    {
    }

CVPbkTopContactManagerImpl::~CVPbkTopContactManagerImpl()
    {
    delete iOurContactManager;
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::GetTopContactsViewL(
        MVPbkOperationResultObserver<MVPbkContactViewBase*>& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return CVPbkTopContactOperation::NewGetViewOperationL(
            *iContactManager,
            aObserver, 
            aErrorObserver,
            CVPbkTopContactOperation::EGetTopContacts );
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::GetNonTopContactsViewL(
        MVPbkOperationResultObserver<MVPbkContactViewBase*>& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return CVPbkTopContactOperation::NewGetViewOperationL(
            *iContactManager,
            aObserver, 
            aErrorObserver,
            CVPbkTopContactOperation::EGetNonTopContacts );
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::GetTopContactLinksL(
        MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return CVPbkTopContactOperation::NewGetViewLinksOperationL(
            *iContactManager,
            aObserver, 
            aErrorObserver,
            CVPbkTopContactOperation::EGetTopContactLinks );
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::GetNonTopContactLinksL(
        MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return CVPbkTopContactOperation::NewGetViewLinksOperationL(
            *iContactManager,
            aObserver, 
            aErrorObserver,
            CVPbkTopContactOperation::EGetNonTopContactLinks );
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::AddToTopL(
        const MVPbkContactLink& aContactLink,
        MVPbkOperationObserver& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC();
    MVPbkContactLink* link = aContactLink.CloneLC();
    links->AppendL( link );
    CleanupStack::Pop(); // link
    MVPbkContactOperationBase* op = AddToTopL(
            *links, 
            aObserver,
            aErrorObserver );
    CleanupStack::PopAndDestroy( links );
    return op;
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::AddToTopL(
        const MVPbkContactLinkArray& aContactLinks,
        MVPbkOperationObserver& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return CVPbkTopContactOperation::NewTopOperationL(
            *iContactManager,
            aContactLinks,
            aObserver,
            aErrorObserver,
            CVPbkTopContactOperation::EAddToTop );
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::AddToTopL(
        const TDesC8& aPackedLinks,
        MVPbkOperationObserver& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    MVPbkContactLinkArray* links = iContactManager->CreateLinksLC(
            aPackedLinks );
    MVPbkContactOperationBase* op = AddToTopL(
            *links,
            aObserver,
            aErrorObserver );
    CleanupStack::PopAndDestroy(); //links
    return op;
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::RemoveFromTopL(
        const MVPbkContactLink& aContactLink,
        MVPbkOperationObserver& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC();
    MVPbkContactLink* link = aContactLink.CloneLC();
    links->AppendL( link );
    CleanupStack::Pop(); // link
    MVPbkContactOperationBase* op = RemoveFromTopL(
            *links, 
            aObserver, 
            aErrorObserver );
    CleanupStack::PopAndDestroy( links );
    return op;
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::RemoveFromTopL(
        const MVPbkContactLinkArray& aContactLinks,
        MVPbkOperationObserver& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return CVPbkTopContactOperation::NewTopOperationL(
            *iContactManager,
            aContactLinks,
            aObserver,
            aErrorObserver,
            CVPbkTopContactOperation::ERemoveFromTop );
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::RemoveFromTopL(
        const TDesC8& aPackedLinks,
        MVPbkOperationObserver& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    MVPbkContactLinkArray* links = iContactManager->CreateLinksLC(
            aPackedLinks );
    MVPbkContactOperationBase* op = RemoveFromTopL(
            *links, 
            aObserver, 
            aErrorObserver );
    CleanupStack::PopAndDestroy(); //links
    return op;
    }

MVPbkContactOperationBase* CVPbkTopContactManagerImpl::SetTopOrderL(
        const MVPbkContactLinkArray& aContactLinks,
        MVPbkOperationObserver& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return CVPbkTopContactOperation::NewTopOperationL(
            *iContactManager,
            aContactLinks,
            aObserver,
            aErrorObserver,
            CVPbkTopContactOperation::EReorderTop );
    }

TBool CVPbkTopContactManagerImpl::IsTopContact(
        const MVPbkBaseContact& aContact )
    {
    return CVPbkTopContactOperation::TopOrder( aContact ) != KErrNotFound;
    }

// end of file
