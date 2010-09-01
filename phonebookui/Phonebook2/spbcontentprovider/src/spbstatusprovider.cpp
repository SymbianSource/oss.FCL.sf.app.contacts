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
* Description: Status provider
*
*/


#include "spbstatusprovider.h"
#include "spbcontent.h"

#include <presencecachereader2.h>
#include <mpresencebuddyinfo2.h>
#include <presencecachebuddyidcreator.h>

// ---------------------------------------------------------------------------
// CSpbStatusProvider::NewL
// ---------------------------------------------------------------------------
//
CSpbStatusProvider* CSpbStatusProvider::NewL(
    RPointerArray<CSpbContent>& aContentCache)
    {
    CSpbStatusProvider* self = new (ELeave) CSpbStatusProvider(aContentCache);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CSpbStatusProvider::CSpbStatusProvider
// ---------------------------------------------------------------------------
//
inline CSpbStatusProvider::CSpbStatusProvider(
    RPointerArray<CSpbContent>& aContentCache ) :
    iContentCache(aContentCache) 
    {
    }

// ---------------------------------------------------------------------------
// CSpbStatusProvider::ConstructL
// ---------------------------------------------------------------------------
//
inline void CSpbStatusProvider::ConstructL()
    {	
    iPresenceCache = MPresenceCacheReader2::CreateReaderL();
    iPresenceCache->SetObserverForSubscribedNotifications( this );
    iBuddyIdCreator = CPresenceCacheBuddyIdCreator::NewL( );
    }

// ---------------------------------------------------------------------------
// CSpbStatusProvider::~CSpbStatusProvider
// ---------------------------------------------------------------------------
//
CSpbStatusProvider::~CSpbStatusProvider()
    {
    delete iBuddyIdCreator;
    delete iPresenceCache;
    }

// ---------------------------------------------------------------------------
// CSpbStatusProvider::FindContentByBuddyId
// ---------------------------------------------------------------------------
//
CSpbContent* CSpbStatusProvider::FindContentByBuddyId(
    const TDesC& aBuddyId)
    {
    const TInt count(iContentCache.Count());
    for( TInt i = 0 ; i < count ; ++i )
        {
        CSpbContent* content = iContentCache[i];
        if(content->Match(aBuddyId))
            {
            return content;
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSpbStatusProvider::NotifyChangeEventL
// ---------------------------------------------------------------------------
//
void CSpbStatusProvider::NotifyChangeEventL(
    MPresenceBuddyInfo2& aBuddyInfo,
    CSpbContent& aContent)
    {
    // Check status text
    TPtrC statusMessage(aBuddyInfo.StatusMessage());
    // Check service icon
    TPtrC8 brandingID( 
        aBuddyInfo.GetAnyField( NPresenceCacheFieldName::KServiceIconBrand));
    TPtrC8 iconEntry( 
        aBuddyInfo.GetAnyField( NPresenceCacheFieldName::KServiceIconId));
    aContent.StatusDataUpdatedL(statusMessage, brandingID, iconEntry);
    }

// ---------------------------------------------------------------------------
// CSpbStatusProvider::FetchStatusDataL
// ---------------------------------------------------------------------------
//
void CSpbStatusProvider::FetchStatusDataL( CSpbContent& aContent )
    {
    HBufC8* linkbuf = aContent.Link().PackLC();
    HBufC* buddyId = iBuddyIdCreator->CreateBuddyIdLC( *linkbuf );
    aContent.SetBuddyId( buddyId ); // takes ownership
    CleanupStack::Pop( buddyId );
    CleanupStack::PopAndDestroy( linkbuf );
    
    iPresenceCache->SubscribePresenceBuddyChangeL(*buddyId);

    MPresenceBuddyInfo2* buddyInfo = iPresenceCache->PresenceInfoLC(*buddyId);
    if(buddyInfo)
        {
        NotifyChangeEventL(*buddyInfo,aContent);
        CleanupStack::PopAndDestroy(); // buddyInfo
        }
    else
        {
        aContent.StatusDataUpdatedL(KNullDesC, KNullDesC8, KNullDesC8);
        }
    }

// ---------------------------------------------------------------------------
// CSpbStatusProvider::HandlePresenceReadL
// ---------------------------------------------------------------------------
//
void CSpbStatusProvider::HandlePresenceReadL(
    TInt /*aErrorCode*/,
    RPointerArray<MPresenceBuddyInfo2>& /*aPresenceBuddyInfoList*/ )
    {
    // Not used
    }

// ---------------------------------------------------------------------------
// CSpbStatusProvider::HandlePresenceNotificationL
// ---------------------------------------------------------------------------
//
void CSpbStatusProvider::HandlePresenceNotificationL( 
    TInt aErrorCode,
    MPresenceBuddyInfo2* aPresenceBuddyInfo )
    {
    CleanupDeletePushL( aPresenceBuddyInfo );
    
    if( KErrNone == aErrorCode && aPresenceBuddyInfo )
        {
        CSpbContent* content = 
            FindContentByBuddyId( aPresenceBuddyInfo->BuddyId() );
        if( content )
            {
            NotifyChangeEventL( *aPresenceBuddyInfo,*content );
            }
        }
    
    CleanupStack::PopAndDestroy(); // aPresenceBuddyInfo
    }


// end of file
