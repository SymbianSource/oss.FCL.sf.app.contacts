/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common constant etc for store implementation
*
*/



#ifndef VPBKSIMSTOREIMPL_SIMSTORECOMMON_H
#define VPBKSIMSTOREIMPL_SIMSTORECOMMON_H

//  INCLUDES
#include <e32std.h>

// CONSTANTS

// Using this index the contact is saved to the first free index
// defined in MM ETel specification
const TInt KVPbkSimStoreFirstFreeIndex = -1;
// 0 is not a valid sim index
const TInt KVPbkSimStoreInvalidETelIndex = 0;
// ETel sim indexes start from 1
const TInt KVPbkSimStoreFirstETelIndex = 1;

// DATA TYPES

/**
* The identifier for stores
*/
enum TVPbkSimStoreIdentifier
    {
    EVPbkSimAdnGlobalStore  = 1,
    EVPbkSimFdnGlobalStore  = 2,
    EVPbkSimSdnGlobalStore  = 3,
    EVPbkSimONStore         = 4
    };

/**
* Parameters for the view
*/ 
enum TVPbkSimViewConstructionPolicy
    {
    EVPbkSortedSimView,
    EVPbkUnsortedSimView    
    };

#endif      // VPBKSIMSTOREIMPL_SIMSTORECOMMON_H

// End of File
