/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  BPAS information holder
*
*/


#include <e32base.h>

#include "bpasinfo.h"
#include "bpasheaders.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CBPASInfo::NewL
// ---------------------------------------------------------------------------
//
CBPASInfo* CBPASInfo::NewL(const TDesC& aIdentity)
    {
    CBPASInfo* self = NewLC(aIdentity);
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CBPASInfo::NewLC
// ---------------------------------------------------------------------------
//
CBPASInfo* CBPASInfo::NewLC(const TDesC& aIdentity)
    {
    CBPASInfo* self = new(ELeave) CBPASInfo();
    CleanupStack::PushL( self );
    self->ConstructL(aIdentity);
    return self;
    }
    
// ---------------------------------------------------------------------------
// CBPASInfo::CBPASInfo
// ---------------------------------------------------------------------------
//
CBPASInfo::CBPASInfo( ):
iPresenceInfo( NULL ),
iServiceInfo( NULL ),
iIconInfo( NULL )
    {
    }
    
// ---------------------------------------------------------------------------
// CBPASInfo::~CBPASInfo
// ---------------------------------------------------------------------------
//
CBPASInfo::~CBPASInfo()
    {
    if(iPresenceInfo)
        delete iPresenceInfo;
    if(iServiceInfo)
        delete iServiceInfo;
    if(iIconInfo)
        delete iIconInfo;
    if(iIdentity)
        delete iIdentity;
    }

// ---------------------------------------------------------------------------
// CBPASInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CBPASInfo::ConstructL(const TDesC& aIdentity )
    {
    iIdentity = aIdentity.AllocL();
    }

// ---------------------------------------------------------------------------
// CBPASInfo::PresenceInfo
// ---------------------------------------------------------------------------
//
EXPORT_C CBPASPresenceInfo* CBPASInfo::PresenceInfo() const
    {
    return iPresenceInfo;
    }
    
// ---------------------------------------------------------------------------
// CBPASInfo::ServiceInfo
// ---------------------------------------------------------------------------
//
EXPORT_C CBPASServiceInfo* CBPASInfo::ServiceInfo() const
    {
    return iServiceInfo;
    }

// ---------------------------------------------------------------------------
// CBPASInfo::IconInfo
// ---------------------------------------------------------------------------
//
EXPORT_C CBPASIconInfo* CBPASInfo::IconInfo()
    {
    return iIconInfo;
    }
    
// ---------------------------------------------------------------------------
// CBPASInfo::Identity
// ---------------------------------------------------------------------------
//
EXPORT_C const TPtrC CBPASInfo::Identity() const
    {
    return iIdentity ? *iIdentity : TPtrC();
    }
    
// ---------------------------------------------------------------------------
// CBPASInfo::HasPresence
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CBPASInfo::HasPresence() const
    {
    if(iPresenceInfo)
        return ETrue;
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CBPASInfo::SetPresenceInfoL
// ---------------------------------------------------------------------------
//
void CBPASInfo::SetPresenceInfoL( CBPASPresenceInfo* aPresenceInfo )
    {
    if(iPresenceInfo)
        delete iPresenceInfo;
    iPresenceInfo = aPresenceInfo;
    }

// ---------------------------------------------------------------------------
// CBPASInfo::SetServiceInfoL
// ---------------------------------------------------------------------------
//
void CBPASInfo::SetServiceInfoL( CBPASServiceInfo* aServiceInfo )
    {
    if(iServiceInfo)
        delete iServiceInfo;
    iServiceInfo = aServiceInfo;
    }

// ---------------------------------------------------------------------------
// CBPASInfo::SetIconInfoL
// ---------------------------------------------------------------------------
//
void CBPASInfo::SetIconInfoL( CBPASIconInfo* aIconInfo )
    {
    if(iIconInfo)
        delete iIconInfo;
    iIconInfo = aIconInfo;
    }

// ---------------------------------------------------------------------------
// CBPASInfo::SetIdentityL
// ---------------------------------------------------------------------------
//
void CBPASInfo::SetIdentityL(const TDesC& aIdentity)
    {
    if (iIdentity)
        {
        delete iIdentity;
        iIdentity = NULL;
        }
    iIdentity = aIdentity.AllocL();
    }
