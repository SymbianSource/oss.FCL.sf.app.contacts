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
* Description:  An instance of class CCCAAppDocument is the 
*                Document part of the AVKON application 
*                framework for the CCAApp 
*
*/


#include "ccaappappui.h"
#include "ccaappdocument.h"

// ---------------------------------------------------------------------------
// CCCAAppDocument::NewL
// ---------------------------------------------------------------------------
//
CCCAAppDocument* CCCAAppDocument::NewL( CEikApplication& aApp )
    {
    CCCAAppDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAAppDocument::NewLC
// ---------------------------------------------------------------------------
//
CCCAAppDocument* CCCAAppDocument::NewLC( CEikApplication& aApp )
    {
    CCCAAppDocument* self = new (ELeave) CCCAAppDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAAppDocument::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAAppDocument::ConstructL()
    {
    }    

// ---------------------------------------------------------------------------
// CCCAAppDocument::CCCAAppDocument
// ---------------------------------------------------------------------------
//
CCCAAppDocument::CCCAAppDocument( CEikApplication& aApp ) 
    : CAknDocument( aApp ) 
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// CCCAAppDocument::~CCCAAppDocument
// ---------------------------------------------------------------------------
//
CCCAAppDocument::~CCCAAppDocument()
    {
    //As last operation call FinalClose to let Ecom unload plugin dlls  
    REComSession::FinalClose();      
    }

// ---------------------------------------------------------------------------
// CCCAAppDocument::CreateAppUiL
// ---------------------------------------------------------------------------
//
CEikAppUi* CCCAAppDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CCCAAppAppUi;
    return appUi;
    }

//End Of File

