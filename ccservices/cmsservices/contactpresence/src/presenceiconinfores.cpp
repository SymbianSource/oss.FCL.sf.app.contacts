/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  contact presence
*
*/


// INCLUDE FILES

#include <e32std.h>
#include <e32base.h>
#include "presenceiconinfores.h"

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CPresenceIconInfoRes::CPresenceIconInfoRes
// ----------------------------------------------------------
//
CPresenceIconInfoRes::CPresenceIconInfoRes( TInt aOpId )
: iOpId( aOpId )
    {
    }

// ----------------------------------------------------------
// CPresenceIconInfoRes::~CPresenceIconInfoRes
// ----------------------------------------------------------
//
CPresenceIconInfoRes::~CPresenceIconInfoRes()
    {    
    delete iBrandId;
    iBrandId = NULL;
    
    delete iElementId;
    iElementId = NULL;
    
    delete iServiceName;
    iServiceName = NULL;
    }

// ----------------------------------------------------------
// CPresenceIconInfoRes::NewL
// ----------------------------------------------------------
//
CPresenceIconInfoRes* CPresenceIconInfoRes::NewL(
        const TDesC8& aBrandId,
        const TDesC8& aElementId,
        const TDesC& aServiceName,
        TInt aOpId )
    {
    CPresenceIconInfoRes* self = new (ELeave) CPresenceIconInfoRes( aOpId );
    CleanupStack::PushL( self );
    self->ConstructL(  aBrandId, aElementId, aServiceName);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPresenceIconInfoRes::ConstructL
// ---------------------------------------------------------------------------
//
void CPresenceIconInfoRes::ConstructL(         
    const TDesC8& aBrandId,
    const TDesC8& aElementId,
    const TDesC& aServiceName )
    {
    iBrandId = aBrandId.AllocL();
    iElementId = aElementId.AllocL();
    iServiceName = aServiceName.AllocL();        
    }

// ----------------------------------------------------------
TPtrC8 CPresenceIconInfoRes::BrandId() 
    {
    return iBrandId ? iBrandId->Des() : TPtrC8();
    }

// ----------------------------------------------------------
TPtrC8 CPresenceIconInfoRes::ElementId()
    {
    return iElementId ? iElementId->Des() : TPtrC8();
    }
    
// ----------------------------------------------------------
TPtrC CPresenceIconInfoRes::ServiceName() 
    {
    return iServiceName ? iServiceName->Des() : TPtrC();
    }

// ----------------------------------------------------------
TInt CPresenceIconInfoRes::OpId() 
    {
    return iOpId;
    }
    

    
// ----------------------------------------------------------

