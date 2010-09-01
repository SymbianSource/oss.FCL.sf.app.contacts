/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is a class for starting services
 *                provided by CCA contactor service
 *
*/

#include <e32std.h>

#include "ccacontactorserviceheaders.h"

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CCAContactorService::ExecuteServiceL
// --------------------------------------------------------------------------
//
EXPORT_C CCAContactorService* CCAContactorService::NewL()
    {
    CCAContactorService* self = new (ELeave) CCAContactorService();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// Destructor
// --------------------------------------------------------------------------
//
CCAContactorService::~CCAContactorService()
    {
    delete iOperator;
    }

// --------------------------------------------------------------------------
// Constructor
// --------------------------------------------------------------------------
//
CCAContactorService::CCAContactorService()
    {
    }

// --------------------------------------------------------------------------
// CCAContactorService::ConstructL
// --------------------------------------------------------------------------
//
void CCAContactorService::ConstructL()
    {
    iOperator = CCCAContactorServiceOperator::NewL();
    }

// --------------------------------------------------------------------------
// CCAContactorService::ExecuteServiceL
// --------------------------------------------------------------------------
//
EXPORT_C void CCAContactorService::ExecuteServiceL(
    const TCSParameter& aParameter)
    {
    CCA_DP (KCCAContactorServiceLoggerFile, CCA_L ("CCAContactorService::ExecuteServiceL()"));

    iCommandIsBeingHandled = ETrue;

    // All leaves are trapped in op class.
    iOperator->Execute(aParameter);

    iCommandIsBeingHandled = EFalse;

    CCA_DP (KCCAContactorServiceLoggerFile, CCA_L ("CCAContactorService::ExecuteServiceL(): Done."));
    }

// --------------------------------------------------------------------------
// CCAContactorService::IsBusy
// --------------------------------------------------------------------------
//
EXPORT_C TBool CCAContactorService::IsBusy() const
    {
    return iCommandIsBeingHandled;
    }

// --------------------------------------------------------------------------
// CCAContactorService::IsSelected
// --------------------------------------------------------------------------
//
EXPORT_C TBool CCAContactorService::IsSelected() const
    {
    return iOperator->IsSelected();
    }
// End of file
