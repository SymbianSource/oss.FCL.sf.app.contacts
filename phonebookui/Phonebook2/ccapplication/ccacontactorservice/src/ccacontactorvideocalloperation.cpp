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
CCCAContactorVideocallOperation::CCCAContactorVideocallOperation(const TDesC& aParam,
        const TDesC8& aContactLinkArray ) : 
        CCCAContactorOperation(aParam),
        iContactLinkArray(aContactLinkArray)
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
CCCAContactorVideocallOperation* CCCAContactorVideocallOperation::NewLC(const TDesC& aParam, 
                const TDesC8& aContactLinkArray)
    {
    CCCAContactorVideocallOperation* self = new (ELeave)CCCAContactorVideocallOperation(
                        aParam, aContactLinkArray);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorVideocallOperation::NewL()
// -----------------------------------------------------------------------------
//
CCCAContactorVideocallOperation* CCCAContactorVideocallOperation::NewL(const TDesC& aParam, 
        const TDesC8& aContactLinkArray)
    {
    CCCAContactorVideocallOperation* self=
            CCCAContactorVideocallOperation::NewLC(aParam, aContactLinkArray);
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
    CCCAPhoneCall::ExecuteL(iParam, iContactLinkArray, CCCAPhoneCall::ECCACallTypeVideo);
    CleanupStack::PopAndDestroy(this);
    }
// End of File
