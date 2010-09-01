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
* Description:  An instance of CCCAAppApplication is the application part 
*                of the AVKON application framework for the CCAApp 
*                application
*
*/

#include "ccaappdocument.h"
#include "ccaappapplication.h"
#include "ccaappserverapp.h"
#include "ccauids.h"
#include "ccalogger.h"

// UID for the application, this should correspond to the uid defined in the mmp file
static const TUid KUidCCAApp = {KCCAAppUID};

// ---------------------------------------------------------------------------
// Create an App document, and return a pointer to it
// ---------------------------------------------------------------------------
//
CApaDocument* CCCAAppApplication::CreateDocumentL()
    {  
    CApaDocument* document = CCCAAppDocument::NewL(*this);
    return document;
    }

// ---------------------------------------------------------------------------
// Return the UID 
// ---------------------------------------------------------------------------
//
TUid CCCAAppApplication::AppDllUid() const
    {
    return KUidCCAApp;
    }

// ---------------------------------------------------------------------------
// The NewAppServerL() function, it only has to "new" the server,
// further construction is called by the framework.
// ---------------------------------------------------------------------------
//
void CCCAAppApplication::NewAppServerL(CApaAppServer*& aAppServer)
	{

    //PERFORMANCE LOGGING: 2. Starting up application
    WriteToPerfLog();

	aAppServer = new(ELeave) CCAAppServerApp();
	}

