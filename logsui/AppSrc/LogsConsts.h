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
*     Contains constants used in Logs
*
*/


#ifndef __LogsConsts_H__
#define __LogsConsts_H__

/// Unnamed namespace for local definitons
namespace {

// CONSTANT LITERALS

_LIT( KSpace, " " );                // Adapter string format 
_LIT( KTab, "\t" );                 // Adapter string format 
_LIT( KPanicText, "Logs.app" );     // Panic string
_LIT( KPanicText_DetailsCC, "DetailsCC" );

// descriptor format strings
_LIT( KOpenBracket, "(");
_LIT( KCloseBracket, ")");
_LIT( KThreeDots, "..." );

// resource file constants 
_LIT( KLogsIconFileDrive, "Z:" );
_LIT( KLogsOldIconFileDirectory,    "\\system\\data\\" );  
_LIT( KLogsIconFile, "logs.mbm" );    //system will automatically select between .mif and .mbm files

// icon index strings
_LIT( KFirstIconAndTab, "0\t" );
_LIT( KSecondIconAndTab, "1\t" );
_LIT( KThirdIconAndTab, "2\t" );
_LIT( KFourthIconAndTab, "3\t" );

// Used by CLogsSettingsControlContainer
// listbox descriptor format string
_LIT( KEmptySpaceAndTabulator, " \t" );
_LIT( KTabulatorTwice, "\t\t" );

// CONSTANT NUMBERS
// Logs ui spec details chapter for shown lines
const TInt KLogsDetailsCallLines = 4;
const TInt KLogsDetailsDataLines = 3;
const TInt KLogsDetailsFaxLines = 4;
const TInt KLogsDetailsSMSLines = 5;
const TInt KLogsDetailsPacketDataLines = 5;

//For RecentList iconarray 
enum RecentListIconArrayIcons
    {
    EIconMobile = 0,
    EIconVideo,
    EIconVoip,
    EIconPoc,
    EIconAls,
    EIconNewMissed,
    EIconlandphone,
    EIconPager,
    EIconFax,
    EIconAsstPhone,
    EIconCarPhone,
    EIconDefault,
    };

// phone number max len + some 4 extra for space brackets and recent count
const TInt KRecentMaxTextLength = 50;  //[FIXME: This probably needs to be longer]


const TInt KNonClipDuplBufferLength = 10; 
const TInt KListBoxSubCellNr = 1;               // Sub Cell Number

// Max number of trailing icons we can show in Recent call lists
const TInt KMaxNbrOfRecentTrailIcons = 2;

const TInt KUndefined = -1;

// Control Container's number of items
const TInt KAppListCCNrOfItems = 4;
const TInt KSubAppListCCNrOfItems = 3;
const TInt KAocCtCCNrOfItems = 7;
const TInt KGprsCtCCNrOfItems = 2;
const TInt KEventListCCNrOfItems = 3;
const TInt KRecentListCCNrOfItems = 12;
// Used by CLogsGprsCtAdapter
const TInt KAmountOfGprsCounters = 2;
const TInt KDigitsAfterDot = 2;
const TInt KMaxPercentage = 100;

// CLogsBaseView::ProcessKeyEventL's delay
const TInt KDelayMicroSeconds = 250000;

// MACROS

// DATA TYPES

}  // namespace

#endif  // __LogsConsts_H__
           
// End of File
