/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __TNEWVIEWIDINFO_H__
#define __TNEWVIEWIDINFO_H__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

/**
 * This class represents a single view ID
 */
class TNewViewIdInfo
	{
	public:		
	
		/**
		 * Constructor
		 *
		 * @param aId The new ID for this class instance
		 */
		TNewViewIdInfo( TInt aId ) : iId( aId ), iFree( ETrue ) {}
					
	public:
		/// view id					
		TInt iId;
		
		/// Flag whether this id is available
		TBool iFree;		
	};
        

#endif // __TNEWVIEWIDINFO_H__

// End of File
