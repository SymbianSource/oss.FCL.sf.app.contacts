/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Main application class
*
*/

// INCLUDE FILES
#include "NewContactLauncher.hrh"
#include "NewContactLauncherDocument.h"
#include "NewContactLauncherApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNewContactLauncherApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CNewContactLauncherApplication::CreateDocumentL()
    {
    // Create an NewContactLauncher document, and return a pointer to it
    return CNewContactLauncherDocument::NewL(*this);
    }

// -----------------------------------------------------------------------------
// CNewContactLauncherApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CNewContactLauncherApplication::AppDllUid() const
    {
    // Return the UID for the NewContactLauncher application
    return KUidNewContactLauncherApp;
    }

// End of File
