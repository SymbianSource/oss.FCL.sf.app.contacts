/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A factory for creating a SIM phone
*
*/



// INCLUDE FILES
#include <VPbkSimStoreFactory.h>

#include "CVPbkSimPhone.h"
#include "CSimStateInformation.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// VPbkSimStoreFactory::Phone
// -----------------------------------------------------------------------------
//
EXPORT_C MVPbkSimPhone* VPbkSimStoreFactory::CreatePhoneL()
    {
    return CVPbkSimPhone::NewL();
    }

// -----------------------------------------------------------------------------
// VPbkSimStoreFactory::GetSimStateInformationL
// -----------------------------------------------------------------------------
//
EXPORT_C MVPbkSimStateInformation* VPbkSimStoreFactory::GetSimStateInformationL()
    {
    return VPbkSimStoreService::CSimStateInformation::NewL();
    }

//  End of File
