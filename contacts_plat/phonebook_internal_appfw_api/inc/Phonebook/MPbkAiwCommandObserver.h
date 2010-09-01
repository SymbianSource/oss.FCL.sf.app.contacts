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
*       Abstract Phonebook AIW command observer interface.
*
*/


#ifndef __MPbkAiwCommandObserver_H__
#define __MPbkAiwCommandObserver_H__

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Abstract Phonebook AIW command observer interface.
 */
class MPbkAiwCommandObserver
    {
    public:  // Interface

        /**
         * Command processed without problems callback.
         * @param aMenuCommandId menu command id used to launch the command
         * @param aServiceCommandId service command id used to
         *        launch the command
         * @param aErrorCode error code
         */
        virtual void AiwCommandHandledL(TInt aMenuCommandId,
            TInt aServiceCommandId,
            TInt aErrorCode) = 0;
    };

#endif // __MPbkAiwCommandObserver_H__
            
// End of File
