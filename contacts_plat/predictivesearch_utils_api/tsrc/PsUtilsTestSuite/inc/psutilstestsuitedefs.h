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
* Description:  Ps Utils test suite header file
*
*/


#ifndef PS_UTILS_TEST_DEFS_H
#define PS_UTILS_TEST_DEFS_H

// Logging path
_LIT( KPsTestSuiteLogPath, "\\logs\\testframework\\PsTestSuite\\" ); 

// Log file
_LIT( KPsTestSuiteLogFile, "PsUtilsTestSuite.txt" ); 
_LIT( KPsTestSuiteLogFileWithTitle, "PsUtilsTestSuite_[%S].txt" );

_LIT(KNull, "");
_LIT(KItut, "EItut");
_LIT(KQwerty, "EQwerty");
_LIT(KAlphabetic, "EAlphabetical");
_LIT(KPattern, "EPatternBased");


_LIT(KSearchQueryTag,"SQ");             // The search query  
_LIT(KUriTag,"URI");                    // The data store uri 
_LIT(KDataTag,"DATA");                    // The data  
_LIT(KIdTag,"ID");                    // The contact Id
_LIT(KSortTag,"SORT");                  // Sort order for results
_LIT(KGroupIdTag,"GRPID");                  // Group Ids for results

_LIT(KSetDisplayFields,"DISP");         // Display fields 

#endif // PS_UTILS_TEST_DEFS_H

// End of file
