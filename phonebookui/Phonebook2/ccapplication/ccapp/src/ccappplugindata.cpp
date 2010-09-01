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
* Description:  Class for handling the view plugin data
*
*/


#include "ccappheaders.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppPluginData::CCCAppPluginData
// ---------------------------------------------------------------------------
//
CCCAppPluginData::CCCAppPluginData( 
    CCCAppViewPluginBase& aPlugin )
    : iPlugin ( aPlugin ),
      iPluginIsPrepared( EFalse )
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppPluginData::CCCAppPluginData"));
    }

// ---------------------------------------------------------------------------
// CCCAppPluginData::~CCCAppPluginData
// ---------------------------------------------------------------------------
//
CCCAppPluginData::~CCCAppPluginData()
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::~CCCAppPluginData"));

    // If plugin is added to AppUi -framework, it will take care of deleting. If
    // that is not done yet, we need to delete the plugin here.
    if ( !iPluginIsPrepared )
        {
        delete &iPlugin;
        }

    delete iPluginBitmap;
    delete iPluginBitmapMask;  
    
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::~CCCAppPluginData"));
    }

// ---------------------------------------------------------------------------
// CCCAppPluginData::NewLC
// ---------------------------------------------------------------------------
//
CCCAppPluginData* CCCAppPluginData::NewLC( 
    CCCAppViewPluginBase& aPlugin )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppPluginData::NewLC"));
    
    CCCAppPluginData* self = new( ELeave ) CCCAppPluginData( aPlugin );//, aPluginLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppPluginData::NewLC"));
    return self;
	}

// ---------------------------------------------------------------------------
// CCCAppPluginData::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppPluginData::ConstructL()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppPluginData::ConstructL"));
    }

// ---------------------------------------------------------------------------
// CCCAppPluginData::Plugin
// ---------------------------------------------------------------------------
//
CCCAppViewPluginBase& CCCAppPluginData::Plugin()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppPluginData::Plugin"));
    return iPlugin;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginData::PluginIsPrepared
// ---------------------------------------------------------------------------
//    
void CCCAppPluginData::PluginIsPrepared()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppPluginData::IsPluginPrepared"));
    iPluginIsPrepared = ETrue;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginData::IsPluginPrepared
// ---------------------------------------------------------------------------
//    
TBool CCCAppPluginData::IsPluginPrepared()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppPluginData::IsPluginPrepared"));
    return iPluginIsPrepared;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginData::SetPluginVisibility
// ---------------------------------------------------------------------------
//    
void CCCAppPluginData::SetPluginVisibility( TInt aPluginVisibility)
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppPluginData::SetPluginVisibility"));
    iPluginVisibility = aPluginVisibility;
    }

// ---------------------------------------------------------------------------
// CCCAppPluginData::PluginVisibility
// ---------------------------------------------------------------------------
//    
TInt CCCAppPluginData::PluginVisibility()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppPluginData::PluginVisibility"));
    return iPluginVisibility;
    }

// End of File
