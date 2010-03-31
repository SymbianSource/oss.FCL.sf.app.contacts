/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for CxSPSortViewControl.
*
*/


// INCLUDE FILES
#include "CxSPSortViewControl.h"

// System includes
#include <aknlists.h>   // CAknSingleStyleListBox
#include <avkon.mbg>
#include <eikclbd.h>
#include <gulicon.h>
#include <AknGlobalNote.h>
#include <StringLoader.h>
#include <avkon.hrh>

// internal includes
#include "CxSPLoader.h"
#include "MxSPFactory.h"
#include "CxSPViewIdChanger.h"
#include "CxSPViewInfo.h"
#include <ExtensionManagerRes.rsg>
#include "xSPOrderOrganizer.h"
#include "CGlobalNoteObserver.h"
#include <MPbk2ApplicationServices2.h>
#include <MPbk2ApplicationServices.h>
#include <CPbk2ServiceManager.h>
#include <MPbk2AppUi.h>
#include <CPbk2UIExtensionView.h>

// CONSTANTS
_LIT( KTab, "\t" );
const TUint KMaxListBoxBufSize( 256 ); // Maximum listbox item text size


void CleanUpResetAndDestroy( TAny* aArray )
	{
	if( aArray )
		{
		CArrayPtrFlat<TDesC>* array = (CArrayPtrFlat<TDesC>*)aArray;
		array->ResetAndDestroy();
		delete array;
		}
	}

// ================= MEMBER FUNCTIONS =======================

CxSPSortViewControl::CxSPSortViewControl( CxSPViewIdChanger& aViewIdChanger, 
											CxSPArray& aExtensions,
											CPbk2UIExtensionView& aView ) :
											iViewIdChanger( aViewIdChanger ),
											iExtensions(aExtensions),
											iGlobalNote(0),
											iGlobalNoteObserver(0),
											iView( aView )
    {
    }

void CxSPSortViewControl::ConstructL()
    {
    CreateWindowL();
    
    // Initialize the list box
    iListBox = new (ELeave) CAknSingleGraphicStyleListBox;
    TInt flags = EAknListBoxLoopScrolling | EAknListBoxMarkableList;
    iListBox->ConstructL(this, flags);
    iListBox->SetContainerWindowL(*this);
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
        (CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);	
	SetListBoxContentsL();
    }

CxSPSortViewControl* CxSPSortViewControl::NewL( CxSPViewIdChanger& aViewIdChanger,
											CxSPArray& aExtensions, 
											CPbk2UIExtensionView& aView )
    {
    CxSPSortViewControl* self = NewLC( aViewIdChanger, aExtensions, aView );
    CleanupStack::Pop(self);
    return self;
    }

CxSPSortViewControl* CxSPSortViewControl::NewLC( CxSPViewIdChanger& aViewIdChanger,
											CxSPArray& aExtensions, 
											CPbk2UIExtensionView& aView )
    {
    CxSPSortViewControl* self =
        new (ELeave) CxSPSortViewControl( aViewIdChanger, aExtensions, aView );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CxSPSortViewControl::~CxSPSortViewControl()
    {
    delete iListBox;   
    iSortedOrder.Close();
    delete iGlobalNote;
    delete iGlobalNoteObserver;
    }

TKeyResponse CxSPSortViewControl::OfferKeyEventL
        (const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    TKeyResponse result = EKeyWasNotConsumed;
    
	if( iListBox && result == EKeyWasNotConsumed )
        {
        result = iListBox->OfferKeyEventL(aKeyEvent, aType);
        }
	
    // Update popup when moving up or down in the list
     switch(aKeyEvent.iCode)
         {
         case EKeyUpArrow:
         case EKeyDownArrow:
             {
             const CListBoxView::CSelectionIndexArray* inds = iListBox->SelectionIndexes();
             TInt count = inds->Count();
             
             if ( count <= 0 )
                 {
                 iView.Cba()->SetCommandSetL( R_EXTENSION_MANAGER_SORT_VIEW_SOFTKEYS  );
                 }
             else
                 {
                 iView.Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OK_CANCEL );
                 }
             iView.Cba()->DrawDeferred();

             break;
             }
         case EKeyEnter:
         case EKeyOK: 
             {
             const CListBoxView::CSelectionIndexArray* inds = iListBox->SelectionIndexes();
             TInt count = inds->Count();
             
             if ( count <= 0 )
                 {
                 SetCurrentItemMarkedL( ETrue );
                 iView.Cba()->SetCommandSetL( R_AVKON_SOFTKEYS_OK_CANCEL );
                 }
             else
                 {
                 MoveMarkedItemL();
                 iView.Cba()->SetCommandSetL( R_EXTENSION_MANAGER_SORT_VIEW_SOFTKEYS  );
                 }
             iView.Cba()->DrawDeferred();
             break;
             }
         default:
             {
             break;
             }
         }
    return result;
    }

void CxSPSortViewControl::SizeChanged()
    {
    if ( iListBox )
        {
        iListBox->SetRect(Rect());
        }    
    }

TInt CxSPSortViewControl::CountComponentControls() const
    {
    TInt controls = 0;
    if (iListBox) ++controls;
    return controls;
    }

CCoeControl* CxSPSortViewControl::ComponentControl
        (TInt /*aIndex*/) const
    {
    return iListBox;
    }

void CxSPSortViewControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    if ( iListBox )
        {
        iListBox->HandlePointerEventL( aPointerEvent );
        }
    }

CCoeControl& CxSPSortViewControl::ComponentControl() const
    {
    return *iListBox;
    }

void CxSPSortViewControl::SetListBoxContentsL()
	{ 
	_LIT( KFormat, "%d" );
	
	CArrayPtr<CGulIcon>* icons = new(ELeave) CArrayPtrFlat<CGulIcon>( 1 );
    CleanupStack::PushL( TCleanupItem( CleanUpResetAndDestroy, icons ));
    
    CFbsBitmap* bitmap = NULL;
	CFbsBitmap* mask = NULL;	
	// Load the avkon "mark" icon
	AknIconUtils::CreateIconLC(	bitmap,	mask, AknIconUtils::AvkonIconFileName(),
    				  EMbmAvkonQgn_indi_marked_add,
    				  EMbmAvkonQgn_indi_marked_add_mask );

	CGulIcon* icon = CGulIcon::NewL(bitmap,mask);
	CleanupStack::Pop( 2 ); // mask, bitmap
	CleanupStack::PushL( icon );
    icons->AppendL( icon );
	CleanupStack::Pop( icon );
	
		  		        
    RPointerArray<CxSPViewInfo> infoArray;
    CleanupClosePushL( infoArray );
    iViewIdChanger.GetTabViewInfoL( infoArray );
    const TInt count = infoArray.Count();
    CDesC16ArrayFlat* texts = new (ELeave) CDesC16ArrayFlat( count );
    CleanupStack::PushL(texts);
    
    for( TInt i = 0; i < count; i++ )
    	{
    	const CxSPViewInfo& info = *infoArray[i];    		    
	    TBool found( EFalse );
	    for( TInt j = 0; j < iExtensions.Count() && !found; j++ )
	    	{
	    	if( info.Id() == iExtensions.At( j )->ExtensionFactory()->Id() )
	    		{
	    		TBuf<KMaxListBoxBufSize> buf;
	    		CFbsBitmap* bitmap = NULL;
	    		CFbsBitmap* mask = NULL;
                //info->Name() denotes the ServiceName of the Service in SpSettings                    
                //PBK2 ServiceManager has the brandicons of all the services installed
                //to the device.
                //So iterate thro the PBK2ServiceManager, to find the service which matches
                //the info->Name()
                //If matches use this icon else use the default icon for the tabs
                //get the XSP ServiceName 
                        
                // CPbk2ServiceManager stores all the brandinfo
                // related to the services configured to the phone
                // use this to show uniform icon throughout PhoneBook
                MPbk2ApplicationServices2* servicesExtension = 
                    reinterpret_cast<MPbk2ApplicationServices2*>
                        ( Phonebook2::Pbk2AppUi()->ApplicationServices().
                            MPbk2ApplicationServicesExtension(
                                KMPbk2ApplicationServicesExtension2Uid ) );
                CPbk2ServiceManager& servMan = servicesExtension->ServiceManager();
                const CPbk2ServiceManager::RServicesArray& services = servMan.Services();    
                for ( TInt i = 0; i < services.Count(); i++ )
                    {
                    const CPbk2ServiceManager::TService& service = services[i];
                    //Found the appropriate service info
                    if ( service.iName == info.Name() )
                        {
                        if ( service.iBitmap && service.iMask )
                            {
                            TSize size( 25, 25 ); //default size used in PB
                            
                            AknIconUtils::SetSize(
                                    service.iBitmap,
                                    size );
                            AknIconUtils::SetSize(
                                    service.iMask,
                                    size );                               
                            
                            bitmap = new (ELeave) CFbsBitmap;
                            CleanupStack::PushL( bitmap );
                            bitmap->Duplicate( service.iBitmap->Handle() );
                            
                            mask = new (ELeave) CFbsBitmap;
                            CleanupStack::PushL( mask );
                            mask->Duplicate( service.iMask->Handle() );
                            }
                        break;
                        }
                    } 
                
    			buf.Format( KFormat, icons->Count() );
    			
    			if ( !bitmap )
    			    {
    			    AknIconUtils::CreateIconLC(	bitmap,	mask, info.SortIconFile(),
    									info.SortIconId(), info.SortMaskId() );
    			    }
    			
				CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
				CleanupStack::Pop( 2 ); // mask, bitmap
				CleanupStack::PushL( icon );
    			icons->AppendL( icon );
				CleanupStack::Pop( icon );													
	    					
				buf.Append( KTab );
	    		buf.Append( info.Name() );
	    		buf.Append( KTab );
	    		buf.Append( KTab );
	    		texts->AppendL( buf );
	    		iSortedOrder.AppendL( i ); // create the initial sort order	
	    		found = ETrue;
	    		}
	    	}	        		    
    	}            	    	                        	    	    	
    
    	
    iListBox->Model()->SetItemTextArray( texts );
    iListBox->Model()->SetOwnershipType(ELbmOwnsItemArray);
    CleanupStack::Pop(texts);    
    iListBox->HandleItemAdditionL();
    
    CleanupStack::PopAndDestroy(); // infoArray
        	
    CleanupStack::Pop( icons );
    
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );
	}
	
	
void CxSPSortViewControl::SetCurrentItemMarkedL( TBool aMark )
	{
	if( aMark )
		{
		AknSelectionService::HandleMarkableListProcessCommandL( EAknCmdMark, iListBox );
		}
	else
		{
		AknSelectionService::HandleMarkableListProcessCommandL( EAknUnmarkAll, iListBox );
		}
	iListBox->DrawNow();		
	}
	
	
void CxSPSortViewControl::MoveMarkedItemL()
	{
	TInt currentIndex = iListBox->CurrentItemIndex();

	TInt markedIndex = -1;
	
	const CListBoxView::CSelectionIndexArray* inds = iListBox->SelectionIndexes();
	TInt count = inds->Count();
	if( count == 1 ) // sanity check, single selection list should have only one selection
		{
		markedIndex = (*inds)[0];
		if( markedIndex == currentIndex )
			{
			AknSelectionService::HandleMarkableListProcessCommandL( EAknUnmarkAll, iListBox );
			iListBox->DrawNow();
			return;
			}
		}	
	else
		{
		AknSelectionService::HandleMarkableListProcessCommandL( EAknUnmarkAll, iListBox );
		iListBox->DrawNow();
		return;
		}
		
	CTextListBoxModel* model = iListBox->Model();
    CDesCArray* texts = static_cast<CDesCArray*>( model->ItemTextArray() );
    if(( texts->Count() > markedIndex ) && ( markedIndex >= 0 ))
    	{
	    TBuf<KMaxListBoxBufSize> markedText = (*texts)[markedIndex];
		texts->Delete( markedIndex );
		iListBox->HandleItemRemovalL();

		TUint32 moveItem = iSortedOrder[markedIndex];
		iSortedOrder.Remove( markedIndex );
	
		texts->InsertL( currentIndex, markedText );
		iListBox->HandleItemAdditionL();
		User::LeaveIfError( iSortedOrder.Insert( moveItem, currentIndex ));
    	}
    else if( markedIndex < 0 )
    	{
    	User::Leave( KErrUnderflow );
    	}
    else
    	{
    	User::Leave( KErrOverflow );
    	}
	
	AknSelectionService::HandleMarkableListProcessCommandL( EAknUnmarkAll, iListBox );
	iListBox->DrawNow();
	}
	

TBool CxSPSortViewControl::CommitSortL( MGlobalNoteObserver* aObserver )	
	{
    TBool changes = EFalse;
	for( TInt i=0; i<iSortedOrder.Count(); i++)
		{
		// check if the order has really changed
		if( i != iSortedOrder[i] )
			{
			changes = ETrue;
			break;
			}
		}

	if( changes )
		{
		// new re-sorted array of extension IDs
		RArray<TxSPOrderUnit> idArray;
		CleanupClosePushL( idArray );
		RPointerArray<CxSPViewInfo> infoArray;
    	CleanupClosePushL( infoArray );
    	iViewIdChanger.GetTabViewInfoL( infoArray );
		
		for( TInt i = 0; i < iSortedOrder.Count(); i++ )
			{
			const CxSPViewInfo& info = *infoArray[iSortedOrder[i]];
			TxSPOrderUnit orderUnit;
			orderUnit.iId = info.Id();
			orderUnit.iViewId = info.OldViewId();			
			idArray.AppendL( orderUnit );
			}
		
		xSPOrderOrganizer::Reorganize( idArray );				
		
		CleanupStack::PopAndDestroy( 2 ); // infoArray, idArray

		// Load the used text label from resource
	    HBufC* stringholder = StringLoader::LoadLC
    	    ( R_QTN_EXTENSION_MANAGER_SHUTDOWN_NOTE, iEikonEnv );

	    // Create observer for listening global note dismissal 
	    delete iGlobalNoteObserver;
	    iGlobalNoteObserver = 0;
	    iGlobalNoteObserver = CGlobalNoteObserver::NewL( aObserver );

	    // Create and show global note
	    // The note cannot be deleted immediately because deleting it also
	    // causes the observer to be notified with KErrCancel
	    delete iGlobalNote;
	    iGlobalNote = 0;
	    iGlobalNote = CAknGlobalNote::NewL();
	    iGlobalNote->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY );
	    iGlobalNote->ShowNoteL(
                iGlobalNoteObserver->iStatus,
                EAknGlobalInformationNote,
                *stringholder );

        CleanupStack::PopAndDestroy( stringholder );	
		}	
	return changes;
	}

// End of file
