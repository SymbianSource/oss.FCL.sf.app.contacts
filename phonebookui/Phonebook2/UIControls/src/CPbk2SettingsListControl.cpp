/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Phonebook 2 names list control.
*
*/


// INCLUDE FILES
#include "CPbk2SettingsListControl.h"
#include "CPbk2NameOrderSettingItem.h"
#include "CPbk2MemorySelectionSettingItem.h"
#include "CPbk2PredictiveSearchSettingItem.h"
#include "CPbk2DefaultSavingStorageSettingItem.h"
#include "Pbk2UIControls.hrh"
#include "CPbk2SettingsListState.h"
#include "Phonebook2PrivateCRKeys.h"

// From Phonebook 2
#include <Pbk2UIControls.rsg>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionFactory.h>
#include <MPbk2SettingsControlExtension.h>
#include <CPbk2SortOrderManager.h>

#include <StringLoader.h>
#include <centralrepository.h>
#include <featmgr.h>
#include <AknFepInternalCRKeys.h>
#include <akncheckboxsettingpage.h>
#include <sysutil.h>

//Pbk2Debug
#include "Pbk2Debug.h"


// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::CPbk2SettingsListControl
// -----------------------------------------------------------------------------
//
CPbk2SettingsListControl::CPbk2SettingsListControl(
        CPbk2SortOrderManager& aSortOrderManager,        
        MPbk2SettingsControlExtension& aControlExtension ) :
    iSortOrderManager( aSortOrderManager ),     
    iControlExtension( aControlExtension )
    {
    }
    
// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::~CPbk2SettingsListControl
// -----------------------------------------------------------------------------
//
CPbk2SettingsListControl::~CPbk2SettingsListControl()
    {                     
    delete iSettingListState;
    }
    
// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::NewL
// -----------------------------------------------------------------------------
//    
EXPORT_C CPbk2SettingsListControl* CPbk2SettingsListControl::NewL(
        const CCoeControl* aContainer,
        CPbk2SortOrderManager& aSortOrderManager,            
        MPbk2SettingsControlExtension& aControlExtension )
    {
    CPbk2SettingsListControl* self = new (ELeave) CPbk2SettingsListControl(
            aSortOrderManager, aControlExtension );
    CleanupStack::PushL( self );
    self->ConstructL( aContainer );
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::CommitSettingsL
// -----------------------------------------------------------------------------
//    
EXPORT_C void CPbk2SettingsListControl::CommitSettingsL()
    {
    StoreSettingsL();
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::StoreSettings
// -----------------------------------------------------------------------------
//  
void CPbk2SettingsListControl::StoreSettings()
	{
	TRAPD( err, StoreSettingsL() );
    if ( err != KErrNone )
		{
		iEikonEnv->HandleError( err );
		}
	}

// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::SizeChanged
// -----------------------------------------------------------------------------
//
void CPbk2SettingsListControl::SizeChanged()
    {
    CAknSettingItemList::SizeChanged();

    CEikListBox* listBox = ListBox();
    if( listBox )
        {
        listBox->SetRect( Rect() );  // Set container's rect to listbox
        }
    } 
    
// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::EditItemL
// -----------------------------------------------------------------------------
//    
void CPbk2SettingsListControl::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    switch ( aIndex )
        {
        case EPbk2SettingMemorySelection:
            {
            LaunchMemorySelectionSettingPageL();  
            this->StoreSettings();
            break;
            }   
        case EPbk2SettingPredictiveSearch:
            {
            // aCalledFromMenu = EFalse to make it in place, not show the popup
            CAknSettingItemList::EditItemL( aIndex, EFalse );
            this->StoreSettings();
                
            break;
            }       
        case EPbk2SettingDefaultSavingStorage: //Fall through
        case EPbk2SettingNameOrder: //Fall through
        default:
			{
            CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
            this->StoreSettings();
			break;
			}
        }
    }

void CPbk2SettingsListControl::HandleResourceChange( TInt aType )
    {
    ListBox()->HandleResourceChange( aType );
    CAknSettingItemList::HandleResourceChange( aType );         
    }


// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::HandleListBoxEventL
// -----------------------------------------------------------------------------
//	
void CPbk2SettingsListControl::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
	{
	if( AknLayoutUtils::PenEnabled() )
		{
		switch( aEventType )
			{
			case EEventItemSingleClicked: //fall through
			case EEventEnterKeyPressed:
				{
				EditItemL( ListBox()->CurrentItemIndex(), ETrue );
				break;
				}
			default :
			    //ignore rest events
			    break;
			}
		}
	}

// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::ConstructL
// -----------------------------------------------------------------------------
//
inline void CPbk2SettingsListControl::ConstructL(
        const CCoeControl* aContainer )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SettingsListControl::ConstructL(0x%x)"),
        aContainer);
    
    if ( !aContainer )
        {
        User::Leave( KErrGeneral );
        }

    // First set container
    SetContainerWindowL( *aContainer );

    iSettingListState = CPbk2SettingsListState::NewL();
    
    // Check language specific variation for name ordering item
    if ( ShowNameOrderingSettingL() )
        {
        ConstructFromResourceL( R_PHONEBOOK2_SETTINGS_LIST );
        }
    else
        {
        ConstructFromResourceL( R_PHONEBOOK2_SETTINGS_LIST_NO_NAME_ORDERING );
        }
    
    iControlExtension.ModifySettingItemListL( *this );
    // call this to make extension modifications visible
    HandleChangeInItemArrayOrVisibilityL();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SettingsListControl::ConstructL end"));
    }
    
// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::CreateSettingItemL
// -----------------------------------------------------------------------------
//
CAknSettingItem* CPbk2SettingsListControl::CreateSettingItemL( TInt aSettingId )
	{
	CAknSettingItem* settingItem = 
        iControlExtension.CreateSettingItemL( aSettingId );

    if ( !settingItem )
        {        
	    switch ( aSettingId )
		    {		    
		    case EPbk2SettingMemorySelection:
		        {
		        iSelectionSettingItem = 
		            new(ELeave) CPbk2MemorySelectionSettingItem(
		                aSettingId, 
		                iSettingListState->SupportedStores(), 
		                iSettingListState->SelectionItemList() );		        
		        settingItem = iSelectionSettingItem;
		        break;
		        }
		    case EPbk2SettingDefaultSavingStorage:
		        {
		        iDefaultSavingStorage = 0;
		        iDefaultSavingStorageItem = 
		            new(ELeave) CPbk2DefaultSavingStorageSettingItem(
		                aSettingId, iDefaultSavingStorage,		            
		                iSettingListState->SupportedStores(), 
		                iSettingListState->SelectionItemList() );
		        settingItem = iDefaultSavingStorageItem;
		        break;
		        }
		    case EPbk2SettingNameOrder:
			    {
		        CPbk2SortOrderManager::TPbk2NameDisplayOrder order = 
                    iSortOrderManager.NameDisplayOrder();
                iDefaultNameDisplayOrder = TInt( order );
		        settingItem = new (ELeave) CPbk2NameOrderSettingItem(
                    iSortOrderManager, aSettingId, iDefaultNameDisplayOrder );
			    break;
			    }
		    case EPbk2SettingPredictiveSearch:
		        {
                if(FeatureManager::FeatureSupported(KFeatureIdFfContactsPredictiveSearch))
                    {
                    TBool physicalKeyboardExist = PhysicalKeyboardExistL();
                    if ( physicalKeyboardExist )
                        {
                        PredictiveSearchEnableInfoFromCenrepL();
                        iPredictiveSearchItem = new ( ELeave ) CPbk2PredictiveSearchSettingItem( aSettingId, iPredictiveSearchSettingsValue );
                        settingItem = iPredictiveSearchItem;
                        }
                    else // if no physical keyboard, make this settings invisible, 
                         // and reset cenrep for predictive search
                        {
                        DisablePredictiveSearchInCenrepL();
                        }    
                    }
				break;
		        }		        
		    default:
			    {
			    // Do nothing
			    break;
			    }
		    }
        }

	return settingItem;
	}
    
// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::LaunchMemorySelectionSettingPageL
// -----------------------------------------------------------------------------
//	
void CPbk2SettingsListControl::LaunchMemorySelectionSettingPageL()	
    {
    //Store selection setting item list  
    iSettingListState->StoreItemStateL();
              
    TBool result( EFalse );
    do
        {
        //Edit selection setting item 
        //result is ETrue if OK pressed
        result = iSelectionSettingItem->EditItemL();
        
        TBool driveSpaceFull( EFalse );
        driveSpaceFull = SysUtil::FFSSpaceBelowCriticalLevelL( &CCoeEnv::Static()->FsSession() );
        if ( driveSpaceFull )
            {
            //If the disk is full, a write operation cannot complete. So restore 
            //the previous state.
            iEikonEnv->HandleError(KErrDiskFull);
            CSelectionItemList& oldItem( iSettingListState->ItemListState() );
            CSelectionItemList& item( iSettingListState->SelectionItemList() );
            TInt count = item.Count();
            for ( TInt i(0); i < count; ++i )
                {
                TBool select = oldItem.At(i)->SelectionStatus();
                item.At(i)->SetSelectionStatus( select );
                }
            iSelectionSettingItem->RestoreStateL();
            result = EFalse;
            }
        
        if ( result )
            {
            //Is selection setting list changed by user
            //result is ETrue if it is changed
            result = iSettingListState->IsChanged();
            if ( result )
                {                        
                //Update default saving storage
                //result is ETrue if success                        
                result = !iDefaultSavingStorageItem->UpdateL();                        
                DrawDeferred();
                }
            }    
        else
            {
            //restore selection setting item list
            iSettingListState->RestoreItemStateL();
            iDefaultSavingStorageItem->RestoreStateL();
            }
        }while( result );    
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::ShowNameOrderingSettingL
// -----------------------------------------------------------------------------
//	
TBool CPbk2SettingsListControl::ShowNameOrderingSettingL()
    {
    // Language specific variation for name ordering setting item
    _LIT( KVisible, "1");
    HBufC* nameOrderVisibilityBuf = 
        StringLoader::LoadLC( R_QTN_PHOB_NAME_ORDERING_SETTINGS_VISIBILITY );
    TBool result = ( nameOrderVisibilityBuf->Compare( KVisible ) == 0 );
    CleanupStack::PopAndDestroy( nameOrderVisibilityBuf );
    return result;
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::PredictiveSearchEnableInfoFromCenrepL
// -----------------------------------------------------------------------------
//  
void CPbk2SettingsListControl::PredictiveSearchEnableInfoFromCenrepL()
    {
    CRepository* repository = CRepository::NewLC(TUid::Uid(KCRUidPhonebook));
 
    TBool predictiveSearch = EFalse;
    TInt ret = KErrNone;
    
    ret = repository->Get( KPhonebookPredictiveSearchEnablerFlags, predictiveSearch );
  
    if ( ret == KErrNotFound )
        {
        predictiveSearch = EFalse;
        repository->Create(KPhonebookPredictiveSearchEnablerFlags, predictiveSearch );
        }
   
    if ( predictiveSearch )
        {
        iPredictiveSearchSettingsValue = ETrue;
        }
    else
        {
        iPredictiveSearchSettingsValue = EFalse;
        }
    
    CleanupStack::PopAndDestroy( repository );
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::PhysicalKeyboardExistL
// -----------------------------------------------------------------------------
//  
TBool CPbk2SettingsListControl::PhysicalKeyboardExistL()
    { 
    TBool ret = ETrue;
    
    TInt physicalKeyboard = 0;  
    CRepository* aknFepRepository = CRepository::NewL( KCRUidAknFep );
    TInt err = aknFepRepository->Get( KAknFepPhysicalKeyboards, physicalKeyboard );
    delete aknFepRepository;  
    
    if ( err == KErrNone )
        {
        ret = ( physicalKeyboard != 0 ) ? ETrue : EFalse;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CPbk2SettingsListControl::DisablePredictiveSearchInCenrepL
// -----------------------------------------------------------------------------
// 
void CPbk2SettingsListControl::DisablePredictiveSearchInCenrepL()
    {
    TBool predictiveSearch = EFalse;
    CRepository* rep = CRepository::NewL(TUid::Uid(KCRUidPhonebook));

    TInt err( rep->Set( KPhonebookPredictiveSearchEnablerFlags, predictiveSearch ) ); 
    
    if ( err )
        {
        rep->Create( KPhonebookPredictiveSearchEnablerFlags, predictiveSearch );
        }
    
    delete rep;
    }
//  End of File
