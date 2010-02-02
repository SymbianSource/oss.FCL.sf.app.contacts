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
*    Interface for providing an UI for Phonebook database compression.
*
*/



#ifndef __PbkCompressUi_H__
#define __PbkCompressUi_H__

//  INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Interface for a Phonebook database compression UI.
 */
class MPbkCompressUi
    {
    public:
        /**
         * Called by the engine to check if compression can be started
         *
         * @return ETrue if compression can be started EFalse otherwise.
         */
        virtual TBool PbkCompressCanStartNow() =0;

        /**
         * Called when the engine decides to start a compression. 
         *
         * @param aStepsTogo    amount of steps the compression will take.
         *                      This can be used to set up a progress bar.
         * @exception   any leave occuring when setting up the UI. If this 
         *              function leaves, the engine doesn't start the
         *              compression.
         */
        virtual void PbkCompressStartL(TInt aStepsTogo) =0;

        /**
         * Called as the compression proceeds.
         *
         * @param aStep number of current step, counting down.
         */
        virtual void PbkCompressStep(TInt aStep) =0;

        /**
         * Called when the compression completes succesfully.
         */
        virtual void PbkCompressComplete() =0;

        /**
         * Called by the engine if the compression is cancelled.
         */
        virtual void PbkCompressCanceled() =0;

        /**
         * Called if there is an error during compression.
         *
         * @param aError    error code.
         */
        virtual void PbkCompressError(TInt aError) =0;
    };


#endif // __PbkCompressUi_H__

// End of File
