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
* Description:  Offers different sim status information
*
*/


// INCLUDE FILES

#include "CSimStateInformation.h"   // This class's declaration

// From Virtual Phonebook
#include <CVPbkSimStateInformation.h>

// System includes

namespace VPbkSimStoreService {
    

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSimStateInformation::NewL
// -----------------------------------------------------------------------------
//
CSimStateInformation* CSimStateInformation::NewL()
    {
    CSimStateInformation* self = new(ELeave) CSimStateInformation;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSimStateInformation::ConstructL
// -----------------------------------------------------------------------------
//
void CSimStateInformation::ConstructL()
    {
    iSimStateInfo = CVPbkSimStateInformation::NewL();
    }

// -----------------------------------------------------------------------------
// CSimStateInformation::~CSimStateInformation
// -----------------------------------------------------------------------------
//
CSimStateInformation::~CSimStateInformation()
    {
    delete iSimStateInfo;
    }

// -----------------------------------------------------------------------------
// CSimStateInformation::SimInserted
// -----------------------------------------------------------------------------
//
TBool CSimStateInformation::SimInsertedL()
    {
    return iSimStateInfo->SimInsertedL();
    }

// -----------------------------------------------------------------------------
// CSimStateInformation::BTSapEnabled
// -----------------------------------------------------------------------------
//
TBool CSimStateInformation::BTSapEnabledL()
    {
    return iSimStateInfo->BTSapEnabledL();
    }

// -----------------------------------------------------------------------------
// CSimStateInformation::ActiveFdnBlocksAdnStoreL
// -----------------------------------------------------------------------------
//
TBool CSimStateInformation::ActiveFdnBlocksAdnStoreL(
        MVPbkSimPhone& aSimPhone)
    {
    return iSimStateInfo->ActiveFdnBlocksAdnStoreL(aSimPhone);
    }

// -----------------------------------------------------------------------------
// CSimStateInformation::NewSimCardL
// -----------------------------------------------------------------------------
//
TBool CSimStateInformation::NewSimCardL()
    {
    return iSimStateInfo->NewSimCardL();
    }    
} // namespace VPbkSimStoreService
//  End of File  
