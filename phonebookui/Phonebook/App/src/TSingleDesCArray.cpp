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
*           Provides string array class methods.
*
*/


// INCLUDE FILES
#include    "TSingleDesCArray.h"
#include    <pbkdebug.h>

// ================= MEMBER FUNCTIONS =======================

TInt TSingleDesCArray::MdcaCount() const
    {
    return iString ? 1 : 0;
    }

TPtrC TSingleDesCArray::MdcaPoint(TInt PBK_DEBUG_ONLY(aIndex)) const
    {
    __ASSERT_DEBUG(aIndex==0 && iString, Panic(EPanicPreCond_MdcaPoint));
    return *iString;
    }

#ifdef _DEBUG
void TSingleDesCArray::Panic(TInt aReason)
    {
    _LIT(KPanicText, "TSingleDesCArray");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

//  End of File  
