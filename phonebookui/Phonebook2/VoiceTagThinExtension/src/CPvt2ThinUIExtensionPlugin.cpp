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
* Description:  A Voice tag Thin UI extension implementation.
*
*/


// INCLUDE FILES
#include "cpvt2thinuiextensionplugin.h"

// Phonebook 2
#include <mpbk2contactuicontrol.h>
#include <pbk2commands.rsg>
#include <pbk2commands.hrh>

// Virtual Phonebook

// System includes
#include <eikmenup.h>
#include <coemain.h>
#include <featmgr.h>

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CPvt2ThinUIExtensionPlugin::CPvt2ThinUIExtensionPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CPvt2ThinUIExtensionPlugin::CPvt2ThinUIExtensionPlugin()
    {
    }

// --------------------------------------------------------------------------
// CPvt2ThinUIExtensionPlugin::~CPvt2ThinUIExtensionPlugin
// --------------------------------------------------------------------------
//
CPvt2ThinUIExtensionPlugin::~CPvt2ThinUIExtensionPlugin()
    {
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPvt2ThinUIExtensionPlugin::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CPvt2ThinUIExtensionPlugin* CPvt2ThinUIExtensionPlugin::NewL()
    {
    CPvt2ThinUIExtensionPlugin* self = new(ELeave) CPvt2ThinUIExtensionPlugin;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CPvt2ThinUIExtensionPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CPvt2ThinUIExtensionPlugin::ConstructL()
    {
    // Initialize feature manager
    FeatureManager::InitializeLibL();
    }

// --------------------------------------------------------------------------
// CPvt2ThinUIExtensionPlugin::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPvt2ThinUIExtensionPlugin::DynInitMenuPaneL(
        TInt /*aResourceId*/,
        CEikMenuPane* /*aMenuPane*/, 
        MPbk2ContactUiControl& /*aControl*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPvt2ThinUIExtensionPlugin::ExtensionStartupL
// --------------------------------------------------------------------------
//
void CPvt2ThinUIExtensionPlugin::ExtensionStartupL(
        MPbk2StartupMonitor& aStartupMonitor)
    {
    aStartupMonitor.HandleStartupComplete();
    }


//  End of File
