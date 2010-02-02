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
* Description:  Implementation of the video call operation
*
*/


// INCLUDE FILES
#include "ccacontactorheaders.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCCAContactorVideocallOperation::CCCAContactorVideocallOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorVideocallOperation::CCCAContactorVideocallOperation(const TDesC& aParam) : CCCAContactorOperation(aParam)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCCAContactorVideocallOperation::~CCCAContactorVideocallOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorVideocallOperation::~CCCAContactorVideocallOperation()
    {
    }

// -----------------------------------------------------------------------------
// CCCAContactorVideocallOperation::NewLC()
// -----------------------------------------------------------------------------
//
CCCAContactorVideocallOperation* CCCAContactorVideocallOperation::NewLC(const TDesC& aParam)
    {
    CCCAContactorVideocallOperation* self = new (ELeave)CCCAContactorVideocallOperation(aParam);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorVideocallOperation::NewL()
// -----------------------------------------------------------------------------
//
CCCAContactorVideocallOperation* CCCAContactorVideocallOperation::NewL(const TDesC& aParam)
    {
    CCCAContactorVideocallOperation* self=
            CCCAContactorVideocallOperation::NewLC(aParam);
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorVideocallOperation::ConstructL()
// -----------------------------------------------------------------------------
//
void CCCAContactorVideocallOperation::ConstructL()
    {
    
    }

// -----------------------------------------------------------------------------
// CCCAContactorVideocallOperation::ExecuteLD()
// -----------------------------------------------------------------------------
//
void CCCAContactorVideocallOperation::ExecuteLD()
    {
    CleanupStack::PushL(this);
    CCCAPhoneCall::ExecuteL(iParam, CCCAPhoneCall::ECCACallTypeVideo);
    CleanupStack::PopAndDestroy(this);
    }
// End of File
