/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Identifies a contact in cache pools
*
*/

#ifndef T_CACHE_INFO_ALGORITHM2
#define T_CACHE_INFO_ALGORITHM2

// INCLUDES
#include <e32base.h>
#include <e32hashtab.h>

class CPsData;

// CLASS DECLARATION
class TIndexInfo
{
	public:
	    TUint8  iPool;
	    TUint16 iIndex;
};

class TCacheInfo
{     
	public:	
		  
	    /**
	     * To store pool id and pool array index 
	     */
	    TSglQue<TIndexInfo> poolMap; 	  	    
};

#endif // T_CACHE_INFO_ALGORITHM2
