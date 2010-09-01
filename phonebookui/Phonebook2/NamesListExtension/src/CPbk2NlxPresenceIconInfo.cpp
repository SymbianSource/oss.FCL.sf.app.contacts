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
* Description:  Holds icon ID information and an actual icon and its mask.
*
*/


#include "CPbk2NlxPresenceIconInfo.h"

// System includes
#include <fbs.h>




CPbk2NlxPresenceIconInfo* CPbk2NlxPresenceIconInfo::NewLC(
    const TDesC8& aBrandId,
    const TDesC8& aElementId,
    TInt aIconId )
    {
    CPbk2NlxPresenceIconInfo* self = new ( ELeave ) CPbk2NlxPresenceIconInfo( aIconId );
    CleanupStack::PushL( self );
    self->ConstructL( aBrandId, aElementId );
    return self;
    }


CPbk2NlxPresenceIconInfo::~CPbk2NlxPresenceIconInfo()
    {
    delete iBrandId;
    delete iElementId;
    delete iBitmap;
    delete iBitmapMask;
    }

CPbk2NlxPresenceIconInfo::CPbk2NlxPresenceIconInfo( TInt aIconId ) : iIconId( aIconId )
    {
    }

void CPbk2NlxPresenceIconInfo::ConstructL( const TDesC8& aBrandId, const TDesC8& aElementId )
    {
    iBrandId = aBrandId.AllocL();
    iElementId = aElementId.AllocL();
    }

TPtrC8 CPbk2NlxPresenceIconInfo::BrandId() const
    {
    return *iBrandId; //always exists
    }

TPtrC8 CPbk2NlxPresenceIconInfo::ElementId() const
    {
    return *iElementId; //always exists
    }

void CPbk2NlxPresenceIconInfo::SetIcons( CFbsBitmap* aBitmap, CFbsBitmap* aBitmapMask )
    {
    delete iBitmap;
    delete iBitmapMask;
    iBitmap = aBitmap;
    iBitmapMask = aBitmapMask;
    }

const CFbsBitmap* CPbk2NlxPresenceIconInfo::Bitmap() const
    {
    return iBitmap;
    }

const CFbsBitmap* CPbk2NlxPresenceIconInfo::BitmapMask() const
    {
    return iBitmapMask;
    }

TInt CPbk2NlxPresenceIconInfo::IconId() const
    {
    return iIconId;
    }

// End of File
