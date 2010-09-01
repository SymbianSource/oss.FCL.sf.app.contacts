/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Implementation for extension dll loader.
*
*/


// INCLUDE FILES
#include "CxSPLoader.h"
#include "MxSPFactory.h"
#include "CxSPFactory.h"


// ================= MEMBER FUNCTIONS =======================

inline CxSPLoader::CxSPLoader()								
    {
    }

CxSPLoader* CxSPLoader::NewL( TUid aUid )
    {
    CxSPLoader* self = new (ELeave) CxSPLoader();
    CleanupStack::PushL(self);
    self->LoadExtensionDllL(aUid);
    CleanupStack::Pop(self);
    return self;
    }

CxSPLoader::~CxSPLoader()
    {
    delete iFactory;
    }

MxSPFactory* CxSPLoader::ExtensionFactory() const
    {
    return iFactory;
    }

void CxSPLoader::LoadExtensionDllL( TUid aUid )
    {
    iFactory = CxSPFactory::NewL( aUid );
    }    

// End of File
