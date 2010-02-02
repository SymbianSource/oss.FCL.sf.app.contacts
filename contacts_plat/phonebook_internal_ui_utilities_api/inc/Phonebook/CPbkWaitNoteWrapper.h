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
*     Declares a wait note wrapper.
*
*/


#ifndef __CPbkWaitNoteWrapper_H__
#define __CPbkWaitNoteWrapper_H__

// INCLUDES
#include <CPbkWaitNoteWrapperBase.h>


// CLASS DECLARATION

/**
 * Runs a MPbkBackgroundProcess with a wait note on display.
 */
class CPbkWaitNoteWrapper : 
    public CPbkWaitNoteWrapperBase
    {
    public: // Constructor and destructor
        /**
         * Creates a new instance of this class.
         */
        IMPORT_C static CPbkWaitNoteWrapper* NewL();
        
        /**
         * Destructor.
         */
        ~CPbkWaitNoteWrapper();

    private:  // Implementation
        CPbkWaitNoteWrapper();

    private:  // from CPbkWaitNoteWrapperBase
        CAknProgressDialog* CreateDialogL
            (CEikDialog** aSelfPtr, TBool aVisibilityDelayOff);
        TBool ShowProgressInfo() const;
    };

#endif // __CPbkWaitNoteWrapper_H__

// End of File
