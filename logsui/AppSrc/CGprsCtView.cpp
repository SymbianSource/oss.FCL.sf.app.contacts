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
*     GPRS counters view
*
*/


//  INCLUDE FILES
#include <aknnotedialog.h>

#include <Logs.rsg>

#include "CGprsCtView.h"
#include "CGprsCtControlContainer.h"
#include "CLogsAppUi.h"
#include "CLogsEngine.h"
#include "CLogsAocObjFactory.h"
#include "MLogsSystemAgent.h"
#include "MLogsSharedData.h"
#include "MLogsAocUtil.h" 

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CGprsCtView::NewL
// ----------------------------------------------------------------------------
//
CGprsCtView* CGprsCtView::NewL()
    {
    CGprsCtView* self = new( ELeave ) CGprsCtView;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CGprsCtView::~CGprsCtView
// ----------------------------------------------------------------------------
//
CGprsCtView::~CGprsCtView()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        }
        
    delete iAocUtil;
    }

// ----------------------------------------------------------------------------
// CGprsCtView::ConstructL
// ----------------------------------------------------------------------------
//
void CGprsCtView::ConstructL()
    {
    BaseConstructL( R_LOGS_GPRS_COUNTER_VIEW );
    iAocUtil = CLogsAocObjFactory::AocUtilL();
    }

// ----------------------------------------------------------------------------
// CGprsCtView::ProcessPointerEventL
//
// Handler for pointer events, when the current focused item is tapped
// ----------------------------------------------------------------------------
// 
void CGprsCtView::ProcessPointerEventL( TInt /* aIndex */)
	{
	// Open the context sensitive menu
	ProcessCommandL( EAknSoftkeyContextOptions );    
	}

// ----------------------------------------------------------------------------
// CGprsCtView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CGprsCtView::HandleCommandL( TInt aCommandId )
    {
    switch ( aCommandId )
        {
        case ELogsCmdMenuClearGprs:
            if( Engine()->SystemAgentL()->GprsConnectionActive() )
                {
                CAknNoteDialog* noteDlg = new( ELeave ) CAknNoteDialog(
                                        CAknNoteDialog::ENoTone, 
                                        CAknNoteDialog::ELongTimeout);
                noteDlg->ExecuteLD( R_COUNTER_CLEARING_PROHIB_NOTE );
                }
            else
                {
                if( iAocUtil->AskSecCodeL() == KErrNone )
                    {
                    Engine()->SharedDataL()->ClearGprsCounters();
                    if( iContainer )
                        {
                        iContainer->UpdateL();
                        }
                    }        
                }
            break;

        case ELogsCmdHandleMSK:
            OkOptionCtMenuL();        
            break;            

        default:
            CLogsBaseView::HandleCommandL( aCommandId );
        }
    }


// ----------------------------------------------------------------------------
// CGprsCtView::Id
// ----------------------------------------------------------------------------
//
TUid CGprsCtView::Id() const
    {
    return TUid::Uid( EGprsCounterViewId );
    }


// ----------------------------------------------------------------------------
// CGprsCtView::HandleClientRectChange
// ----------------------------------------------------------------------------
//
void CGprsCtView::HandleClientRectChange()
    {
    if (iContainer)
        {
        iContainer->SetRect( ClientRect() );  
        }
    }


// ----------------------------------------------------------------------------
// CGprsCtView::DoActivateL
// ----------------------------------------------------------------------------
//
void CGprsCtView::DoActivateL( 
    const TVwsViewId& aPrevViewId,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    if( ! iContainer )
        {
        iContainer = CGprsCtControlContainer::NewL( this, ClientRect() );
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
// CGprsCtView::StateChangedL
// ----------------------------------------------------------------------------
//
void CGprsCtView::StateChangedL( MLogsStateHolder* /*aHolder*/ )
    {
    if( iContainer )
        {
        iContainer->UpdateL();
        }
    }

// ----------------------------------------------------------------------------
// CGprsCtView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CGprsCtView::DoDeactivate()
    {
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CGprsCtView::OkOptionCtMenuL
// ----------------------------------------------------------------------------
//
void CGprsCtView::OkOptionCtMenuL()
    {
    LaunchPopupMenuL( R_LOGS_GPRS_COUNTER_OK_MENUBAR );
    }



//  End of File
