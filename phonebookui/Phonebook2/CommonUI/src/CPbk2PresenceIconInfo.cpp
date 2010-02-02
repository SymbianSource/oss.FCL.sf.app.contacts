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
* Description:  Phonebook 2 presence icon information.
*
*/


#include "CPbk2PresenceIconInfo.h"

// System includes
#include <fbs.h>

EXPORT_C CPbk2PresenceIconInfo* CPbk2PresenceIconInfo::NewL( 
    const TDesC8& aBrandId,
    const TDesC8& aElementId,
    const TDesC16& aServiceName )
    {
    CPbk2PresenceIconInfo* self =
        new ( ELeave ) CPbk2PresenceIconInfo();
    CleanupStack::PushL( self );
    self->ConstructL( aBrandId, aElementId, aServiceName );
    CleanupStack::Pop( self );
    return self;
    }
                            
CPbk2PresenceIconInfo::~CPbk2PresenceIconInfo()
    {
    delete iBrandId;
    delete iElementId;
    delete iServiceName;
    delete iBitmap;
    delete iBitmapMask;    
    }
            
CPbk2PresenceIconInfo::CPbk2PresenceIconInfo()
    {
    }

void CPbk2PresenceIconInfo::ConstructL(
    const TDesC8& aBrandId,
    const TDesC8& aElementId,
    const TDesC16& aServiceName )
    {
    iBrandId = aBrandId.AllocL();
    iElementId = aElementId.AllocL();
    iServiceName = aServiceName.AllocL();
    }
            
EXPORT_C void CPbk2PresenceIconInfo::SetBitmap(
    CFbsBitmap* aBitmap,
    CFbsBitmap* aBitmapMask )
    {
    if ( iBitmap != NULL )
        {
        delete iBitmap;
        }
    iBitmap = aBitmap;
    
    if ( iBitmapMask != NULL )
        {
        delete iBitmapMask;
        }
    iBitmapMask = aBitmapMask;
    }

EXPORT_C const TDesC8& CPbk2PresenceIconInfo::BrandId()
    {
    return *iBrandId;
    }
                
EXPORT_C const TDesC8& CPbk2PresenceIconInfo::ElementId()
    {
    return *iElementId;
    }
                
EXPORT_C const TDesC16& CPbk2PresenceIconInfo::ServiceName()
    {
    return *iServiceName;
    }

EXPORT_C const CFbsBitmap* CPbk2PresenceIconInfo::IconBitmap()
    {
    return iBitmap;
    }

EXPORT_C const CFbsBitmap* CPbk2PresenceIconInfo::IconBitmapMask()
    {
    return iBitmapMask;
    }
    
// End of File
