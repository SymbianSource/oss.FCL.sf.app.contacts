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
* Description:  VirtualPhonebook profiling support.
*
*/


#ifndef VPBKPROFILE_H
#define VPBKPROFILE_H

#include <e32def.h>
#include <e32std.h>
#include "VPbkConfig.hrh"

/**
 * VirtualPhonebook profiling implementation.
 *
 */
#ifdef VPBK_ENABLE_PROFILE

    /* profiling implementation */
    #define VPBK_PROFILE_START(aBin) \
        VPbkProfile::TVPbkProfiling::GlobalL()->StartProfile(aBin);

    #define VPBK_PROFILE_END(aBin) \
        VPbkProfile::TVPbkProfiling::GlobalL()->EndProfile(aBin);

    /// Needs VPBK_ENABLE_DEBUG_LOGGER
    #define VPBK_PROFILE_DISPLAY(aBin)
    #define VPBK_PROFILE_RESET(aBin)
    #define VPBK_PROFILE_DELETE() \
        delete VPbkProfile::TVPbkProfiling::GlobalL()

#else // VPBK_ENABLE_PROFILE

    /* Profiling not enabled - empty implementation */
    #define VPBK_PROFILE_START(aBin)
    #define VPBK_PROFILE_END(aBin)
    #define VPBK_PROFILE_DISPLAY(aBin)
    #define VPBK_PROFILE_RESET(aBin)
    #define VPBK_PROFILE_DELETE()

#endif // VPBK_ENABLE_PROFILE


namespace VPbkProfile {

    // CONSTANTS
    enum TVPbkProfileBin
        {       
        /////////////////////////////////////////////////////////////////////
        // Composite view's mapping
        /////////////////////////////////////////////////////////////////////
        ECompositeContactViewMapping = 1,        
        
        EOrderedCompositeViewMapping,// = 2,
                                
        ESortedCompositeViewMapping,// = 3        
        
        EFilteredSortedCompositeViewMapping, // = 4     
        
        /////////////////////////////////////////////////////////////////////
        // Finds
        /////////////////////////////////////////////////////////////////////
        ECntModelFind,// = 5
        ECntModelRefine,// = 6
        ESimStoreFind,// = 7
        ESimStoreRefine,// = 8
                
        EEndProfileBins = 9
        };


/**
 * Utility class for VirtualPhonebook profiling.
 */
class TVPbkProfiling
    {
    public: // Interface

        /**
         * Returns the global shared object of this class.
         * Creates the object if it does not exist.
         *
         * @return  Instance of this class.
         */
        IMPORT_C static TVPbkProfiling* GlobalL();

        /**
         * Destructor.
         */
        IMPORT_C ~TVPbkProfiling();

        /**
         * Starts profiling.
         *
         * @param aBin  Integer identifying the profile.
         */
        IMPORT_C void StartProfile(
                TInt aBin );

        /**
         * Ends profiling.
         *
         * @param aBin  Integer identifying the profile.
         */
        IMPORT_C void EndProfile(
                TInt aBin ) const;

    private: // Implementation
        TVPbkProfiling();

    private:
        /// Own: Flat C array
        TTime iProfileArray[EEndProfileBins];
    };

}  // namespace VPbkProfile

#endif // VPBKPROFILE_H

// End of File
