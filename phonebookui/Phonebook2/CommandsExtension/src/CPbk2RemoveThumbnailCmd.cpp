/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*           Remove thumbnail command event handling class.
*
*/


// INCLUDE FILES
#include "CPbk2RemoveThumbnailCmd.h"

#include "CPbk2ImageManager.h"
#include <MVPbkStoreContact.h>

#include <Pbk2Debug.h>


// ================= MEMBER FUNCTIONS =======================

CPbk2RemoveThumbnailCmd::CPbk2RemoveThumbnailCmd
        (MPbk2ContactUiControl& aUiControl) :
	CPbk2ThumbnailCmdBase(aUiControl)
    {
    }

void CPbk2RemoveThumbnailCmd::ConstructL()
    {
    CPbk2ThumbnailCmdBase::BaseConstructL();
    }

CPbk2RemoveThumbnailCmd* CPbk2RemoveThumbnailCmd::NewL
        (MPbk2ContactUiControl& aUiControl)
    {
    CPbk2RemoveThumbnailCmd* self = new (ELeave) 
        CPbk2RemoveThumbnailCmd(aUiControl);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
CPbk2RemoveThumbnailCmd::~CPbk2RemoveThumbnailCmd()
    {
    }

TBool CPbk2RemoveThumbnailCmd::ExecuteCommandL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2RemoveThumbnailCmd::ExecuteCommandL(0x%x), start"), this);
    
    iThumbnailManager->RemoveImage(*iStoreContact, *iFieldType);
    iStoreContact->CommitL(*this);	

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2RemoveThumbnailCmd::ExecuteCommandL(0x%x), end"), this);
    
    return ETrue; 
    }
    

//  End of File  
