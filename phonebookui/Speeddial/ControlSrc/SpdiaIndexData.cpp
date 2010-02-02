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
* Description:     Declares index data for SpdCtrl.
*
*/




// INCLUDE FILES
#include <s32strm.h>
#include <gulicon.h>

#include <CPbkContactItem.h>        // Phonebook Contact
#include <fbs.h>
#include "SpdiaIndexData.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
TSpdiaIndexData::TSpdiaIndexData()
        : iIconIndex(-1), iThumbIndex(-1),
          iContactItem(NULL), iOperation(NULL), iLastThumb(EFalse)
    {
    }

// ---------------------------------------------------------
// TSpdiaIndexData::OffsetValue
// Return offset address of member data 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TSpdiaIndexData::OffsetValue(TDataIndex aIndex)
    {
    TInt size(0);
    switch (aIndex)
        {
        default:
        case EIndex:
            size = _FOFF(TSpdiaIndexData, iIndex);
            break;
        case ENumber:
            size = _FOFF(TSpdiaIndexData, iNumber);
            break;
        case EContactId:
            size = _FOFF(TSpdiaIndexData, iContactId);
            break;
        case EPhoneNumber:
            size = _FOFF(TSpdiaIndexData, iPhoneNumber);
            break;
        case ETelNumber:
            size = _FOFF(TSpdiaIndexData, iTelNumber);
            break;
        case EContactItem:
            size = _FOFF(TSpdiaIndexData, iContactItem);
            break;
        case EOperation:
            size = _FOFF(TSpdiaIndexData, iOperation);
            break;
        case ELastThumb:
            size = _FOFF(TSpdiaIndexData, iLastThumb);
            break;
        }
    return size;
    }

// End of File

