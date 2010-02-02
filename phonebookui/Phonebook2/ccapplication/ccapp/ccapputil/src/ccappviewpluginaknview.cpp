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
* Description:  CAknView tailored for the needs of CCApp plugins.
*
*/


// INCLUDE FILES
#include "ccapputilheaders.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknView::CCCAppViewPluginAknView
// ---------------------------------------------------------------------------
//
EXPORT_C CCCAppViewPluginAknView::CCCAppViewPluginAknView()
    : iResourceLoader( *iCoeEnv )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppViewPluginAknView::CCCAppViewPluginAknView()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknView::~CCCAppViewPluginAknView
// ---------------------------------------------------------------------------
//
EXPORT_C CCCAppViewPluginAknView::~CCCAppViewPluginAknView()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppViewPluginAknView::~CCCAppViewPluginAknView()"));    
    iResourceLoader.Close(); 
    DeleteContainerIfExists();   
    }

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknView::DoActivateL
// ---------------------------------------------------------------------------
//
EXPORT_C void CCCAppViewPluginAknView::DoActivateL( const TVwsViewId& /*aPrevViewId*/,
                                        TUid /*aCustomMessageId*/,
                                        const TDesC8& /*aCustomMessage*/ )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppViewPluginAknView::DoActivateL()"));    
    DeleteContainerIfExists();
    TRAPD( error, CreateContainerL());
    if( KErrNone != error )
        {// Creating of container failed, cleanup and try to recover
        CCA_DP(KCCAppUtilLogFile, CCA_L("::DoActivateL() - CreateContainerL failed: %d"), error);    
        delete iContainer;
        iContainer = NULL;
        static_cast<CCCAAppAppUi*>(AppUi())->RecoverFromBadPluginL(); 
        }
    else
        {// Container created ok
        AppUi()->AddToViewStackL( *this, iContainer );
        }
    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppViewPluginAknView::DoActivateL()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknView::DoDeactivate
// ---------------------------------------------------------------------------
//
EXPORT_C void CCCAppViewPluginAknView::DoDeactivate()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppViewPluginAknView::DoDeactivate()"));    
    DeleteContainerIfExists();
    }

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknView::CreateContainerL
// ---------------------------------------------------------------------------
//
void CCCAppViewPluginAknView::CreateContainerL()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppViewPluginAknView::CreateContainerL()"));    
    
    //Ask container from the plugin
    NewContainerL();    
    CCA_DP(KCCAppUtilLogFile, CCA_L("::CreateContainerL() - iContainer available: %d"), iContainer ? 1 : 0 );    
    User::LeaveIfError( iContainer ? KErrNone : KErrArgument );
    
    //Setup container to FW
    iContainer->SetMopParent( this );
    CCA_DP(KCCAppUtilLogFile, CCA_L("::CreateContainerL() - iContainer BaseConstructL"));    
    iContainer->BaseConstructL( ClientRect(), *AppUi() );

    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppViewPluginAknView::CreateContainerL()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknView::HandleCommandL
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCCAppViewPluginAknView::HandleCommandL( TInt aCommand )
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppViewPluginAknView::HandleCommandL() - aCommand : %d"), aCommand );    
    AppUi()->HandleCommandL( aCommand );
    }    

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknView::DeleteContainerIfExists
// ---------------------------------------------------------------------------
//
void CCCAppViewPluginAknView::DeleteContainerIfExists()
    {
    CCA_DP(KCCAppUtilLogFile, CCA_L("->CCCAppViewPluginAknView::DeleteContainerIfExists()"));    

    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        CCA_DP(KCCAppUtilLogFile, CCA_L("::DeleteContainerIfExists() - container deleted"));    
        }

    CCA_DP(KCCAppUtilLogFile, CCA_L("<-CCCAppViewPluginAknView::DeleteContainerIfExists()"));    
    }

// ---------------------------------------------------------------------------
// CCCAppViewPluginAknView::SetTitleL
// ---------------------------------------------------------------------------
//    
EXPORT_C void CCCAppViewPluginAknView::SetTitleL( const TDesC& aTitle )
    {	
    CCA_DP(KCCAppUtilLogFile, CCA_L("CCCAppViewPluginAknView::SetTitleL() - aTitle : %S"), &aTitle );
    static_cast<CCCAAppAppUi*>( AppUi() )->SetTitleL( aTitle );	
    }  

//End of File
