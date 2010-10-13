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
*     Call Timers view
*
*/


//  INCLUDE FILES
#include <eikmenub.h>

#include <Logs.rsg>

#include "CCtView.h"
#include "CCtControlContainer.h"
#include "CLogsAppUi.h"
#include "CLogsEngine.h"
#include "MLogsSharedData.h"
#include "MLogsSystemAgent.h"
#include "MLogsCallStatus.h"
#include "MLogsAocUtil.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CCtView::CCtView
// ----------------------------------------------------------------------------
//
CCtView::CCtView() :
        iTimersAls( EAllLines )
    {
    }

// ----------------------------------------------------------------------------
// CCtView::NewL
// ----------------------------------------------------------------------------
//
CCtView* CCtView::NewL()
    {
    CCtView* self = new( ELeave ) CCtView;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CCtView::~CCtView
// ----------------------------------------------------------------------------
//
CCtView::~CCtView()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
    }

// ----------------------------------------------------------------------------
// CCtView::ProcessPointerEventL
//
// Handler for pointer events, when the current focused item is tapped
// ----------------------------------------------------------------------------
// 
void CCtView::ProcessPointerEventL( TInt /* aIndex */)
	{
	// Open the context sensitive menu
	ProcessCommandL( EAknSoftkeyContextOptions );    
	}
	
// ----------------------------------------------------------------------------
// CCtView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CCtView::HandleCommandL( TInt aCommandId )
    {
    TBool activeCall( EFalse );

    switch ( aCommandId )
        {
        case EAocCtCmdMenuClear:            
            iContainer->CallStatus().CallIsActive( activeCall );
            if( !activeCall )
                {              
                TInt ret = KErrNotFound;
                
                SetInputBlockerL();
                TRAPD(err, ret = iContainer->AocUtil().AskSecCodeL() );
                RemoveInputBlocker();
                
                User::LeaveIfError(err);
                 
                if( ret == KErrNone)
                    {          
                    Engine()->SharedDataL()->ClearCallTimers();
                    iContainer->UpdateListBoxContentL();    
                    }                 
                }
            else
                {
                LogsAppUi()->HandleCommandL( ELogsCmdFailedDueActiveCall );
                }
            break;

        case EAocCtCmdMenuFilterAls:
            if( CmdSetAlsFilterL() )
                {
                iContainer->UpdateListBoxContentL();
                }
            break;

        case EAocCtCmdMenuDetailsVoip:
        case EAocCtCmdMenuDetailsCs:        
            iContainer->ShowDurationDetailsL( aCommandId );            
            break; 
            
        case ELogsCmdHandleMSK:
            OkOptionCtMenuL();        
            break;            

        default:
            CLogsBaseView::HandleCommandL( aCommandId );
        }
    }

// ----------------------------------------------------------------------------
// CCtView::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CCtView::DynInitMenuPaneL( 
    TInt aResourceId, 
    CEikMenuPane* aMenuPane )
    {
    CLogsBaseView::DynInitMenuPaneL( aResourceId, aMenuPane );

    if( aResourceId == R_AOCCT_CT_MENU_TOP )
        {
        // 1. If no ALS support, remove FilterAls support from the menu
        if( ! Engine()->SystemAgentL()->AlsSupport() )
            {
            aMenuPane->DeleteMenuItem( EAocCtCmdMenuFilterAls );
            }

        // 2. If no VOIP support, remove duration details support from the menu        
        if ( !Engine()->SharedDataL()->IsVoIPEnabledInPlatform() ) 
            {
            aMenuPane->DeleteMenuItem( EAocCtCmdMenuDetails );
            }
        }
    }

// ----------------------------------------------------------------------------
// CCtView::Id
// ----------------------------------------------------------------------------
//
TUid CCtView::Id() const
    {
    return TUid::Uid( ECtViewId );
    }

// ----------------------------------------------------------------------------
// CCtView::HandleClientRectChange
// ----------------------------------------------------------------------------
//
void CCtView::HandleClientRectChange()
    {
    if (iContainer)
        {
        iContainer->SetRect( ClientRect() );  
        }
    }

// ----------------------------------------------------------------------------
// CCtView::DoActivateL
// ----------------------------------------------------------------------------
//
void CCtView::DoActivateL( 
    const TVwsViewId& aPrevViewId,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    if( iContainer == NULL )
        {
        iContainer = CCtControlContainer::NewL( this, ClientRect() );
        AppUi()->AddToViewStackL( *this, iContainer );
        }
    
    Engine()->SharedDataL()->SetObserver( this );
    iContainer->ListBox()->SetListBoxObserver( this );

    LogsAppUi()->SetPreviousViewId( aPrevViewId.iViewUid );
    LogsAppUi()->SetCurrentViewId( Id() );
    
    // Just to make sure the inputblocker is not on
    RemoveInputBlocker();
    }

// ----------------------------------------------------------------------------
// CCtView::StateChangedL
// ----------------------------------------------------------------------------
//
void CCtView::StateChangedL( MLogsStateHolder* /*aHolder*/ )
    {
    if( iContainer )
        {
        iContainer->UpdateListBoxContentL();
        }
    }

// ----------------------------------------------------------------------------
// CCtView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CCtView::DoDeactivate()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CCtView::ConstructL
// ----------------------------------------------------------------------------
//
void CCtView::ConstructL()
    {
    BaseConstructL( R_AOCCT_CT_VIEW );
    }

// ----------------------------------------------------------------------------
// CCtView::OkOptionCtMenuL
// ----------------------------------------------------------------------------
//
void CCtView::OkOptionCtMenuL()
    {
    LaunchPopupMenuL( R_AOCCT_CT_OK_MENUBAR );
    }

// ----------------------------------------------------------------------------
// CCtView::CmdSetAlsFilterL
// ----------------------------------------------------------------------------
//
TBool CCtView::CmdSetAlsFilterL()
    {
    return iContainer->AlsSettingQueryL( iTimersAls );
    }

// ----------------------------------------------------------------------------
// CCtView::AlsLine
// ----------------------------------------------------------------------------
//
const TAlsEnum& CCtView::AlsLine() const
    {
    return iTimersAls;
    }
    
// ----------------------------------------------------------------------------
// CCtView::SetAlsLine
// ----------------------------------------------------------------------------
//
void CCtView::SetAlsLine( TAlsEnum aTimerAls ) 
    {
    iTimersAls = aTimerAls;
    }
    
    

//  End of File
