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
*       Composite view extension.
*
*/


// INCLUDE FILES
#include <Pbk2Commands.rsg>
#include <coemain.h>
#include <barsread.h>
#include <eikmenup.h>
#include "CxSPViewManager.h"
#include "ForEachUtil.h"
#include "MxSPView.h"
#include "CxSPCommandInfo.h"
#include "ExtensionManager.hrh"

// CONSTANTS
const TInt KExtensionGranularity = 1;

// ==================== MEMBER FUNCTIONS ====================
CxSPViewManager::CxSPViewManager() :
    iViews(KExtensionGranularity),
    iNewCommands( EExtensionManagerFirstFreeCommand )
    {
    }

CxSPViewManager::~CxSPViewManager()
    {
    const TInt count = iViews.Count();
    for (TInt i=0; i<count; ++i)
        {
        Release( iViews[i] );
        }
    iViews.Reset();
    iCommandIdMap.ResetAndDestroy();
    iCommandIdMap.Close();
    }

CxSPViewManager* CxSPViewManager::NewL()
    {
    CxSPViewManager* self = new (ELeave) CxSPViewManager;
    return self;
    }

void CxSPViewManager::AppendL( MxSPView* aView, TUint32 aId )
    { 
    iViews.AppendL( aView );   
	TInt res = aView->CommandInfoResource();
	if( res != KErrNotFound )
		{
		TResourceReader reader;
		CCoeEnv::Static()->CreateResourceReaderLC( reader, res );
		TInt resCount = reader.ReadInt16();
		for( TInt j = 0; j < resCount; j++ )
			{
			CxSPCommandInfo* commandInfo = NULL;
			commandInfo = CxSPCommandInfo::NewLC( reader,
												aId,
												iNewCommands );
			User::LeaveIfError( iCommandIdMap.Append( commandInfo ) );
			CleanupStack::Pop(); // commandInfo
			}
		CleanupStack::PopAndDestroy(); // reader						
		aView->RegisterCommandMapper( *this );
		}	            	    	
    }

void CxSPViewManager::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if( aResourceId == R_PHONEBOOK2_NAMESLIST_MAIN_MENU )
    	{
    	ForEachL(iViews,
             VoidMemberFunction(&MxSPView::DynInitNamesListMainMenuPaneL),
             aResourceId,
             aMenuPane);
    	}
        
    ForEachL(iViews,
             VoidMemberFunction(&MxSPView::DynInitMenuPaneL),
             aResourceId,
             aMenuPane);
    }
  
        
TBool CxSPViewManager::HandleCommandL(TInt aCommandId, MPbk2ContactUiControl* aUiControl)
    {    
    typedef TBool (MxSPView::* HandleCommandPtr)
        (TInt,MPbk2ContactUiControl*);    
    HandleCommandPtr handleFunc = &MxSPView::HandleCommandL;
    return TryEachL(iViews,
                    MemberFunction(handleFunc),
                    aCommandId,
                    aUiControl);        
    }
    
TInt CxSPViewManager::GetOldCommand( TUint32 aId, TInt aNewCommand,
        							TInt& aOldCommand ) const
	{
	TInt mapCount = iCommandIdMap.Count();
	TInt err( KErrNotFound );
	for( TInt i = 0; i < mapCount && err; i++ )
		{
		CxSPCommandInfo* info = iCommandIdMap[i];
		if( info->Id() == aId && info->NewCommandId() == aNewCommand )
			{
			aOldCommand = info->OldCommandId();
			err = KErrNone;
			}
		}	
	return err;
	}
	
TInt CxSPViewManager::GetNewCommand( TUint32 aId, TInt aOldCommand,
        							TInt& aNewCommand ) const
	{
	TInt mapCount = iCommandIdMap.Count();
	TInt err( KErrNotFound );
	for( TInt i = 0; i < mapCount && err; i++ )
		{
		CxSPCommandInfo* info = iCommandIdMap[i];
		if( info->Id() == aId && info->OldCommandId() == aOldCommand )
			{
			aNewCommand = info->NewCommandId();
			err = KErrNone;
			}
		}	
	return err;	
	}


//  End of File
