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
*     Logs application "Sub application list" view class implementation
*
*/


// INCLUDE FILES
#include <AknQueryDialog.h>
#include <featmgr.h>

#include <logs.rsg>

#include "CLogsSubAppListView.h"
#include "CLogsSubAppListControlContainer.h"
#include "CLogsAppUi.h"
#include "CLogsEngine.h"
#include "MLogsClearLog.h"
#include "MLogsSharedData.h"


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
// CLogsSubAppListView::NewL
// ----------------------------------------------------------------------------
//
CLogsSubAppListView* CLogsSubAppListView::NewL()
    {
    CLogsSubAppListView* self = new ( ELeave ) CLogsSubAppListView;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListView::~CLogsSubAppListView
// ----------------------------------------------------------------------------
//
CLogsSubAppListView::~CLogsSubAppListView()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLogsSubAppListView::HandleCommandL( TInt aCommandId )
    {
    switch( aCommandId )
        {
        case ELogsCmdMenuClearRecentRegisters:
            CmdClearRecentListsL();
            break;

        case ELogsCmdMenuOpen:
            LogsAppUi()->CmdOkL( iContainer->ListBox()->CurrentItemIndex() );
            break;

        default:
            CLogsBaseView::HandleCommandL( aCommandId );
        }
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListView::Id
// ----------------------------------------------------------------------------
//
TUid CLogsSubAppListView::Id() const
    {
    return TUid::Uid( ELogSubAppListViewId );
    }


// ----------------------------------------------------------------------------
// CLogsSubAppListView::HandleClientRectChange
// ----------------------------------------------------------------------------
//
void CLogsSubAppListView::HandleClientRectChange()
    {
    if (iContainer)
        {
        iContainer->SetRect( ClientRect() );  
        }
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLogsSubAppListView::DoActivateL( const TVwsViewId& aPrevViewId, 
          TUid aCustomMessageId, const TDesC8& /*aCustomMessage*/ )
    {
    //Reset ProvideOnlyRecentViews always when other than CLogsRecentListView is activated. 
    LogsAppUi()->SetProvideOnlyRecentViews( EFalse );
    
    if( iContainer == NULL )
        {
        iContainer = CLogsSubAppListControlContainer::NewL( this, ClientRect() );
        AppUi()->AddToViewStackL( *this, iContainer );
        }

    if ( aCustomMessageId.iUid == ELogsViewActivationBackground )
        {
        // when activating in background, restore the initial selection
        iSelectedLine = 0;
        }

    //Set focus to correct row if we return from subselection view
    switch( aPrevViewId.iViewUid.iUid )
        {
        case EStmReceivedListViewId:
            iSelectedLine = 1;
            break;    
        case EStmDialledListViewId:
            iSelectedLine = 2;
            break;    
        case EStmMissedListViewId:
        default:
            iSelectedLine = 0;    
        }

	iContainer->ListBox()->SetCurrentItemIndex( iSelectedLine );    
    Engine()->SharedDataL()->SetObserver( this );
    iContainer->ListBox()->SetListBoxObserver( this );

    LogsAppUi()->SetPreviousViewId( aPrevViewId.iViewUid );
    LogsAppUi()->SetCurrentViewId( Id() );    

    if ( aCustomMessageId.iUid == ELogsViewActivationBackground )
        {
        // We have activated this view because we are putting the 
        // Logs into background so this view is waiting when Logs is again foregrounded.
        AppUi()->SetCustomControl(0); 
        AppUi()->HideInBackground(); 
        }
    
    // Just to make sure the inputblocker is not on
    RemoveInputBlocker();
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLogsSubAppListView::DoDeactivate()
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
// CLogsSubAppListView::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsSubAppListView::ConstructL()
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdSimpleLogs ) )
        {
        BaseConstructL( R_LOGS_SUB_APP_LIST_SIMPLE_VIEW );
        }
    else
        {
        BaseConstructL( R_LOGS_SUB_APP_LIST_VIEW );
        }
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListView::CLogsSubAppListView
// ----------------------------------------------------------------------------
//
CLogsSubAppListView::CLogsSubAppListView()
    {
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListView::CmdClearRecentListsL
// ----------------------------------------------------------------------------
//
void CLogsSubAppListView::CmdClearRecentListsL()
    {
    CAknQueryDialog* queryDlg = CAknQueryDialog::NewL();
    
    if( queryDlg->ExecuteLD( R_CLEAR_RECENT_LISTS_CONFIRMATION_QUERY ) )
        {
        Engine()->ClearLogsL()->ClearModelL( ELogsAllRecent );
        Engine()->SharedDataL()->NewMissedCalls( ETrue );
        if( iContainer )
            {
            iContainer->CreateListBoxContentsL();
            }
        }
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListView::StateChangedL
// ----------------------------------------------------------------------------
//
void CLogsSubAppListView::StateChangedL( MLogsStateHolder* /*aHolder*/ )
    {
    if( iContainer )
        {
        iContainer->CreateListBoxL();
        iContainer->ListBox()->SetListBoxObserver( this );
        }
    }


//  End of File

