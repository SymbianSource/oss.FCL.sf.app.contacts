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
*     This file contains declarations for resources of PbkEng.dll
*     The file can be included in C++ or resource file.
*
*/


#ifndef __Phonebook_PbkDefaults_h__
#define __Phonebook_PbkDefaults_h__

// INCLUDES
#include <cntdef.h>
#include "PbkUID.h"


////////////////////////////////////////////////////////////
// Phonebook Default vCard mappings added types

/// Integer UID for call and email defaults.
#define KIntPbkDefaultFieldPref            KIntContactFieldVCardMapPREF
/// UID for call and email defaults.
#define KUidPbkDefaultFieldPref            KUidContactFieldVCardMapPREF

/// Integer UID for sms default.
#define KIntPbkDefaultFieldSms             KPbkCustomContentTypeUid1
/// UID for sms default.
const TUid KUidPbkDefaultFieldSms          ={KIntPbkDefaultFieldSms};

/// Integer UID for MMS default.
#define KIntPbkDefaultFieldMms             KPbkCustomContentTypeUid2
/// UID for MMS default.
const TUid KUidPbkDefaultFieldMms          ={KIntPbkDefaultFieldMms};

/// Integer UID for Video default
#define KIntPbkDefaultFieldVideo           KPbkCustomContentTypeUid3
/// UID for Video default
const TUid KUidPbkDefaultFieldVideo        ={KIntPbkDefaultFieldVideo};

/// Integer UID for Email Over Sms default.
#define KIntPbkDefaultFieldEmailOverSms    KPbkCustomContentTypeUid4
/// UID for EmailOverSms default.
const TUid KUidPbkDefaultFieldEmailOverSms ={KIntPbkDefaultFieldEmailOverSms};

/// Integer UID for POC default.
#define KIntPbkDefaultFieldPoc             KPbkCustomContentTypeUid5
/// UID for POC default.
const TUid KUidPbkDefaultFieldPoc          ={KIntPbkDefaultFieldPoc};

/// Integer UID for VOIP default.
#define KIntPbkDefaultFieldVoip             KPbkCustomContentTypeUid6
/// UID for POC default.
const TUid KUidPbkDefaultFieldVoip          ={KIntPbkDefaultFieldVoip};

#endif

// End of File
