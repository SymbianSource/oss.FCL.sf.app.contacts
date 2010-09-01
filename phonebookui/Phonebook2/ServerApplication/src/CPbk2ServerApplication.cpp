/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 application server application class. 
*
*/


#include "CPbk2ServerApplication.h"

// Phonebook 2
#include "CPbk2ServerApp.h"
#include "CPbk2ServerAppDocument.h"
#include <Pbk2InternalUID.h>

// System includes
#include <eikstart.h>

// --------------------------------------------------------------------------
// CPbk2ServerApplication::CreateDocumentL
// --------------------------------------------------------------------------
//
CApaDocument* CPbk2ServerApplication::CreateDocumentL()
    {
    return CPbk2ServerAppDocument::NewL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplication::AppDllUid
// --------------------------------------------------------------------------
//    
TUid CPbk2ServerApplication::AppDllUid() const
    {
    return TUid::Uid( KPbk2ServerAppUID3 );
    }

// --------------------------------------------------------------------------
// CPbk2ServerApplication::NewAppServerL
// --------------------------------------------------------------------------
//    
void CPbk2ServerApplication::NewAppServerL( CApaAppServer*& aAppServer )
    {
    aAppServer = CPbk2ServerApp::NewL();
    }

// --------------------------------------------------------------------------
// NewApplication()
// --------------------------------------------------------------------------
//    
EXPORT_C CApaApplication* NewApplication()
    {
	return new CPbk2ServerApplication;
    }

// --------------------------------------------------------------------------
// E32Main()
// --------------------------------------------------------------------------
//    
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
            
// End of File
