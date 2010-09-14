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
*     Logs "GPRS counter" view container control class implementation
*
*/


// INCLUDE FILES
#include <AknIconArray.h>		// icon array
#include <aknnavide.h>
#include <akntabgrp.h>			// tab group
#include <AknsConstants.h>      // for skinned icons

#include <logs.rsg>				// note structure
#include <logs.mbg>				// Index file for Logs icons.
#include "CGprsCtControlContainer.h"
#include "CGprsCtView.h"
#include "CLogsGprsCtAdapter.h"
#include "CLogsEngine.h"
#include "CLogsNaviDecoratorWrapper.h"

#include "LogsIcons.hrh"
#include "Logs.hrh"

#include "LogsUID.h"
#include <csxhelp/log.hlp.hrh>

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
// CGprsCtControlContainer::NewL
// ----------------------------------------------------------------------------
//
CGprsCtControlContainer* CGprsCtControlContainer::NewL( 
    CLogsBaseView* aAppView,
    const TRect& aRect )
    {
    CGprsCtControlContainer* self = new( ELeave ) CGprsCtControlContainer( aAppView );
    self->SetMopParent( aAppView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();  // self
    return self;
    }


// ----------------------------------------------------------------------------
// CGprsCtControlContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CGprsCtControlContainer::ConstructL( const TRect& aRect )
    {
    BaseConstructL();
    iListBox = new( ELeave ) CAknDoubleLargeStyleListBox; 
    iListBox->ConstructL( this, EEikListBoxMultipleSelection );

	AddControlContainerIconsL();

    CLogsGprsCtAdapter* adapter = CLogsGprsCtAdapter::NewL( iAppView->
                                    Engine()->SharedDataL() );
    iListBox->Model()->SetItemTextArray( adapter );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    MakeTitleL( R_GPRS_COUNTER_TITLE_TEXT );

    CAknNavigationDecorator* decoratedTabGroup = NavigationTabGroupL( R_LOGS_SUB_APP_PANE_TAB_GROUP,
                                                                      CLogsNaviDecoratorWrapper::InstanceL() );

    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( decoratedTabGroup->
                                    DecoratedControl() );
    tabGroup->SetActiveTabById( ESubAppGprsTabId );

    MakeScrollArrowsL( iListBox );
    SetRect( aRect );
    ActivateL();
    }


// ----------------------------------------------------------------------------
// CGprsCtControlContainer::CGprsCtControlContainer
// ----------------------------------------------------------------------------
//
CGprsCtControlContainer::CGprsCtControlContainer( CLogsBaseView* aAppView ):
    CLogsBaseControlContainer( aAppView ),
    iAppView( aAppView )
    {
    }

// ----------------------------------------------------------------------------
// CGprsCtControlContainer::~CGprsCtControlContainer
// ----------------------------------------------------------------------------
//
CGprsCtControlContainer::~CGprsCtControlContainer()
    {
    delete iListBox;
    }


// ----------------------------------------------------------------------------
// CGprsCtControlContainer::ComponentControl
//
//  Returns the child controls at aIndex
// ----------------------------------------------------------------------------
//
CCoeControl* CGprsCtControlContainer::ComponentControl
        ( TInt /*aIndex*/) const
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CGprsCtControlContainer::SizeChanged
//
// Called when control's size changed
// ----------------------------------------------------------------------------
//
void CGprsCtControlContainer::SizeChanged()
    {
    if( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }

// ----------------------------------------------------------------------------
// CGprsCtControlContainer::ListBox
//
// Returns the iListBox
// ----------------------------------------------------------------------------
//
CAknDoubleLargeStyleListBox* CGprsCtControlContainer::ListBox()
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CGprsCtControlContainer::UpdateL
// ----------------------------------------------------------------------------
//
void CGprsCtControlContainer::UpdateL()
    {
    iListBox->HandleItemAdditionL();
    }

// ----------------------------------------------------------------------------
// CGprsCtControlContainer::GetHelpContext
// ----------------------------------------------------------------------------
//
void CGprsCtControlContainer::GetHelpContext( TCoeHelpContext& aContext ) const
	{
    aContext.iMajor = TUid::Uid( KLogsAppUID3 );
    aContext.iContext = KGPRS_HLP_MAIN;
    }

// ----------------------------------------------------------------------------
// CGprsCtControlContainer::AddControlContainerIconsL
// ----------------------------------------------------------------------------
//
void CGprsCtControlContainer::AddControlContainerIconsL()
	{
	if( iListBox->ItemDrawer()->ColumnData()->IconArray() != NULL )
		{
		CArrayPtr<CGulIcon>* iconArray = iListBox->ItemDrawer()->ColumnData()->IconArray();
		delete iconArray;
		iconArray = NULL;
		iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray);  
		}

	// Load and assign icons to the list box control; 2 is number of icons
	CAknIconArray* icons = new( ELeave ) CAknIconArray( KGprsCtCCNrOfItems );

	iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );

	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogGprsSent      //AknsConstants.h
			,	qgn_prop_log_gprs_sent          //LogsIcons.hrh
			,	qgn_prop_log_gprs_sent_mask     //LogsIcons.hrh
			);

	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogGprsReceived
			,	qgn_prop_log_gprs_received
			,	qgn_prop_log_gprs_received_mask
			);
	}
	
// ----------------------------------------------------------------------------
// CGprsCtControlContainer::FocusChanged
//
// This is needed to hand focus changes to list. Otherwise animations are not displayed.
// ----------------------------------------------------------------------------
//
void CGprsCtControlContainer::FocusChanged(TDrawNow /* aDrawNow */ )
    {
    if( iListBox)
        {
        iListBox->SetFocus( IsFocused() );
        }
    }	

//  End of File
