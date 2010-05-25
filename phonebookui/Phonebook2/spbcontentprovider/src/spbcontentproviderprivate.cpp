/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  .
*
*/


#include "spbcontentproviderprivate.h"
#include "spbcontentprovider.h"
#include "spbstatusprovider.h"
#include "spbcontent.h"
#include "spbcontactstorelistener.h"

#include <MVPbkContactStore.h>

// CONSTANTS 
namespace{

    /// Status info cache size
    const TInt KSpbStatusInfoCacheSize = 20;
}

// ----------------------------------------------------------------------------
// CSpbContentProviderPrivate::NewL
// ----------------------------------------------------------------------------
//
CSpbContentProviderPrivate* CSpbContentProviderPrivate::NewL( 
    CVPbkContactManager& aContactManager,
    CPbk2StoreManager& aStoreManager,
    TInt32 aFeatures )
    {
    CSpbContentProviderPrivate* self = new (ELeave) CSpbContentProviderPrivate( 
        aContactManager, aStoreManager, aFeatures );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }            

// ----------------------------------------------------------------------------
// CSpbContentProviderPrivate::CSpbContentProviderPrivate
// ----------------------------------------------------------------------------
//
inline CSpbContentProviderPrivate::CSpbContentProviderPrivate(
        CVPbkContactManager& aContactManager,
        CPbk2StoreManager& aStoreManager,
        TInt32 aFeatures) :
    iContactManager(aContactManager),
    iStoreManager( aStoreManager ),
    iFeatures(aFeatures),
    iContentCache( KSpbStatusInfoCacheSize )
    {
    }            

// ----------------------------------------------------------------------------
// CSpbContentProviderPrivate::ConstructL
// ----------------------------------------------------------------------------
//
inline void CSpbContentProviderPrivate::ConstructL()
    {
    if( IsFeature( CSpbContentProvider::EStatusMessage ) )
        {
        iStatusProvider = CSpbStatusProvider::NewL( iContentCache );
        
        // icon service can only exist with status service
        if( IsFeature( CSpbContentProvider::EServiceIcon ) )
            {
            iIconProvider = CSpbServiceIconProvider::NewL();
            }
        }
    if( IsFeature( CSpbContentProvider::EPhoneNumber ) )
    	{
		// create contact store listener
        iStoreListener = CSpbContactStoreListener::NewL( 
		    iStoreManager, iContentCache );
    	}
    }            

// ----------------------------------------------------------------------------
// CSpbContentProviderPrivate::~CSpbContentProviderPrivate
// ----------------------------------------------------------------------------
//
CSpbContentProviderPrivate::~CSpbContentProviderPrivate()
    {
    delete iIconProvider;
    delete iStatusProvider;
    delete iStoreListener;
    iObservers.Close();
    iContentCache.ResetAndDestroy();
    }            

// ----------------------------------------------------------------------------
// CSpbContentProviderPrivate::ContentByLinkL
// ----------------------------------------------------------------------------
//
CSpbContent* CSpbContentProviderPrivate::ContentByLinkL( 
    const MVPbkContactLink& aLink )
    {
    const TInt count( iContentCache.Count() );
    for( TInt i = 0 ; i < count ; ++i )
        {
        CSpbContent* content = iContentCache[i];
        if( content->Match( aLink ) )
            {
            iContentCache.Remove( i );
            iContentCache.InsertL( content, 0 );
            return content;
            }
        }
    return NULL;
    }
        
// ----------------------------------------------------------------------------
// CSpbContentProviderPrivate::GetContentL
// ----------------------------------------------------------------------------
//
void CSpbContentProviderPrivate::GetContentL(
    MVPbkContactLink& aLink,
    HBufC*& aText,
    TPbk2IconId& aIconId,
    CSpbContentProvider::TSpbContentType& aType )
    {
    aType = CSpbContentProvider::ETypeNone;
    CSpbContent* content = ContentByLinkL(aLink);
    if( !content )
        {
        // new contact -> fetch and cache
        CSpbContent::TParameters parameters;
        parameters.iContactManager = &iContactManager;
        parameters.iFeatures = iFeatures;
        parameters.iObservers = &iObservers;
        parameters.iIconProvider = iIconProvider;
        parameters.iStatusProvider = iStatusProvider;
        
        CSpbContent* content = CSpbContent::NewLC(parameters, aLink);
        iContentCache.InsertL(content, 0);
        CleanupStack::Pop(content);
        
        // limit cache size
        const TInt contentCount = iContentCache.Count();
        if(contentCount > KSpbStatusInfoCacheSize)
            {
            delete iContentCache[contentCount-1];
            iContentCache.Remove(contentCount-1);
            }
        }
    else if( content->IsReady() )
        {
        // cached value ready 
        const TDesC& txt = content->Text();
        if( txt.Length() )
            {
            aText = txt.AllocL();
            aIconId = content->IconId();
            aType = content->Type();
            }
        }
    }

// ----------------------------------------------------------------------------
// CSpbContentProviderPrivate::CleanContentL
// ----------------------------------------------------------------------------
//
void CSpbContentProviderPrivate::CleanContentL( 
       MVPbkContactStore& aStore )
    {
    TInt i=0;
    while( i < iContentCache.Count() )
       {
       CSpbContent* content = iContentCache[i];
       if ( &content->Link().ContactStore() == &aStore )
           {
           iContentCache.Remove( i );
           delete content;
           }
       else
           {
           i++;
           }
        }
    }

// end of file
