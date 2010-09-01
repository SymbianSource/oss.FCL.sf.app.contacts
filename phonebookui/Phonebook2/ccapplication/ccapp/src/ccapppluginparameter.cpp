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
* Description:  Parameters used between CCApp and CCApp plugin views
*
*/


#include "ccappheaders.h"

// ---------------------------------------------------------------------------
// CCCAppPluginParameter::NewL
// ---------------------------------------------------------------------------
//
CCCAppPluginParameter* CCCAppPluginParameter::NewL( CCCAppView& aAppView )
    {
    return new (ELeave) CCCAppPluginParameter( aAppView );
    }

// ---------------------------------------------------------------------------
// CCCAppPluginParameter::CCCAppPluginParameter
// ---------------------------------------------------------------------------
//
CCCAppPluginParameter::CCCAppPluginParameter( CCCAppView& aAppView ) 
    : iVersion ( 1 ),
      iAppView ( aAppView )
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// CCCAppPluginParameter::~CCCAppPluginParameter
// ---------------------------------------------------------------------------
//
CCCAppPluginParameter::~CCCAppPluginParameter()
    {
    }

// ---------------------------------------------------------------------------
// CCCAppPluginParameter::Version
// ---------------------------------------------------------------------------
//
TInt CCCAppPluginParameter::Version()
    {
    return iVersion;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginParameter::CCAppLaunchParameter
// ---------------------------------------------------------------------------
//
MCCAParameter& CCCAppPluginParameter::CCAppLaunchParameter()
    {
    return iAppView.AppUi().Parameter();
    }

// ---------------------------------------------------------------------------
// CCCAppPluginParameter::ReservedKeys
// ---------------------------------------------------------------------------
//    
const RArray<TInt>& CCCAppPluginParameter::ReservedKeys()
    {
    return iAppView.AppUi().ReservedKeys();
    }


//End Of File

