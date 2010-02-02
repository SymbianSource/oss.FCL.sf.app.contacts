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
*       Provides methods for a wait note wrapper.
*
*/


// INCLUDE FILES
#include <CPbkProgressNoteWrapper.h>
#include <AknWaitDialog.h>


// ================= MEMBER FUNCTIONS =======================

inline CPbkProgressNoteWrapper::CPbkProgressNoteWrapper()
    {
    }

EXPORT_C CPbkProgressNoteWrapper* CPbkProgressNoteWrapper::NewL()
    {
    CPbkProgressNoteWrapper* self = new(ELeave) CPbkProgressNoteWrapper;
    return self;
    }

CPbkProgressNoteWrapper::~CPbkProgressNoteWrapper()
    {
    }

CAknProgressDialog* CPbkProgressNoteWrapper::CreateDialogL
        (CEikDialog** aSelfPtr, TBool aVisibilityDelayOff)
    {
    return (new(ELeave) CAknProgressDialog(aSelfPtr,aVisibilityDelayOff));
    }

TBool CPbkProgressNoteWrapper::ShowProgressInfo() const
    {
    // Show progress indicator
    return ETrue;
    }


//  End of File  
