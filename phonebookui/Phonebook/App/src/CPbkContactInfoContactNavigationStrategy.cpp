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
*       Contact Info View all contacts navigation stategy.
*
*/


// INCLUDE FILES
#include "CPbkContactInfoContactNavigationStrategy.h"  // This class' declaration
#include "CPbkContactEngine.h"
#include <pbkdebug.h>


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactInfoContactNavigationStrategy::CPbkContactInfoContactNavigationStrategy
        (CPbkContactEngine& aEngine,
        MPbkContactInfoNavigationCallback& aCallback) :
    CPbkContactInfoBaseNavigationStrategy(aEngine, aCallback)
    {
    }

inline void CPbkContactInfoContactNavigationStrategy::ConstructL()
    {
    // Register this object as observer to all contacts view
    if (!iScrollView)
        {
        iScrollView = &iEngine.AllContactsView();
        iScrollView->OpenL(*this);
        }
    }

CPbkContactInfoContactNavigationStrategy* CPbkContactInfoContactNavigationStrategy::NewL
        (CPbkContactEngine& aEngine,
        MPbkContactInfoNavigationCallback& aCallback)
    {
    CPbkContactInfoContactNavigationStrategy* self = new(ELeave) CPbkContactInfoContactNavigationStrategy(
            aEngine, aCallback);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

CPbkContactInfoContactNavigationStrategy::~CPbkContactInfoContactNavigationStrategy()
    {
    if (iScrollView)
        {
        iScrollView->Close(*this);
        }
    }

//  End of File  
