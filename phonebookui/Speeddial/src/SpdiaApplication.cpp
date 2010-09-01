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
* Description:     Declares main application class.
*
*/






// INCLUDE FILES
#include    "SpdiaApplication.h"
#include    "SpdiaDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSpdiaApplication::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CSpdiaApplication::AppDllUid() const
    {
    return KUidSpdia;
    }

// ---------------------------------------------------------
// CSpdiaApplication::CreateDocumentL()
// Creates CSpdiaDocument object
// ---------------------------------------------------------
//
CApaDocument* CSpdiaApplication::CreateDocumentL()
    {
    return CSpdiaDocument::NewL(*this);
    }

// ================= OTHER EXPORTED FUNCTIONS ==============


#include <eikstart.h>

LOCAL_C CApaApplication* NewApplication()
    {
    return new CSpdiaApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }



// End of File  

