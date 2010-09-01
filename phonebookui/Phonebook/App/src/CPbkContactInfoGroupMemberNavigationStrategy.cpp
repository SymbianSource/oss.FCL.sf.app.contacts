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
*       Contact Info View group members navigation stategy.
*
*/


// INCLUDE FILES
#include "CPbkContactInfoGroupMemberNavigationStrategy.h"  // This class' declaration
#include <cntview.h>
#include "CPbkContactEngine.h"
#include <pbkdebug.h>


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactInfoGroupMemberNavigationStrategy::CPbkContactInfoGroupMemberNavigationStrategy
        (CPbkContactEngine& aEngine,
        MPbkContactInfoNavigationCallback& aCallback) :
    CPbkContactInfoBaseNavigationStrategy(aEngine, aCallback)
    {
    }

inline void CPbkContactInfoGroupMemberNavigationStrategy::ConstructL
        (TContactItemId aGroupId)
    {
    iScrollView = CContactGroupView::NewL(
        iEngine.Database(), iEngine.AllContactsView(),
        *this, aGroupId, CContactGroupView::EShowContactsInGroup);
    }

CPbkContactInfoGroupMemberNavigationStrategy* CPbkContactInfoGroupMemberNavigationStrategy::NewL
        (CPbkContactEngine& aEngine,
        MPbkContactInfoNavigationCallback& aCallback,
        TContactItemId aGroupId)
    {
    CPbkContactInfoGroupMemberNavigationStrategy* self = new(ELeave) CPbkContactInfoGroupMemberNavigationStrategy(
            aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL(aGroupId);
    CleanupStack::Pop();  // self
    return self;
    }

CPbkContactInfoGroupMemberNavigationStrategy::~CPbkContactInfoGroupMemberNavigationStrategy()
    {
    if (iScrollView)
        {
        iScrollView->Close(*this);
        }
    }

//  End of File  
