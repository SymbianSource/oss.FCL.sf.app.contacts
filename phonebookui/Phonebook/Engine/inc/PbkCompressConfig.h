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
*     Phonebook database compression configuration.
*
*/


#ifndef PbkCompressConfig_H
#define PbkCompressConfig_H

// INCLUDE FILES
#include <cntdb.h>
#include <PbkDebug.h>


// MACROS
#ifdef PBK_COMPRESS_ALWAYS
    #pragma message("Warning: PBK_COMPRESS_ALWAYS is set.")
#endif

/// Define to turn on compress debug logging
#undef PBK_COMPRESS_LOGGING

#ifdef PBK_COMPRESS_LOGGING
    #pragma message("Warning: PBK_COMPRESS_LOGGING is set.")
    #define PBK_COMPRESS_LOG RDebug::Print
    #define PBK_COMPRESS_STRING(s) _L(s)
#else
    #define PBK_COMPRESS_LOG PBK_DEBUG_PRINT
    #define PBK_COMPRESS_STRING PBK_DEBUG_STRING
#endif


/**
 * Phonebook compression configuration.
 */
namespace PbkCompressConfig
    {

    /**
     * Returns ETrue if aContactDb requires compression.
     */
    inline TBool CompressRequired(CContactDatabase& aContactDb)
        {
        #ifdef PBK_COMPRESS_ALWAYS
            const TBool result = ETrue;
        #else
            const TBool result = aContactDb.CompressRequired();
        #endif

        if (result)
            {
            PBK_COMPRESS_LOG(PBK_COMPRESS_STRING("PbkCompressConfig::CompressRequired() returns ETrue"));
            }
        return result;
        }

    }


#endif  // PbkCompressConfig_H

// End of File
