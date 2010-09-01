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
*       xSP view info
*
*/


// INCLUDE FILES
#include <AknIconUtils.h>
#include <coemain.h>
#include <barsread.h>
#include <f32file.h>

#include "MxSPViewInfo.h"
#include "CxSPViewInfo.h"
#include "TNewViewIdInfo.h"
#include "ExtensionManagerIcons.hrh"

namespace {

// CONSTANTS
_LIT( K_MBM_FILE_LOCATION, "\\resource\\ExtensionManager.mbm" );
}


// CONSTANTS

// ==================== MEMBER FUNCTIONS ====================
CxSPViewInfo::CxSPViewInfo( TUint32 aId ) : iId( aId ),
											iNewViewId( KErrNotFound )   
    {
    }

CxSPViewInfo::~CxSPViewInfo()
    { 
    delete iTabIconFile;
    delete iName;
    delete iSortIconFile;   
    }

CxSPViewInfo* CxSPViewInfo::NewLC( TResourceReader& aReader, TUint32 aId )
    {
    CxSPViewInfo* self = new (ELeave) CxSPViewInfo( aId );
    CleanupStack::PushL(self);
    self->ConstructL( aReader );
    return self;
    }
    
CxSPViewInfo* CxSPViewInfo::NewLC( const MxSPViewInfo& aInfo, TUint32 aId )
	{
	CxSPViewInfo* self = new (ELeave) CxSPViewInfo( aId );
    CleanupStack::PushL(self);
    self->ConstructL( aInfo );
    return self;
	}
        
void CxSPViewInfo::ConstructL( TResourceReader& aReader )
    {
    iOldViewId = aReader.ReadInt32();
    iTabView = aReader.ReadInt32();
    iTabIconId = aReader.ReadInt32();
    iTabMaskId = aReader.ReadInt32();
    HBufC* pathAndFile = aReader.ReadHBufCL();
    CleanupStack::PushL( pathAndFile );
    TFindFile findFile( CCoeEnv::Static()->FsSession() );
    TInt err = findFile.FindByDir( *pathAndFile, KNullDesC );
    if( err )
    	{
    	iTabIconFile = KNullDesC().AllocL();
    	}
    else
    	{
    	iTabIconFile = findFile.File().AllocL();
    	}    
    CleanupStack::PopAndDestroy( pathAndFile );
    pathAndFile = NULL;
    
    TBool tabIconOk = IsBitmapCreatable( *iTabIconFile, iTabIconId );
	TBool tabMaskOk = IsBitmapCreatable( *iTabIconFile, iTabMaskId );
    if( !tabIconOk || !tabMaskOk )
    	{
    	iTabIconId = qgn_empty_tab;
    	iTabMaskId = qgn_empty_tab_mask;
    	findFile.FindByDir( K_MBM_FILE_LOCATION(), KNullDesC );
    	delete iTabIconFile;
    	iTabIconFile = NULL;
    	iTabIconFile = findFile.File().AllocL();
    	}	
        
    iName = aReader.ReadHBufCL();
    if( !iName )
    	{
    	iName = KNullDesC().AllocL();
    	}
    iSortIconId = aReader.ReadInt32();
    iSortMaskId = aReader.ReadInt32();
        
    pathAndFile = aReader.ReadHBufCL();
    CleanupStack::PushL( pathAndFile );
    err = findFile.FindByDir( *pathAndFile, KNullDesC );
    if( err )
    	{
    	iSortIconFile = KNullDesC().AllocL();
    	}
    else
    	{
    	iSortIconFile = findFile.File().AllocL();
    	}    
    CleanupStack::PopAndDestroy( pathAndFile );
    
    TBool sortIconOk = IsBitmapCreatable( *iSortIconFile, iSortIconId );
	TBool sortMaskOk = IsBitmapCreatable( *iSortIconFile, iSortMaskId );
    if( !sortIconOk || !sortMaskOk )
    	{
    	iSortIconId = qgn_empty_tab;
    	iSortMaskId = qgn_empty_tab_mask;
    	findFile.FindByDir( K_MBM_FILE_LOCATION(), KNullDesC );
    	delete iSortIconFile;
    	iSortIconFile = NULL;
    	iSortIconFile = findFile.File().AllocL();
    	}	
    }
 
void CxSPViewInfo::ConstructL( const MxSPViewInfo& aInfo )
	{
	iOldViewId = aInfo.OldViewId();
    iTabView = aInfo.TabView();
    iTabIconId = aInfo.TabIconId();
    iTabMaskId = aInfo.TabMaskId();
    HBufC* pathAndFile = aInfo.TabIconFile().AllocL();
    CleanupStack::PushL( pathAndFile );
    TFindFile findFile( CCoeEnv::Static()->FsSession() );
    TInt err = findFile.FindByDir( *pathAndFile, KNullDesC );
    if( err )
    	{
    	iTabIconFile = KNullDesC().AllocL();
    	}
    else
    	{
    	iTabIconFile = findFile.File().AllocL();
    	}    
    CleanupStack::PopAndDestroy( pathAndFile );
    pathAndFile = NULL;
    
    TBool tabIconOk = IsBitmapCreatable( *iTabIconFile, iTabIconId );
	TBool tabMaskOk = IsBitmapCreatable( *iTabIconFile, iTabMaskId );
    if( !tabIconOk || !tabMaskOk )
    	{
    	iTabIconId = qgn_empty_tab;
    	iTabMaskId = qgn_empty_tab_mask;
    	findFile.FindByDir( K_MBM_FILE_LOCATION(), KNullDesC );
    	delete iTabIconFile;
    	iTabIconFile = NULL;
    	iTabIconFile = findFile.File().AllocL();
    	}
    	
    iName = aInfo.Name().AllocL();
    
    iSortIconId = aInfo.SortIconId();
    iSortMaskId = aInfo.SortMaskId();
    
    pathAndFile = aInfo.SortIconFile().AllocL();
    CleanupStack::PushL( pathAndFile );
    err = findFile.FindByDir( *pathAndFile, KNullDesC );
    if( err )
    	{
    	iSortIconFile = KNullDesC().AllocL();
    	}
    else
    	{
    	iSortIconFile = findFile.File().AllocL();
    	}    
    CleanupStack::PopAndDestroy( pathAndFile );
    
    TBool sortIconOk = IsBitmapCreatable( *iSortIconFile, iSortIconId );
	TBool sortMaskOk = IsBitmapCreatable( *iSortIconFile, iSortMaskId );
    if( !sortIconOk || !sortMaskOk )
    	{
    	iSortIconId = qgn_empty_tab;
    	iSortMaskId = qgn_empty_tab_mask;
    	findFile.FindByDir( K_MBM_FILE_LOCATION(), KNullDesC );
    	delete iSortIconFile;
    	iSortIconFile = NULL;
    	iSortIconFile = findFile.File().AllocL();
    	}
	}
    
TInt CxSPViewInfo::SetNewId( RArray<TNewViewIdInfo>& aPool )
	{
	TInt err( KErrNotFound);
	const TInt count = aPool.Count();
	for( TInt i = 0; i < count && err; i++ )
		{
		TNewViewIdInfo& info = aPool[i];
		if( info.iFree )
			{
			info.iFree = EFalse;
			err = KErrNone;
			iNewViewId = info.iId;
			}
		}		
	return err;
	}

TUint32 CxSPViewInfo::Id() const
	{
	return iId;	
	}
	
TInt32 CxSPViewInfo::NewViewId() const
	{
	return iNewViewId;	
	}

TInt32 CxSPViewInfo::OldViewId() const
	{
	return iOldViewId;	
	}

TBool CxSPViewInfo::TabView() const
	{
	if ( iTabView == 0 ) 
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	}

TInt32 CxSPViewInfo::TabIconId() const
	{
	return iTabIconId;	
	}

TInt32 CxSPViewInfo::TabMaskId() const
	{
	return iTabMaskId;	
	}

const TDesC& CxSPViewInfo::TabIconFile() const
	{
	return *iTabIconFile;	
	}
	
const TDesC& CxSPViewInfo::Name() const
	{
	return *iName;
	}
        
TInt32 CxSPViewInfo::SortIconId() const
	{
	return iSortIconId;
	}

TInt32 CxSPViewInfo::SortMaskId() const
	{
	return iSortMaskId;
	}

const TDesC& CxSPViewInfo::SortIconFile() const
	{
	return *iSortIconFile;
	}
	
TBool CxSPViewInfo::IsBitmapCreatable( const TDesC& aFileName, TInt aId )
	{	
	TBool result( EFalse );
	if( aFileName.Length() && aId >= 0 )
		{
		CFbsBitmap* bitmap = NULL;	
		TInt err;	
		TRAP( err, bitmap = AknIconUtils::CreateIconL( aFileName, aId ) );
		if( !err )
			{
			delete bitmap;
			result = ETrue;
			}	
		}			
	return result;						
	}
			       
// end of file
