/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Helper class. 
*
*/


#ifndef C_CCACLIENTUTILS_H
#define C_CCACLIENTUTILS_H

#include <e32base.h>
#include <bamdesca.h>
#include <barsc.h>
#include <bautils.h>

/**
 *  Helper class. Contains functions for example performance measurements.
 *
 *  @lib ccaapp.exe
 *  @since S60 v5.0
 */ 
class CCAClientUtils : public CBase
	{

public:

	/**
	 * Marks timestamp to performance logs.
     * @since S60 v5.0
	 */
	IMPORT_C static void MarkPerformanceTimeStampL( );

	/**
	 * Marks string to performance logs.
	 *
     * @since S60 v5.0
     * @param aString     Text to write to file. 
	 */
	IMPORT_C static void MarkPerformanceLogStringL( TPtrC aString );
	
	};
	
#endif // C_CCACLIENTUTILS_H

// End of file
