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
* Description:  PCS general definitions 
*
*/

#ifndef __PCSSERVERDEFS_H__
#define __PCSSERVERDEFS_H__

#include <e32base.h>
#include <BidiText.h>

/**
 * PCS Server Exe Name
 */
_LIT(KPcsServerName,           "PcsServer");
_LIT(KPcsServerExeName,        "PcsServer.exe");
_LIT(KPcsAlgorithm_NonChinese, "PCS-ALG-1");
_LIT(KPcsAlgorithm_Chinese,    "PCS-ALG-2");

/**
 * PCS Server Version details
 */
const TInt KPcsServerMajorVersionNumber = 1;
const TInt KPcsServerMinorVersionNumber = 0;
const TInt KPcsServerBuildVersionNumber = 0;

/**
 * PCS Server request Commands
 */
enum TPsServerRequestCommands
{ 
    ESearchSettings,
    ESearch,
    ESearchInput,
    ECancelSearch,
    ELangSupport,
    EGetDataOrder,
    EGetSortOrder,
    ESetSortOrder,
    EShutdown,
    ESearchMatchString
};

/**
 * PS Server response commands
 */
enum TPsServerResponseCommand
{
	ESearchComplete,
	ECancelComplete
};

/**
 * PS Keyboard Modes
 */
enum TKeyboardModes
{
   EItut,
   EQwerty,
   EModeUndefined
};

/**
 * Different states of Caching
 */
enum TCachingStatus
{
	ECachingNotStarted         =  0,
	ECachingInProgress         = 10,
	ECachingComplete           = 20,
	ECachingCompleteWithErrors = 30
};

/**
 * Different sort types
 */
enum TSortType
{
	EPatternBased,
	EAlphabetical
};

/**
 * PCS Plugin Interface UID
 */
const TUid KPcsPluginInterfaceUid   = { 0x2000B5BD };
const TUid KPcsAlgorithm1           = { 0x2000B5BE };
const TUid KPsDataStoreInterfaceUid = { 0x2000B5C0 };

const TInt KLinksToFetchInOneGo = 500;

/**
 * Max number of supported languages for Key Map
 */
const TInt KMaxNbrOfLangKeymapping = 10;

/**
 * Central Repository constants
 */
const TInt KCRMaxLen = 255;
const TInt KPsQueryMaxLen = 50;
const TInt KBufferMaxLen = 255;
const TInt KSearchResultsBufferLen = 8192;

/**
 * Data Stores Constants 
 */
_LIT(KVPbkDefaultCntDbURI, "cntdb://c:contacts.cdb"); //Phone Store
_LIT(KVPbkSimGlobalAdnURI, "sim://global_adn");       // SIM ADN Store
_LIT(KVPbkSimGlobalFdnURI, "sim://global_fdn");       // SIM FDN Store
_LIT(KVPbkSimGlobalSdnURI, "sim://global_sdn");       // SIM SDN Store
_LIT(KVPbkDefaultGrpDbURI, "cntdb://c:contacts.gdb"); // groups Store

/**
 * Data Structure to identify the matched locations
 */
struct TPsMatchLocation
{
	public:
		TInt index;   						  // starting index for the match sequence
		TInt length;  						  // lenght of the match sequence
		TBidiText::TDirectionality direction; // directionality for the sequence
};

/**
 * Character constants
 */
const TInt KUnitSeparator  = 31;
const TInt KSpaceCharacter = 32;

#endif // __PCSSERVERDEFS_H__

// End of file
