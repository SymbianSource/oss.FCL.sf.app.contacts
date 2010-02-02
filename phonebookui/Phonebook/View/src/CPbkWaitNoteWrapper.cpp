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
*       Provides methods for the phonebook wait note wrapper.
*
*/


// INCLUDE FILES
#include <CPbkWaitNoteWrapper.h>
#include <AknWaitDialog.h>


// ================= MEMBER FUNCTIONS =======================

inline CPbkWaitNoteWrapper::CPbkWaitNoteWrapper()
    {
    }

EXPORT_C CPbkWaitNoteWrapper* CPbkWaitNoteWrapper::NewL()
    {
    CPbkWaitNoteWrapper* self = new(ELeave) CPbkWaitNoteWrapper;
    return self;
    }

CPbkWaitNoteWrapper::~CPbkWaitNoteWrapper()
    {
    }

CAknProgressDialog* CPbkWaitNoteWrapper::CreateDialogL
        (CEikDialog** aSelfPtr, TBool aVisibilityDelayOff)
    {
    return (new(ELeave) CAknWaitDialog(aSelfPtr,aVisibilityDelayOff));
    }

TBool CPbkWaitNoteWrapper::ShowProgressInfo() const
    {
    // Wait note doesn't have a progress indicator
    return EFalse;
    }


//  End of File  
