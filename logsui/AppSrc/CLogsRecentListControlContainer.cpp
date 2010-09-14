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
*     STM's Received Calls view control container
*
*/


//  INCLUDE FILES
#include <AknIconArray.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <AknsConstants.h>  //For skinned icons
#include <csxhelp/log.hlp.hrh>
#include <logs.rsg>
#include <logs.mbg>   //Index file for Logs icons.

#include "CLogsRecentListControlContainer.h"
#include "CLogsEngine.h"
#include "MLogsModel.h"
#include "MLogsSharedData.h"
#include "CLogsAppUi.h"
#include "CLogsRecentListView.h"
#include "CLogsRecentListAdapter.h"
#include "CLogsViewGlobals.h"
#include "MLogsExtensionFactory.h"
#include "MLogsUiControlExtension.h"
#include "LogsIcons.hrh"
#include "CLogsNaviDecoratorWrapper.h"

#include "LogsUID.h"


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
// CLogsRecentListControlContainer::NewL
// ----------------------------------------------------------------------------
//
CLogsRecentListControlContainer* CLogsRecentListControlContainer::NewL(
    CLogsRecentListView* aView, 
    const TRect& aRect )
    {
    CLogsRecentListControlContainer* self = new ( ELeave )
                        CLogsRecentListControlContainer( aView );
    self->SetMopParent( aView );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop(self);  // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::~CLogsRecentListControlContainer
// ----------------------------------------------------------------------------
//
CLogsRecentListControlContainer::~CLogsRecentListControlContainer()
    {
    delete iListBox;  
    Release(iControlExtension);
    }


// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::CLogsRecentListControlContainer
// ----------------------------------------------------------------------------
//
CLogsRecentListControlContainer::CLogsRecentListControlContainer(
    CLogsRecentListView* aView ) :
         CLogsBaseControlContainer( aView ),
         iView( aView )
    {
    }

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsRecentListControlContainer::ConstructL( const TRect& aRect )
    {
    BaseConstructL();
    iListBox = new( ELeave ) CAknDoubleGraphicStyleListBox; 
    iListBox->ConstructL( this, EEikListBoxMultipleSelection );  

    //Create control extension (i.e. the extension that provides additional PEC information
    //if PEC present in the image)
    CreateControlExtensionL();
    iControlExtension->SetObserver( *iView );

    AddControlContainerIconsL();
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );//Ownership of iAdapter transferred 

    iAdapter = CLogsRecentListAdapter::NewL( this, iLastOwnIconOffset );

    // Shows "No missed calls" in this case
     MakeEmptyTextListBoxL( iListBox, EmptyListboxResourceL() );
    iListBox->Model()->SetItemTextArray( iAdapter );
    
    MakeScrollArrowsL( iListBox );

    //Create recent list view tab group (Logs.rss)
    CAknNavigationDecorator* naviTabGroup = NavigationTabGroupL( R_STM_NAVI_PANE_TAB_GROUP, 
                                                                 CLogsNaviDecoratorWrapper::InstanceL() );
    //Get the control inside navigation decorator object
    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>( naviTabGroup->DecoratedControl() );
                                    

    // Activate the correct tab that corresponds to current recent view
    switch( iView->Id().iUid )
        {
        case EStmMissedListViewId:
            MakeTitleL( R_STM_TITLE_TEXT_MISSED );                        
            if ( tabGroup->ActiveTabId() != EMissedTabId )
                {
                tabGroup->SetActiveTabById( EMissedTabId );  //hightlight (activate) the tab
                }
            break;

        case EStmReceivedListViewId:
            MakeTitleL( R_STM_TITLE_TEXT_RECEIVED );
            if ( tabGroup->ActiveTabId() != EReceivedTabId )
                {
                tabGroup->SetActiveTabById( EReceivedTabId );  //hightlight (activate) the tab
                }
            break;

        case EStmDialledListViewId:
            MakeTitleL( R_STM_TITLE_TEXT_DIALLED );
            if ( tabGroup->ActiveTabId() != EDialledTabId )
                {
                tabGroup->SetActiveTabById( EDialledTabId );  //hightlight (activate) the tab
                }
            break;

        default:
            break;
        }

    SetRect( aRect ); 
    ActivateL();
    }

/**********************************************************************
Not in use anymore, Phonebook icons replaced by own icons 

void CLogsRecentListControlContainer::AddPhoneTypeIconsL( CAknIconArray* aIcons )
    {
    CPbkContactEngine* pbkEngine = iView->LogsAppUi()->Engine()->PbkEngine();
	CPbkIconInfoContainer* iconInfoContainer = iView->LogsAppUi()->IconInfoContainer();	//Container providing info of pbk's icons.
	CArrayFix<TPbkIconId>* idArray = iView->LogsAppUi()->IconIdArray(); 		    	//IconIdArray() maps icon ids corresponding to pbk's 
    idArray->Reset();                                                                   // data fields (one to many).
	const CPbkFieldsInfo& fieldsInfo = pbkEngine->FieldsInfo(); 
	TInt count( fieldsInfo.Count() );
    RArray<TInt> readIcons( count ); 					    //Temp array for bookkeeping the already added icons (add icon only once to array)

    for( TInt i = 0; i < count; i++ ) 
        {
    	CPbkFieldInfo* field = fieldsInfo.Find( i );        //We add icons in order of Phonebook field type ids, so can assume that 
        TInt idx( KErrNotFound );                           // first icon relates to EPbkFieldIdNone (so we'll have empty icon as first)
    
		if( field )  
			{
			TPbkIconId iconId = field->IconId();
			TPbkIconInfo* iconInfo = const_cast<TPbkIconInfo*>(iconInfoContainer->Find( iconId )); 

        	if( iconInfo )
            	{
            	idx = readIcons.Find( iconInfo->IconId() );
            	
                if( idx == KErrNotFound )	                //Add only new icons to icon array
                    {
                    readIcons.Append( iconInfo->IconId() ); //readIcons needs to be in synch with aIcons  
                	AddIconWithPathL( aIcons, iconInfo->MbmFileName(),   
                                      iconInfo->SkinId(), iconInfo->IconId(), iconInfo->MaskId() );
                    idx = aIcons->Count() - 1;              //Icon was added as last (count - 1)
                    }   
            	}
			}
		idArray->AppendL( TPbkIconId( idx ));
        }

    readIcons.Close();  
    aIcons->Compress();
    }
**********************************************************************/  
  
// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::ForceEmptyTextListBoxL
// ----------------------------------------------------------------------------
//
void CLogsRecentListControlContainer::ForceEmptyTextListBox()
    {
    TRAPD(err, MakeEmptyTextListBoxL( iListBox, KErrNotFound ));
    if (err)
        {
        CCoeEnv::Static()->HandleError( err );
        }   
    }

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::EmptyListboxResource
// ----------------------------------------------------------------------------
//
TInt CLogsRecentListControlContainer::EmptyListboxResourceL() const
    {   
    TInt res( R_LOGS_EVENTVIEW_FETCHING_TEXT );  

    TLogsState state = iView->State();

    if( state == EStateUndefined || state == EStateInitializing )
        {
        return res;                       //Show "retrieving data"
        }
    else if( state == EStateReaderDeletedOrStopped || state == EStateArrayReseted)
        {
        return KErrNotFound; 
        }
   
    if (iView->Engine()->SharedDataL()->IsLoggingEnabled())
        {
        switch( iView->RecentListType() )
            {
            case ELogsReceivedModel:
                res = R_STM_RECEIVED_EMPTY_LISTBOX_TEXT;
                break;

            case ELogsDialledModel:
                res = R_STM_DIALLED_EMPTY_LISTBOX_TEXT;
                break;

            case ELogsMissedModel:
                res = R_STM_MISSED_EMPTY_LISTBOX_TEXT;
                break;

            default:
                break;
            } 
        }
    else
        {
        res = R_QTN_LOGS_LOGGING_OFF;
        }
        
    return res;
    }
     									 
// ----------------------------------------------------------------------------
// Possible future need: Initiate some other type of call  
// with a long tap event (like video call)
// 
// CLogsRecentListControlContainer::HandleLongTapEventL
// ----------------------------------------------------------------------------
// 
//void CLogsRecentListControlContainer::HandleLongTapEventL( const TPoint& /* aPenEventLocation */, 
//                              							   const TPoint& /* aPenEventScreenLocation */ )
//	{
//    iView->HandleCommandL( ELogsCmdLongTapCall );    
//	}

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::ListBox
// ----------------------------------------------------------------------------
//
CAknDoubleGraphicStyleListBox* CLogsRecentListControlContainer::ListBox()
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CLogsRecentListControlContainer::ComponentControl( 
    TInt /*aIndex*/ ) const
    {
    return iListBox;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::SizeChanged
// ----------------------------------------------------------------------------
//
void CLogsRecentListControlContainer::SizeChanged()
    {
    if( iListBox )
        {
        iListBox->SetRect( Rect() );
        // Calculate line widths and update these to adapter
        TRect main_pane = iListBox->Rect();
        TAknWindowLineLayout line3 = AknLayout::list_gen_pane(0);
        TAknLayoutRect rect2;
        rect2.LayoutRect(main_pane, line3);
        TRect list_gen_pane = rect2.Rect();

        for(TInt trailIcons = 0; trailIcons <= KMaxNbrOfRecentTrailIcons; trailIcons++) 
            {
            // Let's first calculate the max width in pixels available for a row for a number of icons
            TAknWindowLineLayout line2 = AknLayout::list_double_graphic_pane_list_double2_graphic_pane(trailIcons);
            TAknLayoutRect rect;
            rect.LayoutRect(list_gen_pane, line2);
            TRect list_double2_graphic_pane = rect.Rect();

            TAknTextLineLayout line = AknLayout::List_pane_texts__double_graphic__Line_1( trailIcons );            
            TAknLayoutText text;
            text.LayoutText(list_double2_graphic_pane, line);  
            // Then provide to adapter the pixel value for this number of trailing icons as adapter needs 
            // to truncate shown data if necessary
            iAdapter->SetLineWidth( text.TextRect().Width(), trailIcons ); 
            }
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::UpdateL
// ----------------------------------------------------------------------------
//
void CLogsRecentListControlContainer::UpdateL()
    {    
    if( iView->CurrentModel()->Count() == 0 )
        {
        TInt res( EmptyListboxResourceL() );  //Shows either "retrieving" or "no data"
        MakeEmptyTextListBoxL( iListBox, res );        
        iListBox->Reset();
        }

    iListBox->HandleItemAdditionL();
    }

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::GetHelpContext
// ----------------------------------------------------------------------------
//
void CLogsRecentListControlContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = TUid::Uid( KLogsAppUID3 );

    if( iView->Id() ==  TUid::Uid( EStmMissedListViewId ) )
        {
        aContext.iContext = KSTM_HLP_MISSED;
        }
    else if( iView->Id() == TUid::Uid( EStmReceivedListViewId ) )
        {
        aContext.iContext = KSTM_HLP_RECEIVED;
        }
    else
        {
        aContext.iContext = KSTM_HLP_DIALLED;
        }
    }

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::CreateControlExtensionL
// ----------------------------------------------------------------------------
//
void CLogsRecentListControlContainer::CreateControlExtensionL()
    {
    CLogsViewGlobals* viewGlobals = CLogsViewGlobals::InstanceL();

    viewGlobals->PushL();

    iControlExtension = viewGlobals->ExtensionFactoryL().
            CreateLogsUiControlExtensionL();

    CleanupStack::PopAndDestroy(viewGlobals);    
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::ControlExtension
// ----------------------------------------------------------------------------
//
MLogsUiControlExtension* CLogsRecentListControlContainer::ControlExtension()
    {
    return iControlExtension;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::View
// ----------------------------------------------------------------------------
//
CLogsRecentListView* CLogsRecentListControlContainer::View()
    {
    return iView;
    }
    
// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::AddControlContainerIconsL
// ----------------------------------------------------------------------------
//
void CLogsRecentListControlContainer::AddControlContainerIconsL()
    {
    iLastOwnIconOffset = KErrNotFound;

    //CAknIconArray owns the CGulIcons it contains (for leave-safety). 
    //Destructor destroys all icons in the array.
    if( iListBox->ItemDrawer()->ColumnData()->IconArray() != NULL )
        {
        CArrayPtr<CGulIcon>* iconArray = iListBox->ItemDrawer()->ColumnData()->IconArray();
        delete iconArray;
        iconArray = NULL;
        iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconArray);  
        }

    /**********************************************************************
    Not in use anymore, Phonebook icons replaced by own icons starting from offset 0 below
    
    // First load PhoneTypeIcons 
    TInt count( iView->Engine()->PbkEngine()->FieldsInfo().Count() );

    CAknIconArray* icons = new( ELeave ) CAknIconArray( count );  //count = granularity. Needs to be # of icons from Pbk
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );   
    AddPhoneTypeIconsL( icons );

    //Then load on top of PhoneTypeIcons the other own icons: Poc, Voip, Video, Als2, ... 
    **********************************************************************/        
    
    CAknIconArray* icons = new( ELeave ) CAknIconArray( KRecentListCCNrOfItems );  
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );   

    //Icons for column A (phone call type). Offsets referred by enum RecentListIconArrayIcons 
    //EIconDefault;//refers to this icon (offset 0)
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnPropNrtypMobile     
            ,   qgn_prop_nrtyp_mobile     
            ,   qgn_prop_nrtyp_mobile_mask
            );

    //EIconVideo;//refers to this icon:
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnPropNrtypVideo 
            ,   qgn_prop_nrtyp_video      
            ,   qgn_prop_nrtyp_video_mask 
            );

    //EIconVoip;//refers to this icon:
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnPropNrtypVoip  
            ,   qgn_prop_nrtyp_voip    
            ,   qgn_prop_nrtyp_voip_mask  
            );
    
    //EIconPoc;//refers to this icon:
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnPropNrtypPoc
            ,   qgn_prop_nrtyp_poc        
            ,   qgn_prop_nrtyp_poc_mask   
            );

    //EIconAls;//refers to this icon:
    AddColorIconL(   icons                       
            ,   KLogsIconFile
            ,   KAknsIIDQgnIndiAlsLine2Add
            ,   qgn_indi_als_line2_add
            ,   qgn_indi_als_line2_add_mask
            );
    //EIconNewMissed refers to this icon:
    AddColorIconL(   icons                       
            ,   KLogsIconFile
            ,   KAknsIIDQgnIndiLogMissedCallAdd
            ,   qgn_indi_log_missed_call_add
            ,   qgn_indi_log_missed_call_add_mask
            );

    //EIconLandphone;//refers to this icon:
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnPropNrtypPhone
            ,   qgn_prop_nrtyp_phone        
            ,   qgn_prop_nrtyp_phone_mask   
            );
    
    //EIconPager;//refers to this icon:
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnPropNrtypPager
            ,   qgn_prop_nrtyp_pager        
            ,   qgn_prop_nrtyp_pager_mask   
            );
     
    //EIconFax;//refers to this icon:
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnPropNrtypFax
            ,   qgn_prop_nrtyp_fax        
            ,   qgn_prop_nrtyp_fax_mask   
            );      
       
    //EIconAsstPhone;//refers to this icon:
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnPropNrtypAssistant
            ,   qgn_prop_nrtyp_assistant        
            ,   qgn_prop_nrtyp_assistant_mask   
            );  
    
    //EIconCarPhone;//refers to this icon
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnPropNrtypCar     
            ,   qgn_prop_nrtyp_car     
            ,   qgn_prop_nrtyp_car_mask
            );
    //EIconCommCall;//refers to this icon
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnPropNrtypCall     
            ,   qgn_prop_nrtyp_call     
            ,   qgn_prop_nrtyp_call_mask
            );
    /**********************************************************************    
    Not in use anymore, 
    
    EIconPoc2;//refers to this icon:    
    AddIconL(   icons                      
            ,   KLogsIconFile 
            ,   KAknsIIDQgnIndiPocAdd     
            ,   qgn_indi_poc_add    
            ,   qgn_indi_poc_add_mask  
            );

    **********************************************************************/                    

    // Offset of last own icon in icon array 
    iLastOwnIconOffset = iListBox->ItemDrawer()->ColumnData()->IconArray()->Count() -1;

    //Finally load other icons related to control extension(if any). Control extension 
    //takes care of these so we'll discard them in iLastOwnIconOffset
    //Control extension icons (i.e. PEC)
    iControlExtension->AppendIconsL( icons );

    return;
    }

// ----------------------------------------------------------------------------
// CLogsRecentListControlContainer::FocusChanged
//
// This is needed to hand focus changes to list. Otherwise animations are not displayed.
// ----------------------------------------------------------------------------
//
void CLogsRecentListControlContainer::FocusChanged(TDrawNow /* aDrawNow */ )
    {
    if( iListBox)
        {
        iListBox->SetFocus( IsFocused() );
        }
    }


//  End of File
