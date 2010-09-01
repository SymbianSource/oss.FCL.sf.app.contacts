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
* Description:  Icon file provider.
*
*/



// INCLUDE FILES
#include <aknappui.h>
#include <AknIconUtils.h>

#include "clogsiconfileprovider.h"
#include "simpledebug.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CLogsIconFileProvider::CIconFileProvider
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CLogsIconFileProvider::CLogsIconFileProvider()
    {    
    }


// ---------------------------------------------------------------------------
// CLogsIconFileProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CLogsIconFileProvider::ConstructL( RFile& aFile )
    {
    _LOG("CLogsIconFileProvider::ConstructL() begin" )  
    User::LeaveIfError( iFile.Duplicate( aFile ) );
    _LOG("CLogsIconFileProvider::ConstructL() end" )  
    }


// ---------------------------------------------------------------------------
// CLogsIconFileProvider::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CLogsIconFileProvider* CLogsIconFileProvider::NewL(
        RFile& aFile )
    {
    CLogsIconFileProvider* self = new( ELeave ) CLogsIconFileProvider;
    CleanupStack::PushL( self );
    self->ConstructL( aFile );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogsIconFileProvider::~CLogsIconFileProvider()
    {
    _LOG("CLogsIconFileProvider::~CLogsIconFileProvider(): begin" )  
    iFile.Close();    
    _LOG("CLogsIconFileProvider::~CLogsIconFileProvider(): end" )  
    }


// ---------------------------------------------------------------------------
// CLogsIconFileProvider::RetrieveIconFileHandleL
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CLogsIconFileProvider::RetrieveIconFileHandleL(
            RFile& aFile, const TIconFileType /*aType*/ )
    {
    _LOG("CLogsIconFileProvider::RetrieveIconFileHandleL(): begin" )  
    User::LeaveIfError( aFile.Duplicate( iFile ) );
    _LOG("CLogsIconFileProvider::RetrieveIconFileHandleL(): end" )  
    }


// ---------------------------------------------------------------------------
// CLogsIconFileProvider::Finished
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CLogsIconFileProvider::Finished()
    {
    // commit suicide because Avkon Icon Server said so    
    delete this;    
    }

