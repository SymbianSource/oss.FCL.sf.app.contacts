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

#ifndef SPBCONTENT_H
#define SPBCONTENT_H

//  INCLUDES
#include <e32base.h>
#include <TPbk2IconId.h>
#include <MVPbkContactLink.h>
#include "spbcontentprovider.h"

class CVPbkContactManager;
class CSpbServiceIconProvider;
class CSpbStatusProvider;
class MSpbContentProviderObserver;
class CSpbPhoneNumberParser;

NONSHARABLE_CLASS( CSpbContent ) : public CBase
    {
public:
struct TParameters
    {
    CVPbkContactManager* iContactManager;
    TInt32 iFeatures;
    RPointerArray<MSpbContentProviderObserver>* iObservers;
    CSpbServiceIconProvider* iIconProvider; // can be NULL
    CSpbStatusProvider* iStatusProvider;  // can be NULL
    };
    
    static CSpbContent* NewLC(
            const CSpbContent::TParameters& aParameters,
            const MVPbkContactLink& aLink);
    
    ~CSpbContent();

public:
    inline TBool IsReady() const;
    inline TPtrC Text() const;
    inline const TPbk2IconId& IconId() const;
    inline TBool Match(const MVPbkContactLink& aLink) const;
    inline TBool Match(const TDesC& aBuddyId) const;
    inline MVPbkContactLink& Link() const;
    inline void SetBuddyId( HBufC* aId );
    inline CSpbContentProvider::TSpbContentType Type() const;

    void StatusDataUpdatedL(
            const TDesC& aStatusMessage, 
            const TDesC8& aBrandId,
            const TDesC8& aIconEntry );
    void PhoneNumberUpdatedL(
            const TDesC& aPhoneNumber );
    /*
	 * Function reloads the phonenumber, if there is no status text provided
	 */
    void RefreshNumber();
    
private:
    inline CSpbContent(
            const CSpbContent::TParameters& aParameters);
    inline void ConstructL(
            const MVPbkContactLink& aLink);
private:
    static TInt StartFetchContent( 
            TAny* aPtr );
    void DoStartFetchContent();
    void NotifyObservers(
            MSpbContentProviderObserver::TSpbContentEvent aEvent);
    
private:
        
    TParameters iParameters;
    /// Own
    MVPbkContactLink* iLink;
    // Own
    CSpbPhoneNumberParser* iPhoneNumberParser;
    /// 
    CAsyncCallBack iAsyncCallBack;
    /// Own
    HBufC* iText;
    /// Own
    HBufC* iBuddyId;
    TPbk2IconId iIconId;
    TBool iReady;
    /// Content type
    CSpbContentProvider::TSpbContentType iType;
    };

inline void CSpbContent::SetBuddyId( HBufC* aId )
    {
    delete iBuddyId;
    iBuddyId = aId;
    }

inline MVPbkContactLink& CSpbContent::Link() const
    {
    return *iLink;
    }

inline TBool CSpbContent::Match(const MVPbkContactLink& aLink) const
    {
    return iLink->IsSame( aLink );
    }

inline TBool CSpbContent::Match(const TDesC& aBuddyId) const
    {
    if(iBuddyId && iBuddyId->Compare(aBuddyId) == 0)
        {
        return ETrue;
        }
    return EFalse;
    }

inline TBool CSpbContent::IsReady() const
    {
    return iReady;
    }

inline TPtrC CSpbContent::Text() const
    {
    return iText ? *iText : KNullDesC();
    }

inline const TPbk2IconId& CSpbContent::IconId() const
    {
    return iIconId;
    }

inline CSpbContentProvider::TSpbContentType CSpbContent::Type() const
    {
    return iType;
    }

#endif // SPBCONTENT_H
