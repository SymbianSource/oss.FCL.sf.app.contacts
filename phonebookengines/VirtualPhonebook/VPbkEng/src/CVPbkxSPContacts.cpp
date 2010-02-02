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
* Description:  xSP contact fetcher
 *
*/


// INCLUDES
#include <CVPbkxSPContacts.h>
#include "CVPbkxSPContactsImpl.h"

EXPORT_C CVPbkxSPContacts* CVPbkxSPContacts::NewL(
        CVPbkContactManager& aContactManager )
    {
    CVPbkxSPContacts* self =
    new (ELeave) CVPbkxSPContacts();
    CleanupStack::PushL( self );
    self->iImpl = CVPbkxSPContactsImpl::NewL( aContactManager );
    CleanupStack::Pop( self );
    return self;
    }

EXPORT_C CVPbkxSPContacts::~CVPbkxSPContacts()
    {
    delete iImpl;
    }

EXPORT_C MVPbkContactOperationBase* CVPbkxSPContacts::GetxSPContactLinksL(
        const MVPbkStoreContact& aContact,
        MVPbkOperationResultObserver<MVPbkContactLinkArray*>& aResultObserver,
        MVPbkOperationErrorObserver& aErrorObserver )
    {    
    return iImpl->GetxSPContactLinksL( aContact, aResultObserver, aErrorObserver );
    }

CVPbkxSPContacts::CVPbkxSPContacts()
    {
    }

// end of file
