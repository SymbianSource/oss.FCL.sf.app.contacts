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
* Description:  Top Contact management
*
*/


// INCLUDES
#include <CVPbkTopContactManager.h>
#include "CVPbkTopContactManagerImpl.h"

EXPORT_C CVPbkTopContactManager* CVPbkTopContactManager::NewL(
    CVPbkContactManager& aContactManager )
    {
    CVPbkTopContactManager* self =
        new (ELeave) CVPbkTopContactManager();
    CleanupStack::PushL( self );
    self->iImpl = CVPbkTopContactManagerImpl::NewL( aContactManager ); 
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CVPbkTopContactManager* CVPbkTopContactManager::NewL()
	{
    CVPbkTopContactManager* self =
        new (ELeave) CVPbkTopContactManager();
	CleanupStack::PushL( self );
    self->iImpl = CVPbkTopContactManagerImpl::NewL(); 
	CleanupStack::Pop( self );
	return self;
	}

EXPORT_C CVPbkTopContactManager::~CVPbkTopContactManager()
    {
    delete iImpl;
    }

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::GetTopContactsViewL(
        MVPbkOperationResultObserver<MVPbkContactViewBase*>& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return iImpl->GetTopContactsViewL( aObserver, aErrorObserver );
    }    

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::GetNonTopContactsViewL(
        MVPbkOperationResultObserver<MVPbkContactViewBase*>& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return iImpl->GetNonTopContactsViewL( aObserver, aErrorObserver );
    }   

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::GetTopContactLinksL(
        MVPbkOperationResultObserver<MVPbkContactLinkArray*>&  aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return iImpl->GetTopContactLinksL( aObserver, aErrorObserver );
    }

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::GetNonTopContactLinksL(
        MVPbkOperationResultObserver<MVPbkContactLinkArray*>&  aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return iImpl->GetNonTopContactLinksL( aObserver, aErrorObserver );
    }

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::AddToTopL(
	const MVPbkContactLink& aContactLink,
	MVPbkOperationObserver& aObserver,
	MVPbkOperationErrorObserver& aErrorObserver )
    {
    return iImpl->AddToTopL( aContactLink, aObserver, aErrorObserver );
    }

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::AddToTopL(
	const MVPbkContactLinkArray& aContactLinks,
	MVPbkOperationObserver& aObserver,
	MVPbkOperationErrorObserver& aErrorObserver )
    {
    return iImpl->AddToTopL( aContactLinks, aObserver, aErrorObserver );
    }

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::AddToTopL(
        const TDesC8& aPackedLinks,
        MVPbkOperationObserver& aObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {
    return iImpl->AddToTopL( aPackedLinks, aObserver, aErrorObserver );
    }

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::RemoveFromTopL(
	const MVPbkContactLink& aContactLink,
	MVPbkOperationObserver& aObserver,
	MVPbkOperationErrorObserver& aErrorObserver )
    {
    return iImpl->RemoveFromTopL( aContactLink, aObserver, aErrorObserver );
    }

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::RemoveFromTopL(
	const MVPbkContactLinkArray& aContactLinkArray,
	MVPbkOperationObserver& aObserver,
	MVPbkOperationErrorObserver& aErrorObserver )
    {
    return iImpl->RemoveFromTopL( aContactLinkArray, aObserver, aErrorObserver );
    }

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::RemoveFromTopL(
    const TDesC8& aPackedLinks,
    MVPbkOperationObserver& aObserver,
    MVPbkOperationErrorObserver& aErrorObserver )
    {
    return iImpl->RemoveFromTopL( aPackedLinks, aObserver, aErrorObserver );
    }

EXPORT_C MVPbkContactOperationBase* CVPbkTopContactManager::SetTopOrderL(
	const MVPbkContactLinkArray& aContactLinks,
	MVPbkOperationObserver& aObserver,
	MVPbkOperationErrorObserver& aErrorObserver )
	{
    return iImpl->SetTopOrderL( aContactLinks, aObserver, aErrorObserver );
	}

EXPORT_C TBool CVPbkTopContactManager::IsTopContact(
        const MVPbkBaseContact& aContact )
    {
    return CVPbkTopContactManagerImpl::IsTopContact( aContact );
    }

CVPbkTopContactManager::CVPbkTopContactManager()
    {
    }

// end of file
