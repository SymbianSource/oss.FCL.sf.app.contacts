/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of MyCard plugin factory
*
*/

// INCLUDES
#include "ccappmycardpluginfactory.h"
#include "ccappmycardplugin.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppMycardPluginFactory::NewL
// ---------------------------------------------------------------------------
//
MCcaPluginFactory* CCCAppMycardPluginFactory::NewL()  
    {
    return new(ELeave) CCCAppMycardPluginFactory;
    }

// ---------------------------------------------------------------------------
// CCCAppMycardPluginFactory::CCCAppMycardPluginFactory
// ---------------------------------------------------------------------------
//
CCCAppMycardPluginFactory::CCCAppMycardPluginFactory()
    {
    }

// ---------------------------------------------------------------------------
// CCCAppMycardPluginFactory::~CCCAppMycardPluginFactory
// ---------------------------------------------------------------------------
//
CCCAppMycardPluginFactory::~CCCAppMycardPluginFactory()
    {
    }

// ---------------------------------------------------------------------------
// CCCAppMycardPluginFactory::TabViewCount
// ---------------------------------------------------------------------------
//
TInt CCCAppMycardPluginFactory::TabViewCount()
    {
    return 1;
    }
    
// ---------------------------------------------------------------------------
// CCCAppMycardPluginFactory::CreateTabViewL
// ---------------------------------------------------------------------------
//
CCCAppViewPluginBase* CCCAppMycardPluginFactory::CreateTabViewL( TInt aIndex )
    {
    switch( aIndex )
        {
        case 0:
            {
            return CCCAppMyCardPlugin::NewL();
            }
        default:
			{
			// TODO: Panic?
			}
        }
    return NULL;
    }


