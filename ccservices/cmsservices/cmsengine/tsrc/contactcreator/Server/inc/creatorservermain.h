/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __CREATORSERVERMAIN_H__
#define __CREATORSERVERMAIN_H__

// INCLUDES
#include <e32std.h>

//FORWARD DECLARATIONs

class CBPAS;

//A global variable that is available
//as long as the server thread is alive
CBPAS*                       BrandedPresenceService = NULL;

#endif   //__CREATORSERVERMAIN_H__

// End of File
