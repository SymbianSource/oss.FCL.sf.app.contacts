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
* Description:  Implementation of the call operation
*
*/


// INCLUDE FILES
#include "ccacontactorheaders.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCCAContactorCallOperation::CCCAContactorCallOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorCallOperation::CCCAContactorCallOperation(const TDesC& aParam, const TDesC8& aContactLinkArray) : CCCAContactorOperation(aParam), 
        iContactLinkArray(aContactLinkArray)
    {
    }

// -----------------------------------------------------------------------------
// CCCAContactorCallOperation::~CCCAContactorCallOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorCallOperation::~CCCAContactorCallOperation()
    {
    }

// -----------------------------------------------------------------------------
// CCCAContactorCallOperation::NewLC()
// -----------------------------------------------------------------------------
//
CCCAContactorCallOperation* CCCAContactorCallOperation::NewLC(const TDesC& aParam, const TDesC8& aContactLinkArray)
    {
    CCCAContactorCallOperation* self = new (ELeave)CCCAContactorCallOperation(aParam, aContactLinkArray);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorCallOperation::NewL()
// -----------------------------------------------------------------------------
//
CCCAContactorCallOperation* CCCAContactorCallOperation::NewL(const TDesC& aParam, const TDesC8& aContactLinkArray)
    {
    CCCAContactorCallOperation* self=CCCAContactorCallOperation::NewLC(aParam, aContactLinkArray);
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorCallOperation::ConstructL()
// -----------------------------------------------------------------------------
//
void CCCAContactorCallOperation::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CCCAContactorCallOperation::ExecuteLD()
// -----------------------------------------------------------------------------
//
void CCCAContactorCallOperation::ExecuteLD()
    {
    CleanupStack::PushL(this);
    CCCAPhoneCall::ExecuteL(iParam, iContactLinkArray, CCCAPhoneCall::ECCACallTypeVoice);
    CleanupStack::PopAndDestroy(this);
    }
// End of File
