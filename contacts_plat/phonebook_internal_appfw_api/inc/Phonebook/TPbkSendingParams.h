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
*       Includes sending parameters
*
*/


#ifndef __TPbkSendingParams_H__
#define __TPbkSendingParams_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <TSendingCapabilities.h>

/**
 * Sending parameter class
 */
class TPbkSendingParams
	{
	public: // Construction and destruction
        /**
         * Standard C++ constructor.
         * @param aMtmFilter, mtm filter
         * @param aCapabilities, sending capabilities
         */
        TPbkSendingParams( CArrayFixFlat<TUid>* aMtmFilter,
					  			 TSendingCapabilities aCapabilities );

        /**
         * Standard C++ destructor.
         */
        ~TPbkSendingParams(){};

    public:  // Data
        /// Ref: mtm filter
        CArrayFixFlat<TUid>* iMtmFilter;
        /// sending capabilities
        TSendingCapabilities iCapabilities;

	};

inline TPbkSendingParams::TPbkSendingParams(CArrayFixFlat<TUid>* aMtmFilter,
					  TSendingCapabilities aCapabilities ):
					  iMtmFilter( aMtmFilter ), iCapabilities( aCapabilities ){}

#endif // __TPbkSendingParams_H__

// End of File
