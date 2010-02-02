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
* Description:  Array of Contact store Uri
*
*/


#include "CVPbkContactStoreUriArray.h"
#include "CVPbkContactStoreUri.h"
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkError.h>


#include <s32mem.h>

namespace KUriArrayStatic 
    {
    const TInt KVPbkPackagedUriArrayVersion = 1;
    }
    
namespace KUriArrayExternalizeSizes
    {
    const TInt KVersionNumberSize = 1;
    const TInt KUriCountSize = 4;
    const TInt KUriLengthSize = 2;
    }

CVPbkContactStoreUriArray::CVPbkContactStoreUriArray()
    {
    }
    
inline void CVPbkContactStoreUriArray::ConstructL()
    {
    }

EXPORT_C CVPbkContactStoreUriArray* CVPbkContactStoreUriArray::NewLC()
    {
    CVPbkContactStoreUriArray* self = new(ELeave) CVPbkContactStoreUriArray;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
EXPORT_C CVPbkContactStoreUriArray* CVPbkContactStoreUriArray::NewL()
    {
    CVPbkContactStoreUriArray* self = CVPbkContactStoreUriArray::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

inline void CVPbkContactStoreUriArray::ConstructL(const TDesC8& aPackedUris)
    {
    // unencrypt the packed links descriptor to this array
    RDesReadStream readStream(aPackedUris);
    readStream.PushL();
    
    const TInt versionNumber = readStream.ReadUint8L();
    __ASSERT_ALWAYS(versionNumber == KUriArrayStatic::KVPbkPackagedUriArrayVersion, 
            VPbkError::Panic(VPbkError::EInvalidTypeParameter));
    
    // read uri count from the stream
    TInt uriCount = readStream.ReadUint32L();
    while (uriCount--)
        {
        const TInt uriLength = readStream.ReadUint16L();
        HBufC* uriBuffer = HBufC::NewLC(uriLength);
        TPtr uriPtr = uriBuffer->Des();
        readStream.ReadL(uriPtr, uriLength);
        
        AppendL(TVPbkContactStoreUriPtr(uriPtr));
        
        CleanupStack::PopAndDestroy(uriBuffer);
        }
    
    CleanupStack::PopAndDestroy(&readStream);
    }

EXPORT_C CVPbkContactStoreUriArray* CVPbkContactStoreUriArray::NewLC(const TDesC8& aPackedUris)
    {
    CVPbkContactStoreUriArray* self = CVPbkContactStoreUriArray::NewLC();
    self->ConstructL(aPackedUris);
    return self;
    }
    
CVPbkContactStoreUriArray::~CVPbkContactStoreUriArray()
    {
    iUris.ResetAndDestroy();
    }
    
EXPORT_C void CVPbkContactStoreUriArray::AppendL(const TVPbkContactStoreUriPtr& aStoreUri)
    {
    CVPbkContactStoreUri* uri = CVPbkContactStoreUri::NewL(aStoreUri);
    CleanupStack::PushL(uri);
    iUris.AppendL(uri);
    CleanupStack::Pop(uri);
    }
    
EXPORT_C void CVPbkContactStoreUriArray::AppendAllL(
        const CVPbkContactStoreUriArray& aUriArray)
    {
    const TInt count = aUriArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        AppendL(aUriArray[i]);
        }
    }
    
EXPORT_C void CVPbkContactStoreUriArray::Remove(const TVPbkContactStoreUriPtr& aStoreUri)
    {
    const TInt count = Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iUris[i]->Uri().Compare(aStoreUri, 
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
            {
            CVPbkContactStoreUri* uri = iUris[i];
            iUris.Remove(i);
            delete uri;
            break;
            }
        }
    }
    
EXPORT_C TInt CVPbkContactStoreUriArray::Count() const
    {
    return iUris.Count();
    }

EXPORT_C TVPbkContactStoreUriPtr CVPbkContactStoreUriArray::operator[](TInt aIndex) const
    {
    return iUris[aIndex]->Uri();
    }

EXPORT_C HBufC8* CVPbkContactStoreUriArray::PackLC() const
    {
    HBufC8* packed = HBufC8::NewLC(CalculateExternalizedSize());
    TPtr8 ptr = packed->Des();

    RDesWriteStream writeStream(ptr);
    writeStream.PushL();
    
    // Write version number
    writeStream.WriteUint8L(KUriArrayStatic::KVPbkPackagedUriArrayVersion);
    // URI count
    const TInt count = iUris.Count();
    writeStream.WriteUint32L(count);

    for (TInt i = 0; i < count; ++i)
        {
        const TDesC& uri = iUris[i]->Uri().UriDes();
        writeStream.WriteUint16L(uri.Length());
        writeStream.WriteL(uri, uri.Length());
        }
    
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(&writeStream);
    
    return packed;
    }
    
EXPORT_C TBool CVPbkContactStoreUriArray::IsIncluded(
        const TVPbkContactStoreUriPtr& aStoreUri) const
    {
    TBool result = EFalse;
    
    const TInt count = iUris.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iUris[i]->Uri().Compare(aStoreUri, 
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
            {
            result = ETrue;
            break;
            }
        }
        
    return result;
    }
    
TInt CVPbkContactStoreUriArray::CalculateExternalizedSize() const
    {
    TInt result = KUriArrayExternalizeSizes::KVersionNumberSize + 
        KUriArrayExternalizeSizes::KUriCountSize;
        
    const TInt count = iUris.Count();
    for (TInt i = 0; i < count; ++i)
        {
        result += KUriArrayExternalizeSizes::KUriLengthSize;
        result += iUris[i]->Uri().UriDes().Size();
        }
    return result;
    }
    
// End of File
