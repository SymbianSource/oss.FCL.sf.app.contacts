/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef COMPRESSCONFIG_H
#define COMPRESSCONFIG_H

// INCLUDE FILES
#include <cntdb.h>
#include <vpbkdebug.h>

namespace VPbkCntModel {
	
// MACROS
#ifdef PBK2_COMPRESS_ALWAYS
    #pragma message("Warning: PBK_COMPRESS_ALWAYS is set.")
#endif

/**
 * Phonebook compression configuration.
 */
namespace CompressConfig
    {

    /**
     * Returns ETrue if aContactDb requires compression.
     */
    inline TBool CompressRequired(CContactDatabase& aContactDb)
        {
        #ifdef PBK2_COMPRESS_ALWAYS
            const TBool result = ETrue;
        #else
            const TBool result = aContactDb.CompressRequired();
        #endif

        if (result)
            {
            VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING("CompressConfig::CompressRequired() returns ETrue"));
            }
        return result;
        }

    } // namespace CompressConfig
} // namespace VPbkCntModel

#endif  // COMPRESSCONFIG_H

// End of File
