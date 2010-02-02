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

#include <fbs.h>
#include "SpdiaIndexDataVPbk.h"
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>
#include <CPbk2ImageManager.h>

// ================= MEMBER FUNCTIONS =======================

TSpdiaIndexDataVPbk::TOperationInfo::TOperationInfo():iContactOperation(NULL),
								  iOperation(OPERATION_NONE)
								  {
								  	
								  }
// C++ default constructor can NOT contain any code, that
// might leave.
//
TSpdiaIndexDataVPbk::TSpdiaIndexDataVPbk()
        : iIconIndex(-1), iThumbIndex(-1),
          iContactLink(NULL),
          iContact(NULL),
          iImageOperation(NULL),
          iLastThumb(EFalse)
    {
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::OffsetValue
// Return offset address of member data 
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt TSpdiaIndexDataVPbk::OffsetValue(TDataIndex aIndex)
    {
    TInt size(0);
    switch (aIndex)
        {
        default:
        case EIndex:
            size = _FOFF(TSpdiaIndexDataVPbk, iIndex);
            break;
        case ENumber:
            size = _FOFF(TSpdiaIndexDataVPbk, iNumber);
            break;
        case EContact:
            size = _FOFF(TSpdiaIndexDataVPbk, iContact);
            break;
        case EPhoneNumber:
            size = _FOFF(TSpdiaIndexDataVPbk, iPhoneNumber);
            break;
        case ETelNumber:
            size = _FOFF(TSpdiaIndexDataVPbk, iTelNumber);
            break;
 
        case EOperationInfo:
            size = _FOFF(TSpdiaIndexDataVPbk, iOperationInfo);
            break;
        case ELastThumb:
            size = _FOFF(TSpdiaIndexDataVPbk, iLastThumb);
            break;
        }
    return size;
    }

// ---------------------------------------------------------
// TSpdiaIndexDataVPbk::OffsetValue
// 
// ---------------------------------------------------------
//
void TSpdiaIndexDataVPbk::Reset()
{
	if(iContactLink)
	{
		delete iContactLink;
		iContactLink = NULL;
	}
	
	iIconIndex = -1;
	iThumbIndex = -1;
	iLastThumb = EFalse;
	
	if(iContact != NULL)
	{
		delete iContact;
		iContact = NULL;
	}
		
	if(iOperationInfo.iContactOperation)
    {
    	delete iOperationInfo.iContactOperation;
    	iOperationInfo.iContactOperation = NULL;
    }
    
    iOperationInfo.iOperation = OPERATION_NONE;
    
    if(iImageOperation)
    {
    	delete iImageOperation;
    	iImageOperation = NULL;
    }
    	
}

// End of File

