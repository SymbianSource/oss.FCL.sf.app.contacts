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
* Description: Class handling ownership of cca ui view factory instantiated 
* from ecom. The factory needs to be kept alive as long as the last
* ui view instantiated from this factory is alive.
*/

// INCLUDE FILES
#include <ecom.h>
#include "ccapluginfactoryowner.h"
#include <mccapluginfactory.h> 
   

// ---------------------------------------------------------------------------
// CcaPluginFactoryOwner::~CcaPluginFactoryOwner
// ---------------------------------------------------------------------------
//   
CcaPluginFactoryOwner* CcaPluginFactoryOwner::NewL(
    CImplementationInformation* aImplementationInformation)
    {
    CcaPluginFactoryOwner* self = new (ELeave) CcaPluginFactoryOwner();
    CleanupStack::PushL(self);
    self->ConstructL(aImplementationInformation);
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// CcaPluginFactoryOwner::ConstructL
// ---------------------------------------------------------------------------
//   
void CcaPluginFactoryOwner::ConstructL(
    CImplementationInformation* aImplementationInformation)
    {
    iFactory = REINTERPRET_CAST( MCcaPluginFactory*, 
        REComSession::CreateImplementationL(
            aImplementationInformation->ImplementationUid(),
            iDtor_ID_Key) ) ;
    }

// ---------------------------------------------------------------------------
// CcaPluginFactoryOwner::CcaPluginFactoryOwner
// ---------------------------------------------------------------------------
//   
CcaPluginFactoryOwner::CcaPluginFactoryOwner()
    {
    }

// ---------------------------------------------------------------------------
// CcaPluginFactoryOwner::~CcaPluginFactoryOwner
// ---------------------------------------------------------------------------
//   
CcaPluginFactoryOwner::~CcaPluginFactoryOwner()
    {
    delete iFactory;    
    REComSession::DestroyedImplementation ( iDtor_ID_Key ); 
    }

// ---------------------------------------------------------------------------
// CcaPluginFactoryOwner::Factory
// ---------------------------------------------------------------------------
//   
MCcaPluginFactory& CcaPluginFactoryOwner::Factory()
    {
    return *iFactory;
    }

// ---------------------------------------------------------------------------
// CcaPluginFactoryOwner::AddedChild
// ---------------------------------------------------------------------------
//   
TInt CcaPluginFactoryOwner::AddedChild()
    {
    return ++iChildCount;
    }

// ---------------------------------------------------------------------------
// CcaPluginFactoryOwner::DeletedChild
// ---------------------------------------------------------------------------
//   
TInt CcaPluginFactoryOwner::DeletedChild()
    {
    return --iChildCount;    
    }

