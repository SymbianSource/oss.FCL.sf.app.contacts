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
* Description:  The virtual phonebook get own contact link operation
*
*/


#include "COwnContactLinkOperation.h"

#include <MVPbkContactLink.h>
#include <MVPbkSingleContactLinkOperationObserver.h>

namespace VPbkCntModel {

COwnContactLinkOperation::COwnContactLinkOperation(
		MVPbkContactLink* aOwnContactLink,
		MVPbkSingleContactLinkOperationObserver& aObserver ):
		CActive( CActive::EPriorityStandard ),
		iOwnContactLink( aOwnContactLink ),
		iObserver( aObserver )
	{
	CActiveScheduler::Add( this );

	// this operation just delivers ready contact link asynchronnously
	// it's ready right away
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
	}

COwnContactLinkOperation::~COwnContactLinkOperation()
	{
	delete iOwnContactLink;
	Cancel();
	}

void COwnContactLinkOperation::RunL()
	{
	if ( iOwnContactLink )
		{
		iObserver.VPbkSingleContactLinkOperationComplete( *this, iOwnContactLink );
		iOwnContactLink = NULL; //pass ownership
		}
	else
		{
		iObserver.VPbkSingleContactLinkOperationFailed( *this, KErrNotFound );
		}
	}

void COwnContactLinkOperation::DoCancel()
	{
	}

} // namespace VPbkCntModel
//End of file
