/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Logs application "App List" view class implementation
*
*/


// INCLUDE FILES
#include <logs.rsg>
#include <w32std.h>
#include <apgtask.h>

#include "CLogsAppListView.h"
#include "CLogsAppListControlContainer.h"
#include "CLogsAppUi.h"
#include "CLogsEngine.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLogsAppListView::NewL
// ----------------------------------------------------------------------------
//
CLogsAppListView* CLogsAppListView::NewL() 
    {
    CLogsAppListView* self = new ( ELeave ) CLogsAppListView;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsAppListView::~CLogsAppListView
// ----------------------------------------------------------------------------
//
CLogsAppListView::~CLogsAppListView()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    }

// ----------------------------------------------------------------------------
// CLogsAppListView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLogsAppListView::HandleCommandL( TInt aCommandId )
    {
    switch( aCommandId )
        {
        case ELogsCmdMenuOpen:
            LogsAppUi()->CmdOkL( iContainer->ListBox()->CurrentItemIndex() );
            break;

        default:
            CLogsBaseView::HandleCommandL( aCommandId );
        }
    }

// ----------------------------------------------------------------------------
// CLogsAppListView::Id
// ----------------------------------------------------------------------------
//
TUid CLogsAppListView::Id() const
    {
    return TUid::Uid( ELogAppListViewId );
    }

// ----------------------------------------------------------------------------
// CLogsAppListView::HandleClientRectChange
// ----------------------------------------------------------------------------
//
void CLogsAppListView::HandleClientRectChange()
    {
    if (iContainer)
        {
        iContainer->SetRect( ClientRect() );  
        }
    }

// ----------------------------------------------------------------------------
// CLogsAppListView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLogsAppListView::DoActivateL( const TVwsViewId& aPrevViewId, 
          TUid aCustomMessageId, const TDesC8& /* aCustomMessage */ )
    {
    //Reset ProvideOnlyRecentViews always when other than CLogsRecentListView is activated. 
    LogsAppUi()->SetProvideOnlyRecentViews( EFalse );
    
    if( ! iContainer )
        {
        iContainer = CLogsAppListControlContainer::NewL( this, ClientRect() );
        AppUi()->AddToViewStackL( *this, iContainer );
        }

    //Set focus to correct row if we return from subselection view
    switch( aPrevViewId.iViewUid.iUid )
        {
        case ECtViewId:
            iSelectedLine = 1;
            break;    
        case EGprsCounterViewId:
            iSelectedLine = 2;
            break;    
        case ELogSubAppListViewId:
        default:
            iSelectedLine = 0;    
        }

    // See Error ELJG-7PKAC8  
    iContainer->ListBox()->SetCurrentItemIndex( iSelectedLine );
    iContainer->ListBox()->SetListBoxObserver( this );

    LogsAppUi()->SetPreviousViewId( aPrevViewId.iViewUid );
    LogsAppUi()->SetCurrentViewId( Id() );

    Engine()->DeleteSMSEventUpdater();  // <>StartSMSEventUpdaterL()

    if( aCustomMessageId.iUid == ELogsViewActivationBackground )
        {
        // We have activated this view because we are hiding Logs into 
        // background (faking exit) so this view is waiting when Logs is 
        // again foregrounded. 
        // Now we can enable bring-to-foreground on view activation:         
        AppUi()->SetCustomControl(0); 
        AppUi()->HideInBackground(); 
        }
    
    // Just to make sure the inputblocker is not on
    RemoveInputBlocker();
    
    // Avoid the flicking when transfer to foreground from background.
    if(LogsAppUi()->IsBackground())
       {
           iContainer->DrawNow();        
           iEikonEnv->AppUiFactory()->StatusPane()->DrawNow();
           LogsAppUi()->SetCustomControl(0); 
       
           CCoeEnv * env = CCoeEnv::Static();
           env->WsSession().SetWindowGroupOrdinalPosition(env->RootWin().Identifier(),0); 
       }
    }

// ----------------------------------------------------------------------------
// CLogsAppListView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLogsAppListView::DoDeactivate()
    {
    if( iContainer )
        {
        iSelectedLine = iContainer->ListBox()->CurrentItemIndex();
        // Remove view and its control from the view stack
        AppUi()->RemoveFromViewStack( *this, iContainer );

        // Destroy the container control
        delete iContainer;
        iContainer = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CLogsAppListView::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsAppListView::ConstructL()
    {
    BaseConstructL( R_LOGS_APP_LIST_VIEW );
    }

// ----------------------------------------------------------------------------
// CLogsAppListView::CLogsAppListView
// ----------------------------------------------------------------------------
//
CLogsAppListView::CLogsAppListView()
    {
    }


void CLogsAppListView::ViewActivatedL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,const TDesC8& aCustomMessage)
    {
    // To avoid the flicking when transfer to foreground from background,we control the view show manually.
    if(LogsAppUi()->IsBackground()  &&
            LogsAppUi()->ActiveViewId() != ELogAppListViewId)
        {
            LogsAppUi()->SetCustomControl(1);
        }    
    CLogsBaseView::ViewActivatedL(aPrevViewId,aCustomMessageId,aCustomMessage);
    }


//  End of File

