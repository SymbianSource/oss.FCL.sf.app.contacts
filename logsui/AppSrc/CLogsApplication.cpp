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
*     Logs Application class
*
*/


// INCLUDE FILES
#include "CLogsApplication.h"
#include "CLogsDocument.h"
#include "LogsUID.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

void CLogsApplication::Panic( TInt aPanic )  // the panic code
    {
    _LIT( KPanicText, "Logs.app" );
    User::Panic( KPanicText, aPanic );
    }

CApaDocument* CLogsApplication::CreateDocumentL()
    {
    return CLogsDocument::NewL( *this );
    }

TUid CLogsApplication::AppDllUid() const
    {
    return TUid::Uid( KLogsAppUID3 );
    }

//  OTHER EXPORTED FUNCTIONS
//=============================================================================



#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CLogsApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

//  End of File

