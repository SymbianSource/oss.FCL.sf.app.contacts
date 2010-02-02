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
*     Implements basic AIW interest item functionality.
*
*/


// INCLUDE FILES
#include "CPbkAiwInterestItemBase.h"

#include <AiwServiceHandler.h>
#include <AiwCommon.hrh>

// debugging headers
#include <pbkdebug.h>
#include "PbkProfiling.h"

// ================= MEMBER FUNCTIONS =======================

CPbkAiwInterestItemBase::CPbkAiwInterestItemBase
        (CAiwServiceHandler& aServiceHandler) : 
            iServiceHandler(aServiceHandler),
            iAttachedResources(1 /*granularity*/) 
    {
    }

CPbkAiwInterestItemBase::~CPbkAiwInterestItemBase()
    {
    }


void CPbkAiwInterestItemBase::AttachL(TInt aMenuResourceId,
        TInt aInterestResourceId,
        TBool aAttachBaseServiceInterest)
    {
    TBool alreadyAttached = EFalse;
    TBool baseServiceAttached = EFalse;
    TAttachedResources resource;
    TInt index = KErrNotFound;

    TInt count = iAttachedResources.Count();

    // First check has the resource-interest pair already been attached
    for (TInt i=0; i<count; ++i)
        {
        if ((iAttachedResources[i].iMenuResourceId == aMenuResourceId) && 
            (iAttachedResources[i].iInterestResourceId == aInterestResourceId))
            {
            alreadyAttached = ETrue;
            if (iAttachedResources[i].iBaseServiceAttached)
                {
                baseServiceAttached = ETrue;
                }
            // Store the index and break out
            index = i;
            break;
            }
        }

    if (!alreadyAttached)
        {
        __PBK_PROFILE_START(PbkProfiling::EViewAiwAttach);
        // Attach criteria items to menu
        iServiceHandler.AttachMenuL(aMenuResourceId,
            aInterestResourceId);
        __PBK_PROFILE_END(PbkProfiling::EViewAiwAttach);

        resource.iInterestResourceId = aInterestResourceId;
        resource.iMenuResourceId = aMenuResourceId;
        }

    // If wanted, attach also base interest...
    if (aAttachBaseServiceInterest)
        {
        // ...if not already done
        if (!baseServiceAttached)
            {
            __PBK_PROFILE_START(PbkProfiling::EViewAiwAttach);
            iServiceHandler.AttachL(aInterestResourceId);
            __PBK_PROFILE_END(PbkProfiling::EViewAiwAttach);
            resource.iBaseServiceAttached = ETrue;
            }
        }

    if (!alreadyAttached)
        {
        // Append to the array
        iAttachedResources.AppendL(resource);
        }
    else if (!baseServiceAttached && resource.iBaseServiceAttached)
        {
        // We need to modify the base service flag
        iAttachedResources[index].iBaseServiceAttached = ETrue;
        }
    }

//  End of File
