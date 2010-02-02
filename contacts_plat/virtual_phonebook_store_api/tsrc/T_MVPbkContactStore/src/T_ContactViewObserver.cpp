/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


// T_ContactViewObserver.cpp

// INCLUDES
#include "T_ContactViewObserver.h"
#include <e32base.h>


// FUNCTIONS 

void T_ContactViewObserver::ContactViewReady(MVPbkContactViewBase& /*aView*/)
	{
    iLastError = KErrNone;
    iLastEvent = EContactViewReady;
    CActiveScheduler::Stop();
	}

void T_ContactViewObserver::ContactViewUnavailable(MVPbkContactViewBase& /*aView*/)
	{
    iLastError = KErrNone;
    iLastEvent = EContactViewUnavailable;
    CActiveScheduler::Stop();
	}
void T_ContactViewObserver::ContactAddedToView(MVPbkContactViewBase& /*aView*/, 
                                               TInt /*aIndex*/, 
                                               const MVPbkContactLink& /*aLink*/)
	{
    iLastError = KErrNone;
    iLastEvent = EContactAddedToView;
    CActiveScheduler::Stop();
	}

void T_ContactViewObserver::ContactRemovedFromView(MVPbkContactViewBase& /*aView*/, 
                                                   TInt /*aIndex*/, 
                                                   const MVPbkContactLink& /*aLink*/)
	{
    iLastError = KErrNone;
    iLastEvent = EContactRemovedFromView;
    CActiveScheduler::Stop();
	}

void T_ContactViewObserver::ContactViewError(MVPbkContactViewBase& /*aView*/, 
                                             TInt aError, 
                                             TBool /*aErrorNotified*/)
	{
    iLastError = aError;
    iLastEvent = EContactViewError;
    CActiveScheduler::Stop();
	}

// End of file
