/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 exit callback interface.
*
*/


#ifndef MPBK2EXITCALLBACK_H
#define MPBK2EXITCALLBACK_H

// CLASS DECLARATION

/**
 * Phonebook 2 exit callback interface.
 */
class MPbk2ExitCallback
    {
    public: // Interface

        /**
         * Asks a permission to exit.
         *
         * @param aCommandId    Exit command id.
         * @return  ETrue to exit, EFalse to not to exit.
         */
        virtual TBool OkToExitL(
                TInt aCommandId ) = 0;
    };

#endif // MPBK2EXITCALLBACK_H

// End of File
