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
* Description:  Contact store Uri
*
*/


// INCLUDES
#include <CVPbkContactStoreUri.h>
#include <TVPbkContactStoreUriPtr.h>


// CVPbkContactStoreUri implementation
inline CVPbkContactStoreUri::CVPbkContactStoreUri()
    {
    }

inline void CVPbkContactStoreUri::ConstructL(
        const TDesC& aStoreUri)
    {
    iUriBuffer = aStoreUri.AllocL();
    }

EXPORT_C CVPbkContactStoreUri* CVPbkContactStoreUri::NewL(
        const TVPbkContactStoreUriPtr& aStoreUri)
    {
    CVPbkContactStoreUri* self = new(ELeave) CVPbkContactStoreUri;
    CleanupStack::PushL(self);
    self->ConstructL(aStoreUri.UriDes());
    CleanupStack::Pop();
    return self;
    }

CVPbkContactStoreUri::~CVPbkContactStoreUri()
    {
    delete iUriBuffer;
    }

EXPORT_C TVPbkContactStoreUriPtr CVPbkContactStoreUri::Uri() const
    {
    return TVPbkContactStoreUriPtr(*iUriBuffer);
    }


// end of file
