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
*       xSP view id changer
*
*/


// INCLUDE FILES
#include <coemain.h>
#include <barsread.h>
#include <centralrepository.h>


#include "CxSPViewIdChanger.h"
#include "MxSPFactory.h"
#include "MxSPViewInfo.h"
#include "ExtensionManager.hrh"
#include "CxSPViewInfo.h"
#include <extensionmanagerres.rsg>
#include "ExtensionManagerUID.H"
#include "xSPOrderOrganizer.h"
#include "Phonebook2PrivateCRKeys.h"


// CONSTANTS

// ==================== MEMBER FUNCTIONS ====================
CxSPViewIdChanger::CxSPViewIdChanger()
    {
    }

CxSPViewIdChanger::~CxSPViewIdChanger()
    {
    iViewIdMap.ResetAndDestroy();
    iViewIdMap.Close();
    iTabViewNewIds.Close();
    iNonTabViewNewIds.Close();
    }

CxSPViewIdChanger* CxSPViewIdChanger::NewL()
    {
    CxSPViewIdChanger* self = new (ELeave) CxSPViewIdChanger;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

void CxSPViewIdChanger::ConstructL()
    {
    // construct iTabViewNewIds from constants
    for( TInt id = EExtensionManagerTabViewStartId; id <= EExtensionManagerTabViewMaxId; id++ )
    	{
    	User::LeaveIfError( iTabViewNewIds.Append( TNewViewIdInfo( id )));
    	}
    
    // construct iNonTabViewNewIds from constants
    for( TInt id = EExtensionManagerViewStartId; id <= EExtensionManagerViewMaxId; id++ )
    	{
    	User::LeaveIfError( iNonTabViewNewIds.Append( TNewViewIdInfo( id )));
    	}
    }

void CxSPViewIdChanger::ChangeViewIdsL( CArrayPtrFlat<MxSPFactory>& aFactories )
	{
	for( TInt i = 0; i < aFactories.Count(); i++ )
		{
		TInt res = aFactories[i]->ViewInfoResource();
		if( res != KErrNotFound )
			{
			TResourceReader reader;
			CCoeEnv::Static()->CreateResourceReaderLC( reader, res );
			TInt resCount = reader.ReadInt16();
			for( TInt j = 0; j < resCount; j++ )
				{
				CxSPViewInfo* viewInfo = NULL;
				viewInfo = CxSPViewInfo::NewLC( reader, aFactories[i]->Id() );
				User::LeaveIfError( iViewIdMap.Append( viewInfo ) );
				CleanupStack::Pop(); // viewInfo
				}
			CleanupStack::PopAndDestroy(); // reader
			aFactories[i]->RegisterViewMapper( *this );	
			}
		else
			{
			TInt viewInfoCount = aFactories[i]->ViewInfoCount();
			for( TInt j = 0; j < viewInfoCount; j++ )
				{
				CxSPViewInfo* viewInfo = NULL;
				const MxSPViewInfo* info = aFactories[i]->ViewInfo( j );
				if( info )
					{
					viewInfo = CxSPViewInfo::NewLC( *info, aFactories[i]->Id() );
					User::LeaveIfError( iViewIdMap.Append( viewInfo ) );
					CleanupStack::Pop(); // viewInfo
					}
				}
			aFactories[i]->RegisterViewMapper( *this );
			}			
		}
		
	xSPOrderOrganizer::OrderCheck( iViewIdMap );
	
	for( TInt i = 0; i < iViewIdMap.Count(); i++ )
		{
		CxSPViewInfo& viewInfo = *iViewIdMap[i];
		if( viewInfo.TabView() )
			{
			User::LeaveIfError( viewInfo.SetNewId( iTabViewNewIds ) );
			}
		else
			{
			User::LeaveIfError( viewInfo.SetNewId( iNonTabViewNewIds ) );
			}
		}
	}

void CxSPViewIdChanger::ChangeManagerViewIdsL()
	{
	TInt res = R_EXTENSION_MANAGER_VIEW_INFO;
	TResourceReader reader;
	CCoeEnv::Static()->CreateResourceReaderLC( reader, res );
	TInt resCount = reader.ReadInt16();
	for( TInt j = 0; j < resCount; j++ )
		{
		CxSPViewInfo* viewInfo = CxSPViewInfo::NewLC( reader, KEXTENSIONMANAGERIMPLEMENTATIONUID );
		User::LeaveIfError( viewInfo->SetNewId( iNonTabViewNewIds ) );										
		User::LeaveIfError( iViewIdMap.Append( viewInfo ) );
		CleanupStack::Pop(); // viewInfo
		}
	CleanupStack::PopAndDestroy(); // reader
	}


TInt CxSPViewIdChanger::GetOldView( TUint32 aId, TInt aNewView, TInt& aOldView ) const
	{
	TInt mapCount = iViewIdMap.Count();
	TInt err( KErrNotFound );
	for( TInt i = 0; i < mapCount && err; i++ )
		{
		CxSPViewInfo* info = iViewIdMap[i];
		if( info->Id() == aId && info->NewViewId() == aNewView )
			{
			aOldView = info->OldViewId();
			err = KErrNone;
			}
		}
	return err;
	}

TInt CxSPViewIdChanger::GetNewView( TUint32 aId, TInt aOldView, TInt& aNewView ) const
	{
	TInt mapCount = iViewIdMap.Count();
	TInt err( KErrNotFound );
	for( TInt i = 0; i < mapCount && err; i++ )
		{
		CxSPViewInfo* info = iViewIdMap[i];
		if( info->Id() == aId && info->OldViewId() == aOldView )
			{
			aNewView = info->NewViewId();
			err = KErrNone;
			}
		}
	return err;
	}

void CxSPViewIdChanger::GetViewCount( TInt &aTabs, TInt &aOthers ) const
	{
	aTabs = 0;
	aOthers = 0;
	TInt mapCount = iViewIdMap.Count();
	for( TInt i = 0; i < mapCount; i++ )
		{
		CxSPViewInfo* info = iViewIdMap[i];
		if( info->TabView() )
			{
			aTabs++;
			}
		else
			{
			aOthers++;
			}
		}
	}

void CxSPViewIdChanger::GetTabViewInfoL( TUint32 aId, 
										 RPointerArray<CxSPViewInfo>& aInfoArray ) const
	{
	TInt mapCount = iViewIdMap.Count();
	for( TInt i = 0; i < mapCount; i++ )
		{
		CxSPViewInfo* info = iViewIdMap[i];
		if( info->TabView() && info->Id() == aId )
			{
			User::LeaveIfError( aInfoArray.Append( info ) );
			}
		}
	}
	
void CxSPViewIdChanger::GetTabViewInfoL( RPointerArray<CxSPViewInfo>& aInfoArray ) const
	{
	TInt mapCount = iViewIdMap.Count();
	for( TInt i = 0; i < mapCount; i++ )
		{
		CxSPViewInfo* info = iViewIdMap[i];
		if( info->TabView() )
			{
			User::LeaveIfError( aInfoArray.Append( info ) );
			}
		}
	}	

void CxSPViewIdChanger::WriteTabViewNamesToRepositoryL()
    {
    HBufC* nameStr = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr namePtr = nameStr->Des();
    TInt cenRepError = KErrNone;
    
    CRepository* repository = CRepository::NewLC( TUid::Uid(KCRUidPhonebook) );
        
    // delete anyway even though aError==KErrNone
    cenRepError = repository->Delete( KPhonebooksXPExtensionPluginsName ); 
    // create a new CenRep by KPhonebooksXPExtensionPluginsName
    cenRepError = repository->Create(KPhonebooksXPExtensionPluginsName, namePtr);    
    
    if ( cenRepError==KErrNone )
        {
        for( TInt i = 0; i < iViewIdMap.Count(); i++ )
            {
            CxSPViewInfo* info = iViewIdMap[i];   
            TInt len = namePtr.Length();
            TInt maxLen = namePtr.MaxLength();
            TInt viewNameLen = info->Name().Length();
            if( info && info->TabView() && viewNameLen > 0 && 
                // use > instead of >=, one more char "|" need to be appended
                ( maxLen - len ) > viewNameLen )
                {
                // append service tab view name to CenRep Key
                namePtr.Append(info->Name());
                namePtr.Append( '|' ); // used for seperator   
                }
            }
        
        // set sXPExtension service tab view name string to Cenrep. 
        // this string looks like "View1Name|View2Name|View3Name|...."
        cenRepError = repository->Set( KPhonebooksXPExtensionPluginsName, namePtr );
        }
  
    CleanupStack::PopAndDestroy( repository );
    CleanupStack::PopAndDestroy( nameStr );
    }
// end of file
