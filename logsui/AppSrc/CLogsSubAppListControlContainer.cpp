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
*     Logs "Sub application list" view container control class implementation
*
*/


// INCLUDE FILES
#include <StringLoader.h> 
#include <AknIconArray.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <AknsConstants.h>  //for skinned icons
#include <featmgr.h>

#include <Logs.rsg>
#include <logs.mbg>

#include "CLogsSubAppListControlContainer.h"
#include "CLogsBaseView.h"
#include "CLogsEngine.h"
#include "MLogsSharedData.h"
#include "CLogsNaviDecoratorWrapper.h"

#include "Logs.hrh"
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
// CLogsSubAppListControlContainer::NewL
// ----------------------------------------------------------------------------
//
CLogsSubAppListControlContainer* CLogsSubAppListControlContainer::NewL
        (   CLogsBaseView* aAppView, const TRect& aRect)
    {
    CLogsSubAppListControlContainer* self = new( ELeave ) 
                         CLogsSubAppListControlContainer( aAppView );
    self->SetMopParent( aAppView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::CLogsSubAppListControlContainer
// ----------------------------------------------------------------------------
//
CLogsSubAppListControlContainer::~CLogsSubAppListControlContainer()
    {
    delete iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::CLogsSubAppListControlContainer
// ----------------------------------------------------------------------------
//
CLogsSubAppListControlContainer::CLogsSubAppListControlContainer
	(	CLogsBaseView* aAppView
	)
	: CLogsBaseControlContainer( aAppView ), iAppView( aAppView )
    {
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsSubAppListControlContainer::ConstructL( const TRect& aRect )
    {
    BaseConstructL();

    if ( FeatureManager::FeatureSupported( KFeatureIdSimpleLogs ) )
        {
        // use alt. title and do not create navi decorator for simple logs
        MakeTitleL( R_LOGS_TITLE_TEXT );
        }
    else
        {
        MakeTitleL( R_STM_TITLE_TEXT );

        CAknNavigationDecorator* decoratedTabGroup = NavigationTabGroupL( R_LOGS_SUB_APP_PANE_TAB_GROUP, 
                                                                          CLogsNaviDecoratorWrapper::InstanceL() );

        CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( decoratedTabGroup->
                                        DecoratedControl() );
        tabGroup->SetActiveTabById( ESubAppRecentsTabId );
        }

    SetRect( aRect );
    CreateListBoxL();
    }


// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CLogsSubAppListControlContainer::ComponentControl
        ( TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::SizeChanged
// ----------------------------------------------------------------------------
//
void CLogsSubAppListControlContainer::SizeChanged()
    {
    if( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }


// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::ListBox
// ----------------------------------------------------------------------------
//
CAknDoubleLargeStyleListBox* CLogsSubAppListControlContainer::ListBox()
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::CreateListBoxL
// ----------------------------------------------------------------------------
//
void CLogsSubAppListControlContainer::CreateListBoxL()
    {
    if( iListBox )
        {
        delete iListBox;
        iListBox = NULL;
        }

    iListBox = new( ELeave ) CAknDoubleLargeStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionList );

    CreateListBoxContentsL();

	//These need to be here as this function is called from CLogsSubAppListView too. Otherwise
	//screen is not updated.
    MakeScrollArrowsL( iListBox );
    SizeChanged();
    DrawNow();
    ActivateL();
    }


// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::CreateListBoxContentsL
// ----------------------------------------------------------------------------
//
void CLogsSubAppListControlContainer::CreateListBoxContentsL()
    {
    if( !iListBox )
        {
        return;
        }

    TInt newMissedCalls( 0 );
    HBufC* textBuf = NULL;

    //  Creation of the items, in which the temporary text will be inserted
    CDesCArrayFlat* items = new( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( items );
    
    // Fix for EILU-757CLC:
    // Only fetch the new missed calls amount if logging is turned on
    // (otherwise has the inital value of '0' and it wont be displayed)
    if (iAppView->Engine()->SharedDataL()->IsLoggingEnabled())
        {
        newMissedCalls = iAppView->Engine()->SharedDataL()->NewMissedCalls();   
        }
     
    //  Missed Calls second line has text "1 Missed call", if 1 missed call
    if( newMissedCalls == 1 )
        {
        textBuf = StringLoader::LoadLC( R_STM_TEXT_ONE_NEW_CALL );
        MakeListBoxLineL( items, KFirstIconAndTab, R_STM_APP_MISSED_RECENT,
                          *textBuf );
        CleanupStack::PopAndDestroy(); // textBuf
        }
    //  Missed Calls second line has text "n Missed calls", if n missed calls
    else if( newMissedCalls > 1 )
        {
        textBuf = StringLoader::LoadLC( R_STM_TEXT_N_NEW_CALLS, newMissedCalls );
        MakeListBoxLineL( items, KFirstIconAndTab, R_STM_APP_MISSED_RECENT,
                          *textBuf );
        CleanupStack::PopAndDestroy(); // textBuf
        }
    //  Missed Calls second line is empty, if no new missed calls
    else
        {
        MakeListBoxLineL( items, KFirstIconAndTab, R_STM_APP_MISSED_RECENT );
        }
    
    //  Received Calls line
    MakeListBoxLineL( items, KSecondIconAndTab, R_STM_APP_RECEIVED_RECENT );

    //  Dialled Calls line
    MakeListBoxLineL( items, KThirdIconAndTab, R_STM_APP_DIALLED_RECENT );

    //  Creation of the model, which handles the text items
    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop();    // items
    iListBox->HandleItemAdditionL();

    AddControlContainerIconsL();
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::GetHelpContext
// ----------------------------------------------------------------------------
//
void CLogsSubAppListControlContainer::GetHelpContext( TCoeHelpContext& aContext ) const
	{
    aContext.iMajor = TUid::Uid( KLogsAppUID3 );
    aContext.iContext = KSTM_HLP_MAIN;
    }

// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::AddControlContainerIconsL
// ----------------------------------------------------------------------------
//
void CLogsSubAppListControlContainer::AddControlContainerIconsL()
	{
	if( iListBox->ItemDrawer()->FormattedCellData()->IconArray() != NULL )
		{
		CArrayPtr<CGulIcon>* iconArray = iListBox->ItemDrawer()->FormattedCellData()->IconArray();
		delete iconArray;
		iconArray = NULL;
		iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( iconArray);  
		}

	// Load and assign icons to the list box control; 3 is number of icons
	CAknIconArray* icons = new( ELeave ) CAknIconArray( KSubAppListCCNrOfItems );

	iListBox->ItemDrawer()->FormattedCellData()->SetIconArray( icons );

	// First icon & mask ( Missed calls icon & mask )
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogMissedSub
			,	qgn_prop_log_missed_sub
			,	qgn_prop_log_missed_sub_mask
			);

	// Second icon & mask ( Received calls icon & mask )
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogInSub
			,	qgn_prop_log_in_sub
			,	qgn_prop_log_in_sub_mask
			);

	// Third icon & mask ( Dialled calls icon & mask )
	AddIconL(	icons
			,	KLogsIconFile
            ,   KAknsIIDQgnPropLogOutSub
			,	qgn_prop_log_out_sub
			,	qgn_prop_log_out_sub_mask
			);
	}
	
// ----------------------------------------------------------------------------
// CLogsSubAppListControlContainer::FocusChanged
//
// This is needed to hand focus changes to list. Otherwise animations are not displayed.
// ----------------------------------------------------------------------------
//
void CLogsSubAppListControlContainer::FocusChanged(TDrawNow /* aDrawNow */ )
    {
    if( iListBox)
        {
        iListBox->SetFocus( IsFocused() );
        }
    }	

//  End of File
