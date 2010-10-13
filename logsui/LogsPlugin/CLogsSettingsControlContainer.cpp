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
*     Logs "Settings" view container control class implementation
*
*/


// INCLUDE FILES
#include <aknnavide.h>
#include <AknUtils.h>
#include <Logs.rsg>

#include "CLogsSettingsControlContainer.h"
#include "CLogsSettingsView.h"
#include "CLogsEngine.h"
#include "MLogsSharedData.h"

#include "LogsConstants.hrh"

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
// CLogsSettingsControlContainer::NewL
// ----------------------------------------------------------------------------
//
CLogsSettingsControlContainer* CLogsSettingsControlContainer::NewL(
    CLogsSettingsView* aSettingsView,
    const TRect& aRect,
    TBool aForGs )  
    {
    CLogsSettingsControlContainer* self = new( ELeave ) 
                        CLogsSettingsControlContainer( aSettingsView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aForGs );
    CleanupStack::Pop();  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsSettingsControlContainer::ConstructL( 
    const TRect& aRect,
    TBool aForGs )  
    {
    BaseConstructL();
    CreateListBoxL();        //Creates empty iListBox  
    
    if( aForGs )
        {
        MakeTitleL( R_LOGS_SETTINGS_TITLE_FOR_GS );
        }
    else
        {
        MakeTitleL( R_LOGS_SETTINGS_TITLE_FOR_LOGS );
        }
    
    MakeEmptyTextListBoxL( iListBox, R_LOGS_EVENTVIEW_FETCHING_TEXT );    
    NaviPaneL()->PushDefaultL();
    MakeScrollArrowsL( iListBox ); 
    SetRect( aRect );
    ActivateL();    
    }


// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::CLogsSettingsControlContainer
// ----------------------------------------------------------------------------
//
CLogsSettingsControlContainer::CLogsSettingsControlContainer(
    CLogsSettingsView* aSettingsView ) :
        CLogsBaseControlContainer( aSettingsView ),
        iSettingsView( aSettingsView )
    {
    }

// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::~CLogsSettingsControlContainer
// ----------------------------------------------------------------------------
//
CLogsSettingsControlContainer::~CLogsSettingsControlContainer()
    {
    delete iListBox;   
    }

// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CLogsSettingsControlContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType
    )
    {
    TKeyResponse res = CLogsBaseControlContainer::
                                    OfferKeyEventL(aKeyEvent, aType );
    iCurrentListBoxTopIndex = iListBox->TopItemIndex();
    iCurrentListBoxIndex = iListBox->CurrentItemIndex();
    return res;
    }


// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CLogsSettingsControlContainer::ComponentControl(
    TInt /*aIndex*/) const
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::SizeChanged
// ----------------------------------------------------------------------------
//
void CLogsSettingsControlContainer::SizeChanged()
    {
    if( iListBox )
        {
        iListBox->SetRect( Rect() );
        }
    }


// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::ListBox
// ----------------------------------------------------------------------------
//
CAknSettingStyleListBox* CLogsSettingsControlContainer::ListBox()
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::CreateListBoxL
// ----------------------------------------------------------------------------
//
void CLogsSettingsControlContainer::CreateListBoxL()
    {
    if( iListBox )
        {
        delete iListBox;
        iListBox = NULL;
        }

    iListBox = new( ELeave ) CAknSettingStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionListWithShortcuts );
    SizeChanged();
    }

// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::AddOneListBoxTextLineL
// ----------------------------------------------------------------------------
//
void CLogsSettingsControlContainer::AddOneListBoxTextLineL(
    CDesCArrayFlat* aItems,
    TInt aFirstResourceText,
    TInt aSecondResourceText )
    {
    // temporary place for the resource texts
    HBufC* textFromResourceFile;
    // Place where to put text to display
    HBufC* textBuf = HBufC::NewLC( KLogsBuff128 ); 
    TPtr ptr = textBuf->Des();
    ptr.Format( KEmptySpaceAndTabulator );

    textFromResourceFile = iCoeEnv->AllocReadResourceLC( aFirstResourceText );
    TPtr ptr2 = textFromResourceFile->Des();
    AknTextUtils::LanguageSpecificNumberConversion( ptr2 );
    ptr .AppendFormat( *textFromResourceFile );
    CleanupStack::PopAndDestroy();  // textFromResourceFile

    ptr .AppendFormat( KTabulatorTwice );

    textFromResourceFile = iCoeEnv->AllocReadResourceLC( aSecondResourceText );
    TPtr ptr3 = textFromResourceFile->Des();
    AknTextUtils::LanguageSpecificNumberConversion( ptr3 );
    ptr .AppendFormat( *textFromResourceFile );

    aItems->AppendL( *textBuf );
    CleanupStack::PopAndDestroy( 2 );  // textFromResourceFile, textBuf
    }

// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::UpdateListBoxContentL
// ----------------------------------------------------------------------------
//
void CLogsSettingsControlContainer::UpdateListBoxContentL()
    {
    // Create item array for listbox
    CDesCArrayFlat* items = new( ELeave ) CDesCArrayFlat( 2 );  // for number of lines, see number of AddOneListBoxTextLineL calls
    CleanupStack::PushL( items );

    //  First line: Log Length ( 0 / 1 / 10 / 30 days )
    switch( iSettingsView->LogAge() )
        {
        case ELogsConfigAgeNoLog:
            AddOneListBoxTextLineL( items, R_LOGS_SETTINGS_LOG_LENGTH_TEXT,
                                       R_LOGS_SV_LOG_LENGTH_0_TEXT );
            break;
        
        case ELogsConfigAge1Days:
            AddOneListBoxTextLineL( items, R_LOGS_SETTINGS_LOG_LENGTH_TEXT,
                                       R_LOGS_SV_LOG_LENGTH_1_TEXT );
            break;

        case ELogsConfigAge10Days:
            AddOneListBoxTextLineL( items, R_LOGS_SETTINGS_LOG_LENGTH_TEXT,
                                       R_LOGS_SV_LOG_LENGTH_10_TEXT );
            break;

        case ELogsConfigAge30Days:
            AddOneListBoxTextLineL( items, R_LOGS_SETTINGS_LOG_LENGTH_TEXT,
                                       R_LOGS_SV_LOG_LENGTH_30_TEXT );
            break;

        default:
            AddOneListBoxTextLineL( items, R_LOGS_SETTINGS_LOG_LENGTH_TEXT,
                                       R_LOGS_SV_LOG_LENGTH_30_TEXT );
            break;
        }
    /************************************************
    "Show Call Duration" moved General Settings application, not in use any more in Logs 
    //  Second line: Show Call Duration in Phone application ( ON / OFF )
    if( iSettingsView->Engine()->SharedDataL()->ShowCallDurationPhone() )
        {
        AddOneListBoxTextLineL( items, 
                                R_LOGS_SETTINGS_SHOW_CALL_DURATION_TEXT,
                                R_QTN_LOGS_LDV3_TIMER_ON_TEXT );
        }
    else
        {
        AddOneListBoxTextLineL( items, 
                                R_LOGS_SETTINGS_SHOW_CALL_DURATION_TEXT,
                                R_QTN_LOGS_LDV3_TIMER_OFF_TEXT );
        }
    ************************************************/

    //  Creation of the model, which handles the text items
    CTextListBoxModel* model = iListBox->Model();
    model->SetItemTextArray( items );
    model->SetOwnershipType( ELbmOwnsItemArray );       //Ownership of items transferred 
    CleanupStack::Pop( items );                         

    if( iCurrentListBoxIndex != KErrNotFound )
        {
        iListBox->SetCurrentItemIndex( iCurrentListBoxIndex );
        }

    iListBox->SetTopItemIndex( iCurrentListBoxTopIndex );

    //MakeScrollArrowsL( iListBox );    --> moved to ConstructL to avoid flickering, fix for: SMEA-75MEVX 
    iListBox->HandleItemAdditionL(); 
    
    // Remember to hand focus to the newly created list if the container
    // already has the focus.
    iListBox->SetFocus( IsFocused() );    
    }

// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::GetHelpContext
// ----------------------------------------------------------------------------
//
void CLogsSettingsControlContainer::GetHelpContext( TCoeHelpContext& aContext ) const
	{
    aContext.iMajor = TUid::Uid( KLogsAppUID3 );
    aContext.iContext = KLOGS_HLP_SETTING;
    }

// ----------------------------------------------------------------------------
// CLogsSettingsControlContainer::FocusChanged
//
// This is needed to hand focus changes to list. Otherwise animations are not displayed.
// ----------------------------------------------------------------------------
//
void CLogsSettingsControlContainer::FocusChanged(TDrawNow /* aDrawNow */ )
    {
    if( iListBox )
        {
        iListBox->SetFocus( IsFocused() );
        }
    }

//  End of File
