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
*       xSP View definitions.
*
*/


#ifndef __XSPVIEWSERVICES_PRIV_H__
#define __XSPVIEWSERVICES_PRIV_H__

// INCLUDES
#include <e32def.h>

#include <Pbk2UID.h>
#include <CPbk2ViewState.h>

// CONSTANTS
const TInt KxSPViewServerMajor = 1;
const TInt KxSPViewServerMinor = 0;
const TInt KxSPViewServerBuild = 0;

const TInt KTimeoutActivate = 10000000;

_LIT(KxSPServer, "xSPSvr");

enum TxSPServerMessage
	{
	EActivateView1,
	EActivateView2,
    EActivateView2Async,
    ECancelActivateView2Async,
    EGetViewCount,
    EGetViewCountAsync,
    ECancelGetViewCountAsync,
    EGetViewDataPackLength,
    EGetViewDataPackLengthAsync,
    ECancelGetViewDataPackLengthAsync,
    EGetViewData,
    EGetViewDataAsync,
    ECancelGetViewDataAsync
	};

const TInt KUid = KPbk2UID3;
_LIT(KPhonebookExe, "Phonebook2");

#endif // __XSPVIEWSERVICES_PRIV_H__

// End of file.
