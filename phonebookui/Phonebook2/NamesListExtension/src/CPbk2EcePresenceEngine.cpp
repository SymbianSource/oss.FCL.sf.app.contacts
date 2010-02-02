/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CPbk2EcePresenceEngine.h"

#include <CPbk2IconArray.h>
#include <TPbk2IconId.h>
#include <Pbk2IconId.hrh>
#include <Pbk2UID.h>
#include <MPbk2ContactUiControlUpdate.h>
#include <MVPbkViewContact.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactManager.h>

//ECE
#include "CPbk2UIExtensionInformation.h"
#include <CPbk2UIExtensionPlugin.h>
#include <Pbk2UIExtension.hrh>
#include <TPbk2IconId.h>
#include "Pbk2NamesListExUID.h"
#include <gulicon.h>
#include <fbs.h>

#include <contactpresencefactory.h>
#include <mcontactpresence.h>
/// Unnamed namespace for local definitions
namespace {

enum TNlxPanicCodes
    {
    ENlxPanicEmptyPackage,
    ENlxPanicIconNull,
    ENlxPanicNoIconInfo,
    ENlxPanicNoIcons,
    ENlxPanicNoLinks,
    ENlxPanicStrayIcon,
    ENlxPanicStrayIconInfo
    };

_LIT(KPanicText, "CPbk2EcePresenceEngine");

void Panic(TNlxPanicCodes aReason)
    {
    _LIT( KPanicText, "CPbk2_nlx" );
    User::Panic(KPanicText,aReason);
    }

} /// namespace

const TInt KEcePresFirstIconId = 1000;
const TInt KEcePresLastIconId  = 2000;
// The maximum number of icons infos that are kept live
const TInt KEceMaxIconInfoCashe = 100;
/// The maximum number of subscriptions that are kept live.
const TInt KEceMaxSubscriptionsCached = 20;

// -----------------------------------------------------------------------------
// CPbk2EcePresenceEngine::NewL
// -----------------------------------------------------------------------------
//
CPbk2EcePresenceEngine* CPbk2EcePresenceEngine::NewL(
    CVPbkContactManager& aContactManager,
    MPbk2EcePresenceEngineObserver& aObserver,
    TSize aIconSize )
    {
    CPbk2EcePresenceEngine* self = 
        new ( ELeave ) CPbk2EcePresenceEngine( aContactManager, aObserver, aIconSize );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPbk2EcePresenceEngine::~CPbk2EcePresenceEngine
// -----------------------------------------------------------------------------
//
CPbk2EcePresenceEngine::~CPbk2EcePresenceEngine()
    {
    if ( iPresenceClient )
        {
        iPresenceClient->Close();
        }
    delete iSubscriptionMonitor;
    iSubscriptions.ResetAndDestroy();
    iIconInfoArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CPbk2EcePresenceEngine::CPbk2EcePresenceEngine
// -----------------------------------------------------------------------------
//
CPbk2EcePresenceEngine::CPbk2EcePresenceEngine
        (
    CVPbkContactManager& aContactManager,
    MPbk2EcePresenceEngineObserver& aObserver,
    TSize aIconSize )  :
    iContactManager( aContactManager ),
    iObserver( aObserver ),
    iIconSize( aIconSize )
    {
    }

// -----------------------------------------------------------------------------
// CPbk2EcePresenceEngine::ConstructL
// -----------------------------------------------------------------------------
//
void CPbk2EcePresenceEngine::ConstructL()
    {
    iPresenceClient = TContactPresenceFactory::NewContactPresenceL( iContactManager, *this );
    
    iNextIconId = KEcePresFirstIconId;
    iSubscriptionMonitor = CIdle::NewL(CActive::EPriorityIdle); 
    }

void CPbk2EcePresenceEngine::SubscribeContactPresenceL( const MVPbkContactLink& aLink )
    {
    CPbk2NlxPresenceSubscriptionInfo* subscriptionInfo = FindSubscription( aLink );
       
    if ( !subscriptionInfo )
        {
        // This contact is not observed yet.
        // Costruct our own placeholder for information about this request
        MVPbkContactLink* link = aLink.CloneLC();
        subscriptionInfo = CPbk2NlxPresenceSubscriptionInfo::NewL(
            link ); // takes ownership of link
        CleanupStack::Pop(); // link
        CleanupStack::PushL( subscriptionInfo );
        User::LeaveIfError( iSubscriptions.Append( subscriptionInfo ) );
        CleanupStack::Pop( subscriptionInfo );
        
        if(!iSubscriptionMonitor->IsActive())
            {
            iSubscriptionMonitor->Start(TCallBack(CPbk2EcePresenceEngine::SubscriptionMonitorL, this));
            }
        }
    CleanUpOldSubscriptionsL();
    }

TBool CPbk2EcePresenceEngine::GetIconIdForContact(
    const MVPbkContactLink& aLink,
    TPbk2IconId& aIconId )
    {
    // Do we have an icon NOW?
    CPbk2NlxPresenceSubscriptionInfo* subscriptionInfo = FindSubscription( aLink );
    TBool hasIcon = EFalse;
    if ( subscriptionInfo )
        {
        // We have subscribed for the presence of this contact.
        const CPbk2NlxPresenceIconInfo* iconInfo = subscriptionInfo->IconInfo();
        // But do we have an icon for it?
        if ( iconInfo )
            {
            if ( iconInfo->Bitmap() && iconInfo->BitmapMask() )
                {
                // We have received the icon information for this contact and the actual icons.
                TPbk2IconId iconId( TUid::Uid( KPbk2UID3 ), iconInfo->IconId()  );
                aIconId = iconId;
                hasIcon = ETrue;
                }
            }
        }
    return hasIcon;
    }

CGulIcon* CPbk2EcePresenceEngine::CreateIconCopyLC( const TPbk2IconId& aIconId )
    {
    CPbk2NlxPresenceIconInfo* iconInfo = FindIconInfo( aIconId );
    __ASSERT_DEBUG( iconInfo , Panic( ENlxPanicNoIconInfo ) );
    __ASSERT_ALWAYS( iconInfo , User::Leave( KErrGeneral ) );
    // According to the contract of GetIconIdForContact(), it returns ETrue
    // only if the actual icons exist.
    __ASSERT_DEBUG( iconInfo->Bitmap() && iconInfo->BitmapMask(),
        Panic( ENlxPanicNoIcons ) );
    __ASSERT_ALWAYS( iconInfo->Bitmap() && iconInfo->BitmapMask(),
    	User::Leave( KErrGeneral ) );
    CFbsBitmap* newBitmap = CloneBitmapL( *iconInfo->Bitmap() );
    CFbsBitmap* newBitmapMask = CloneBitmapL( *iconInfo->BitmapMask() );
    CGulIcon* gulIcon = CGulIcon::NewL( newBitmap, newBitmapMask );
    CleanupStack::PushL( gulIcon );
    return gulIcon;
    }


void CPbk2EcePresenceEngine::ReceiveIconInfoL(
    const TDesC8& aPackedLink, const TDesC8& aBrandId, const TDesC8& aElementId )
    {
    __ASSERT_DEBUG( aPackedLink.Length() > 0, Panic( ENlxPanicEmptyPackage) );
    __ASSERT_ALWAYS( aPackedLink.Length() > 0, User::Leave( KErrGeneral ) );

    // Unpack the link array
    CVPbkContactLinkArray* linkArray =
        CVPbkContactLinkArray::NewLC( aPackedLink, iContactManager.ContactStoresL() );
    __ASSERT_DEBUG( linkArray->Count() > 0, Panic( ENlxPanicNoLinks) );
    __ASSERT_ALWAYS( linkArray->Count() > 0, User::Leave( KErrGeneral ) );
    const MVPbkContactLink& link = linkArray->At( 0 ); // There is icon info for just one contact.

    // Find the corresponding contact info
    CPbk2NlxPresenceSubscriptionInfo* subscriptionInfo = FindSubscription( link );
    __ASSERT_DEBUG( subscriptionInfo, Panic( ENlxPanicStrayIconInfo) );
    if ( subscriptionInfo )
        {
        // Check to see if we already have a placeholder object for this icon ID.
        CPbk2NlxPresenceIconInfo* iconInfo = FindIconInfo( aBrandId, aElementId );
        if ( !iconInfo )
            {
            // No placeholder exists yet. This is the first time that we heard
            // of this icon. Create an object to hold the icon and its ID information.
            // (we don't have the actual icon yet)
            iconInfo = CPbk2NlxPresenceIconInfo::NewLC( aBrandId, aElementId, iNextIconId );
            User::LeaveIfError( iIconInfoArray.Append( iconInfo ) );
            CleanupStack::Pop( iconInfo );
            ++iNextIconId;
            if ( iNextIconId > KEcePresLastIconId )
                {
                iNextIconId = KEcePresFirstIconId;
                }

            // Request the icon. It will arrive later through ReceiveIconFileL().
            iPresenceClient->GetPresenceIconFileL( aBrandId, aElementId );
            }

        // Create a mapping from subscriptionInfo to iconInfo.
        subscriptionInfo->SetIconInfo( iconInfo ); // ownership not transferred

        // Inform Phonebook UI that contact needs to be redrawn
        iObserver.ContactPresenceChanged( subscriptionInfo->ContactLink() );
        }
    else
        {
        // It was a stray contact info which we didn't request.
        }
    CleanupStack::PopAndDestroy( linkArray );
    }

void CPbk2EcePresenceEngine::ReceiveIconFileL(
    const TDesC8& aBrandId,
    const TDesC8& aElementId,
    CFbsBitmap* aBrandedBitmap,
    CFbsBitmap* aMask )
    {
   // __ASSERT_DEBUG( aBrandedBitmap && aMask, Panic( ENlxPanicIconNull) );
   // __ASSERT_ALWAYS( aBrandedBitmap && aMask, User::Leave( KErrGeneral ) );
    // Find the corresponding icon info
    CPbk2NlxPresenceIconInfo* iconInfo = FindIconInfo( aBrandId, aElementId );

    if ( iconInfo )
        {
        iconInfo->SetIcons( aBrandedBitmap, aMask ); // takes ownership

        InformObserverOfContactChanges( aBrandId, aElementId );
        }
    else
        {
        // It was a stray icon which we didn't request.
        delete aBrandedBitmap;
        delete aMask;
        }
    }

void CPbk2EcePresenceEngine::ReceiveIconInfosL(
    const TDesC8& /*aPackedLink*/,
    RPointerArray <MContactPresenceInfo>& /*aInfoArray*/,
    TInt /*aOpId*/ )
    {
    }

void CPbk2EcePresenceEngine::InformObserverOfContactChanges(
    const TDesC8& aBrandId,
    const TDesC8& aElementId )
    {
    // Tell observer that the UI needs to be refreshed
    // Loop through all subscriptions and check which ones have a matching icon.
    for ( TInt n = 0; n < iSubscriptions.Count(); ++n )
        {
        CPbk2NlxPresenceSubscriptionInfo* subscr = iSubscriptions[n];
        if ( subscr->IconInfo() )
            {
            if ( subscr->IconInfo()->BrandId().Compare( aBrandId ) == 0 &&
                subscr->IconInfo()->ElementId().Compare( aElementId ) == 0 )
                {
                // The icon matches
                // Inform Phonebook about this contact
                iObserver.ContactPresenceChanged( subscr->ContactLink() );
                }
            }
        }
    }

/**
 * Find a contact watching info object.
 * @return The object, or NULL if not found.
 */
CPbk2NlxPresenceSubscriptionInfo* CPbk2EcePresenceEngine::FindSubscription(
    const MVPbkContactLink& aContactLink )
    {
    CPbk2NlxPresenceSubscriptionInfo* info = NULL;
    TInt count = iSubscriptions.Count();
    for ( TInt n = 0; n < count && !info; ++n )
        {
        CPbk2NlxPresenceSubscriptionInfo* tmpInfo = iSubscriptions[n];
        if ( tmpInfo->ContactLink().IsSame( aContactLink ) )
            {
            // The info was found
            info = tmpInfo;
            }
        }
    return info;
    }

/**
 * Find an icon info object.
 * @return The object, or NULL if not found.
 */
CPbk2NlxPresenceIconInfo* CPbk2EcePresenceEngine::FindIconInfo(
    const TDesC8& aBrandId,
    const TDesC8& aElementId )
    {
    CPbk2NlxPresenceIconInfo* info = NULL;
    TInt count = iIconInfoArray.Count();
    for ( TInt n = 0; n < count && !info; ++n )
        {
        CPbk2NlxPresenceIconInfo* tmpInfo = iIconInfoArray[n];
        if ( tmpInfo->BrandId().Compare( aBrandId ) == 0
            && tmpInfo->ElementId().Compare( aElementId ) == 0 )
            {
            // The icon info was found
            info = tmpInfo;
            }
        }
    return info;
    }

/**
 * Find an icon info object.
 *
 * @todo The UID part of the icon ID is ignored currently.
 * @return The object, or NULL if not found.
 */
CPbk2NlxPresenceIconInfo* CPbk2EcePresenceEngine::FindIconInfo( const TPbk2IconId& aIconId )
    {
    CPbk2NlxPresenceIconInfo* info = NULL;
    TInt count = iIconInfoArray.Count();
    for ( TInt n = 0; n < count && !info; ++n )
        {
        CPbk2NlxPresenceIconInfo* tmpInfo = iIconInfoArray[n];
        // Create a temporary pbk icon id for comparison purposes
        TPbk2IconId iconId( TUid::Uid( KPbk2UID3 ), tmpInfo->IconId()  );
        if ( iconId == aIconId )
            {
            // The icon info was found
            info = tmpInfo;
            }
        }
    return info;
    }

void CPbk2EcePresenceEngine::CleanUpOldSubscriptionsL()
    {  
    // Unsubscribe the oldest excess subscriptions
    const TInt count = iSubscriptions.Count();
    // Loop through the excess subscriptions, starting from the oldest
    for ( TInt n = count - 1; n >= KEceMaxSubscriptionsCached; --n )
        {
        CPbk2NlxPresenceSubscriptionInfo* subscr = iSubscriptions[KEceMaxSubscriptionsCached-n];

        // Construct a package that contains the link.
        CDesC8ArrayFlat* desArray = new (ELeave) CDesC8ArrayFlat( 2 ); // granularity of 2
        CleanupStack::PushL( desArray );
        HBufC8* packedLink = subscr->ContactLink().PackLC();
        desArray->AppendL( *packedLink );
        CleanupStack::PopAndDestroy( packedLink );
        if(subscr->IsSubscribed())
            {
            iPresenceClient->CancelSubscribePresenceInfo( *desArray );
            }
        CleanupStack::PopAndDestroy( desArray );

        iSubscriptions.Remove( 0 );
        delete subscr;
        }
    }

void CPbk2EcePresenceEngine::CleanUpExtraIcons()
    {
    const TInt subscriptionsCount = iSubscriptions.Count();
    TInt iconCount = iIconInfoArray.Count();
    CPbk2NlxPresenceIconInfo* iconinfo = NULL;
    CPbk2NlxPresenceSubscriptionInfo* subscriptionInfo = NULL;
    
    for (TInt i=0; i<iconCount; i++)
        {
        TBool found = EFalse;
        iconinfo = iIconInfoArray[i];
        
        for (TInt j=0; j<subscriptionsCount; j++)
            {
            subscriptionInfo = iSubscriptions[j];
            if(iconinfo == subscriptionInfo->IconInfo())
                 {
                 found = ETrue;
                 }
            }
        if(!found)
            {
            iIconInfoArray.Remove(i);
            delete iconinfo;
            i--;
            iconCount--;
            }
        }
    }

CFbsBitmap* CPbk2EcePresenceEngine::CloneBitmapL( const CFbsBitmap& aOriginalBitmap )
    {
    CFbsBitmap* newBitmap = new (ELeave) CFbsBitmap;
    newBitmap->Duplicate(aOriginalBitmap.Handle());   
    return newBitmap;
    }
void CPbk2EcePresenceEngine::PresenceSubscribeError( 
        const TDesC8& aContactLink,
        TInt aStatus )
    {
    }       

void CPbk2EcePresenceEngine::ErrorOccured( 
        TInt aOpId, 
        TInt aStatus )
    {
    }
TInt CPbk2EcePresenceEngine::SubscriptionMonitorL(TAny* aPtr)
    {
    CPbk2EcePresenceEngine* ptr = (CPbk2EcePresenceEngine*) aPtr;
    TInt result = ptr->SubscriptionMonitorL();
    return result;//if False, CIdle ends loop
    }
TInt CPbk2EcePresenceEngine::SubscriptionMonitorL()
    {
    const TInt StepGranularity = 1; 
    const TInt count = iSubscriptions.Count();
    CPbk2NlxPresenceSubscriptionInfo* subscriptionInfo=NULL;
    TInt progress=0;
    TInt n=count;
    // Loop through the excess subscriptions, starting from the newest      
    while(n>0 && progress<StepGranularity)
        {
        n--;
        subscriptionInfo = iSubscriptions[n];
        
        if(!subscriptionInfo->IsSubscribed())
            {
            // Construct a package that contains the link.
            CDesC8ArrayFlat* desArray = new (ELeave) CDesC8ArrayFlat( 2 ); // granularity of 2
            CleanupStack::PushL( desArray );
        
            HBufC8* packedLink = subscriptionInfo->ContactLink().PackLC();
            desArray->AppendL( *packedLink );
            CleanupStack::PopAndDestroy( packedLink );
            
            iPresenceClient->SetPresenceIconSize( iIconSize );
            iPresenceClient->SubscribePresenceInfoL( *desArray );
            CleanupStack::PopAndDestroy( desArray );
            // The results will arrive through the observer interface - ReceiveIconInfoL().
            subscriptionInfo->Subscribed();
            progress++;
            }  
        }
    
    if(iIconInfoArray.Count() > KEceMaxIconInfoCashe)
        {
        CleanUpExtraIcons();
        }
    return progress;
    }

// End of File
