/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook Util
*
*/


// INCLUDES
#include "VPbkUtil.h"

const TInt KGranularity( 4 );

namespace VPbkEngUtils
    {

    EXPORT_C CTextStore::CTextStore() 
        : iTextArray( KGranularity )
        {
        }

    EXPORT_C TPtrC8 CTextStore::AddL(const TDesC8& aText)
        {
        // Store in order and keep only one copy of duplicates
        TInt index;
        if (iTextArray.FindIsq(aText, index))
            {
            iTextArray.InsertL(index, aText);
            }
        // Return a pointer to the text stored in the array
        return iTextArray[index];
        }   
    }

