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
* Description:  Implementation of unieditor (SMS/MMS) operation
*
*/


// INCLUDE FILES
#include "ccacontactorheaders.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCCAContactorUniEditorOperation::CCCAContactorUniEditorOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorUniEditorOperation::CCCAContactorUniEditorOperation(const TDesC& aParam, const TDesC& aName) : 
    CCCAContactorOperation(aParam, aName)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCCAContactorUniEditorOperation::~CCCAContactorUniEditorOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorUniEditorOperation::~CCCAContactorUniEditorOperation()
    {
    }

// -----------------------------------------------------------------------------
// CCCAContactorUniEditorOperation::NewLC()
// -----------------------------------------------------------------------------
//
CCCAContactorUniEditorOperation* CCCAContactorUniEditorOperation::NewLC(const TDesC& aParam, const TDesC& aName)
    {
    CCCAContactorUniEditorOperation* self = new (ELeave)CCCAContactorUniEditorOperation(aParam, aName);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorUniEditorOperation::NewL()
// -----------------------------------------------------------------------------
//
CCCAContactorUniEditorOperation* CCCAContactorUniEditorOperation::NewL(const TDesC& aParam, const TDesC& aName)
    {
    CCCAContactorUniEditorOperation* self=
            CCCAContactorUniEditorOperation::NewLC(aParam, aName);
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CCCAContactorUniEditorOperation::ConstructL()
// -----------------------------------------------------------------------------
//
void CCCAContactorUniEditorOperation::ConstructL()
    {

    }

// -----------------------------------------------------------------------------
// CCCAContactorUniEditorOperation::ExecuteLD()
// -----------------------------------------------------------------------------
//
void CCCAContactorUniEditorOperation::ExecuteLD()
    {
    CleanupStack::PushL(this);
    CCCAMsgEditors::ExecuteL (KSenduiMtmUniMessageUid,
                iParam, iName);
    CleanupStack::PopAndDestroy(this);
    }
// End of File
