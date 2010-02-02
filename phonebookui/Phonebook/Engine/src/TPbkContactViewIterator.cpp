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
*		Safe CContactViewBase iterator
*
*/


// INCLUDE FILES
#include "TPbkContactViewIterator.h"
#include <cntviewbase.h>


// ================= MEMBER FUNCTIONS =======================

EXPORT_C TContactItemId TPbkContactViewIterator::NextL()
    {
    TContactItemId result = KNullContactId;
    TRAPD(err, result = iContactView.AtL(++iIndex));
    switch (err)
        {
        case KErrNone:
            {
            break;
            }
        case KErrNotFound:
            {
            // at the end of iteration
            break;
            }
        default:
            {
            // Some other error than out of bounds
            User::Leave(err);
            break;
            }
        }
    return result;
    }


//  End of File  
