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
* Description:
*
*/


#ifndef SPBCONTENTPROVIDERPRIVATE_H_
#define SPBCONTENTPROVIDERPRIVATE_H_

//  INCLUDES
#include <e32base.h>
#include "spbcontentprovider.h"
#include "spbserviceiconprovider.h"

class MSpbContentProviderObserver;
class CSpbServiceIconProvider;
class CSpbStatusProvider;
class CSpbContent;
class CVPbkContactManager;
class CSpbContactStoreListener;
class CPbk2StoreManager;
class MVPbkContactStore;

/**
 * Private implementation of the content provider
 */
NONSHARABLE_CLASS( CSpbContentProviderPrivate ) : public CBase
    {
   
public: // Construction & destruction

    /// Constructor (see CSpbContentProvider for params)
    static CSpbContentProviderPrivate* NewL( 
        CVPbkContactManager& aContactManager,
        CPbk2StoreManager& aStoreManager,        
        TInt32 aFeatures );
            
    /// Destructor
    ~CSpbContentProviderPrivate();

public: // From CSpbContentProvider
    
    /// From CSpbContentProvider
    inline void AddObserverL(
        MSpbContentProviderObserver& aObserver );

    /// From CSpbContentProvider
    inline void RemoveObserver(
        MSpbContentProviderObserver& aObserver );
            
    /// From CSpbContentProvider
    void GetContentL(
        MVPbkContactLink& aLink,
        HBufC*& aText,
        TPbk2IconId& aIconId,
        CSpbContentProvider::TSpbContentType& aType );
            
    /// From CSpbContentProvider
    inline CGulIcon* CreateServiceIconLC( 
        const TPbk2IconId& aIconId );		
    
    /**
     * Clean the content in the cache if it belongs to the specified store
     */
    void CleanContentL( MVPbkContactStore& aStore );
    
private: // constructors
    
    /// Constructor
    inline CSpbContentProviderPrivate(
        CVPbkContactManager& aContactManager,
        CPbk2StoreManager& aStoreManager,        
        TInt32 aFeatures);

    /// Constructor
    inline void ConstructL();
    
private: // new methods
    
    /**
     * Find content for aLink or NULL
     */
    CSpbContent* ContentByLinkL( const MVPbkContactLink& aLink );
    
    /**
     * Check if feature is enabled
     */
    TBool IsFeature( TInt aFeature ) const;
    
private: // data
    
    /// Contact manager
    CVPbkContactManager& iContactManager;

    /// Store manager
    CPbk2StoreManager& iStoreManager;

    /// Required features / content
    const TInt32 iFeatures;
    
    /// Content not own. Observers
    RPointerArray<MSpbContentProviderObserver> iObservers;
    
    /// Content Own. Cached content
    RPointerArray<CSpbContent> iContentCache;
    
    /// Own. Icon provider
    CSpbServiceIconProvider* iIconProvider;
    
    /// Own. Status provider
    CSpbStatusProvider* iStatusProvider;
    
    // Own. contact store listener for detecting contact changes (phonenumber)
    CSpbContactStoreListener* iStoreListener;
    };

//------------------------------------------------------------------------------
// INLINE METHODS
//------------------------------------------------------------------------------

inline void CSpbContentProviderPrivate::AddObserverL(
        MSpbContentProviderObserver& aObserver )
    {
    iObservers.AppendL( &aObserver );
    }

inline void CSpbContentProviderPrivate::RemoveObserver(
        MSpbContentProviderObserver& aObserver )
    {
    const TInt index = iObservers.Find( &aObserver );
    if( index >= 0 )
        {
        iObservers.Remove( index );
        }
    }

inline CGulIcon* CSpbContentProviderPrivate::CreateServiceIconLC( 
        const TPbk2IconId& aIconId )
    {
    if( iIconProvider )
        {
        return iIconProvider->CreateIconLC( aIconId );
        }
    CGulIcon* icon = NULL;
    CleanupStack::PushL( icon );
    return icon;
    }            

inline TBool CSpbContentProviderPrivate::IsFeature( TInt aFeature ) const
    {
    return iFeatures & aFeature;
    }


#endif /*SPBCONTENTPROVIDERPRIVATE_H_*/
