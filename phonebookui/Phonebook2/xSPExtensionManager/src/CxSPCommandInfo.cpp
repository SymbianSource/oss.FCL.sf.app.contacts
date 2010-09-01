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
*       xSP command info
*
*/


// INCLUDE FILES
#include <barsread.h>
#include "CxSPCommandInfo.h"


// CONSTANTS

// ==================== MEMBER FUNCTIONS ====================
CxSPCommandInfo::CxSPCommandInfo( TUint32 aId ) : iId( aId )   
    {
    }

CxSPCommandInfo::~CxSPCommandInfo()
    { 
    }

CxSPCommandInfo* CxSPCommandInfo::NewLC( TResourceReader& aReader,
        							TUint32 aId,
        							TInt& aNewCommandIds )
    {
    CxSPCommandInfo* self = new (ELeave) CxSPCommandInfo( aId );
    CleanupStack::PushL(self);
    self->ConstructL( aReader, aNewCommandIds );
    return self;
    }
    
void CxSPCommandInfo::ConstructL( TResourceReader& aReader,
        						TInt& aNewCommandIds )
    {
    iOldCommandId = aReader.ReadInt32();
    iNewCommandId = aNewCommandIds++;    
    }
    
TUint32 CxSPCommandInfo::Id() const
	{
	return iId;	
	}
	
TInt32 CxSPCommandInfo::NewCommandId() const
	{
	return iNewCommandId;	
	}

TInt32 CxSPCommandInfo::OldCommandId() const
	{
	return iOldCommandId;	
	}
    
		       
// end of file
