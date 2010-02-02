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
*        Interface for ui controls, which handles contacts.
*
*/


#ifndef __MPbkContactUiControlUpdate_H__
#define __MPbkContactUiControlUpdate_H__


//  INCLUDES
#include <cntdef.h>     // TContactItemId


// CLASS DECLARATION

/**
 * Interface for ui controls, which handles contacts.
 */
class MPbkContactUiControlUpdate
    {
    protected: // Destructor
        virtual ~MPbkContactUiControlUpdate() {}

    public: // Interface
        /**
         * Updates contact information.
         *
         * @param aContactId contact to be updated
         */
        virtual void UpdateContact(TContactItemId aContactId) = 0;
    };


#endif // __MPbkContactUiControlUpdate_H__

// End of File
