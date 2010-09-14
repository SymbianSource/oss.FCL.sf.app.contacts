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
*     Logs "Event list" view container control class implementation
*
*/


// INCLUDE FILES
#include <AknIconArray.h>
#include <eikclbd.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <AknsConstants.h>  //for skinned icons

#include <logs.rsg>
#include <logs.mbg>

#include "CLogsEventListControlContainer.h"
#include "CLogsEventListView.h"
#include "CLogsAppUi.h"
#include "CLogsEventListAdapter.h"
#include "CLogsEngine.h"
#include "MLogsModel.h"
#include "MLogsSharedData.h"
#include "CLogsNaviDecoratorWrapper.h"

#include "LogsIcons.hrh"

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
// CLogsEventListControlContainer::NewL
// ----------------------------------------------------------------------------
//
CLogsEventListControlContainer* CLogsEventListControlContainer::NewL( 
    CLogsEventListView* aView, 
    const TRect& aRect )
    {
    CLogsEventListControlContainer* self = new( ELeave ) 
                CLogsEventListControlContainer( aView );
    self->SetMopParent( aView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::~CLogsEventListControlContainer
// ----------------------------------------------------------------------------
//
CLogsEventListControlContainer::~CLogsEventListControlContainer()
    {
    delete iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::CLogsEventListControlContainer
// ----------------------------------------------------------------------------
//
CLogsEventListControlContainer::CLogsEventListControlContainer(
    CLogsEventListView* aView ) : 
        CLogsBaseControlContainer( aView ),
        iView( aView )
    {
    }


// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CLogsEventListControlContainer::ComponentControl(
    TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::SizeChanged
// ----------------------------------------------------------------------------
//
void CLogsEventListControlContainer::SizeChanged()
    {
    if( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsEventListControlContainer::ConstructL( const TRect& aRect)
    {
    BaseConstructL();

    // Constructing a compound control or a window-owning control.
    iListBox = new ( ELeave ) CAknSingleGraphicHeadingStyleListBox();
    iListBox->ConstructL( this, EEikListBoxMultipleSelection );

    AddControlContainerIconsL();
    CLogsEventListAdapter* adapter = CLogsEventListAdapter::NewL( this );

    iListBox->Model()->SetItemTextArray( adapter );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );

    if( iView->Engine()->Model( ELogsMainModel )->Count() == 0 )
        {
        UpdateEmptyViewTextL();
        }
    else
        {
        InitListBoxL();
        }

    MakeScrollArrowsL( iListBox );

    MakeTitleL( R_LOGS_TITLE_TEXT );
    
    CAknNavigationDecorator* decoratedTabGroup = NavigationTabGroupL( R_LOGS_NAVI_PANE_TAB_GROUP, 
                                                                      CLogsNaviDecoratorWrapper::InstanceL() );

    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( 
                                 decoratedTabGroup->DecoratedControl() );
    tabGroup->SetActiveTabById( EEventListTabId );
    
    SetRect( aRect );
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::ListBox
// ----------------------------------------------------------------------------
//
CAknSingleGraphicHeadingStyleListBox* CLogsEventListControlContainer::ListBox()
    {
    return iListBox;
    }
    
// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::View
// ----------------------------------------------------------------------------
//
CLogsEventListView* CLogsEventListControlContainer::View()
    {
    return iView;
    }

// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::InitListBoxL
// ----------------------------------------------------------------------------
//
void CLogsEventListControlContainer::InitListBoxL()
    {
    TInt current( iView->EventListCurrent() );
    TInt top( iView->EventListTop() );

    if (top < 0 || current < 0)   
        {
        // In case top and current are illegal, set top and current to zero. 
        // For example it occasionally happens when switching quickly back and forth 
        // between Log and Main view, that top and current are -1 (KErrNotFound).
        // See error "EAJA-7389Y9: Crash when changing views quickly" 
        iView->SetEventListTop( 0 );
        iListBox->SetCurrentItemIndex( 0 ); 
        }
    else if( top > current ) 
        { 
        // user has browsed upwards
        top = current;
        iView->SetEventListTop( top );
        iListBox->SetCurrentItemIndex( current );
        }
    else 
        {  
        // user has browsed downwards
        // past "one" window size
        if( current - top > iListBox->ItemHeight() ) 
            {
            top = current - iListBox->ItemHeight();
            iView->SetEventListTop( top );
            }
        iListBox->SetCurrentItemIndex( current );
        iListBox->SetTopItemIndex( top );
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::UpdateEmptyViewTextL
//
// Show "Retrieving", "No data", "No data due to filter selection
// or "Logging is not enabled"
// ----------------------------------------------------------------------------
//
void CLogsEventListControlContainer::UpdateEmptyViewTextL()
    {
    // Construct informative text based on following constraints:
    // * logging on/off
    // * event reading finished yes/no
    // * filter selected yes/no
    //      
    if (iView->Engine()->SharedDataL()->IsLoggingEnabled())
        {
        if( iView->State() == EStateUndefined || iView->State() == EStateInitializing )
            {
            MakeEmptyTextListBoxL( iListBox, R_LOGS_EVENTVIEW_FETCHING_TEXT );
            }
        /// If filter is "on" ( != all communications) and there are no events
        /// in the log DB display text "No events due to filter selection".
        else if( iView->CurrentFilter() != ELogsFilterAllComms
                  && iView->Engine()->Model( ELogsMainModel )->Count() == 0 )
            {                                   
            MakeEmptyTextListBoxL( iListBox, R_LOGS_EVENTVIEW_NO_DATA_DTFS_TEXT );
            }
        else
            {
            /// If filter is "off" (all communications) and there are no events
            /// in the log DB display text "No events".
            MakeEmptyTextListBoxL( iListBox, R_LOGS_EVENTVIEW_NO_DATA_TEXT );
            }  
        }
     // If logging is off, show informative text
    else   
        {
        MakeEmptyTextListBoxL( iListBox, R_QTN_LOGS_LOGGING_OFF);
        }
    }


// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::UpdateL
// ----------------------------------------------------------------------------
//
void CLogsEventListControlContainer::UpdateL()
    {
    iListBox->HandleItemAdditionL();
    if( iListBox->Model()->NumberOfItems() == 0 )
        {
        UpdateEmptyViewTextL();
        }
    }

// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::GetHelpContext
// ----------------------------------------------------------------------------
//
void CLogsEventListControlContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = TUid::Uid( KLogsAppUID3 );
    aContext.iContext = KLOGS_HLP_MAIN;
    }

// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::AddControlContainerIconsL
// ----------------------------------------------------------------------------
//
void CLogsEventListControlContainer::AddControlContainerIconsL()
    {
    if( iListBox->ItemDrawer()->ColumnData()->IconArray() != NULL )
        {
        CArrayPtr<CGulIcon>* iconArray = iListBox->ItemDrawer()->ColumnData()->IconArray();
        delete iconArray;
        iconArray = NULL;
        iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray);  
        }

    // Load and assign icons to the list box control; 3 is number of icons
    CAknIconArray* icons = new( ELeave ) CAknIconArray( KEventListCCNrOfItems );

    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );

    // First icon & mask.
    AddIconL(   icons
            ,   KLogsIconFile
            ,   KAknsIIDQgnPropLogIn  //KAknsIIDNone   
            ,   icon_prop_log_in
            ,   icon_prop_log_in_mask
            );

    // Second icon & mask.
    AddIconL(   icons
            ,   KLogsIconFile
            ,   KAknsIIDQgnPropLogOut //KAknsIIDNone   
            ,   icon_prop_log_out
            ,   icon_prop_log_out_mask
            );

    // Third icon & mask.
    AddIconL(   icons
            ,   KLogsIconFile
            ,   KAknsIIDQgnPropLogMissed //KAknsIIDNone   
            ,   icon_prop_log_missed
            ,   icon_prop_log_missed_mask
            );
    }
    
// ----------------------------------------------------------------------------
// CLogsEventListControlContainer::FocusChanged
//
// This is needed to hand focus changes to list. Otherwise animations are not displayed.
// ----------------------------------------------------------------------------
//
void CLogsEventListControlContainer::FocusChanged(TDrawNow /* aDrawNow */ )
    {
    if( iListBox)
        {
        iListBox->SetFocus( IsFocused() );
        }
    }

//  End of File
