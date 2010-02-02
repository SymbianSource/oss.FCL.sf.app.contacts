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
* Description: Content info container
*
*/

#include "spbcontent.h"
#include "spbstatusprovider.h"
#include "spbserviceiconprovider.h"
#include "spbphonenumberparser.h"

// LOCAL METHODS AND CONSTANTS
namespace {

inline TBool IsFlag( TInt aFlags, TInt aFlag ) 
    {
    return ( aFlags & aFlag );
    }
}

// ----------------------------------------------------------------------------
// CSpbContent::NewLC
// ----------------------------------------------------------------------------
//
CSpbContent* CSpbContent::NewLC(
    const CSpbContent::TParameters& aParameters,
    const MVPbkContactLink& aLink)
    {
    CSpbContent* self = new (ELeave) CSpbContent( aParameters );
    CleanupStack::PushL( self );
    self->ConstructL(aLink);
    return self;
    }

// ----------------------------------------------------------------------------
// CSpbContent::~CSpbContent
// ----------------------------------------------------------------------------
//
CSpbContent::~CSpbContent()
    {
    delete iBuddyId;
    delete iLink;
    delete iText;
    delete iPhoneNumberParser;
    }

// ----------------------------------------------------------------------------
// CSpbContent::ConstructL
// ----------------------------------------------------------------------------
//
inline void CSpbContent::ConstructL(const MVPbkContactLink& aLink)
    {
    iLink = aLink.CloneLC();
    CleanupStack::Pop(); // iLink
    
    if( IsFlag(iParameters.iFeatures, CSpbContentProvider::EPhoneNumber) )
        {
        iPhoneNumberParser = CSpbPhoneNumberParser::NewL(
            *(iParameters.iContactManager), *this );
        }

    iAsyncCallBack.CallBack();
    }

// ----------------------------------------------------------------------------
// CSpbContent::CSpbContent
// ----------------------------------------------------------------------------
//
inline CSpbContent::CSpbContent(
    const CSpbContent::TParameters& aParameters ) :
    iParameters(aParameters),
    iAsyncCallBack( TCallBack( CSpbContent::StartFetchContent, this ), 
        CActive::EPriorityStandard ),
    iType( CSpbContentProvider::ETypeNone )
    {
    }

// ----------------------------------------------------------------------------
// CSpbContent::RefreshNumber
// ----------------------------------------------------------------------------
//
void CSpbContent::RefreshNumber()
	{
	// if no status text
	if( iType != CSpbContentProvider::ETypeSocialStatus && iPhoneNumberParser )
		{
		// re-fetch phonenumber
		iPhoneNumberParser->FetchPhoneNumber( Link() );
		}
	}

// ----------------------------------------------------------------------------
// CSpbContent::StartFetchContent
// ----------------------------------------------------------------------------
//
TInt CSpbContent::StartFetchContent( TAny* aPtr )
    {
    static_cast<CSpbContent*>(aPtr)->DoStartFetchContent();
    return 0;
    }

// ----------------------------------------------------------------------------
// CSpbContent::PhoneNumberUpdatedL
// ----------------------------------------------------------------------------
//
void CSpbContent::DoStartFetchContent()
    {
    if(iParameters.iStatusProvider)
        {
        iParameters.iStatusProvider->FetchStatusDataL(*this);
        }
    else if(iPhoneNumberParser)
        {
        iPhoneNumberParser->FetchPhoneNumber(Link());
        }
    else
        {
        NotifyObservers( MSpbContentProviderObserver::EContentNotAvailable );
        }
    }

// ----------------------------------------------------------------------------
// CSpbContent::PhoneNumberUpdatedL
// ----------------------------------------------------------------------------
//
void CSpbContent::PhoneNumberUpdatedL( const TDesC& aPhoneNumber )
    {
    delete iText;
    iText = NULL;
    iType = CSpbContentProvider::ETypeNone;
    iReady = EFalse;
    
    if( aPhoneNumber.Length() )
        {
        iText = aPhoneNumber.AllocL();
        iReady = ETrue;
        iType = CSpbContentProvider::ETypePhoneNumber;
        }
    NotifyObservers(MSpbContentProviderObserver::EContentChanged);
    }
    
// ----------------------------------------------------------------------------
// CSpbContent::StatusDataUpdatedL
// ----------------------------------------------------------------------------
//
void CSpbContent::StatusDataUpdatedL(
        const TDesC& aStatusMessage, 
        const TDesC8& aBrandId,
        const TDesC8& aIconEntry )
    {
    if(!aStatusMessage.Length() && iPhoneNumberParser )
        {
        iPhoneNumberParser->FetchPhoneNumber(Link());
        return;
        }
    
    delete iText;
    iText = NULL;
    if(aStatusMessage.Length())
        {
        if(iParameters.iIconProvider && aBrandId.Length() && aIconEntry.Length() )
            {
            MSpbServiceIcon* icon = 
                iParameters.iIconProvider->GetBrandedIconL( aBrandId, aIconEntry );
            iIconId = icon->IconId();
            }
        
        iText = aStatusMessage.AllocL();
        iReady = ETrue;
        iType = CSpbContentProvider::ETypeSocialStatus;
        NotifyObservers(MSpbContentProviderObserver::EContentChanged);
        }
    else
        {
        iReady = EFalse;
        iType = CSpbContentProvider::ETypeNone;
        NotifyObservers(MSpbContentProviderObserver::EContentNotAvailable);
        }
    }

// ----------------------------------------------------------------------------
// CSpbContent::NotifyObservers
// ----------------------------------------------------------------------------
//
void CSpbContent::NotifyObservers(
        MSpbContentProviderObserver::TSpbContentEvent aEvent)
    {
    const TInt count(iParameters.iObservers->Count());
    for(TInt i = 0 ; i < count ; ++i)
        {
        (*iParameters.iObservers)[i]->ContentUpdated(*iLink,aEvent);
        }
    }

// end of file
