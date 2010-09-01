/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Observes the status of MyCard.
*
*/


#ifndef MPBK2MYCARDOBSERVER_H_
#define MPBK2MYCARDOBSERVER_H_

//  INCLUDES
#include <e32base.h>

/**
 *
 */
class MPbk2MyCardObserver
    {
public:
	
	/*
	 * Observer events
	 */
	enum TMyCardStatusEvent
		{
		EStateStatusResolved,
		EStateThumbnailLoaded,
		EStateThumbnailNotFound
		};
	
	virtual void MyCardEvent( TMyCardStatusEvent aEvent ) = 0;
    
    };


#endif /*MPBK2MYCARDOBSERVER_H_*/
