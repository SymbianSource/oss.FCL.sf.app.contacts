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
* Description:  Phonebook 2 USIM UI Extension View Launch Callback.
*
*/


#ifndef MPSU2VIEWLAUNCHCALLBACK_H
#define MPSU2VIEWLAUNCHCALLBACK_H

// INCLUDES

// FORWARD DECLARATIONS
class MVPbkContactViewBase;


// CLASS DECLARATION

/**
 * Phonebook 2 USIM UI Extension View Launch Callback.
 */
class MPsu2ViewLaunchCallback
    {
    public: // interface
        /**
         * Passes the aContactView reference to interested parties.
         */
        virtual void SetContactViewL(MVPbkContactViewBase& aContactView) = 0;

        /**
         * Handle launch error.
         */
        virtual void HandleError(TInt aError) = 0;

    protected:  // destructor
        ~MPsu2ViewLaunchCallback() { };

    };

#endif // MPSU2VIEWLAUNCHCALLBACK_H

// End of File
