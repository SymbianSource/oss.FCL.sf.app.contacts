/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Holds information about a presence subscription.
*
*/



#include "CPbk2NlxPresenceSubscriptionInfo.h"

// Phonebook 2
#include "CPbk2NlxPresenceIconInfo.h"

// Virtual Phonebook
#include <MVPbkContactLink.h>




CPbk2NlxPresenceSubscriptionInfo* CPbk2NlxPresenceSubscriptionInfo::NewL(
    MVPbkContactLink* aContactLink )
    {
    CPbk2NlxPresenceSubscriptionInfo* self =
        new ( ELeave ) CPbk2NlxPresenceSubscriptionInfo();
    CleanupStack::PushL( self );
    self->iContactLink = aContactLink;
    CleanupStack::Pop( self );
    return self;
    }

CPbk2NlxPresenceSubscriptionInfo::~CPbk2NlxPresenceSubscriptionInfo()
    {
    delete iContactLink;
    }

CPbk2NlxPresenceSubscriptionInfo::CPbk2NlxPresenceSubscriptionInfo()
    {
    }

const MVPbkContactLink& CPbk2NlxPresenceSubscriptionInfo::ContactLink() const
    {
    return *iContactLink;
    }

MVPbkContactLink& CPbk2NlxPresenceSubscriptionInfo::ContactLink()
    {
    return *iContactLink;
    }

void CPbk2NlxPresenceSubscriptionInfo::SetIconInfo(
    CPbk2NlxPresenceIconInfo* aIconInfo )
    {
    iIconInfo = aIconInfo; // ownership not taken
    }

const CPbk2NlxPresenceIconInfo* CPbk2NlxPresenceSubscriptionInfo::IconInfo() const
    {
    return iIconInfo;
    }

void CPbk2NlxPresenceSubscriptionInfo::Subscribed()
    {
    iSubscribed=ETrue;
    }
TBool CPbk2NlxPresenceSubscriptionInfo::IsSubscribed()
    {
    return iSubscribed;
    }

// End of File
