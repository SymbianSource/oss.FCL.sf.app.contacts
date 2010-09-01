/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


// INCLUDE FILES
#include "CPbkAiwInterestItemFactory.h"
#include "CPbkAiwInterestItemCall.h"
#include "CPbkAiwInterestItemPoc.h"
#include "CPbkAiwInterestItemSyncMl.h"

#include <AiwCommon.hrh>


// ================= MEMBER FUNCTIONS =======================
CPbkAiwInterestItemFactory::CPbkAiwInterestItemFactory()
    {
    }

CPbkAiwInterestItemFactory* CPbkAiwInterestItemFactory::NewL()
    {
    return new (ELeave) CPbkAiwInterestItemFactory;
    }

CPbkAiwInterestItemFactory::~CPbkAiwInterestItemFactory()
    {
    delete iCallInterest;
    delete iPocInterest;
    delete iSyncMlInterest;
    }

MPbkAiwInterestItem* CPbkAiwInterestItemFactory::CreateInterestItemL(
        TInt aInterestId,
        CAiwServiceHandler& aServiceHandler,
        CPbkContactEngine& aEngine)
    {
    // Create corresponding interest item, if not exist already
    switch (aInterestId)
        {
        case KAiwCmdCall:
            {
            if (!iCallInterest)
                {
                iCallInterest =
                    CPbkAiwInterestItemCall::NewL(aServiceHandler, aEngine);
                }
            return iCallInterest;
            }
        case KAiwCmdPoC:
            {
            if (!iPocInterest)
                {
                iPocInterest =
                    CPbkAiwInterestItemPoc::NewL(aServiceHandler, aEngine);
                }
            return iPocInterest;
            }
        case KAiwCmdSynchronize:
            {
            if (!iSyncMlInterest)
                {
                iSyncMlInterest =
                    CPbkAiwInterestItemSyncMl::NewL(aServiceHandler, aEngine);
                }
            return iSyncMlInterest;
            }
        default:
            {
            return NULL;
            }
        }
    }

// End of File
