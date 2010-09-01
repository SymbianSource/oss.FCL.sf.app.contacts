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
* Description:  Constants for logsserviceextension
*
*/



#ifndef LOGSEXTCONSTS_H
#define LOGSEXTCONSTS_H


// CONSTANT LITERALS

_LIT8( KBrandedBitmapItemId, "service_active_image_small" );//old
_LIT8( KDefaultBrandedBitmapItemId, "default_brand_image" );//new note: at the moment(10.05.07) both version are supported by the branding package
_LIT8( KAppId, "xsp" );
_LIT8( KDefaultBrandId, "xsp" );

_LIT8( KLogsExtDataFldTag_PresentityId,       "PRE");          //PresentityId
_LIT8( KLogsExtDataFldTag_ServiceId,          "SI");           //ServiceId
_LIT8( KLogsExtDataFldTag_ContactLink,        "CL");           //ContactLink

_LIT( KPersonPresTmpl, "person:%S:image" );

// CONSTANT NUMBERS
const TInt KProtocolUid = 0x10282EF2;
const TInt KNumberDelimitersModifiedIconString = 5;
const TInt KNumberDelimitersUnmodifiedIconStringMin = 2;
const TInt KNumberDelimitersUnmodifiedIconStringMax = 4;


// the granularity of the array that contains the event info entries
const TInt KEventInfoManagerArrayGranularity = 2; 
const TInt KPresenceTableManagerGranularity = 2;
const TInt KBrandFetcherArrayGranularity = 2;
const TInt KPresenceFetcherArrayGranularity = 2;
const TInt KPresentityIdFetcherArrayGranularity = 2;
const TInt KLogIdPresIdMappingArrayGranularity = 2;
const TInt KShowPresentityArrayGranularity = 2;
const TInt KInitialIndexValue = -1;

// if defined, we test the new feature ....
//#define PRES_GROUP_TEST 

/** buddy list id */
_LIT( KPresenceBuddyList, "buddylist");
/**  blocked list id */
_LIT( KPresenceBlockedList, "blockedcontacts");
/** for virtual online buddy group */
_LIT( KPresenceOnlineBuddy ,"online_buddies");
/** Subscribed buddys */
_LIT( KPresenceSubcribedBuddys, "subs_buddys");

#endif  // LOGSEXTCONSTS_H
