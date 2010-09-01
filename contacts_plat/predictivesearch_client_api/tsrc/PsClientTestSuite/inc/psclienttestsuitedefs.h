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
* Description:  The defs for client test suite
*
*/

#ifndef PS_CLIENT_TEST_DEFS_H
#define PS_CLIENT_TEST_DEFS_H

// Logging path
_LIT( KPsTestSuiteLogPath, "\\logs\\testframework\\PsTestSuite\\" ); 

// Log file
_LIT( KPsTestSuiteLogFile, "PsClientTestSuite.txt" ); 
_LIT( KPsTestSuiteLogFileWithTitle, "PsClientTestSuite_[%S].txt" );

//  The data structure used to store the contacts information
struct cacheData
{   
	// The vpbkId array.The ids correspond to the identifiers mentioned in vpbkeng.rsg
    RArray<TInt> vpbkId;  
    
    // The corresponding data
	RPointerArray<TDesC> data;
} ;

/* Configration file parameters
 * Key board modes. The string mentioned below are used in the configuration
 * parameters in the psclientapitest.cfg file. 
 */
 
// Keyboard modes for query
_LIT(KItut,                      "EItut");   // Deprecated
_LIT(KQwerty,                    "EQwerty"); // Deprecated
_LIT(KPredictiveItuT,            "EPredictiveItuT");
_LIT(KPredictiveQwerty,          "EPredictiveQwerty");
_LIT(KPredictiveDefaultKeyboard, "EPredictiveDefaultKeyboard");
_LIT(KNonPredictive,             "ENonPredictive");
_LIT(KWrongKeyboardMode,         "EWrongKeyBoardMode");

// Keyboard modes for query-item
_LIT(KPredictiveItuTCode,        "i");
_LIT(KPredictiveQwertyCode,      "q");
_LIT(KPredictiveDefaultCode,     "d");
_LIT(KNonPredictiveCode,         "n");

// Sort Modes
_LIT(KPatternSort, "EPatternSort");
_LIT(KAlphaSort,   "EAlphaSort");

// True/False
_LIT(KTrue,  "ETrue");
_LIT(KFalse, "EFalse");

// Configuration options
_LIT(KCreateContacts,               "CreateContacts"); // For creating contacts
_LIT(KCreateGroups,                 "CreateGroups");   // For creating groups
_LIT(KSearchQueryTag,               "SQ");             // The search query  
_LIT(KUriTag,                       "URI");            // The data store uri 
_LIT(KSortOrderTag,                 "SORTORDER");      // Sort order for results
_LIT(KSetDisplayFields,             "DISP");           // Display fields 
_LIT(KSupprotedLanguage,            "LANG");           // Supported Language
_LIT(KAddGroup,                     "AddGroups");      // Adding groups to the phone
_LIT(KSearchInGroup,                "SrchInGroup");    // Search within a group
_LIT(KSearchExpectedResult,         "RES");            // Expected result of a test case  
_LIT(KSearchExpectedMatchLocations, "MATCHLOC");       // Expected match locations result
_LIT(KSearchOnInputString,          "IPSTR");          // The Input string tag for search in input string
_LIT(KExpectedResultString,         "RESSTR");         // Expected string result 
_LIT(KSearchExpectedResultForInput, "RESIP");          // Expected result (For input search string case)
_LIT(KInputBoolean,                 "BOOL");           // Input Boolean for cases that need it

// The enumeration used to specify the type of test case
enum TTestCase
{
	EDeleteSearchedResults = 0,
	EGetGroupId,
	EConvertToVpbkLink,
	EMarkedContacts,
	EDefaultCase
};

_LIT (KFirstName,   "FN");
_LIT (KLastName,    "LN");
_LIT (KCompanyName, "CN");

#endif // PS_CLIENT_TEST_DEFS_H

// End of file
