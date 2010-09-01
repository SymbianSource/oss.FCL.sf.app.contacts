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
* Description: 
*       Implementation for CxSPSortNamesListViewExtension.
*
*/


// INCLUDE FILES
#include "CxSPSortNamesListViewExtension.h"

// System includes
#include <eikmenup.h>
#include <AknGlobalNote.h>

// From Phonebook2
#include <MPbk2AppUi.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ViewExplorer.h>
#include <CPbk2ViewState.h>

// internal
#include "ExtensionManager.hrh"
#include <ExtensionManagerRes.rsg>
#include "MxSPCommandMapper.h"
#include "ExtensionManagerUID.H"

using namespace Phonebook2;

// ================= MEMBER FUNCTIONS =======================

inline CxSPSortNamesListViewExtension::CxSPSortNamesListViewExtension()
    {
    }

inline void CxSPSortNamesListViewExtension::ConstructL()
    {
    }

CxSPSortNamesListViewExtension* CxSPSortNamesListViewExtension::NewL()
    {
    CxSPSortNamesListViewExtension* self = new(ELeave) CxSPSortNamesListViewExtension();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CxSPSortNamesListViewExtension::~CxSPSortNamesListViewExtension()
    {
    }

void CxSPSortNamesListViewExtension::DoRelease()
    {
    delete this;
    }

void CxSPSortNamesListViewExtension::DynInitMenuPaneL
        ( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    }
    
void CxSPSortNamesListViewExtension::DynInitNamesListMainMenuPaneL
  		( TInt /*aResourceId*/, CEikMenuPane* aMenuPane )
  		{
  		AddMenuContentsL( *aMenuPane );
       	ChangeCommandL( *aMenuPane, EExtensionManagerCmdSort );	
  		}
    
void CxSPSortNamesListViewExtension::AddMenuContentsL( CEikMenuPane& aMenuPane )
    {
    TInt newCommand;
	TInt err = iMapper->GetNewCommand( KEXTENSIONMANAGERIMPLEMENTATIONUID, 
									   EExtensionManagerCmdSort, 
									   newCommand );		
	User::LeaveIfError( err );	        
    TInt pos;
    if( !aMenuPane.MenuItemExists( newCommand, pos ) )
    	{
        aMenuPane.AddMenuItemsL( R_SORT_NAMES_LIST_VIEW_MENU );
    	}
    }    
    
void CxSPSortNamesListViewExtension::ChangeCommandL( CEikMenuPane& aMenuPane, TInt aOldCommand )
	{		
	TInt pos;
	if( aMenuPane.MenuItemExists( aOldCommand, pos ) )
		{
		TInt newCommand;
		TInt err = iMapper->GetNewCommand( KEXTENSIONMANAGERIMPLEMENTATIONUID, 
										   aOldCommand, 
										   newCommand );
		User::LeaveIfError( err );		
		CEikMenuPaneItem::SData& data = aMenuPane.ItemDataByIndexL( pos );
		data.iCommandId = newCommand;
		}
	}

TBool CxSPSortNamesListViewExtension::HandleCommandL( TInt aCommandId,
													MPbk2ContactUiControl* aUiControl )
    {
    TBool result( EFalse );
	TInt oldCommand;
	TInt err = iMapper->GetOldCommand( KEXTENSIONMANAGERIMPLEMENTATIONUID, aCommandId, oldCommand );
	
	if( !err )
		{
		switch( oldCommand )
	        {
	        case EExtensionManagerCmdSort:
	            {
	            CPbk2ViewState* state = aUiControl->ControlStateL();
        		CleanupStack::PushL( state );
        		Pbk2AppUi()->Pbk2ViewExplorer()->
            		ActivatePhonebook2ViewL( TUid::Uid(
                		EExtensionManagerSortViewId ), state );
        		CleanupStack::PopAndDestroy( state );
        		aUiControl->UpdateAfterCommandExecution();
	            result = ETrue;
	            break;
	            }
	        default:
	            {
	            break;
	            }
	        }	
		}
    
    return result;
    }

void CxSPSortNamesListViewExtension::RegisterCommandMapper( MxSPCommandMapper& aMapper )
	{
	iMapper = &aMapper;
	}
	
TInt CxSPSortNamesListViewExtension::CommandInfoResource() const
	{
	return R_EXTENSION_MANAGER_COMMAND_INFO;
	}

//  End of File
