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
* Description:  An array for contact links.
*
*/


// INCLUDES
#include "CVPbkContactLinkArray.h"

#include "MVPbkContactStoreLoader.h"
#include <MVPbkContactLink.h>
#include <MVPbkStreamable.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkError.h>
#include <MVPbkContactLinkPacking.h>
#include <MVPbkContactStoreList.h>

#include <s32mem.h>
#include <badesca.h>
#include <bamdesca.h>

// packaged array static data
namespace KLinkArrayStatic
    {
    /* streaming version 1 doesnt contain any optimizations */
    const TUint8 KVPbkPackagedLinkArrayVersionV1 = 1;
    /* streaming version 2 is optimized for memory usage */
    const TUint8 KVPbkPackagedLinkArrayVersionV2 = 2;    
    }

namespace KLinkArrayExternalizeSizes
    {
    const TInt KVersionNumberSize = 1;
    const TInt KStoreUriCountSize = 2;
    const TInt KContactLinkCountSize = 4;
    const TInt KUriLengthSize = 2;
    const TInt KUriIndexSize = 2;
    const TInt KInternalPackedSize = 4;
    }

namespace
    {
    const TUint8 KUriMarked = 1;
    const TUint8 KUriNotMarked = 0;
    const TInt KUriArrayGranularity = 4;

#ifdef _DEBUG
    enum TPanic
        {
        EPreCond_IsUriMarked,
        EPreCond_MarkUri,
        EPreCond_RemoveUnmarkedUris,
        EPreCond_AppendStoreL
        };
        
    void Panic( TPanic aPanic )
        {
        _LIT(KPanicCat, "CVPbkContactLinkArray");
        User::Panic( KPanicCat, aPanic );
        }
#endif // _DEBUG
    }

/**
 * A helper class for sharing contact store URI descriptors.
 */
NONSHARABLE_CLASS(CVPbkContactLinkArray::CStoreUris)
        :   public CBase
    {
    public:
        static CStoreUris* NewL();
        ~CStoreUris();
        
        /**
         * Appends the URI into array if if not appended earlier
         */
        void AppendIfNotFoundL( const MVPbkContactStore& aStore );
        /**
         * Appends the URI from the link if not appended earlier
         */
        void AppendIfNotFoundL( const MVPbkContactLink& aContactLink );
        /**
         * Appends empty URI to array
         */
        void AppendEmptyL();
        /**
         * @return the number of URIs
         */
        inline TInt Count() const;
        /**
         * @return a URI in the given Index
         */
        inline TPtrC At( TInt aIndex ) const;
        /**
         * @return the index of the store URI
         */
        inline TInt Find( const TDesC& aUri ) const;
        /**
         * @return the index of the URI or KErrNotFound
         */
        inline TInt Find( const MVPbkContactStore& aStore );
        /**
         * Destroys all URIs
         */
        void ResetAndDestroy();
        /**
         * @return ETrue if the URI is marked
         * @param aUri the URI to be checked
         */
        inline TBool IsUriMarked( const MVPbkContactStore& aStore );
        /**
         * Marks the URI
         * @param aUri the URI to be marked
         */
        void MarkUri( const MVPbkContactStore& aStore );
        /**
         * Unmarks all
         */
        void UnmarkAllUris();
        /**
         * Removes URIs that have no mark
         */
        void RemoveUnmarkedUris();
        
    private:
        CStoreUris();
        void ConstructL();
        void AppendStoreL( const MVPbkContactStore* aStore );
        
    private:
        /// Ref: An array for stores that are used to retrieve URIs
        RPointerArray<MVPbkContactStore> iStores;
        /// Own: an array of marks. This must be in balance with iStores
        RArray<TUint8> iMarkedUris;
    };

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::CStoreUris
// ---------------------------------------------------------------------------
//
inline CVPbkContactLinkArray::CStoreUris::CStoreUris()
        :   iStores( KUriArrayGranularity ), 
            iMarkedUris( KUriArrayGranularity )
    {    
    }
    
// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::NewL
// ---------------------------------------------------------------------------
//
CVPbkContactLinkArray::CStoreUris* CVPbkContactLinkArray::CStoreUris::NewL()
    {
    CStoreUris* self = new( ELeave ) CStoreUris;
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::~CStoreUris
// ---------------------------------------------------------------------------
//
CVPbkContactLinkArray::CStoreUris::~CStoreUris()
    {
    iMarkedUris.Close();
    iStores.Close();
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::AppendIfNotFoundL
// ---------------------------------------------------------------------------
//    
void CVPbkContactLinkArray::CStoreUris::AppendIfNotFoundL( 
        const MVPbkContactStore& aStore )
    {        
    if ( iStores.Find( &aStore ) == KErrNotFound )
        {
        AppendStoreL( &aStore );
        }
    }
    
// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::AppendIfNotFoundL
// ---------------------------------------------------------------------------
//    
void CVPbkContactLinkArray::CStoreUris::AppendIfNotFoundL( 
        const MVPbkContactLink& aContactLink )
    {
    AppendIfNotFoundL( aContactLink.ContactStore() );
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::AppendEmptyL
// ---------------------------------------------------------------------------
//    
void CVPbkContactLinkArray::CStoreUris::AppendEmptyL()
    {
    AppendStoreL( NULL );
    }
    
// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::Count
// ---------------------------------------------------------------------------
//    
inline TInt CVPbkContactLinkArray::CStoreUris::Count() const
    {
    return iStores.Count();
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::At
// ---------------------------------------------------------------------------
//    
inline TPtrC CVPbkContactLinkArray::CStoreUris::At( TInt aIndex ) const
    {
    const MVPbkContactStore* store = iStores[aIndex];
    if ( store )
        {
        return store->StoreProperties().Uri().UriDes();
        }
    else
        {
        return TPtrC( KNullDesC );
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::FindL
// ---------------------------------------------------------------------------
//        
TInt CVPbkContactLinkArray::CStoreUris::Find( const TDesC& aUri ) const
    {
    const TInt count = iStores.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iStores[i] && iStores[i]->StoreProperties().Uri().Compare( aUri, 
             TVPbkContactStoreUriPtr::EContactStoreUriAllComponents ) == 0 )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::Find
// ---------------------------------------------------------------------------
//    
TInt CVPbkContactLinkArray::CStoreUris::Find( 
        const MVPbkContactStore& aStore )
    {
    return iStores.Find( &aStore );
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::ResetAndDestroy
// ---------------------------------------------------------------------------
//    
void CVPbkContactLinkArray::CStoreUris::ResetAndDestroy()
    {
    iStores.Reset();
    iMarkedUris.Reset();
    }
    
// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::IsUriMarked
// ---------------------------------------------------------------------------
//    
TBool CVPbkContactLinkArray::CStoreUris::IsUriMarked( 
        const MVPbkContactStore& aStore )
    {
    __ASSERT_DEBUG( iMarkedUris.Count() == iStores.Count(),
        Panic( EPreCond_IsUriMarked ) );
    
    TInt pos = iStores.Find( &aStore );
    if ( pos != KErrNotFound &&
         iMarkedUris[pos] == KUriMarked )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::MarkUri
// ---------------------------------------------------------------------------
//    
void CVPbkContactLinkArray::CStoreUris::MarkUri( 
        const MVPbkContactStore& aStore )
    {
    __ASSERT_DEBUG( iMarkedUris.Count() == iStores.Count(),
        Panic( EPreCond_MarkUri ) );
    
    TInt pos = iStores.Find( &aStore );
    if ( pos != KErrNotFound )
        {
        iMarkedUris[pos] = KUriMarked;
        }
    }
    
// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::UnmarkAllUris
// ---------------------------------------------------------------------------
//    
void CVPbkContactLinkArray::CStoreUris::UnmarkAllUris()
    {
    const TInt count = iMarkedUris.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iMarkedUris[i] = KUriNotMarked;
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::RemoveUnmarkedUris
// ---------------------------------------------------------------------------
//    
void CVPbkContactLinkArray::CStoreUris::RemoveUnmarkedUris()
    {
    __ASSERT_DEBUG( iMarkedUris.Count() == iStores.Count(),
        Panic( EPreCond_RemoveUnmarkedUris ) );
        
    const TInt count = iMarkedUris.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        if ( iMarkedUris[i] == KUriNotMarked )
            {
            iMarkedUris.Remove( i );
            iStores.Remove( i );
            }
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CStoreUris::AppendStoreL
// ---------------------------------------------------------------------------
//    
void CVPbkContactLinkArray::CStoreUris::AppendStoreL( 
        const MVPbkContactStore* aStore )
    {
    __ASSERT_DEBUG( iMarkedUris.Count() == iStores.Count(),
        Panic( EPreCond_AppendStoreL ) );
    User::LeaveIfError( iMarkedUris.Append( KUriNotMarked ) );
    TRAPD( res, iStores.AppendL( aStore ) );
    if ( res != KErrNone )
        {
        // If second AppendL leaves then return to the original state
        // by deleting the item appended in first AppendL
        iMarkedUris.Remove( iMarkedUris.Count() - 1 );
        User::Leave( res );
        }
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::CVPbkContactLinkArray
// ---------------------------------------------------------------------------
//
inline CVPbkContactLinkArray::CVPbkContactLinkArray()
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::ConstructL
// ---------------------------------------------------------------------------
//
inline void CVPbkContactLinkArray::ConstructL()
    {
    iStoreUris = CStoreUris::NewL();
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::ConstructL
// ---------------------------------------------------------------------------
//
inline void CVPbkContactLinkArray::ConstructL(
        const TDesC8& aPackedLinks, 
        const MVPbkContactStoreList& aStoreList)
    {
    iStoreUris = CStoreUris::NewL();
    // unencrypt the packed links descriptor to this array
    RDesReadStream readStream(aPackedLinks);
    readStream.PushL();
    InternalizeFromBufferL(readStream, aStoreList);
    CleanupStack::PopAndDestroy(&readStream);
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::ConstructL
// ---------------------------------------------------------------------------
//
inline void CVPbkContactLinkArray::ConstructL(
        RReadStream& aStream, 
        const MVPbkContactStoreList& aStoreList)
    {
    iStoreUris = CStoreUris::NewL();
    InternalizeFromBufferL( aStream, aStoreList );
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactLinkArray* CVPbkContactLinkArray::NewL()
    {
    CVPbkContactLinkArray* self = CVPbkContactLinkArray::NewLC();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactLinkArray* CVPbkContactLinkArray::NewLC()
    {
    CVPbkContactLinkArray* self = new(ELeave) CVPbkContactLinkArray();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactLinkArray* CVPbkContactLinkArray::NewLC(
        const TDesC8& aPackedLinks, 
        const MVPbkContactStoreList& aStoreList)
    {
    CVPbkContactLinkArray* self = new(ELeave) CVPbkContactLinkArray();
    CleanupStack::PushL(self);
    self->ConstructL(aPackedLinks, aStoreList);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactLinkArray* CVPbkContactLinkArray::NewLC(
        RReadStream& aStream,
        const MVPbkContactStoreList& aStoreList)
    {
    CVPbkContactLinkArray* self = new(ELeave) CVPbkContactLinkArray();
    CleanupStack::PushL(self);
    self->ConstructL(aStream, aStoreList);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::NewLC
// ---------------------------------------------------------------------------
//
CVPbkContactLinkArray* CVPbkContactLinkArray::NewLC(
        RReadStream& aStream,
        MVPbkContactStoreLoader& aLoader )
    {
    CVPbkContactLinkArray* self = new( ELeave ) CVPbkContactLinkArray;
    CleanupStack::PushL( self );
    self->iStoreLoader = &aLoader;
    self->ConstructL( aStream, aLoader.ContactStoreListL() );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::~CVPbkContactLinkArray
// ---------------------------------------------------------------------------
//
CVPbkContactLinkArray::~CVPbkContactLinkArray()
    {
    iLinks.ResetAndDestroy();
    delete iStoreUris;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::Count
// ---------------------------------------------------------------------------
//
TInt CVPbkContactLinkArray::Count() const
    {
    return iLinks.Count();
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::At
// ---------------------------------------------------------------------------
//
const MVPbkContactLink& CVPbkContactLinkArray::At(TInt aIndex) const
    {
    return *iLinks[aIndex];
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::Find
// ---------------------------------------------------------------------------
//
TInt CVPbkContactLinkArray::Find(const MVPbkContactLink& aLink) const
    {
    TInt result = KErrNotFound;
    
    const TInt count = Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (aLink.IsSame(At(i)))
            {
            result = i;
            break;
            }
        }
    return result;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::PackLC
//
// Linkarray formats:
//
// Link array pack format 1:
// stream := VersionNumber ContactLinkCount ContactLink*
// VersionNumber := TUint8
// ContactLinkCount := TUint32
// ContactLink := UriLength UriDescriptor InternalPackedSize PackInternals
// UriLength := TInt16
// UriDescriptor := TChar*
// InternalPackedSize := TUint32
// PackInternals := TChar*
//
// Link array pack format 2:
// stream := VersionNumber StoreUriCount StoreUri* ContactLinkCount ContactLink*
// VersionNumber := TUint8
// StoreUriCount := TUint16
// StoreUri := UriLength UriDescriptor
// UriLength := TUint16
// UriDescriptor := TChar*
// ContactLinkCount := TUint32
// ContactLink := UriIndex InternalPackedSize PackInternals
// UriIndex := TInt16
// InternalPackedSize := TUint32
// PackInternals := TChar*
// ---------------------------------------------------------------------------
HBufC8* CVPbkContactLinkArray::PackLC() const
    {
    const TInt bufferSize = DoCalculatePackedBufferSizeV2();
    HBufC8* packagedLinkArray = HBufC8::NewLC( bufferSize );
    TPtr8 bufPtr( packagedLinkArray->Des() );
    RDesWriteStream writeStream( bufPtr );
    writeStream.PushL();
    DoFillPackedBufferV2L( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    
    return packagedLinkArray;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::Streamable
// ---------------------------------------------------------------------------
//
const MVPbkStreamable* CVPbkContactLinkArray::Streamable() const
    {
    // persistent streaming is not supported
    return NULL;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::AppendL
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactLinkArray::AppendL( MVPbkContactLink* aLink )
    {
    if ( aLink )
        {
        iStoreUris->AppendIfNotFoundL( *aLink );
        iLinks.AppendL( aLink );
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::InsertL
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactLinkArray::InsertL( MVPbkContactLink* aLink, 
        TInt aIndex )
    {
    if ( aLink )
        {
        iStoreUris->AppendIfNotFoundL( *aLink );
        iLinks.InsertL( aLink, aIndex );
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::Remove
// ---------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactLinkArray::Remove( TInt aIndex )
    {
    iLinks.Remove(aIndex);
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::Delete
// ---------------------------------------------------------------------------
//    
EXPORT_C void CVPbkContactLinkArray::Delete( TInt aIndex )
    {
    MVPbkContactLink* link = iLinks[aIndex];
    Remove( aIndex );
    delete link;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::ResetAndDestroy
// ---------------------------------------------------------------------------
//    
EXPORT_C void CVPbkContactLinkArray::ResetAndDestroy()
    {
    iLinks.ResetAndDestroy();
    iStoreUris->ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::Reset
// ---------------------------------------------------------------------------
//        
EXPORT_C void CVPbkContactLinkArray::Reset()
    {
    iLinks.Reset();
    iStoreUris->ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::PackedBufferSize
// ---------------------------------------------------------------------------
//        
EXPORT_C TInt CVPbkContactLinkArray::PackedBufferSize() const
    {
    return DoCalculatePackedBufferSizeV2(); // V2
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::DoCalculatePackedBufferSizeV2
// NOTE: This function also removes URIs that are not refered from links
// ---------------------------------------------------------------------------
//
TInt CVPbkContactLinkArray::DoCalculatePackedBufferSizeV2() const
    {
    // Unmark all URIs
    iStoreUris->UnmarkAllUris();
    
    TInt bufferSize = KLinkArrayExternalizeSizes::KVersionNumberSize +
        KLinkArrayExternalizeSizes::KStoreUriCountSize + 
        KLinkArrayExternalizeSizes::KContactLinkCountSize;
    
    const TInt linkCount = iLinks.Count();
    for (TInt i = 0; i < linkCount; ++i)
        {
        const MVPbkContactLink& link = *iLinks[i];
        const MVPbkContactStore& store = link.ContactStore();
        // Add URI length if not added before
        if ( !iStoreUris->IsUriMarked( store ) )
            {
            bufferSize += KLinkArrayExternalizeSizes::KUriLengthSize;
            bufferSize += store.StoreProperties().Uri().UriDes().Size();
            // Mark URI so that its length is not added again
            iStoreUris->MarkUri( store );
            }
        // Add the link internals size
        bufferSize += KLinkArrayExternalizeSizes::KUriIndexSize;
        bufferSize += KLinkArrayExternalizeSizes::KInternalPackedSize;
        bufferSize += link.Packing().InternalPackedSize();
        }

    // Remove URIs that are not used
    iStoreUris->RemoveUnmarkedUris();
    // Reset the state of iStoreUris marked URIs
    iStoreUris->UnmarkAllUris();
    
    return bufferSize;
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::DoFillPackedBufferV2L
// ---------------------------------------------------------------------------
//
void CVPbkContactLinkArray::DoFillPackedBufferV2L(
        RWriteStream& aWriteStream) const
    {
    // write version number
    aWriteStream.WriteUint8L(
        KLinkArrayStatic::KVPbkPackagedLinkArrayVersionV2);
    // write store URI count
    const TInt uriCount = iStoreUris->Count();
    aWriteStream.WriteUint16L( uriCount );
    // write store URIs
    for ( TInt i = 0; i < uriCount; ++i )
        {
        TPtrC uriPtr( iStoreUris->At( i ) );
        const TInt uriLength = uriPtr.Length();
        aWriteStream.WriteUint16L( uriLength );
        aWriteStream.WriteL( uriPtr, uriLength );
        }
    const TInt linkCount = iLinks.Count();
    // write the contact link count to the stream
    aWriteStream.WriteUint32L( linkCount );
    // write links
    for (TInt i = 0; i < linkCount; ++i)
        {
        const MVPbkContactLink& link = *iLinks[i];
        const TInt uriIndex = iStoreUris->Find( link.ContactStore() );
        // write URI index
        aWriteStream.WriteInt16L( uriIndex );
        // write URI internal size
        aWriteStream.WriteUint32L( link.Packing().InternalPackedSize() );
        // write internals
        link.Packing().PackInternalsL(aWriteStream);
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::InternalizeFromBufferL
// ---------------------------------------------------------------------------
//
void CVPbkContactLinkArray::InternalizeFromBufferL( 
		RReadStream& aReadStream, 
        const MVPbkContactStoreList& aStoreList )
    {
    // check correct version number of link array
    const TInt versionNumber = aReadStream.ReadUint8L();
    if ( versionNumber == KLinkArrayStatic::KVPbkPackagedLinkArrayVersionV1 )
        {
        DoInternalizeFromBufferL( aReadStream, aStoreList );
        }
    else if ( versionNumber == 
              KLinkArrayStatic::KVPbkPackagedLinkArrayVersionV2 )
        {
        DoInternalizeFromBufferV2L( aReadStream, aStoreList );
        }
    else
        {
        // Invalid link array version
        User::Leave( KErrArgument );
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::DoInternalizeFromBufferL
// Version 1
// ---------------------------------------------------------------------------
//
void CVPbkContactLinkArray::DoInternalizeFromBufferL(
        RReadStream& aReadStream, 
        const MVPbkContactStoreList& aStoreList)
    {
    // read contact link count from the stream
    const TInt contactLinkCount = aReadStream.ReadUint32L();

    // read each contact link from the stream
    for (TInt i = 0; i < contactLinkCount; ++i)
        {
        // read the store URI
        const TInt uriLength = aReadStream.ReadUint16L();
        HBufC* uriBuffer = HBufC::NewLC(uriLength);
        TPtr uriPtr = uriBuffer->Des();
        aReadStream.ReadL(uriPtr, uriLength);
        // find the store corresponding to the URI
        MVPbkContactStore* store =
            aStoreList.Find(TVPbkContactStoreUriPtr(uriPtr));
        CleanupStack::PopAndDestroy(uriBuffer);

        const TInt internalPackedSize = aReadStream.ReadUint32L();
        if(store)
            {
            // create link and add it to this array
            MVPbkContactLink* link = 
                store->CreateLinkFromInternalsLC(aReadStream);
            AppendL(link);
            CleanupStack::Pop(); // link
            }
        else
            {
            // skip PackInternals that are unknown
            aReadStream.ReadL(internalPackedSize);
            }
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactLinkArray::DoInternalizeFromBufferV2L
// Version 2
// ---------------------------------------------------------------------------
//
void CVPbkContactLinkArray::DoInternalizeFromBufferV2L(
        RReadStream& aReadStream, 
        const MVPbkContactStoreList& aStoreList)
    {
    // Read store URI count
    TInt uriCount = aReadStream.ReadUint16L();
    // Read URIs
    for ( TInt i = 0; i < uriCount; ++i )
        {
        const TInt uriLength = aReadStream.ReadUint16L();
        HBufC* uriBuffer = HBufC::NewLC(uriLength);
        TPtr uriPtr = uriBuffer->Des();
        aReadStream.ReadL( uriPtr, uriLength );
        MVPbkContactStore* store = aStoreList.Find( uriPtr );
        if ( !store && iStoreLoader )
            {
            // Try to load the store
            iStoreLoader->LoadContactStoreL( uriPtr );
            store = aStoreList.Find( uriPtr );
            }
        
        if ( store )
            {
            iStoreUris->AppendIfNotFoundL( *store );
            }
        else
            {
            // Append empty URI to keep ÜRI indexes valid
            iStoreUris->AppendEmptyL();
            }
        CleanupStack::PopAndDestroy( uriBuffer );
        }
    
    if ( uriCount != iStoreUris->Count() )
        {
        // There must be as many items in the iStoreUris as uriCount
        // because otherwise URI indexes are not valid. We cannot
        // continue reading link data in that case.
        User::Leave( KErrCorrupt );
        }
    
    // read contact link count from the stream
    const TInt contactLinkCount = aReadStream.ReadUint32L();
    // read each contact link from the stream
    for (TInt i = 0; i < contactLinkCount; ++i)
        {
        // read the store URI index
        const TInt uriIndex = aReadStream.ReadInt16L();
        // read the size of the internals
        const TInt internalPackedSize = aReadStream.ReadUint32L();
        if ( uriIndex >= 0 && uriIndex < uriCount )
            {
            const TDesC& uri = iStoreUris->At( uriIndex );
            // find the store corresponding to the URI
            MVPbkContactStore* store = aStoreList.Find( uri );
            if( store )
                {
                // create link and add it to this array
                MVPbkContactLink* link = 
                    store->CreateLinkFromInternalsLC( aReadStream );
                AppendL( link );
                CleanupStack::Pop(); // link
                }
            else
                {
                // skip PackInternals that are unknown
                aReadStream.ReadL( internalPackedSize );
                }
            }
        else
            {
            // skip PackInternals that are unknown
            aReadStream.ReadL( internalPackedSize );
            }
        }
    }
// end of file
