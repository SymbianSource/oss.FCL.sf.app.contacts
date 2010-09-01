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
*       Contact Info View group members navigation strategy factory.
*
*/


// INCLUDE FILES
#include "PbkContactInfoNavigationStrategyFactory.h"  // This class' declaration
#include "CPbkContactInfoGroupMemberNavigationStrategy.h"
#include "CPbkContactInfoContactNavigationStrategy.h"
#include "CPbkContactInfoNoNavigationStrategy.h"
#include <pbkconfig.hrh>
#include <CPbkContactEngine.h>
#include <CPbkConstants.h>



// ================= MEMBER FUNCTIONS =======================

MPbkContactInfoNavigationStrategy* PbkContactInfoNavigationStrategyFactory::CreateL
        (CPbkContactEngine& aEngine,
        MPbkContactInfoNavigationCallback& aCallback,
        TContactItemId aParentContactId)
    {
    MPbkContactInfoNavigationStrategy* ret = NULL;

    // Check is the contact info view navigation feature enabled
    TBool navigationEnabled = aEngine.Constants()->
        LocallyVariatedFeatureEnabled(EPbkLVContactInfoViewNavigation);
    
    if (!navigationEnabled)
        {
        // No navigation
        ret = CPbkContactInfoNoNavigationStrategy::NewL();
        }
    else if (aParentContactId != KNullContactId)
        {
        // Navigate group members
        ret = CPbkContactInfoGroupMemberNavigationStrategy::NewL(
                aEngine, aCallback, aParentContactId);
        }
    else
        {
        // Navigate all contacts
        ret = CPbkContactInfoContactNavigationStrategy::NewL(
                aEngine, aCallback);
        }

    return ret;
    }

//  End of File  
