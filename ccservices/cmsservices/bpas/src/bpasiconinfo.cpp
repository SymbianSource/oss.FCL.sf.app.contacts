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
* Description:  BPAS icon information holder
*
*/


#include <e32base.h>

#include "bpasheaders.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CBPASIconInfo::NewL
// ---------------------------------------------------------------------------
//
CBPASIconInfo* CBPASIconInfo::NewL(const TDesC8& aImageElementId)
    {
    CBPASIconInfo* self = CBPASIconInfo::NewLC(aImageElementId);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CBPASIconInfo::NewL
// ---------------------------------------------------------------------------
//
CBPASIconInfo* CBPASIconInfo::NewLC(const TDesC8& aImageElementId )
    {
    CBPASIconInfo* self = new( ELeave ) CBPASIconInfo();
    CleanupStack::PushL( self );
    self->ConstructL(aImageElementId);
    return self;
    }

// ---------------------------------------------------------------------------
// CBPASIconInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CBPASIconInfo::ConstructL(const TDesC8& aImageElementId)
    {
    iImageElementId = aImageElementId.AllocL();
    }
    
// ---------------------------------------------------------------------------
// CBPASIconInfo::CBPASIconInfo
// ---------------------------------------------------------------------------
//
CBPASIconInfo::CBPASIconInfo(): iTextElementId(NULL)
    {
    }

// ---------------------------------------------------------------------------
// CBPASIconInfo::~CBPASIconInfo
// ---------------------------------------------------------------------------
//
CBPASIconInfo::~CBPASIconInfo()
    {
    if(iImageElementId)
        delete iImageElementId;
    if(iTextElementId)
        delete iTextElementId;
    }

// ---------------------------------------------------------------------------
// CBPASIconInfo::TextElementId
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CBPASIconInfo::TextElementId() const
    {
    return iTextElementId ? *iTextElementId : TPtrC8();  
    }

// ---------------------------------------------------------------------------
// CBPASIconInfo::~CBPASIconInfo
// ---------------------------------------------------------------------------
//     
EXPORT_C TPtrC8 CBPASIconInfo::ImageElementId() const
    {
    return iImageElementId ? *iImageElementId : TPtrC8();
    }
    
// ---------------------------------------------------------------------------
// CBPASIconInfo::SetTextElementIdL
// ---------------------------------------------------------------------------
//     
void CBPASIconInfo::SetTextElementIdL(const TDesC8& aTextElementId)
    {
    if (iTextElementId)
        {
        delete iTextElementId;
        iTextElementId = NULL;
        }
    iTextElementId = aTextElementId.AllocL();
    }



