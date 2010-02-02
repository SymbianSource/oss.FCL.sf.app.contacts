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
* Description:  Fast digit string handler class
*
*/


#ifndef RLOCALIZEDRESOURCEFILE_H
#define RLOCALIZEDRESOURCEFILE_H

#include <barsc.h>

namespace VPbkEngUtils {

class RLocalizedResourceFile : public RResourceFile
    {
    public: // Interface
        /**
         * Finds the localized version of the resource file aName and leaves the file
         * to cleanup stack for closing using CleanupStack::PopAndDestroy.
         * @param aFs File server session that will be used in opening.
		 * @param aDrive Drive letter for the resource file, must also include the colon.
         *               For example "z:".
		 * @param aPath Path to the file name
         * @param aFileName Resource file name.
         * @see BaflUtils::NearestLanguageFile
         */
	    IMPORT_C void OpenLC( RFs& aFs, 
	                          const TDesC& aDrive, 
	                          const TDesC& aPath, 
	                          const TDesC& aFileName );

        /**
         * Finds the localized version of the resource file aName.
         * @param aFs File server session that will be used in opening.
		 * @param aDrive Drive letter for the resource file, must also include the colon.
         *               For example "z:".
		 * @param aPath Path to the file name
         * @param aFileName Resource file name.
         * @see BaflUtils::NearestLanguageFile
         */
	    IMPORT_C void OpenL( RFs& aFs, 
	                         const TDesC& aDrive, 
	                         const TDesC& aPath, 
	                         const TDesC& aFileName );
	};

} // namespace VPbkEngUtils

#endif // RLOCALIZEDRESOURCEFILE_H
//End of file


