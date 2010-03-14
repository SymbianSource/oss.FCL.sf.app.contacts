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
* Description:  Phonebook 2 image manager parameters.
*
*/


#ifndef TPBK2IMAGEMANAGERPARAMS_H
#define TPBK2IMAGEMANAGERPARAMS_H

#include <e32std.h> // TSize
#include <gdi.h>    // TDisplayMode

/**
 * Phonebook 2 image manager parameters.
 */
 class TPbk2ImageManagerParams
    {
    public: // Types

        /// Flags
        enum TFlags
            {
            EFlagsNull = 0,

            /// Scale the image after loading to specified size.
            EScaleImage      = 0x0001,

            /// Scale keeping the aspect ratio.
            EKeepAspectRatio = 0x0002,

            /// Fast scaling of the image close, but not excactly to desired
            /// size (always keeps aspect ratio).
            EUseFastScaling  = 0x0004,

            /// Crop landscape image to square, before thumbnail is scaled.
            ECropImage       = 0x0008,

            /// Speed optimized algorithm is used for scaling. @See AknIconUtils::ScaleBitmapL
            EUseSpeedOptimizedScaling  = 0x0010			
            };

    public: // Interface

        /**
         * Default constructor.
         */
        IMPORT_C TPbk2ImageManagerParams();

        /**
         * Desired size for the bitmap.
         */
        TSize iSize;

        /**
         * Assorted flags, @see TFlags.
         * Default = EFlagsNull.
         */
        TUint iFlags;

        /**
         * Display mode for the loaded bitmap.
         * Default = EColor16M.
         */
        TDisplayMode iDisplayMode;

        /**
         * Number of the frame to read.
         * Default = 0.
         */
        TInt iFrameNumber;

    private: // Data
        /// Own: Spare data
        TInt32 iSpare1;
        /// Own: Spare data
        TInt32 iSpare2;
    };

#endif // TPBK2IMAGEMANAGERPARAMS_H

// End of File
