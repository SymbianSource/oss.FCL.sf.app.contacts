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
*       xSP command
*
*/


// INCLUDE FILES
#include <MPbk2CommandObserver.h>
#include "CxSPCommand.h"
#include "CxSPViewManager.h"

// CONSTANTS

// ==================== MEMBER FUNCTIONS ====================
CxSPCommand::CxSPCommand( TInt aCommandId,
							MPbk2ContactUiControl& aUiControl,
							CxSPViewManager& aViewManager ) :
    								iCommandId( aCommandId ),
    								iUiControl( &aUiControl ),
    								iViewManager( aViewManager )
    {
    }

CxSPCommand::~CxSPCommand()
    {
    }

CxSPCommand* CxSPCommand::NewL( TInt aCommandId,
							MPbk2ContactUiControl& aUiControl,
							CxSPViewManager& aViewManager )
    {
    CxSPCommand* self = new (ELeave) CxSPCommand( aCommandId, aUiControl, aViewManager );
    return self;
    }

void CxSPCommand::ExecuteLD()
	{
	CleanupStack::PushL( this );
	iViewManager.HandleCommandL( iCommandId, iUiControl );
	if( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );
        }
	CleanupStack::Pop( this );
	}
	
void CxSPCommand::ResetUiControl( MPbk2ContactUiControl& aUiControl )
	{
	if( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }	
	}
	
void CxSPCommand::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }	

//  End of File
