/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCoeControl tailored for the needs of CCApp plugins.
*
*/


// INCLUDE FILES
#include "ccapputilheaders.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknContainer::CCCAppViewPluginAknContainer
// ---------------------------------------------------------------------------
//
EXPORT_C CCCAppViewPluginAknContainer::CCCAppViewPluginAknContainer()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppViewPluginAknContainer::CCCAppViewPluginAknContainer()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknContainer::~CCCAppViewPluginAknContainer
// ---------------------------------------------------------------------------
//
EXPORT_C CCCAppViewPluginAknContainer::~CCCAppViewPluginAknContainer()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppViewPluginAknContainer::~CCCAppViewPluginAknContainer()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknContainer::BaseConstructL
// ---------------------------------------------------------------------------
void CCCAppViewPluginAknContainer::BaseConstructL( 
    const TRect& aRect,
    CCoeAppUi& aAppUi )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppViewPluginAknContainer::BaseConstructL()"));    
    iAppUi = &aAppUi;
    
    CreateWindowL();
    CCA_DP(KCCAppUtilLogFile, CCA_L("::BaseConstructL() - CreateWindowL called"));    
    ConstructL();
    CCA_DP(KCCAppUtilLogFile, CCA_L("::BaseConstructL() - sub-class ConstructL called"));    
    
    SetRect( aRect );
    ActivateL();
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppViewPluginAknContainer::BaseConstructL()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
EXPORT_C TKeyResponse CCCAppViewPluginAknContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppViewPluginAknContainer::OfferKeyEventL()"));    
    TKeyResponse returnValue = EKeyWasNotConsumed;

    // Let AppUi check the key-event for the reserved keys
    if ( EEventKey == aType )
        returnValue = static_cast<CCCAAppAppUi*>( iAppUi )
            ->HandleKeyEventL( aKeyEvent, aType );

    CCA_DP(KCCAppUtilLogFile, CCA_L("::OfferKeyEventL() - returnValue : %d"), (TInt)returnValue );    
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppViewPluginAknContainer::OfferKeyEventL()"));    
    return returnValue;
    }

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknContainer::HandleResourceChange
// ---------------------------------------------------------------------------
EXPORT_C void CCCAppViewPluginAknContainer::HandleResourceChange( TInt aType )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppViewPluginAknContainer::HandleResourceChange()"));    
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KAknsMessageSkinChange ||
         aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( 
            AknLayoutUtils::EMainPane,
            mainPaneRect);
        SetRect( mainPaneRect );
        }
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppViewPluginAknContainer::HandleResourceChange()"));    
    }

//  End of File
