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
*     Logs "Aplication List" view container control class implementation
*
*/


// INCLUDE FILES
#include <AknIconArray.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <AknsConstants.h>      // for skinned icons

#include <logs.rsg>
#include <logs.mbg>      // Index file for Logs icons.

#include "CLogsAppListControlContainer.h"
#include "CLogsBaseView.h"

#include "Logs.hrh"
#include "LogsIcons.hrh"

#include "LogsUID.h" 
#include <csxhelp/log.hlp.hrh>
#include "CLogsNaviDecoratorWrapper.h"

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
// CLogsAppListControlContainer::NewL
// ----------------------------------------------------------------------------
//
CLogsAppListControlContainer* CLogsAppListControlContainer::NewL
        ( CLogsBaseView* aAppView, const TRect& aRect )
    {
    CLogsAppListControlContainer* self = new ( ELeave )
                            CLogsAppListControlContainer( aAppView );
    self->SetMopParent( aAppView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::~CLogsAppListControlContainer
// ----------------------------------------------------------------------------
//
CLogsAppListControlContainer::~CLogsAppListControlContainer()
    {
    delete iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::CLogsAppListControlContainer
// ----------------------------------------------------------------------------
//
CLogsAppListControlContainer::CLogsAppListControlContainer
	(	CLogsBaseView* aAppView
	)
	: CLogsBaseControlContainer( aAppView  ), iAppView( aAppView )
    {
    }

// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsAppListControlContainer::ConstructL( const TRect& aRect )
    {
    BaseConstructL();
    CreateListBoxL();
    
    MakeScrollArrowsL( iListBox );

    // activate control and sub-controls
    SetRect( aRect );
    ActivateL();
    
    }

// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CLogsAppListControlContainer::ComponentControl
        ( TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::SizeChanged
// ----------------------------------------------------------------------------
//
void CLogsAppListControlContainer::SizeChanged()
    {
    if( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }

// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::ListBox
// ----------------------------------------------------------------------------
//
CAknDoubleLargeStyleListBox* CLogsAppListControlContainer::ListBox()
    {
    return iListBox;
    }
    
// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::CreateListBoxL
// ----------------------------------------------------------------------------
//
void CLogsAppListControlContainer::CreateListBoxL()
    {
    if( iListBox )
        {
        delete iListBox;
        iListBox = NULL;
        }

    iListBox = new( ELeave ) CAknDoubleLargeStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    CreateListBoxContentsL();    
    }


// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::CreateListBoxContentsL
// ----------------------------------------------------------------------------
//
void CLogsAppListControlContainer::CreateListBoxContentsL()
    {
    if( !iListBox )
        {
        return;
        }
        
    //  Creation of the items in which the temporary text will be inserted
    CDesCArrayFlat* items = new( ELeave )CDesCArrayFlat( KAppListCCNrOfItems);
    CleanupStack::PushL( items );

    // STM main view
    MakeListBoxLineL( items, KFirstIconAndTab, R_SUB_APP_CALL_REGISTER );

    // Call Timers view
    MakeListBoxLineL( items, KSecondIconAndTab, R_SUB_APP_CALL_TIMERS );

    // GPRS counter view
    MakeListBoxLineL( items, KThirdIconAndTab, R_SUB_APP_GPRS_COUNTER );

    //  Creation of the model, which handles the text items
    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop();    // items
    iListBox->HandleItemAdditionL();

	AddControlContainerIconsL();

    MakeTitleL( R_LOGS_TITLE_TEXT );

    CAknNavigationDecorator* decoratedTabGroup = NavigationTabGroupL( R_LOGS_NAVI_PANE_TAB_GROUP, 
                                                                      CLogsNaviDecoratorWrapper::InstanceL() );

    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( decoratedTabGroup->
                                    DecoratedControl() );
	tabGroup->SetActiveTabById( EAppListTabId );
  
    }
    

// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::GetHelpContext
// ----------------------------------------------------------------------------
//
void CLogsAppListControlContainer::GetHelpContext( TCoeHelpContext& aContext ) const
	{
    aContext.iMajor = TUid::Uid( KLogsAppUID3 );
    aContext.iContext = KLOGS_HLP_COUNTER;
    }

// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::AddControlContainerIconsL
// ----------------------------------------------------------------------------
//
void CLogsAppListControlContainer::AddControlContainerIconsL()
	{
	if( iListBox->ItemDrawer()->FormattedCellData()->IconArray() != NULL )
		{
		CArrayPtr<CGulIcon>* iconArray = iListBox->ItemDrawer()->FormattedCellData()->IconArray();
		delete iconArray;
		iconArray = NULL;
		iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray);        
		}

	// Load and assign icons to the list box control; 3 is number of icons
	CAknIconArray* icons = new( ELeave ) CAknIconArray( KAppListCCNrOfItems );

	iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( icons );

	// First icon & mask  ( Recent Calls )
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogCallsSub
			,	qgn_prop_log_calls_sub
			,	qgn_prop_log_calls_sub_mask
			);

	// Second icon & mask  ( Call Timers )
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogTimersSub
			,	qgn_prop_log_timers_sub
			,	qgn_prop_log_timers_sub_mask
			);

	// Third icon & mask  ( GPRS counter )
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogGprsSub
			,	qgn_prop_log_gprs_sub
			,	qgn_prop_log_gprs_sub_mask
			);
	}
	
// ----------------------------------------------------------------------------
// CLogsAppListControlContainer::FocusChanged
//
// This is needed to hand focus changes to list. Otherwise animations are not displayed.
// ----------------------------------------------------------------------------
//
void CLogsAppListControlContainer::FocusChanged(TDrawNow /* aDrawNow */ )
    {
    if( iListBox)
        {
        iListBox->SetFocus( IsFocused() );
        }
    }	
	

//  End of File
