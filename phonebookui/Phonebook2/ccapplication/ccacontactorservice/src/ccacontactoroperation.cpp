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
* Description:  Implementation of the base class of contactor operations
*
*/


// INCLUDE FILES
#include "ccacontactorheaders.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCCAContactorOperation::CCCAContactorOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorOperation::CCCAContactorOperation(const TDesC& aParam, const TDesC& aName) : iParam(aParam), iName(aName)
    {
    }

// -----------------------------------------------------------------------------
// CCCAContactorOperation::~CCCAContactorOperation()
// -----------------------------------------------------------------------------
//
CCCAContactorOperation::~CCCAContactorOperation()
    {
    // No implementation required
    }
// End of File
