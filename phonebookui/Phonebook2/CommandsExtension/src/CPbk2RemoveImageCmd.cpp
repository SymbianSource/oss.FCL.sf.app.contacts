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
*           Remove image command event handling class.
*
*/


// INCLUDE FILES
#include "CPbk2RemoveImageCmd.h"
#include "CPbk2ImageManager.h"
#include <Pbk2Commands.rsg>

#include <AknQueryDialog.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>

#include <Pbk2Debug.h>


// ================= MEMBER FUNCTIONS =======================

CPbk2RemoveImageCmd::CPbk2RemoveImageCmd
        (MPbk2ContactUiControl& aUiControl) :
    CPbk2ImageCmdBase(aUiControl)
    {
    }

void CPbk2RemoveImageCmd::ConstructL()
    {
    CPbk2ImageCmdBase::BaseConstructL();
    }

CPbk2RemoveImageCmd* CPbk2RemoveImageCmd::NewL
        (MPbk2ContactUiControl& aUiControl)
    {
    CPbk2RemoveImageCmd* self = new (ELeave)
        CPbk2RemoveImageCmd(aUiControl);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbk2RemoveImageCmd::~CPbk2RemoveImageCmd()
    {
    }

TBool CPbk2RemoveImageCmd::ExecuteCommandL()
    {
    if( CAknQueryDialog::NewL()->ExecuteLD(
            R_PHONEBOOK2_REMOVE_IMAGE_CONFIRMATION_DIALOG ) )
        {
        // User has confirmed image removal
        TInt imageIndex = FindFieldIndex( *iImageFieldType );
        if ( imageIndex != KErrNotFound )
            {
            iStoreContact->RemoveField( imageIndex );
            }
        iImageManager->RemoveImage( *iStoreContact, *iThumbnailFieldType );
        }
    iStoreContact->CommitL( *this );
    return ETrue;
    }


//  End of File
