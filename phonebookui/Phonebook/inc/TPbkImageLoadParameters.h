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
*     Generic image loading parameters.
*
*/


#ifndef __TPbkImageLoadParameters_H__
#define __TPbkImageLoadParameters_H__

//  INCLUDES
#include <e32std.h> // TSize
#include <gdi.h>    // TDisplayMode

// CLASS DECLARATIONS

/**
 * Generic image loading parameters.
 */
class TPbkImageLoadParameters
    {
    public:  // Types
        /**
         * Flags.
         */
        enum TFlags
            {
            EFlagsNull = 0,
        
            /**
             * Scale the image after loading to specified size.
             */
            EScaleImage      = 0x0001,

            /**
             * Scale keeping the aspect ratio.
             */
            EKeepAspectRatio = 0x0002,

            /** 
             * Fast scaling of the image close, but not excactly to desired 
             * size (always keeps aspect ratio).
             */
            EUseFastScaling  = 0x0004
            };

    public:  // Interface
        /**
         * Default constructor. Inits all the fields to default values.
         */
        IMPORT_C TPbkImageLoadParameters();

        /**
         * Assorted flags, see TFlags.
         * Default = EFlagsNull
         */
        TUint iFlags;

        /**
         * Desired Size for the loaded bitmap. 
         * Ignored if !(iFlags & EReadScaleImage).
         */
        TSize iSize;

        /**
         * Display mode for the loaded bitmap.
         * Default = EColor16M
         */
        TDisplayMode iDisplayMode;

        /**
         * Number of the frame to read.
         * Default = 0
         */
        TInt iFrameNumber;

    private:  // Data
		/// Own: spare data
        TInt32 iSpare1;
		/// Own: spare data
        TInt32 iSpare2;
    };

#endif  // __TPbkImageLoadParameters_H__
	
// End of File
