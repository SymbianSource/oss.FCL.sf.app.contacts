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
* Description:  BPAS presence information holder
*
*/


#include <e32base.h>
#include "bpaspresenceinfo.h"
#include <mpresencebuddyinfo2.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CBPASPresenceInfo::NewL
// ---------------------------------------------------------------------------
//
CBPASPresenceInfo* CBPASPresenceInfo::NewL(const TDesC& aAvailabilityText,
        MPresenceBuddyInfo2 ::TAvailabilityValues aAvailabilityEnum)
    {
    CBPASPresenceInfo* self = CBPASPresenceInfo::NewLC(aAvailabilityText, 
                                                            aAvailabilityEnum);
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CBPASPresenceInfo::NewL
// ---------------------------------------------------------------------------
//
CBPASPresenceInfo* CBPASPresenceInfo::NewLC(const TDesC& aAvailabilityText,
                        MPresenceBuddyInfo2::TAvailabilityValues aAvailabilityEnum)
    {
    CBPASPresenceInfo* self = new( ELeave ) CBPASPresenceInfo(aAvailabilityEnum);
    CleanupStack::PushL( self );
    self->ConstructL(aAvailabilityText);
    return self;
    }

// ---------------------------------------------------------------------------
// CBPASPresenceInfo::CBPASPresenceInfo
// ---------------------------------------------------------------------------
//
CBPASPresenceInfo::CBPASPresenceInfo(
        MPresenceBuddyInfo2::TAvailabilityValues aAvailabilityEnum): 
iAvailabilityEnum(aAvailabilityEnum)
    {
    }
    
// ---------------------------------------------------------------------------
// CBPASPresenceInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CBPASPresenceInfo::ConstructL(const TDesC& aAvailabilityText)
    {
    iAvailabilityText = aAvailabilityText.AllocL();
    }
    
       
// ---------------------------------------------------------------------------
// CBPASPresenceInfo::~CBPASPresenceInfo
// ---------------------------------------------------------------------------
//
CBPASPresenceInfo::~CBPASPresenceInfo()
    {
    if(iAvailabilityText)
        delete iAvailabilityText;
    }

// ---------------------------------------------------------------------------
// CBPASPresenceInfo:AvailabilityText
// ---------------------------------------------------------------------------
//
EXPORT_C const TPtrC CBPASPresenceInfo::AvailabilityText() const
    {
    return iAvailabilityText ? *iAvailabilityText : TPtrC(KNullDesC);
    }
    
// ---------------------------------------------------------------------------
// CBPASPresenceInfo::AvailabilityEnum
// ---------------------------------------------------------------------------
//
EXPORT_C MPresenceBuddyInfo2::TAvailabilityValues CBPASPresenceInfo::AvailabilityEnum( ) const
    {
    return iAvailabilityEnum;
    }
    
//eof
