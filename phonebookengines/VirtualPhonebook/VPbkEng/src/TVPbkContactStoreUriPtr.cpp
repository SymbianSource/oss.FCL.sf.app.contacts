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
* Description:  A class for using contact store URIs
*
*/


// INCLUDES
#include <TVPbkContactStoreUriPtr.h>
#include <s32strm.h>

namespace
    {
    _LIT(KDomainStoreSeparator, "://");  
    _LIT(KDriveSeparator, ":");  
    }

// TVPbkContactStoreUriPtr implementation
EXPORT_C TVPbkContactStoreUriPtr::TVPbkContactStoreUriPtr(
        const TDesC& aStoreUri)
    {
    iUriBufferPointer.Set(aStoreUri);
    }

EXPORT_C TVPbkContactStoreUriPtr::TVPbkContactStoreUriPtr()
    {
    }

EXPORT_C const TDesC& TVPbkContactStoreUriPtr::UriDes() const
    {
    return iUriBufferPointer;
    }

EXPORT_C TInt TVPbkContactStoreUriPtr::Compare(
        const TVPbkContactStoreUriPtr& aUri,
        TVPbkContactStoreUriComponent aComponent) const
    {
    const TVPbkContactStoreUriPtr uri(iUriBufferPointer);
    return Compare(uri.Component(aComponent), aUri.Component(aComponent));
    }

EXPORT_C TInt TVPbkContactStoreUriPtr::Compare(
        const TDesC& aUriComponent, 
        TVPbkContactStoreUriComponent aComponent) const
    {
    const TVPbkContactStoreUriPtr uri(iUriBufferPointer);
    return Compare(uri.Component(aComponent), aUriComponent);
    }

    TInt TVPbkContactStoreUriPtr::Compare(
        const TDesC& aLhsUri,
        const TDesC& aRhsUri) const
    {
    return aLhsUri.Compare(aRhsUri);
    }

EXPORT_C const TPtrC TVPbkContactStoreUriPtr::Component(
        TVPbkContactStoreUriComponent aComponent) const
    {
    TPtrC component; 
    switch(aComponent)
        {
        case EContactStoreUriAllComponents:
            {
            component.Set(iUriBufferPointer);
            }
            break;
        case EContactStoreUriStoreType:
            {
            const TInt offset = iUriBufferPointer.Find(KDomainStoreSeparator);
            if (offset != KErrNotFound)
                {
                component.Set(iUriBufferPointer.Left(offset));
                }
            }
            break;
        case EContactStoreUriStoreDrive:
            {
            const TPtrC nonDomain = iUriBufferPointer.Right(
                iUriBufferPointer.Length()-(iUriBufferPointer.Find(KDomainStoreSeparator)+
                    KDomainStoreSeparator().Length()));
            const TInt offset = nonDomain.Locate( KDriveSeparator()[0] );
            if (offset == KErrNotFound)
                {
                component.Set(KNullDesC);
                }
            else
                {
                component.Set(nonDomain.Left(offset));
                }
            }
            break;
        case EContactStoreUriStoreLocation:
            {
            TInt offset = iUriBufferPointer.Find(KDomainStoreSeparator);
            if (offset != KErrNotFound)
                {
                offset += KDomainStoreSeparator().Length();
                component.Set(iUriBufferPointer.Mid(offset));
                }
            }
            break;
        }
    return component;
    }

EXPORT_C void TVPbkContactStoreUriPtr::Set(const TVPbkContactStoreUriPtr& aUri)
    {
    iUriBufferPointer.Set(aUri.UriDes());
    }

EXPORT_C TInt TVPbkContactStoreUriPtr::Length() const
    {
    return iUriBufferPointer.Length();
    }

// --------------------------------------------------------------------------
// TVPbkContactStoreUriPtr::ExternalizeL
// TVPbkContactStoreUriPtr pack:
//
// URILength = TInt16
// URIDescriptor= char*
// --------------------------------------------------------------------------
//
EXPORT_C void TVPbkContactStoreUriPtr::ExternalizeL(RWriteStream& aStream) const
    {
    aStream.WriteInt16L(iUriBufferPointer.Length()); // URI length
    aStream.WriteL(iUriBufferPointer, iUriBufferPointer.Length()); // URI descriptor
    }

EXPORT_C TInt TVPbkContactStoreUriPtr::ExternalizedSize() const
    {
    // Reserve space for length information, 
    return sizeof(TInt16) + iUriBufferPointer.Size();
    }

// End of file
