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
* Description:  Implementation of the email operation
*
*/


// INCLUDE FILES
#include "ccacontactorheaders.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCCAContactorEmailOperation::CCCAContactorEmailOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorEmailOperation::CCCAContactorEmailOperation(const TDesC& aParam, const TDesC& aName) : 
    CCCAContactorOperation(aParam, aName)
    {
    }

// -----------------------------------------------------------------------------
// CCCAContactorEmailOperation::~CCCAContactorEmailOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorEmailOperation::~CCCAContactorEmailOperation()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCCAContactorEmailOperation::NewLC()
// -----------------------------------------------------------------------------
//
CCCAContactorEmailOperation* CCCAContactorEmailOperation::NewLC(const TDesC& aParam, const TDesC& aName)
    {
    CCCAContactorEmailOperation* self = new (ELeave)CCCAContactorEmailOperation(aParam, aName);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorEmailOperation::NewL()
// -----------------------------------------------------------------------------
//
CCCAContactorEmailOperation* CCCAContactorEmailOperation::NewL(const TDesC& aParam, const TDesC& aName)
    {
    CCCAContactorEmailOperation* self=CCCAContactorEmailOperation::NewLC(aParam, aName);
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorEmailOperation::ConstructL()
// -----------------------------------------------------------------------------
//
void CCCAContactorEmailOperation::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CCCAContactorEmailOperation::ExecuteLD()
// -----------------------------------------------------------------------------
//
void CCCAContactorEmailOperation::ExecuteLD()
    {
	CleanupStack::PushL(this);
	
	TInt err = KErrNone;
	TRAP( err,
    CCCAMsgEditors::ExecuteL( KSenduiMtmSmtpUid, iParam, iName );
	);
	
	if ( KErrNone != err )
		{
	    // Show error note.
	    CCoeEnv::Static()->HandleError( err );
		}
	
    CleanupStack::PopAndDestroy(this);
    }
// Enf of File
