/*
* Copyright (c) 2007 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <biditext.h>

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
    ESearchMatchString,
    EGetAdaptiveGrid
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
    /**
     * Predictive matching mode with default keyboard. 
     * The key mappings used are either those of the first available
     * hardware keyboard or those of virtual-ITU-T if no hardware keyboard
     * is available.
     */
    EPredictiveDefaultKeyboard = 0,
    
    /**
     * Non-predictive matching mode.
     * Keyboard mappings of any kind are not used. Entered characters are
     * matched just as themselves, but using locale specific tolerant
     * matching rules (i.e. the collated comparison).
     */
    ENonPredictive = 1,
    
    /**
     * Predictive matching using 12-key ITU-T keymappings.
     */
    EPredictiveItuT = 2,
    
    /**
     * Predictive matching using hardware QWERTY key mappings.
     * Also half-QWERTY is here considered as QWERTY.
     */
    EPredictiveQwerty = 3,
    
    // These modes are provided for backward source and binary compatibility.
    EItut = EPredictiveDefaultKeyboard,  //! deprecated
    EQwerty = ENonPredictive,            //! deprecated
    
    EModeUndefined = -1
};

/**
 * Different states of Caching
 */
enum TCachingStatus
{
    ECachingNotStarted          =  0,
    ECachingInProgress          = 10,
    ECachingComplete            = 20,
    ECachingCompleteWithErrors  = 30,
    ECacheUpdateContactRemoved  = 40,
    ECacheUpdateContactModified = 50,
    ECacheUpdateContactAdded    = 60
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

const TInt KLinksToFetchInOneGo = 20;

/**
 * Max number of supported languages for Key Map
 */
const TInt KMaxNbrOfLangKeymapping = 10;

/**
 * Central Repository constants
 */
const TInt KCRMaxLen = 255;
const TInt KPsQueryMaxLen = 150;
const TInt KBufferMaxLen = 255;
const TInt KSearchResultsBufferLen = 8192;
const TInt KPsAdaptiveGridSupportedMaxLen = 0; // Max Len of input Search Text
const TInt KPsAdaptiveGridStringMaxLen = 255; // Max Len of returned Adaptive Grid String

/**
 * Data Stores Constants 
 */
_LIT(KVPbkDefaultCntDbURI, "cntdb://c:contacts.cdb"); // Phone Store
_LIT(KVPbkSimGlobalAdnURI, "sim://global_adn");       // SIM ADN Store
_LIT(KVPbkSimGlobalFdnURI, "sim://global_fdn");       // SIM FDN Store
_LIT(KVPbkSimGlobalSdnURI, "sim://global_sdn");       // SIM SDN Store
_LIT(KVPbkDefaultGrpDbURI, "cntdb://c:contacts.gdb"); // Groups Store

/**
 * Data Structure to identify the matched locations
 */
struct TPsMatchLocation
{
	public:
		TInt index;   						  // Starting index for the match sequence
		TInt length;  						  // Lenght of the match sequence
		TBidiText::TDirectionality direction; // Directionality for the sequence
};

/**
 * Character constants
 */
const TInt KUnitSeparator  = 31;
const TInt KSpaceCharacter = 32;

/**
 * Publish & Subscribe ID and keys for internal use of PCS
 */
const TUid KPcsInternalUidCacheStatus = {0x2000B5B6};
enum TPcsInternalKeyCacheStatus
    {
    EPsKeyCacheStatus            = 0,
    EPsKeyCacheError             = 1,
    EPsKeyContactRemovedCounter  = 2,
    EPsKeyContactModifiedCounter = 3,
    EPsKeyContactAddedCounter    = 4
    };


#endif // __PCSSERVERDEFS_H__

// End of file
