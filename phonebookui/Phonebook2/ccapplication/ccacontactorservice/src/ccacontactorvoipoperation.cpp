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
* Description:  Implementation of the voip operation
*
*/


// INCLUDE FILES
#include "ccacontactorheaders.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCCAContactorVOIPOperation::CCCAContactorVOIPOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorVOIPOperation::CCCAContactorVOIPOperation(const TDesC& aParam, TUint32 aServiceId) : CCCAContactorOperation(aParam), iServiceId(aServiceId)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCCAContactorVOIPOperation::~CCCAContactorVOIPOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorVOIPOperation::~CCCAContactorVOIPOperation()
    {
    }

// -----------------------------------------------------------------------------
// CCCAContactorVOIPOperation::NewLC()
// -----------------------------------------------------------------------------
//
CCCAContactorVOIPOperation* CCCAContactorVOIPOperation::NewLC(const TDesC& aParam, TUint32 aServiceId)
    {
    CCCAContactorVOIPOperation* self = new (ELeave)CCCAContactorVOIPOperation(aParam, aServiceId);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorVOIPOperation::NewL()
// -----------------------------------------------------------------------------
//
CCCAContactorVOIPOperation* CCCAContactorVOIPOperation::NewL(const TDesC& aParam, TUint32 aServiceId)
    {
    CCCAContactorVOIPOperation* self=CCCAContactorVOIPOperation::NewLC(aParam, aServiceId);
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorVOIPOperation::ConstructL()
// -----------------------------------------------------------------------------
//
void CCCAContactorVOIPOperation::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CCCAContactorVOIPOperation::ExecuteLD()
// -----------------------------------------------------------------------------
//
void CCCAContactorVOIPOperation::ExecuteLD()
    {
    CleanupStack::PushL(this);
    CCCAPhoneCall::ExecuteL(iParam, CCCAPhoneCall::ECCACallTypeVoIP, iServiceId);
    CleanupStack::PopAndDestroy(this);
    }
// End of File