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
*      Interface for Phonebook database compression strategy.
*
*/


#ifndef __MPbkCompressionStrategy_H__
#define __MPbkCompressionStrategy_H__

// INCLUDES
#include <e32def.h>


// FORWARD DECLARATIONS
class MPbkCompressUi;


// CLASS DECLARATION

/**
 * Abstract interface for Phonebook database compression strategy.
 */
class MPbkCompressionStrategy
    {
    public: // Interface
        /**
         * Sets the compress UI.
		 * @param aComressUi the compress UI
         */
        virtual void SetCompressUi(MPbkCompressUi* aCompressUi) =0;

        /**
         * Returns ETrue if the database could use a compression.
         */
        virtual TBool CheckCompress() =0;

        /**
         * Returns true if compression is currently enabled.
         */
        virtual TBool IsCompressionEnabled() =0;

        /**
         * Compresses the database asynchronously. Implementation can decide
         * whether to start compression or not. Calls back the compress UI on 
         * the compression progress.
         */
        virtual void CompressL() =0;

        /**
         * Stops any compression currently executing.
         */
        virtual void CancelCompress() =0;

        /**
         * Releases this object and any resources it owns.
         */
        virtual void Release() =0;

    protected:
        /**
         * Protected destructor. Clients of this interface should call
         * Release() when they no longer need it.
         */
        virtual ~MPbkCompressionStrategy() { }
    };


#endif // __MPbkCompressionStrategy_H__

// End of File
