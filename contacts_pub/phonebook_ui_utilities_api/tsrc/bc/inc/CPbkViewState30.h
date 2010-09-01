/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*     View states.
*
*/








#ifndef __CPbkViewState30_H__
#define __CPbkViewState30_H__

//CPbkViewState enums as it is in 3.0 SDK
//for verifying against enums in CPbkViewState.h

        enum TDataType30
            {
            EEnd = 0,
            EFocusedContactId,
            ETopContactId,
            EMarkedContactIds,
            EFocusedFieldIndex,
            ETopFieldIndex,
            EFieldDataArray,
            EParentContactId,
            EFlags
            };

        enum TFlags30
            {
            /// Reset flags
            ENullFlags = 0,
            /// Focus the first item in list views
            EFocusFirst  = 0x0001,
            /// Focus the last item in list views
            EFocusLast   = 0x0002,
            /// Reset state to the view's initial state
            EInitialized = 0x0004,
            /// Send application to background
            ESendToBackground = 0x0008
            };


#endif // __CPbkViewState_H__

// End of File
