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


#ifndef SPBSTATUSPROVIDER_H_
#define SPBSTATUSPROVIDER_H_

//  INCLUDES
#include <e32base.h>
#include <presencecachereadhandler2.h>

class MPresenceCacheReader2;
class CVPbkContactManager;
class CSpbContent;
class MVPbkContactLink;
class CPresenceCacheBuddyIdCreator;

/**
 * The real implementation of the status provider
 */
NONSHARABLE_CLASS( CSpbStatusProvider ) :
        public CBase,
        public MPresenceCacheReadHandler2
    {
   
public: // Construction & destruction
    static CSpbStatusProvider* NewL(
    		RPointerArray<CSpbContent>& aContentCache );
            
    ~CSpbStatusProvider();

public: // Interface
    void FetchStatusDataL(
            CSpbContent& aContent);
    
private:    // From MPresenceCacheReadHandler2
    void HandlePresenceReadL(TInt aErrorCode,
            RPointerArray<MPresenceBuddyInfo2>& aPresenceBuddyInfoList);
    void HandlePresenceNotificationL(TInt aErrorCode,
            MPresenceBuddyInfo2* aPresenceBuddyInfo);
    
private:
    HBufC* CreateBuddyIdL(
            const MVPbkContactLink& aLink);
    CSpbContent* FindContentByBuddyId(
            const TDesC& aBuddyId);
    void NotifyChangeEventL(
            MPresenceBuddyInfo2& aBuddyInfo,
            CSpbContent& aContent);
    
private:    // constructors
    inline CSpbStatusProvider(
            RPointerArray<CSpbContent>& aContentCache);
    inline void ConstructL();
   
private: // data
    
    // Own. Presence cache read interface
    MPresenceCacheReader2* iPresenceCache;
    // Own. Buddy ID creator.
    CPresenceCacheBuddyIdCreator* iBuddyIdCreator;
    // Not own. Content cache
    RPointerArray<CSpbContent>& iContentCache;
    };

#endif /*SPBSTATUSPROVIDER_H_*/
