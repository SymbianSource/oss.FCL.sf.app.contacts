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
* Description:  Creates extension's instances.
*
*/


// INCLUDE FILES
#include "clogsextensionfactory.h"

#include "clogsviewextension.h"
#include "clogsuicontrolextension.h"
#include "simpledebug.h"

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
inline CLogsExtensionFactory::CLogsExtensionFactory()
    {
    _LOG("CLogsExtensionFactory::CLogsExtensionFactory()" )
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsExtensionFactory* CLogsExtensionFactory::NewL()
    {
   
    _LOG("CLogsExtensionFactory::NewL()" )
    
    return new (ELeave) CLogsExtensionFactory;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogsExtensionFactory::~CLogsExtensionFactory()
    {
    _LOG("CLogsExtensionFactory::~CLogsExtensionFactory()" )
    }


// ---------------------------------------------------------------------------
// Releases the factory.
// ---------------------------------------------------------------------------
//
void CLogsExtensionFactory::Release()
    {
    _LOG("CLogsExtensionFactory::Release() begin" )
    delete this;
    _LOG("CLogsExtensionFactory::Release() end" ) 
    }


// ---------------------------------------------------------------------------
// Ownership of view is transferred to the caller.
// ---------------------------------------------------------------------------
//
MLogsViewExtension* CLogsExtensionFactory::CreateLogsViewExtensionL
        (CPbkContactEngine& /*aEngine*/, MLogsExtObserver* /*aObserver*/)
    {
    _LOG("CLogsExtensionFactory::CreateLogsViewExtensionL() start" )

    iViewExtension = CLogsViewExtension::NewL();
    
    _LOG("CLogsExtensionFactory::CreateLogsViewExtensionL() end" )
    return iViewExtension;
    }


// ---------------------------------------------------------------------------
// Ownership of extension is transferred to the caller.
// ---------------------------------------------------------------------------
//
MLogsUiControlExtension* CLogsExtensionFactory::CreateLogsUiControlExtensionL()
    {
    _LOG("CLogsExtensionFactory::CreateLogsUiControlExtensionL() start" )
    
    iControlExtension = CLogsUiControlExtension::NewL();
    
    _LOG("CLogsExtensionFactory::CreateLogsUiControlExtensionL() end" )
    return iControlExtension;
    }

//  End of File
