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
*       Contact Info View no navigation stategy.
*
*/


// INCLUDE FILES
#include "CPbkContactInfoNoNavigationStrategy.h"  // This class' declaration
#include <cntdef.h>


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactInfoNoNavigationStrategy::CPbkContactInfoNoNavigationStrategy()
    {
    }

CPbkContactInfoNoNavigationStrategy* CPbkContactInfoNoNavigationStrategy::NewL()
    {
    CPbkContactInfoNoNavigationStrategy* self = new(ELeave) CPbkContactInfoNoNavigationStrategy();
    return self;
    }

CPbkContactInfoNoNavigationStrategy::~CPbkContactInfoNoNavigationStrategy()
    {
    }

TBool CPbkContactInfoNoNavigationStrategy::HandleCommandKeyL
        (const TKeyEvent& /*aKeyEvent*/, 
        TEventCode /*aType*/)
    {
    return EFalse;
    }

//  End of File  
