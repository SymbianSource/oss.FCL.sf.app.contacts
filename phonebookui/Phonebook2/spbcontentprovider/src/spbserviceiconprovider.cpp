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
* Description: Service icon manager
*
*/

#include "spbserviceiconprovider.h"

#include <cbsfactory.h>
#include <mbsaccess.h>
#include <gulicon.h>
#include <TPbk2IconId.h>
#include <AknIconUtils.h>
#include <fbs.h>
#include <Pbk2NamesListExUID.h>

// CONSTS

namespace {

// Application id for branded icons
_LIT8( KApplicationId, "xsp" );

// Staring index for icon id's
const TInt KIconIndexStartValue = 500;

// Used UID for icon id's
const TInt KIconUidValue        = KPbk2NamesListExtPluginUID2;
}

/**
 * Icon information container class
 */
NONSHARABLE_CLASS( CSpbIconItem ) : public CBase, public MSpbServiceIcon
{
public:
    
    /**
     * Constructor
     */
    static CSpbIconItem* NewLC( 
        const TPbk2IconId& aIconId,    
        const TDesC8& aBrandId,
        const TDesC8& aBrandIconId,
        CBSFactory& aBrandFactory );

    /**
     * Destructor
     */
    ~CSpbIconItem();

private:
    
    /**
     * Constructor
     */
    inline CSpbIconItem( 
        const TPbk2IconId& aIconId, 
        CBSFactory& aBrandFactory );
    
    /**
     * Constructor
     */
    inline void ConstructL( 
        const TDesC8& aBrandId,
        const TDesC8& aBrandIconId );

public: // From MPbk2ServiceIcon
    const TPtrC8 BrandId() const { return *iBrandId; }
    const TPtrC8 BrandIconId() const { return *iBrandIconId; }
    const TPbk2IconId& IconId() const { return iIconId; }
    TBool IsValid() const { return iValid; }
    
public:

    /**
     * Precreate icon from branding server. Calling this method makes the next 
     * call to CreateIconLC faster because the icon is already cached.
     */
    void PreCreateIconL();
    
    /**
     * Create branded icon from branding server. Ownership of the returned 
     * icon is transferred to caller.
     * 
     * @return branded icon
     */
    CGulIcon* CreateIconLC();
    
public: // find helpers
    
    /**
     * Helper class to store full branding information
     */
    class TBrandId
        {
        public:
            TBrandId( const TDesC8& aBrand, const TDesC8& aIcon ) :
                iBrandId( aBrand ), iBrandIconId( aIcon ) {}
            const TPtrC8 iBrandId;
            const TPtrC8 iBrandIconId;
        };
    
    /**
     * Comparison method to check if icon item matches with brand id
     */
    static TBool EqualBrand( const TBrandId* aId, const CSpbIconItem& aItem );

    /**
     * Comparison method to check if icon item matches icon id
     */
    static TBool EqualId( const TPbk2IconId* aId, const CSpbIconItem& aItem );
    
private:
    
    /*
     * Returns Access based on language
     */
    MBSAccess* AccessL();
    
private:
    /// Own. Branding id for icon
    HBufC8* iBrandId;
    /// Own. Branding element id for icon
    HBufC8* iBrandIconId;
    /// Own. Bitmap cache for icon
    CFbsBitmap* iBitmap;
    /// Own. Mask cache for icon
    CFbsBitmap* iMask;
    /// Icon ID
    TPbk2IconId iIconId;
    /// Not own. Branding server interface
    CBSFactory& iBrandFactory;
    /// Is icon valid (e.g. no loading errors occured)
    TBool iValid;
};


// ---------------------------------------------------------------------------
// CSpbIconItem::NewLC
// ---------------------------------------------------------------------------
//
CSpbIconItem* CSpbIconItem::NewLC( 
    const TPbk2IconId& aIconId,
    const TDesC8& aBrandId,
    const TDesC8& aBrandIconId,
    CBSFactory& aBrandFactory )
    {
    CSpbIconItem* self = new(ELeave) CSpbIconItem( aIconId, aBrandFactory );
    CleanupStack::PushL( self );
    self->ConstructL( aBrandId, aBrandIconId );
    return self;
    }

// ---------------------------------------------------------------------------
// CSpbIconItem::~CSpbIconItem
// ---------------------------------------------------------------------------
//
CSpbIconItem::~CSpbIconItem()
    {
    delete iBrandId;
    delete iBrandIconId;
    delete iBitmap;
    delete iMask;
    }

// ---------------------------------------------------------------------------
// CSpbIconItem::CSpbIconItem
// ---------------------------------------------------------------------------
//
inline CSpbIconItem::CSpbIconItem( 
    const TPbk2IconId& aIconId, 
    CBSFactory& aBrandFactory ) : 
        iIconId( aIconId ),
        iBrandFactory( aBrandFactory ),
        iValid( ETrue )
    {
    }

// ---------------------------------------------------------------------------
// CSpbIconItem::ConstructL
// ---------------------------------------------------------------------------
//
inline void CSpbIconItem::ConstructL( 
    const TDesC8& aBrandId,
    const TDesC8& aBrandIconId )
    {
    iBrandId = aBrandId.AllocL();
    iBrandIconId = aBrandIconId.AllocL();
    }

// ---------------------------------------------------------------------------
// CSpbIconItem::EqualBrand
// ---------------------------------------------------------------------------
//
TBool CSpbIconItem::EqualBrand( const TBrandId* aId, const CSpbIconItem& aItem )
    {
    if( aId->iBrandId.Compare( *aItem.iBrandId ) || 
        aId->iBrandIconId.Compare( *aItem.iBrandIconId ) )
        {
        // not equal
        return EFalse;
        }
    // equal
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSpbIconItem::EqualId
// ---------------------------------------------------------------------------
//
TBool CSpbIconItem::EqualId( const TPbk2IconId* aId, const CSpbIconItem& aItem )
    {
    return aItem.iIconId == *aId;
    }

// ---------------------------------------------------------------------------
// CSpbIconItem::AccessL
// ---------------------------------------------------------------------------
//
MBSAccess* CSpbIconItem::AccessL()
	{
	MBSAccess* access = NULL;
	TInt err = 0;
	// try with user lang
	TRAP( err, access = iBrandFactory.CreateAccessL( *iBrandId, User::Language() ) );
	if( err )
		{
		// didn't find it, use international english
		access = iBrandFactory.CreateAccessL( *iBrandId, ELangInternationalEnglish );
		}
	return access;
	}


// ---------------------------------------------------------------------------
// CSpbIconItem::PreCreateIconL
// ---------------------------------------------------------------------------
//
void CSpbIconItem::PreCreateIconL()
    {
    // set invalid until loading is successful
    iValid = EFalse;
    
    // access based on language
    MBSAccess* access = AccessL();
    CleanupClosePushL( *access );
    
    // get icon from branding server
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    access->GetBitmapL( *iBrandIconId, bitmap, mask );
    CleanupStack::PopAndDestroy(); // access
    
    // store icon locally for now
    delete iBitmap;
    iBitmap = bitmap;
    delete iMask;
    iMask = mask;
    iValid = ETrue;
    }

// ---------------------------------------------------------------------------
// CSpbIconItem::CreateIconLC
// ---------------------------------------------------------------------------
//
CGulIcon* CSpbIconItem::CreateIconLC()
    {
    if( !iBitmap )
        {
        // we don't have the icon pre-created. create it now. 
        PreCreateIconL();
        }
    CGulIcon* icon = CGulIcon::NewL( iBitmap, iMask );
    iBitmap = NULL; // give out ownership
    iMask = NULL;
    CleanupStack::PushL( icon );
    return icon;
    }

// ---------------------------------------------------------------------------
// CSpbServiceIconProvider::NewL
// ---------------------------------------------------------------------------
//
CSpbServiceIconProvider* CSpbServiceIconProvider::NewL()
    {
    CSpbServiceIconProvider* self = new( ELeave ) CSpbServiceIconProvider;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CSpbServiceIconProvider::~CSpbServiceIconProvider
// ---------------------------------------------------------------------------
//
CSpbServiceIconProvider::~CSpbServiceIconProvider()
    {
    iIconArray.ResetAndDestroy();
    delete iBrandFactory;
    }

// ---------------------------------------------------------------------------
// CSpbServiceIconProvider::CSpbServiceIconProvider
// ---------------------------------------------------------------------------
//
inline CSpbServiceIconProvider::CSpbServiceIconProvider() :
    iNextFreeIconIndex( KIconIndexStartValue )
    {
    }

// ---------------------------------------------------------------------------
// CSpbServiceIconProvider::ConstructL
// ---------------------------------------------------------------------------
//
inline void CSpbServiceIconProvider::ConstructL()
    {
    // no default brand used
    iBrandFactory = CBSFactory::NewL( KNullDesC8, KApplicationId );
    }

// ---------------------------------------------------------------------------
// CSpbServiceIconProvider::GetBrandedIconL
// ---------------------------------------------------------------------------
//
MSpbServiceIcon* CSpbServiceIconProvider::GetBrandedIconL( 
    const TDesC8& aBrandId, const TDesC8& aIconId )
    {   
    CSpbIconItem::TBrandId brand( aBrandId, aIconId );
    TInt index = iIconArray.Find( brand, &CSpbIconItem::EqualBrand );

    if( index < 0 )
        {
        // New icon. Create it
        CSpbIconItem* icon = CSpbIconItem::NewLC( 
            TPbk2IconId( TUid::Uid( KIconUidValue ), iNextFreeIconIndex++ ),
            aBrandId, aIconId, *iBrandFactory );
        
        index = iIconArray.Count();
        iIconArray.AppendL( icon );
        CleanupStack::Pop( icon );

        // attempt pre-creation. if it fails the icon is marked as invalid
        TRAP_IGNORE( icon->PreCreateIconL() );
        }    
    return iIconArray[index];
    }

// ---------------------------------------------------------------------------
// CSpbServiceIconProvider::CreateIconLC
// ---------------------------------------------------------------------------
//
CGulIcon* CSpbServiceIconProvider::CreateIconLC( const TPbk2IconId& aIconId )
    {
    TInt index = iIconArray.Find( aIconId, &CSpbIconItem::EqualId );
    if( index < 0 )
        {
        User::Leave( KErrNotFound );
        }
   
    return iIconArray[index]->CreateIconLC();
    }


// end of file
